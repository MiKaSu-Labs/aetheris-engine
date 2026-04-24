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
 * @file tools.c
 *
 * @brief Server tooling utilities implementation.
 *        GM handbook generation, gacha mapping generation,
 *        language discovery, and resource info parsing.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include "tools.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "aetheris.h"
#include "command_map.h"
#include "file_utils.h"
#include "game_constants.h"
#include "game_data.h"
#include "gacha_handler.h"
#include "json_utils.h"
#include "lang.h"
#include "resource_loader.h"

/* =========================================================================
 * Internal helpers
 * ====================================================================== */

/* Writes 'text' followed by '\n' to every builder in the array. */
static void _new_line(char **builders, size_t *lens, size_t *caps,
                      size_t n, const char *text) {
    size_t tlen = strlen(text);
    for (size_t i = 0; i < n; i++) {
        size_t need = lens[i] + tlen + 2;
        if (need > caps[i]) {
            caps[i] = need * 2;
            builders[i] = realloc(builders[i], caps[i]);
        }
        memcpy(builders[i] + lens[i], text, tlen);
        builders[i][lens[i] + tlen]     = '\n';
        builders[i][lens[i] + tlen + 1] = '\0';
        lens[i] += tlen + 1;
    }
}

/* Writes a section header comment to every builder. */
static void _new_section(char **builders, size_t *lens, size_t *caps,
                         size_t n, const char *title) {
    char buf[256];
    snprintf(buf, sizeof(buf), "\n\n// %s", title);
    _new_line(builders, lens, caps, n, buf);
}

/* Appends 'text' followed by '\n' to builder at index 'idx' only. */
static void _append_to(char **builders, size_t *lens, size_t *caps,
                       size_t idx, const char *text) {
    size_t tlen = strlen(text);
    size_t need = lens[idx] + tlen + 2;
    if (need > caps[idx]) {
        caps[idx] = need * 2;
        builders[idx] = realloc(builders[idx], caps[idx]);
    }
    memcpy(builders[idx] + lens[idx], text, tlen);
    builders[idx][lens[idx] + tlen]     = '\n';
    builders[idx][lens[idx] + tlen + 1] = '\0';
    lens[idx] += tlen + 1;
}

/* Creates "./GM Handbook" directory if it does not exist. */
static int _ensure_handbook_dir(void) {
    struct stat st = {0};
    if (stat("./GM Handbook", &st) == 0) return 0;
#ifdef _WIN32
    return _mkdir("./GM Handbook");
#else
    return mkdir("./GM Handbook", 0755);
#endif
}

/* =========================================================================
 * tools_create_gm_handbooks()
 * ====================================================================== */
int tools_create_gm_handbooks(void) {
    return tools_create_gm_handbooks_ex(true);
}

int tools_create_gm_handbooks_ex(bool message) {
    /* Skip generation if the directory already exists. */
    struct stat st = {0};
    if (stat("./GM Handbook", &st) == 0) return 0;

    resource_loader_load_all();

    int      num_languages = lang_get_num_languages();
    char   **builders      = calloc(num_languages, sizeof(char *));
    size_t  *lens          = calloc(num_languages, sizeof(size_t));
    size_t  *caps          = calloc(num_languages, sizeof(size_t));

    for (int i = 0; i < num_languages; i++) {
        caps[i]     = 4096;
        builders[i] = malloc(caps[i]);
        builders[i][0] = '\0';
        lens[i] = 0;
    }

    /* Preamble */
    char preamble[128];
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char timebuf[32];
    strftime(timebuf, sizeof(timebuf), "%Y/%m/%d %H:%M:%S", tm);
    snprintf(preamble, sizeof(preamble),
             "// Aetheris %s GM Handbook", ae_version);
    _new_line(builders, lens, caps, num_languages, preamble);
    snprintf(preamble, sizeof(preamble), "// Created %s", timebuf);
    _new_line(builders, lens, caps, num_languages, preamble);

    /* Commands */
    _new_section(builders, lens, caps, num_languages, "Commands");
    ae_command_list_t *cmds = command_map_get_handlers_as_list(ae_command_map);
    for (size_t ci = 0; ci < cmds->count; ci++) {
        ae_command_handler_t *cmd = cmds->handlers[ci];
        for (int li = 0; li < num_languages; li++) {
            char line[512];
            const char *desc = lang_get_command_description(li, cmd->description_key);
            snprintf(line, sizeof(line), "%-32s : %s", cmd->label, desc ? desc : "");
            _append_to(builders, lens, caps, li, line);
        }
    }

    /* Avatars */
    _new_section(builders, lens, caps, num_languages, "Avatars");
    ae_avatar_data_iter_t *av_iter = game_data_avatar_iter();
    ae_avatar_data_t *av;
    while ((av = game_data_avatar_iter_next(av_iter))) {
        for (int li = 0; li < num_languages; li++) {
            char line[256];
            const char *name = lang_get_text_map_key(av->name_text_map_hash, li);
            snprintf(line, sizeof(line), "%10d : %s", av->id, name ? name : "");
            _append_to(builders, lens, caps, li, line);
        }
    }
    game_data_avatar_iter_free(av_iter);

    /* Items */
    _new_section(builders, lens, caps, num_languages, "Items");
    ae_item_data_iter_t *it_iter = game_data_item_iter();
    ae_item_data_t *it;
    while ((it = game_data_item_iter_next(it_iter))) {
        for (int li = 0; li < num_languages; li++) {
            char line[256];
            const char *name = lang_get_text_map_key(it->name_text_map_hash, li);
            /* BGM items: append the BGM track name if available. */
            if (it->material_type == AE_MATERIAL_BGM && it->item_use_count > 0) {
                int bgm_id = it->item_use[0].use_param_int;
                ae_home_world_bgm_data_t *bgm = game_data_get_home_world_bgm(bgm_id);
                if (bgm) {
                    const char *bgm_name = lang_get_text_map_key(
                        bgm->bgm_name_text_map_hash, li);
                    snprintf(line, sizeof(line), "%10d : %s - %s",
                             it->id,
                             name     ? name     : "",
                             bgm_name ? bgm_name : "");
                    _append_to(builders, lens, caps, li, line);
                    continue;
                }
            }
            snprintf(line, sizeof(line), "%10d : %s", it->id, name ? name : "");
            _append_to(builders, lens, caps, li, line);
        }
    }
    game_data_item_iter_free(it_iter);

    /* Monsters */
    _new_section(builders, lens, caps, num_languages, "Monsters");
    ae_monster_data_iter_t *mo_iter = game_data_monster_iter();
    ae_monster_data_t *mo;
    while ((mo = game_data_monster_iter_next(mo_iter))) {
        for (int li = 0; li < num_languages; li++) {
            char line[256];
            const char *name = lang_get_text_map_key(mo->name_text_map_hash, li);
            snprintf(line, sizeof(line), "%10d : %s - %s",
                     mo->id, mo->monster_name, name ? name : "");
            _append_to(builders, lens, caps, li, line);
        }
    }
    game_data_monster_iter_free(mo_iter);

    /* Scenes */
    _new_section(builders, lens, caps, num_languages, "Scenes");
    ae_scene_data_iter_t *sc_iter = game_data_scene_iter();
    ae_scene_data_t *sc;
    while ((sc = game_data_scene_iter_next(sc_iter))) {
        char line[256];
        snprintf(line, sizeof(line), "%10d : %s", sc->id, sc->script_data);
        _new_line(builders, lens, caps, num_languages, line);
    }
    game_data_scene_iter_free(sc_iter);

    /* Quests */
    _new_section(builders, lens, caps, num_languages, "Quests");
    ae_quest_data_iter_t *qu_iter = game_data_quest_iter();
    ae_quest_data_t *qu;
    while ((qu = game_data_quest_iter_next(qu_iter))) {
        for (int li = 0; li < num_languages; li++) {
            char line[256];
            int main_title_hash = game_data_get_main_quest_title_hash(qu->main_id);
            const char *title = lang_get_text_map_key(main_title_hash, li);
            const char *desc  = lang_get_text_map_key(qu->desc_text_map_hash, li);
            snprintf(line, sizeof(line), "%10d : %s - %s",
                     qu->id,
                     title ? title : "",
                     desc  ? desc  : "");
            _append_to(builders, lens, caps, li, line);
        }
    }
    game_data_quest_iter_free(qu_iter);

    /* Achievements */
    _new_section(builders, lens, caps, num_languages, "Achievements");
    ae_achievement_data_iter_t *ach_iter = game_data_achievement_iter();
    ae_achievement_data_t *ach;
    while ((ach = game_data_achievement_iter_next(ach_iter))) {
        if (!ach->is_used) continue;
        for (int li = 0; li < num_languages; li++) {
            char line[256];
            const char *title = lang_get_text_map_key(ach->title_text_map_hash, li);
            const char *desc  = lang_get_text_map_key(ach->desc_text_map_hash, li);
            snprintf(line, sizeof(line), "%10d : %s - %s",
                     ach->id,
                     title ? title : "",
                     desc  ? desc  : "");
            _append_to(builders, lens, caps, li, line);
        }
    }
    game_data_achievement_iter_free(ach_iter);

    /* Write output files */
    if (_ensure_handbook_dir() != 0) {
        ae_logger_error(ae_logger, "Failed to create GM Handbook directory.");
        goto cleanup;
    }

    for (int i = 0; i < num_languages; i++) {
        char path[256];
        snprintf(path, sizeof(path),
                 "./GM Handbook/GM Handbook - %s.txt",
                 lang_get_language_name(i));
        FILE *f = fopen(path, "wb");
        if (!f) {
            ae_logger_error(ae_logger, "Failed to open handbook file: %s", path);
            continue;
        }
        fwrite(builders[i], 1, lens[i], f);
        fclose(f);
    }

    if (message)
        ae_logger_info(ae_logger, "GM Handbooks generated!");

cleanup:
    for (int i = 0; i < num_languages; i++) free(builders[i]);
    free(builders);
    free(lens);
    free(caps);
    return 0;
}

/* =========================================================================
 * tools_create_gacha_mapping_jsons()
 * ====================================================================== */
int tools_create_gacha_mapping_jsons(char ***out, size_t *count) {
    int num_languages = lang_get_num_languages();

    /* Text map keys for gacha category labels */
    ae_text_strings_t *CHARACTER          = lang_get_text_map_key_by_hash(4233146695UL);
    ae_text_strings_t *WEAPON             = lang_get_text_map_key_by_hash(4231343903UL);
    ae_text_strings_t *STANDARD_WISH      = lang_get_text_map_key_by_hash(332935371UL);
    ae_text_strings_t *CHARACTER_EVENT_WISH  = lang_get_text_map_key_by_hash(2272170627UL);
    ae_text_strings_t *CHARACTER_EVENT_WISH_2 = lang_get_text_map_key_by_hash(3352513147UL);
    ae_text_strings_t *WEAPON_EVENT_WISH  = lang_get_text_map_key_by_hash(2864268523UL);

    char  **sbs  = calloc(num_languages, sizeof(char *));
    size_t *lens = calloc(num_languages, sizeof(size_t));
    size_t *caps = calloc(num_languages, sizeof(size_t));

    for (int i = 0; i < num_languages; i++) {
        caps[i] = 4096;
        sbs[i]  = malloc(caps[i]);
        strncpy(sbs[i], "{\n", caps[i]);
        lens[i] = 2;
    }

    /* Avatars */
    ae_avatar_data_iter_t *av_iter = game_data_avatar_iter_sorted();
    ae_avatar_data_t *av;
    while ((av = game_data_avatar_iter_next(av_iter))) {
        int avatar_id = av->id;
        if (avatar_id >= 11000000) continue; /* skip test avatars */

        const char *color = "";
        if      (strcmp(av->quality_type, "QUALITY_PURPLE") == 0) color = "purple";
        else if (strcmp(av->quality_type, "QUALITY_ORANGE") == 0) color = "yellow";
        else if (strcmp(av->quality_type, "QUALITY_BLUE")   == 0) color = "blue";

        ae_text_strings_t *avatar_name =
            lang_get_text_map_key_by_hash(av->name_text_map_hash);

        for (int li = 0; li < num_languages; li++) {
            char entry[512];
            int key = avatar_id % 1000 + 1000;
            snprintf(entry, sizeof(entry),
                     "\t\"%d\": [\"%s (%s)\", \"%s\"],\n",
                     key,
                     lang_text_strings_get(avatar_name, li),
                     lang_text_strings_get(CHARACTER, li),
                     color);
            size_t elen = strlen(entry);
            size_t need = lens[li] + elen + 1;
            if (need > caps[li]) {
                caps[li] = need * 2;
                sbs[li]  = realloc(sbs[li], caps[li]);
            }
            memcpy(sbs[li] + lens[li], entry, elen + 1);
            lens[li] += elen;
        }
    }
    game_data_avatar_iter_free(av_iter);

    /* Weapons */
    ae_item_data_iter_t *it_iter = game_data_item_iter_sorted();
    ae_item_data_t *it;
    while ((it = game_data_item_iter_next(it_iter))) {
        if (it->id <= 11101 || it->id >= 20000) continue;

        const char *color = NULL;
        switch (it->rank_level) {
            case 3: color = "blue";   break;
            case 4: color = "purple"; break;
            case 5: color = "yellow"; break;
            default: color = NULL;    break;
        }
        if (!color) continue;

        ae_text_strings_t *weapon_name =
            lang_get_text_map_key_by_hash(it->name_text_map_hash);

        for (int li = 0; li < num_languages; li++) {
            char raw_name[256];
            strncpy(raw_name, lang_text_strings_get(weapon_name, li),
                    sizeof(raw_name) - 1);
            /* Escape embedded double-quotes */
            char escaped[512];
            size_t ei = 0;
            for (size_t ri = 0; raw_name[ri] && ei < sizeof(escaped) - 2; ri++) {
                if (raw_name[ri] == '"') escaped[ei++] = '\\';
                escaped[ei++] = raw_name[ri];
            }
            escaped[ei] = '\0';

            char entry[512];
            snprintf(entry, sizeof(entry),
                     "\t\"%d\": [\"%s (%s)\", \"%s\"],\n",
                     it->id, escaped,
                     lang_text_strings_get(WEAPON, li),
                     color);
            size_t elen = strlen(entry);
            size_t need = lens[li] + elen + 1;
            if (need > caps[li]) {
                caps[li] = need * 2;
                sbs[li]  = realloc(sbs[li], caps[li]);
            }
            memcpy(sbs[li] + lens[li], entry, elen + 1);
            lens[li] += elen;
        }
    }
    game_data_item_iter_free(it_iter);

    /* Wish category entries */
    for (int li = 0; li < num_languages; li++) {
        char footer[1024];
        snprintf(footer, sizeof(footer),
                 "\t\"200\": \"%s\",\n"
                 "\t\"301\": \"%s\",\n"
                 "\t\"400\": \"%s\",\n"
                 "\t\"302\": \"%s\"\n"
                 "}",
                 lang_text_strings_get(STANDARD_WISH, li),
                 lang_text_strings_get(CHARACTER_EVENT_WISH, li),
                 lang_text_strings_get(CHARACTER_EVENT_WISH_2, li),
                 lang_text_strings_get(WEAPON_EVENT_WISH, li));
        size_t flen = strlen(footer);
        size_t need = lens[li] + flen + 1;
        if (need > caps[li]) {
            caps[li] = need * 2;
            sbs[li]  = realloc(sbs[li], caps[li]);
        }
        memcpy(sbs[li] + lens[li], footer, flen + 1);
        lens[li] += flen;
    }

    free(lens);
    free(caps);

    *out   = sbs;
    *count = (size_t)num_languages;
    return num_languages;
}

/* =========================================================================
 * tools_generate_gacha_mappings()
 * ====================================================================== */
void tools_generate_gacha_mappings(void) {
    const char *path = gacha_handler_get_mappings_path();
    struct stat st   = {0};
    if (stat(path, &st) == 0) return;

    ae_logger_debug(ae_logger, "Creating default '%s' data", path);
    int rc = tools_create_gacha_mappings(path);
    if (rc != 0)
        ae_logger_warn(ae_logger, "Failed to create gacha mappings.");
}

int tools_create_gacha_mappings(const char *path) {
    char **jsons = NULL;
    size_t count = 0;
    if (tools_create_gacha_mapping_jsons(&jsons, &count) < 0) return -1;

    /* Build the combined JS mappings object, deduplicating locales. */
    char  *seen[64]   = {0};
    size_t seen_count = 0;

    /* Accumulate into a single heap buffer. */
    size_t buf_cap = 65536;
    size_t buf_len = 0;
    char  *buf     = malloc(buf_cap);
    strncpy(buf, "mappings = {\n", buf_cap);
    buf_len = strlen(buf);

    int num_languages = lang_get_num_languages();
    for (int i = 0; i < num_languages && i < (int)count; i++) {
        char locale[16];
        strncpy(locale, lang_get_language_code(i), sizeof(locale) - 1);
        /* Lowercase the locale code */
        for (int c = 0; locale[c]; c++) locale[c] = (char)tolower((unsigned char)locale[c]);

        /* Skip already-written locales (some fall back to en-us) */
        bool already_seen = false;
        for (size_t si = 0; si < seen_count; si++) {
            if (strcmp(seen[si], locale) == 0) { already_seen = true; break; }
        }
        if (already_seen) { free(jsons[i]); continue; }

        seen[seen_count] = strdup(locale);
        seen_count++;

        /* Indent the JSON block */
        char *indented = NULL;
        size_t ilen = 0;
        const char *src = jsons[i];
        for (size_t si = 0; src[si]; si++) {
            ilen++;
            if (src[si] == '\n' && src[si + 1]) ilen++; /* extra char for tab */
        }
        indented = malloc(ilen + 2);
        size_t di = 0;
        for (size_t si = 0; src[si]; si++) {
            indented[di++] = src[si];
            if (src[si] == '\n' && src[si + 1]) indented[di++] = '\t';
        }
        indented[di] = '\0';
        free(jsons[i]);

        /* Append locale entry */
        char header[64];
        snprintf(header, sizeof(header), "\t\"%s\": ", locale);
        size_t hlen  = strlen(header);
        size_t entry = hlen + strlen(indented) + 3; /* ",\n" */
        if (buf_len + entry + 4 > buf_cap) {
            buf_cap = (buf_len + entry) * 2;
            buf     = realloc(buf, buf_cap);
        }
        memcpy(buf + buf_len, header,   hlen);  buf_len += hlen;
        strcpy(buf + buf_len, indented);         buf_len += strlen(indented);
        strcpy(buf + buf_len, ",\n");            buf_len += 2;
        free(indented);
    }
    free(jsons);
    for (size_t si = 0; si < seen_count; si++) free(seen[si]);

    /* Remove trailing ",\n" and close the object */
    if (buf_len >= 2 && buf[buf_len - 2] == ',' && buf[buf_len - 1] == '\n') {
        buf_len -= 2;
    }
    if (buf_len + 3 > buf_cap) buf = realloc(buf, buf_len + 4);
    strcpy(buf + buf_len, "\n}");
    buf_len += 2;

    /* Create parent directories and write the file */
    char dir[256];
    strncpy(dir, path, sizeof(dir) - 1);
    char *last_sep = strrchr(dir, '/');
    if (!last_sep) last_sep = strrchr(dir, '\\');
    if (last_sep) {
        *last_sep = '\0';
        struct stat st = {0};
        if (stat(dir, &st) != 0) {
#ifdef _WIN32
            _mkdir(dir);
#else
            mkdir(dir, 0755);
#endif
        }
    }

    FILE *f = fopen(path, "wb");
    if (!f) { free(buf); return -1; }
    fwrite(buf, 1, buf_len, f);
    fclose(f);
    free(buf);

    ae_logger_debug(ae_logger, "Mappings generated to %s", path);
    return 0;
}

/* =========================================================================
 * tools_get_available_languages()
 * ====================================================================== */
char **tools_get_available_languages(void) {
    char path[256];
    ae_file_utils_get_resource_path("TextMap", path, sizeof(path));

    char  **list  = calloc(64, sizeof(char *));
    size_t  count = 0;

    ae_dir_iter_t *dir = ae_dir_iter_open(path, "TextMap*.json");
    if (!dir) {
        ae_logger_error(ae_logger, "Failed to get available languages.");
        return list;
    }

    const char *filename;
    while ((filename = ae_dir_iter_next(dir)) && count < 63) {
        /* Strip "TextMap" prefix and ".json" suffix, then lowercase */
        char code[32];
        strncpy(code, filename + 7, sizeof(code) - 1); /* skip "TextMap" */
        size_t clen = strlen(code);
        if (clen > 5) code[clen - 5] = '\0';           /* strip ".json"  */
        for (size_t i = 0; code[i]; i++)
            code[i] = (char)tolower((unsigned char)code[i]);
        list[count++] = strdup(code);
    }
    ae_dir_iter_close(dir);
    list[count] = NULL;
    return list;
}

/* =========================================================================
 * tools_get_language_option()
 * @deprecated
 * ====================================================================== */
char *tools_get_language_option(void) {
    char **langs = tools_get_available_languages();
    size_t  n    = 0;
    while (langs[n]) n++;

    if (n == 1) {
        char *result = strdup(langs[0]);
        /* uppercase */
        for (size_t i = 0; result[i]; i++)
            result[i] = (char)toupper((unsigned char)result[i]);
        for (size_t i = 0; i < n; i++) free(langs[i]);
        free(langs);
        return result;
    }

    /* Build the prompt */
    char prompt[2048] = "The following language mappings are available, "
                        "please select one: [default: EN]\n";
    size_t plen       = strlen(prompt);
    int    group      = 0;

    for (size_t i = 0; i < n; i++) {
        size_t llen = strlen(langs[i]) + 2;
        if (plen + llen + 4 < sizeof(prompt)) {
            if (group == 0) { strcat(prompt, ">\t"); plen += 2; }
            strcat(prompt, langs[i]); plen += strlen(langs[i]);
            strcat(prompt, "\t");     plen++;
            group++;
            if (group == 6) {
                strcat(prompt, "\n"); plen++;
                group = 0;
            }
        }
    }
    if (group > 0) { strcat(prompt, "\n"); }
    strcat(prompt, "\nYour choice: [EN] ");

    char input[32] = {0};
    ae_console_reader_t *reader = ae_get_console();
    console_reader_read_line(reader, prompt, input, sizeof(input));

    /* Lowercase the input for comparison */
    char input_lower[32];
    strncpy(input_lower, input, sizeof(input_lower) - 1);
    for (size_t i = 0; input_lower[i]; i++)
        input_lower[i] = (char)tolower((unsigned char)input_lower[i]);

    char *result = NULL;
    for (size_t i = 0; i < n; i++) {
        if (strcmp(langs[i], input_lower) == 0) {
            result = strdup(input);
            /* uppercase */
            for (size_t j = 0; result[j]; j++)
                result[j] = (char)toupper((unsigned char)result[j]);
            break;
        }
    }

    for (size_t i = 0; i < n; i++) free(langs[i]);
    free(langs);

    if (!result) {
        ae_logger_info(ae_logger, "Invalid option. Will use EN (English) as fallback.");
        return strdup("EN");
    }
    return result;
}

/* =========================================================================
 * tools_resources_info()
 * ====================================================================== */
ae_resource_info_t *tools_resources_info(void) {
    char path[256];
    ae_file_utils_get_resource_path("resources.info", path, sizeof(path));

    ae_resource_info_t *info = calloc(1, sizeof(ae_resource_info_t));
    info->scripts = AE_SCRIPTS_TYPE_UNKNOWN;

    FILE *f = fopen(path, "r");
    if (!f) return info;

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        /* Strip newline */
        line[strcspn(line, "\r\n")] = '\0';

        char *sep = strchr(line, ':');
        if (!sep) continue;
        *sep = '\0';

        char *key = line;
        char *val = sep + 1;

        /* Trim leading whitespace from key and value */
        while (*key == ' ' || *key == '\t') key++;
        while (*val == ' ' || *val == '\t') val++;

        if      (strcmp(key, "repo")            == 0) info->repository          = strdup(val);
        else if (strcmp(key, "ver")             == 0) info->version             = strdup(val);
        else if (strcmp(key, "patches")         == 0) info->patches             = strdup(val);
        else if (strcmp(key, "scripts")         == 0) {
            /* Uppercase for enum comparison */
            char upper[64];
            strncpy(upper, val, sizeof(upper) - 1);
            for (size_t i = 0; upper[i]; i++)
                upper[i] = (char)toupper((unsigned char)upper[i]);
            if      (strcmp(upper, "OFFICIAL") == 0) info->scripts = AE_SCRIPTS_TYPE_OFFICIAL;
            else if (strcmp(upper, "DUMPED")   == 0) info->scripts = AE_SCRIPTS_TYPE_DUMPED;
            else                                      info->scripts = AE_SCRIPTS_TYPE_UNKNOWN;
        }
        else if (strcmp(key, "hasnolocals")     == 0)
            info->has_no_locals        = (strcmp(val, "true") == 0);
        else if (strcmp(key, "hasserverres")    == 0)
            info->has_server_resources = (strcmp(val, "true") == 0);
        else if (strcmp(key, "hasscenescriptdata") == 0)
            info->has_scene_script_data = (strcmp(val, "true") == 0);
    }
    fclose(f);
    return info;
}

/* =========================================================================
 * ae_resource_info_to_json()
 * ====================================================================== */
char *ae_resource_info_to_json(const ae_resource_info_t *info) {
    return json_utils_encode_resource_info(info);
}

/* =========================================================================
 * ae_resource_info_free()
 * ====================================================================== */
void ae_resource_info_free(ae_resource_info_t *info) {
    if (!info) return;
    free(info->repository);
    free(info->version);
    free(info->patches);
    free(info);
}
