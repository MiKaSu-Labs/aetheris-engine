/*
 * 
 *           ______ _______ _    _ ______ _____  _____  _____ 
 *     /\   |  ____|__   __| |  | |  ____|  __ \|_   _|/ ____|
 *    /  \  | |__     | |  | |__| | |__  | |__) | | | | (___  
 *   / /\ \ |  __|    | |  |  __  |  __| |  _  /  | |  \___ \ 
 *  / ____ \| |____   | |  | |  | | |____| | \ \ _| |_ ____) |
 * /_/    \_\______|  |_|  |_|  |_|______|_|  \_\_____|_____/ 
 * 
 *                                                                                     v1.0
 * @name Aetheris
 *
 * @author dkitagawa
 *
 * @file dumpers.c
 *
 * @brief Data dump utilities implementation.
 *        Exports server game data to CSV and JSON files for use by
 *        external tools such as the web GM handbook.
 *        See src/handbook/data/README.md for attributions.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include "dumpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aetheris.h"
#include "command_map.h"
#include "game_data.h"
#include "json_utils.h"
#include "lang.h"
#include "resource_loader.h"

/* =========================================================================
 * ae_dump_quality_from_rank()
 * ====================================================================== */
ae_dump_quality_t ae_dump_quality_from_rank(int rank_level) {
    switch (rank_level) {
        case 0:  return AE_QUALITY_UNKNOWN;
        case 1:  return AE_QUALITY_COMMON;
        case 2:  return AE_QUALITY_UNCOMMON;
        case 3:  return AE_QUALITY_RARE;
        case 4:  return AE_QUALITY_EPIC;
        default: return AE_QUALITY_LEGENDARY;
    }
}

const char *ae_dump_quality_str(ae_dump_quality_t quality) {
    switch (quality) {
        case AE_QUALITY_LEGENDARY: return "LEGENDARY";
        case AE_QUALITY_EPIC:      return "EPIC";
        case AE_QUALITY_RARE:      return "RARE";
        case AE_QUALITY_UNCOMMON:  return "UNCOMMON";
        case AE_QUALITY_COMMON:    return "COMMON";
        default:                   return "UNKNOWN";
    }
}

/* =========================================================================
 * Internal helpers
 * ====================================================================== */

/*
 * Returns the localised description for a command annotation, or the
 * command label itself if the locale is not found.
 */
static char *_command_description(const char *locale,
                                  const ae_command_annotation_t *cmd) {
    ae_language_t *lang = lang_get_language(locale);
    if (!lang) return strdup(cmd->label);

    char key[256];
    snprintf(key, sizeof(key), "commands.%s.description", cmd->label);
    const char *desc = lang_get(lang, key);
    return strdup(desc ? desc : cmd->label);
}

/*
 * Writes a CSV file at 'filename' from an array of (int id, char *row)
 * pairs. 'headers' is an optional comma-separated header line (or NULL).
 * Returns 0 on success, -1 on failure.
 */
typedef struct { int id; char *row; } _csv_entry_t;

static int _write_csv(const char *filename,
                      const char *headers,
                      _csv_entry_t *entries, size_t count) {
    /* Delete existing file */
    remove(filename);

    FILE *f = fopen(filename, "wb");
    if (!f) return -1;

    if (headers) {
        fputs(headers, f);
        fputc('\n', f);
    }

    for (size_t i = 0; i < count; i++) {
        char line[1024];
        snprintf(line, sizeof(line), "%d,%s\n", entries[i].id, entries[i].row);
        fputs(line, f);
    }
    fclose(f);
    return 0;
}

/* =========================================================================
 * ae_dump_commands()
 * ====================================================================== */
int ae_dump_commands(const char *locale) {
    ae_command_map_t *cmd_map = ae_command_map;
    if (!cmd_map) cmd_map = command_map_create(true);

    ae_command_annotation_list_t *anns =
        command_map_get_annotations_as_list(cmd_map);

    /* Build a JSON object: { "label": CommandInfo, ... } */
    ae_json_builder_t *root = json_builder_object();

    for (size_t i = 0; i < anns->count; i++) {
        ae_command_annotation_t *cmd = anns->items[i];
        char *desc = _command_description(locale, cmd);

        ae_json_builder_t *info = json_builder_object();
        /* names: [label, ...aliases] */
        ae_json_builder_t *names_arr = json_builder_array();
        json_array_add_str(names_arr, cmd->label);
        for (size_t ai = 0; ai < cmd->aliases_len; ai++)
            json_array_add_str(names_arr, cmd->aliases[ai]);
        json_object_add(info, "name",        names_arr);
        json_object_add_str(info, "description", desc);
        /* usage */
        ae_json_builder_t *usage_arr = json_builder_array();
        for (size_t ui = 0; ui < cmd->usage_len; ui++)
            json_array_add_str(usage_arr, cmd->usage[ui]);
        json_object_add(info, "usage", usage_arr);
        /* permissions */
        ae_json_builder_t *perm_arr = json_builder_array();
        json_array_add_str(perm_arr, cmd->permission);
        json_array_add_str(perm_arr, cmd->permission_targeted);
        json_object_add(info, "permission", perm_arr);
        /* target */
        json_object_add_str(info, "target",
            ae_target_requirement_str(cmd->target_requirement));

        json_object_add(root, cmd->label, info);
        free(desc);
    }
    command_map_annotation_list_free(anns);

    char *encoded = json_builder_encode(root);
    json_builder_free(root);
    if (!encoded) return -1;

    remove("commands.json");
    FILE *f = fopen("commands.json", "wb");
    if (!f) { free(encoded); return -1; }
    fputs(encoded, f);
    fclose(f);
    free(encoded);
    return 0;
}

/* =========================================================================
 * ae_dump_avatars()
 * ====================================================================== */
int ae_dump_avatars(const char *locale) {
    resource_loader_load_all();
    lang_load_text_maps(false);

    ae_avatar_data_iter_t *iter = game_data_avatar_iter();
    ae_avatar_data_t *av;

    /* Collect entries */
    size_t cap = 256, count = 0;
    _csv_entry_t *entries = malloc(cap * sizeof(_csv_entry_t));

    while ((av = game_data_avatar_iter_next(iter))) {
        const char *name;
        if (av->name_text_map_hash == 0) {
            name = av->name;
        } else {
            ae_text_strings_t *ts =
                lang_get_text_map_key_by_hash(av->name_text_map_hash);
            name = lang_text_strings_get_locale(ts, locale);
        }
        ae_dump_quality_t quality =
            (strcmp(av->quality_type, "QUALITY_PURPLE") == 0)
                ? AE_QUALITY_EPIC
                : AE_QUALITY_LEGENDARY;

        char row[256];
        snprintf(row, sizeof(row), "%s,%s",
                 name ? name : "",
                 ae_dump_quality_str(quality));

        if (count >= cap) { cap *= 2; entries = realloc(entries, cap * sizeof(_csv_entry_t)); }
        entries[count].id  = av->id;
        entries[count].row = strdup(row);
        count++;
    }
    game_data_avatar_iter_free(iter);

    int rc = _write_csv("avatars.csv", NULL, entries, count);
    for (size_t i = 0; i < count; i++) free(entries[i].row);
    free(entries);
    return rc;
}

/* =========================================================================
 * ae_dump_items()
 * ====================================================================== */
int ae_dump_items(const char *locale) {
    resource_loader_load_all();
    lang_load_text_maps(false);

    ae_item_data_iter_t *iter = game_data_item_iter();
    ae_item_data_t *it;

    /* First pass: collect all items */
    size_t raw_cap = 1024, raw_count = 0;
    ae_item_info_t *raw = malloc(raw_cap * sizeof(ae_item_info_t));

    while ((it = game_data_item_iter_next(iter))) {
        ae_text_strings_t *ts =
            lang_get_text_map_key_by_hash(it->name_text_map_hash);
        const char *name = lang_text_strings_get_locale(ts, locale);

        /* Strip leading 3 chars from icon name if non-empty */
        char icon[128] = "";
        if (it->icon && strlen(it->icon) > 3)
            strncpy(icon, it->icon + 3, sizeof(icon) - 1);

        if (raw_count >= raw_cap) {
            raw_cap *= 2;
            raw = realloc(raw, raw_cap * sizeof(ae_item_info_t));
        }
        raw[raw_count].id      = it->id;
        raw[raw_count].name    = strdup(name ? name : "");
        raw[raw_count].quality = ae_dump_quality_from_rank(it->rank_level);
        raw[raw_count].type    = it->item_type;
        raw[raw_count].icon    = strdup(icon);
        raw_count++;
    }
    game_data_item_iter_free(iter);

    /* Second pass: deduplicate by name and id */
    size_t cap = raw_cap, count = 0;
    _csv_entry_t *entries  = malloc(cap * sizeof(_csv_entry_t));
    char        **seen_names = calloc(raw_count, sizeof(char *));
    size_t        seen_count  = 0;
    int          *seen_ids    = calloc(raw_count, sizeof(int));
    size_t        seen_id_cnt = 0;

    for (size_t i = 0; i < raw_count; i++) {
        ae_item_info_t *item = &raw[i];

        /* Skip items with "[CHS]" in name */
        if (strstr(item->name, "[CHS]")) goto next;

        /* Skip duplicate names */
        for (size_t si = 0; si < seen_count; si++)
            if (strcmp(seen_names[si], item->name) == 0) goto next;

        /* Skip duplicate ids */
        for (size_t si = 0; si < seen_id_cnt; si++)
            if (seen_ids[si] == item->id) goto next;

        seen_names[seen_count++] = item->name;
        seen_ids[seen_id_cnt++]  = item->id;

        char row[512];
        snprintf(row, sizeof(row), "%s,%s,%s,%s",
                 item->name,
                 ae_dump_quality_str(item->quality),
                 ae_item_type_str(item->type),
                 item->icon);

        if (count >= cap) { cap *= 2; entries = realloc(entries, cap * sizeof(_csv_entry_t)); }
        entries[count].id  = item->id;
        entries[count].row = strdup(row);
        count++;

        next:;
    }

    free(seen_names);
    free(seen_ids);
    for (size_t i = 0; i < raw_count; i++) { free(raw[i].name); free(raw[i].icon); }
    free(raw);

    int rc = _write_csv("items.csv", NULL, entries, count);
    for (size_t i = 0; i < count; i++) free(entries[i].row);
    free(entries);
    return rc;
}

/* =========================================================================
 * ae_dump_scenes()
 * ====================================================================== */
int ae_dump_scenes(void) {
    resource_loader_load_all();
    lang_load_text_maps(false);

    ae_scene_data_iter_t *iter = game_data_scene_iter();
    ae_scene_data_t *sc;

    size_t cap = 256, count = 0;
    _csv_entry_t *entries = malloc(cap * sizeof(_csv_entry_t));

    while ((sc = game_data_scene_iter_next(iter))) {
        char row[256];
        snprintf(row, sizeof(row), "%s,%s",
                 sc->script_data,
                 ae_scene_type_str(sc->scene_type));

        if (count >= cap) { cap *= 2; entries = realloc(entries, cap * sizeof(_csv_entry_t)); }
        entries[count].id  = sc->id;
        entries[count].row = strdup(row);
        count++;
    }
    game_data_scene_iter_free(iter);

    int rc = _write_csv("scenes.csv", NULL, entries, count);
    for (size_t i = 0; i < count; i++) free(entries[i].row);
    free(entries);
    return rc;
}

/* =========================================================================
 * ae_dump_entities()
 * ====================================================================== */
int ae_dump_entities(const char *locale) {
    resource_loader_load_all();
    lang_load_text_maps(false);

    ae_monster_data_iter_t *iter = game_data_monster_iter();
    ae_monster_data_t *mo;

    size_t cap = 256, count = 0;
    _csv_entry_t *entries = malloc(cap * sizeof(_csv_entry_t));

    while ((mo = game_data_monster_iter_next(iter))) {
        const char *display_name;
        if (mo->name_text_map_hash == 0) {
            display_name = mo->monster_name;
        } else {
            ae_text_strings_t *ts =
                lang_get_text_map_key_by_hash(mo->name_text_map_hash);
            display_name = lang_text_strings_get_locale(ts, locale);
        }

        char row[256];
        snprintf(row, sizeof(row), "%s,%s",
                 display_name ? display_name : "",
                 mo->monster_name);

        if (count >= cap) { cap *= 2; entries = realloc(entries, cap * sizeof(_csv_entry_t)); }
        entries[count].id  = mo->id;
        entries[count].row = strdup(row);
        count++;
    }
    game_data_monster_iter_free(iter);

    int rc = _write_csv("entities.csv", NULL, entries, count);
    for (size_t i = 0; i < count; i++) free(entries[i].row);
    free(entries);
    return rc;
}

/* =========================================================================
 * ae_dump_quests()
 * ====================================================================== */

/* Escapes commas in a string by replacing them with backslashes. */
static char *_escape_commas(const char *src) {
    size_t len = strlen(src);
    char  *out = malloc(len * 2 + 1);
    size_t oi  = 0;
    for (size_t i = 0; src[i]; i++) {
        if (src[i] == ',') out[oi++] = '\\';
        out[oi++] = src[i];
    }
    out[oi] = '\0';
    return out;
}

int ae_dump_quests(const char *locale) {
    resource_loader_load_all();
    lang_load_text_maps(false);

    /* Sub-quests */
    ae_quest_data_iter_t *q_iter = game_data_quest_iter();
    ae_quest_data_t *qu;

    size_t q_cap = 512, q_count = 0;
    _csv_entry_t *q_entries = malloc(q_cap * sizeof(_csv_entry_t));

    while ((qu = game_data_quest_iter_next(q_iter))) {
        const char *raw_desc = "Unknown";
        if (qu->desc_text_map_hash != 0) {
            ae_text_strings_t *ts =
                lang_get_text_map_key_by_hash(qu->desc_text_map_hash);
            const char *loc = lang_text_strings_get_locale(ts, locale);
            if (loc) raw_desc = loc;
        }
        char *desc = _escape_commas(raw_desc);

        char row[512];
        snprintf(row, sizeof(row), "%s,%d", desc, qu->main_id);
        free(desc);

        if (q_count >= q_cap) { q_cap *= 2; q_entries = realloc(q_entries, q_cap * sizeof(_csv_entry_t)); }
        q_entries[q_count].id  = qu->id;
        q_entries[q_count].row = strdup(row);
        q_count++;
    }
    game_data_quest_iter_free(q_iter);

    int rc = _write_csv("quests.csv", "id,description,mainId", q_entries, q_count);
    for (size_t i = 0; i < q_count; i++) free(q_entries[i].row);
    free(q_entries);
    if (rc != 0) return rc;

    /* Main quests */
    ae_main_quest_data_iter_t *mq_iter = game_data_main_quest_iter();
    ae_main_quest_data_t *mq;

    size_t mq_cap = 256, mq_count = 0;
    _csv_entry_t *mq_entries = malloc(mq_cap * sizeof(_csv_entry_t));

    while ((mq = game_data_main_quest_iter_next(mq_iter))) {
        const char *raw_title = "Unknown";
        if (mq->title_text_map_hash != 0) {
            ae_text_strings_t *ts =
                lang_get_text_map_key_by_hash(mq->title_text_map_hash);
            const char *loc = lang_text_strings_get_locale(ts, locale);
            if (loc) raw_title = loc;
        }
        char *title = _escape_commas(raw_title);

        if (mq_count >= mq_cap) { mq_cap *= 2; mq_entries = realloc(mq_entries, mq_cap * sizeof(_csv_entry_t)); }
        mq_entries[mq_count].id  = mq->id;
        mq_entries[mq_count].row = title;
        mq_count++;
    }
    game_data_main_quest_iter_free(mq_iter);

    rc = _write_csv("mainquests.csv", "id,title", mq_entries, mq_count);
    for (size_t i = 0; i < mq_count; i++) free(mq_entries[i].row);
    free(mq_entries);
    return rc;
}

/* =========================================================================
 * ae_dump_areas()
 * ====================================================================== */
int ae_dump_areas(const char *locale) {
    resource_loader_load_all();
    lang_load_text_maps(false);

    ae_world_area_data_iter_t *iter = game_data_world_area_iter();
    ae_world_area_data_t *area;

    size_t cap = 256, count = 0;
    _csv_entry_t *entries = malloc(cap * sizeof(_csv_entry_t));

    while ((area = game_data_world_area_iter_next(iter))) {
        const char *name = "Unknown";
        if (area->text_map_hash != 0) {
            ae_text_strings_t *ts =
                lang_get_text_map_key_by_hash(area->text_map_hash);
            const char *loc = lang_text_strings_get_locale(ts, locale);
            if (loc) name = loc;
        }

        /* Key is child_area if set, otherwise parent_area */
        int key = area->child_area != 0 ? area->child_area : area->parent_area;

        char row[256];
        snprintf(row, sizeof(row), "%d,%s", area->parent_area, name);

        if (count >= cap) { cap *= 2; entries = realloc(entries, cap * sizeof(_csv_entry_t)); }
        entries[count].id  = key;
        entries[count].row = strdup(row);
        count++;
    }
    game_data_world_area_iter_free(iter);

    int rc = _write_csv("areas.csv", "id,parent,name", entries, count);
    for (size_t i = 0; i < count; i++) free(entries[i].row);
    free(entries);
    return rc;
}
