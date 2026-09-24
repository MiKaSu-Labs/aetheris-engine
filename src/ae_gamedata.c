/*
 * 
 *           ______ _______ _    _ ______ _____  _____  _____ 
 *     /\   |  ____|__   __| |  | |  ____|  __ \|_   _|/ ____|
 *    /  \  | |__     | |  | |__| | |__  | |__) | | | | (___  
 *   / /\ \ |  __|    | |  |  __  |  __| |  _  /  | | |  \___ \ 
 *  / ____ \| |____   | |  |  |  | | |____| | \ \ _| |_ ____) |
 * /_/    \_\______|  |_|  |_|  |_|______|_|  \_\_____|_____/ 
 * 
 *                                                                                     v1.0
 * @name Aetheris
 *
 * @author dkitagawa
 *
 * @file ae_gamedata.c
 *
 * @brief Static game data registries loaded from the resource tree.
 *
 * The item registry is the first gamedata registry. The four item
 * excel families (material, weapon, reliquary, furniture) parse into
 * one shared `ae_item_data_t` model exactly like the reference, then
 * run the same post-parse fix-ups and land in an id-keyed hash table.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

#include <aetheris/ae_common.h>
#include <aetheris/ae_error.h>
#include <aetheris/ae_gamedata.h>
#include <aetheris/ae_props.h>
#include <aetheris/ae_resource_loader.h>

/* ============================================================ */
/* Special furniture type                                       */
/* ============================================================ */

typedef struct ae_special_furniture_entry {
    ae_s32 value;
    const char *name;
} ae_special_furniture_entry_t;

static const ae_special_furniture_entry_t k_special_furniture[] = {
    { -1, "NOT_SPECIAL" },
    { 2,  "FARM_FIELD" },
    { 3,  "TELEPORT_POINT" },
    { 5,  "NPC" },
    { 6,  "APARTMENT" },
    { 7,  "FURNITURE_SUITE" },
    { 8,  "PAIMON" },
};

ae_special_furniture_type_t ae_special_furniture_from_value(ae_s32 value)
{
    ae_size i;
    for (i = 0; i < AE_ARRAY_SIZE(k_special_furniture); i++) {
        if (k_special_furniture[i].value == value) {
            return (ae_special_furniture_type_t)value;
        }
    }
    return AE_SPECIAL_FURNITURE_NOT_SPECIAL;
}

ae_special_furniture_type_t ae_special_furniture_from_name(const char *name)
{
    ae_size i;
    if (!name) {
        return AE_SPECIAL_FURNITURE_NOT_SPECIAL;
    }
    for (i = 0; i < AE_ARRAY_SIZE(k_special_furniture); i++) {
        if (k_special_furniture[i].name &&
            strcmp(k_special_furniture[i].name, name) == 0) {
            return (ae_special_furniture_type_t)k_special_furniture[i].value;
        }
    }
    return AE_SPECIAL_FURNITURE_NOT_SPECIAL;
}

ae_s32 ae_special_furniture_value(ae_special_furniture_type_t type)
{
    return (ae_s32)type;
}

/* ============================================================ */
/* JSON helpers                                                 */
/* ============================================================ */

static char *dup_str(const char *text)
{
    size_t len;
    char *out;
    if (!text) {
        return NULL;
    }
    len = strlen(text);
    out = (char *)malloc(len + 1u);
    if (!out) {
        return NULL;
    }
    memcpy(out, text, len + 1u);
    return out;
}

static ae_bool get_number(const cJSON *object, const char *key, double *out)
{
    const cJSON *node = cJSON_GetObjectItemCaseSensitive(object, key);
    if (node && cJSON_IsNumber(node)) {
        *out = node->valuedouble;
        return true;
    }
    return false;
}

static ae_s32 get_s32(const cJSON *object, const char *key, ae_s32 def)
{
    double value;
    if (get_number(object, key, &value)) {
        return (ae_s32)value;
    }
    return def;
}

static ae_u64 get_u64(const cJSON *object, const char *key, ae_u64 def)
{
    double value;
    if (get_number(object, key, &value)) {
        return (ae_u64)value;
    }
    return def;
}

static float get_f32(const cJSON *object, const char *key, float def)
{
    double value;
    if (get_number(object, key, &value)) {
        return (float)value;
    }
    return def;
}

static ae_bool get_bool(const cJSON *object, const char *key, ae_bool def)
{
    const cJSON *node = cJSON_GetObjectItemCaseSensitive(object, key);
    if (!node) {
        return def;
    }
    return cJSON_IsTrue(node);
}

static ae_error_t get_string(const cJSON *object, const char *key,
                             char **out)
{
    const cJSON *node = cJSON_GetObjectItemCaseSensitive(object, key);
    *out = NULL;
    if (!node || !cJSON_IsString(node)) {
        return AE_OK;
    }
    *out = dup_str(node->valuestring);
    return *out ? AE_OK : AE_ERR_OUT_OF_MEMORY;
}

static ae_error_t get_s32_array(const cJSON *object, const char *key,
                                ae_s32 **out, ae_size *out_count)
{
    const cJSON *arr;
    ae_size n;
    ae_size w;
    ae_size bytes;
    ae_s32 *buf;

    *out = NULL;
    *out_count = 0;
    arr = cJSON_GetObjectItemCaseSensitive(object, key);
    if (!arr || !cJSON_IsArray(arr)) {
        return AE_OK;
    }
    n = (ae_size)cJSON_GetArraySize(arr);
    if (n == 0) {
        return AE_OK;
    }
    if (!ae_mul_overflow_uz(n, sizeof(*buf), &bytes)) {
        return AE_ERR_OVERFLOW;
    }
    buf = (ae_s32 *)malloc(bytes);
    if (!buf) {
        return AE_ERR_OUT_OF_MEMORY;
    }
    w = 0;
    for (ae_size i = 0; i < n; i++) {
        const cJSON *item = cJSON_GetArrayItem(arr, (int)i);
        if (cJSON_IsNumber(item)) {
            buf[w++] = (ae_s32)item->valuedouble;
        }
    }
    if (w == 0) {
        free(buf);
        return AE_OK;
    }
    *out = buf;
    *out_count = w;
    return AE_OK;
}

static ae_error_t get_string_array(const cJSON *object, const char *key,
                                   char ***out, ae_size *out_count)
{
    const cJSON *arr;
    ae_size n;
    ae_size w;
    ae_size bytes;
    char **buf;

    *out = NULL;
    *out_count = 0;
    arr = cJSON_GetObjectItemCaseSensitive(object, key);
    if (!arr || !cJSON_IsArray(arr)) {
        return AE_OK;
    }
    n = (ae_size)cJSON_GetArraySize(arr);
    if (n == 0) {
        return AE_OK;
    }
    if (!ae_mul_overflow_uz(n, sizeof(*buf), &bytes)) {
        return AE_ERR_OVERFLOW;
    }
    buf = (char **)malloc(bytes);
    if (!buf) {
        return AE_ERR_OUT_OF_MEMORY;
    }
    w = 0;
    for (ae_size i = 0; i < n; i++) {
        const cJSON *item = cJSON_GetArrayItem(arr, (int)i);
        if (!cJSON_IsString(item)) {
            continue;
        }
        buf[w] = dup_str(item->valuestring);
        if (!buf[w]) {
            for (ae_size k = 0; k < w; k++) {
                free(buf[k]);
            }
            free(buf);
            return AE_ERR_OUT_OF_MEMORY;
        }
        w++;
    }
    if (w == 0) {
        free(buf);
        return AE_OK;
    }
    *out = buf;
    *out_count = w;
    return AE_OK;
}

/* ============================================================ */
/* Item struct parsing                                          */
/* ============================================================ */

static ae_error_t get_item_use_list(const cJSON *object, const char *key,
                                    ae_item_use_data_t **out,
                                    ae_size *out_count)
{
    const cJSON *arr;
    ae_size n;
    ae_size bytes;
    ae_item_use_data_t *list;

    *out = NULL;
    *out_count = 0;
    arr = cJSON_GetObjectItemCaseSensitive(object, key);
    if (!arr || !cJSON_IsArray(arr)) {
        return AE_OK;
    }
    n = (ae_size)cJSON_GetArraySize(arr);
    if (n == 0) {
        return AE_OK;
    }
    if (!ae_mul_overflow_uz(n, sizeof(*list), &bytes)) {
        return AE_ERR_OVERFLOW;
    }
    list = (ae_item_use_data_t *)calloc(n, sizeof(*list));
    if (!list) {
        return AE_ERR_OUT_OF_MEMORY;
    }
    for (ae_size i = 0; i < n; i++) {
        const cJSON *entry = cJSON_GetArrayItem(arr, (int)i);
        const cJSON *use_op;
        ae_error_t err;

        if (!entry || !cJSON_IsObject(entry)) {
            continue;
        }
        use_op = cJSON_GetObjectItemCaseSensitive(entry, "useOp");
        if (use_op && cJSON_IsString(use_op)) {
            list[i].use_op = ae_item_use_op_from_name(use_op->valuestring);
        } else {
            list[i].use_op = AE_ITEM_USE_NONE;
        }
        err = get_string_array(entry, "useParam", &list[i].use_param,
                               &list[i].use_param_count);
        if (err != AE_OK) {
            for (ae_size k = 0; k < i; k++) {
                for (ae_size j = 0; j < list[k].use_param_count; j++) {
                    free(list[k].use_param[j]);
                }
                free(list[k].use_param);
            }
            free(list);
            return err;
        }
    }
    *out = list;
    *out_count = n;
    return AE_OK;
}

static ae_error_t get_weapon_prop_list(const cJSON *object, const char *key,
                                       ae_weapon_prop_t **out,
                                       ae_size *out_count)
{
    const cJSON *arr;
    ae_size n;
    ae_size bytes;
    ae_weapon_prop_t *list;

    *out = NULL;
    *out_count = 0;
    arr = cJSON_GetObjectItemCaseSensitive(object, key);
    if (!arr || !cJSON_IsArray(arr)) {
        return AE_OK;
    }
    n = (ae_size)cJSON_GetArraySize(arr);
    if (n == 0) {
        return AE_OK;
    }
    if (!ae_mul_overflow_uz(n, sizeof(*list), &bytes)) {
        return AE_ERR_OVERFLOW;
    }
    list = (ae_weapon_prop_t *)calloc(n, sizeof(*list));
    if (!list) {
        return AE_ERR_OUT_OF_MEMORY;
    }
    for (ae_size i = 0; i < n; i++) {
        const cJSON *entry = cJSON_GetArrayItem(arr, (int)i);
        const cJSON *prop_type;
        ae_error_t err;

        if (!entry || !cJSON_IsObject(entry)) {
            continue;
        }
        prop_type = cJSON_GetObjectItemCaseSensitive(entry, "propType");
        if (prop_type && cJSON_IsString(prop_type)) {
            list[i].prop_type = ae_fight_prop_from_name(prop_type->valuestring);
        } else {
            list[i].prop_type = AE_FIGHT_PROP_NONE;
        }
        list[i].init_value = get_f32(entry, "initValue", 0.0f);
        err = get_string(entry, "type", &list[i].type);
        if (err != AE_OK) {
            for (ae_size k = 0; k < i; k++) {
                free(list[k].type);
            }
            free(list);
            return err;
        }
    }
    *out = list;
    *out_count = n;
    return AE_OK;
}

static const char *get_string_or_default(const cJSON *object,
                                         const char *key,
                                         const char *def)
{
    const cJSON *node = cJSON_GetObjectItemCaseSensitive(object, key);
    if (node && cJSON_IsString(node)) {
        return node->valuestring;
    }
    return def;
}

/* ============================================================ */

ae_error_t ae_gamedata_item_from_json(const cJSON *object,
                                      ae_item_data_t *out_item)
{
    ae_error_t err;

    if (!object || !out_item) {
        return AE_ERR_INVALID_ARG;
    }
    memset(out_item, 0, sizeof(*out_item));

    out_item->id = (ae_u32)get_s32(object, "id", 0);
    out_item->stack_limit = get_s32(object, "stackLimit", 1);
    out_item->max_use_count = get_s32(object, "maxUseCount", 0);
    out_item->rank_level = get_s32(object, "rankLevel", 0);
    out_item->rank = get_s32(object, "rank", 0);
    out_item->weight = get_s32(object, "weight", 0);
    out_item->gadget_id = get_s32(object, "gadgetId", 0);

    out_item->item_type = ae_item_type_from_name(
        get_string_or_default(object, "itemType", NULL));
    out_item->material_type = ae_material_type_from_name(
        get_string_or_default(object, "materialType", NULL));
    out_item->equip_type = ae_equip_type_from_name(
        get_string_or_default(object, "equipType", NULL));
    out_item->use_target = ae_item_use_target_from_name(
        get_string_or_default(object, "useTarget", NULL));
    out_item->special_furniture_type = ae_special_furniture_from_name(
        get_string_or_default(object, "specialFurnitureType", NULL));

    out_item->use_on_gain = get_bool(object, "useOnGain", false);

    out_item->main_prop_depot_id = get_s32(object, "mainPropDepotId", 0);
    out_item->append_prop_depot_id = get_s32(object, "appendPropDepotId", 0);
    out_item->append_prop_num = get_s32(object, "appendPropNum", 0);
    out_item->set_id = get_s32(object, "setId", 0);
    out_item->base_conv_exp = get_s32(object, "baseConvExp", 0);
    out_item->max_level = get_s32(object, "maxLevel", 0);

    out_item->weapon_promote_id = get_s32(object, "weaponPromoteId", 0);
    out_item->weapon_base_exp = get_s32(object, "weaponBaseExp", 0);
    out_item->story_id = get_s32(object, "storyId", 0);
    out_item->avatar_promote_id = get_s32(object, "avatarPromoteId", 0);
    out_item->awaken_material = get_s32(object, "awakenMaterial", 0);

    out_item->comfort = get_s32(object, "comfort", 0);
    out_item->room_scene_id = get_s32(object, "roomSceneId", 0);
    out_item->name_text_map_hash = get_u64(object, "nameTextMapHash", 0);

    err = get_string(object, "effectName", &out_item->effect_name);
    if (err != AE_OK) {
        goto fail;
    }
    err = get_string(object, "icon", &out_item->icon);
    if (err != AE_OK) {
        goto fail;
    }
    err = get_string(object, "effectType", &out_item->effect_type);
    if (err != AE_OK) {
        goto fail;
    }
    err = get_string(object, "destroyRule", &out_item->destroy_rule);
    if (err != AE_OK) {
        goto fail;
    }
    err = get_string(object, "foodQuality", &out_item->food_quality);
    if (err != AE_OK) {
        goto fail;
    }

    err = get_s32_array(object, "destroyReturnMaterial",
                        &out_item->destroy_return_material,
                        &out_item->destroy_return_material_count);
    if (err != AE_OK) {
        goto fail;
    }
    err = get_s32_array(object, "destroyReturnMaterialCount",
                        &out_item->destroy_return_material_amount,
                        &out_item->destroy_return_material_amount_count);
    if (err != AE_OK) {
        goto fail;
    }
    err = get_s32_array(object, "satiationParams",
                        &out_item->satiation_params,
                        &out_item->satiation_params_count);
    if (err != AE_OK) {
        goto fail;
    }
    err = get_s32_array(object, "addPropLevels",
                        &out_item->add_prop_levels,
                        &out_item->add_prop_levels_count);
    if (err != AE_OK) {
        goto fail;
    }
    err = get_s32_array(object, "awakenCosts",
                        &out_item->awaken_costs,
                        &out_item->awaken_costs_count);
    if (err != AE_OK) {
        goto fail;
    }
    err = get_s32_array(object, "skillAffix",
                        &out_item->skill_affix,
                        &out_item->skill_affix_count);
    if (err != AE_OK) {
        goto fail;
    }
    err = get_s32_array(object, "furnType",
                        &out_item->furn_type,
                        &out_item->furn_type_count);
    if (err != AE_OK) {
        goto fail;
    }
    err = get_s32_array(object, "furnitureGadgetID",
                        &out_item->furniture_gadget_id,
                        &out_item->furniture_gadget_id_count);
    if (err != AE_OK) {
        goto fail;
    }

    err = get_item_use_list(object, "itemUse",
                            &out_item->item_use,
                            &out_item->item_use_count);
    if (err != AE_OK) {
        goto fail;
    }
    err = get_weapon_prop_list(object, "weaponProp",
                               &out_item->weapon_prop,
                               &out_item->weapon_prop_count);
    if (err != AE_OK) {
        goto fail;
    }
    return AE_OK;

fail:
    ae_gamedata_item_destroy(out_item);
    return err;
}

/* ============================================================ */

static void filter_s32_positive(ae_s32 **array, ae_size *count)
{
    ae_s32 *values = *array;
    ae_size n = *count;
    ae_size w = 0;

    for (ae_size i = 0; i < n; i++) {
        if (values[i] > 0) {
            values[w++] = values[i];
        }
    }
    *count = w;
}

ae_error_t ae_gamedata_item_on_load(ae_item_data_t *item)
{
    if (!item) {
        return AE_ERR_INVALID_ARG;
    }

    if (item->item_type == AE_ITEM_RELIQUARY) {
        /* Relics keep their excel equip type and gain add-prop level
         * helpers; nothing additional is materialised here. */
    } else if (item->item_type == AE_ITEM_WEAPON) {
        item->equip_type = AE_EQUIP_WEAPON;
    } else {
        item->equip_type = AE_EQUIP_NONE;
    }

    /* Filter weapon properties that lack a recognised fight prop. */
    if (item->weapon_prop_count != 0) {
        ae_size kept = 0;
        ae_size i;

        for (i = 0; i < item->weapon_prop_count; i++) {
            if (item->weapon_prop[i].prop_type != AE_FIGHT_PROP_NONE) {
                kept++;
            }
        }
        if (kept != item->weapon_prop_count) {
            ae_size bytes;
            ae_weapon_prop_t *filtered;
            ae_size w = 0;

            if (!ae_mul_overflow_uz(kept, sizeof(*filtered), &bytes)) {
                return AE_ERR_OVERFLOW;
            }
            if (kept == 0) {
                filtered = NULL;
            } else {
                filtered = (ae_weapon_prop_t *)malloc(bytes);
                if (!filtered) {
                    return AE_ERR_OUT_OF_MEMORY;
                }
            }
            for (i = 0; i < item->weapon_prop_count; i++) {
                if (item->weapon_prop[i].prop_type != AE_FIGHT_PROP_NONE) {
                    filtered[w++] = item->weapon_prop[i];
                } else {
                    free(item->weapon_prop[i].type);
                    item->weapon_prop[i].type = NULL;
                }
            }
            free(item->weapon_prop);
            item->weapon_prop = filtered;
            item->weapon_prop_count = kept;
        }
    }

    filter_s32_positive(&item->furn_type, &item->furn_type_count);
    filter_s32_positive(&item->furniture_gadget_id,
                        &item->furniture_gadget_id_count);
    return AE_OK;
}

void ae_gamedata_item_destroy(ae_item_data_t *item)
{
    if (!item) {
        return;
    }
    free(item->effect_name);
    free(item->icon);
    free(item->effect_type);
    free(item->destroy_rule);
    free(item->food_quality);
    free(item->destroy_return_material);
    free(item->destroy_return_material_amount);
    free(item->satiation_params);
    free(item->add_prop_levels);
    free(item->awaken_costs);
    free(item->skill_affix);
    free(item->furn_type);
    free(item->furniture_gadget_id);

    if (item->item_use) {
        for (ae_size i = 0; i < item->item_use_count; i++) {
            if (item->item_use[i].use_param) {
                for (ae_size j = 0; j < item->item_use[i].use_param_count;
                     j++) {
                    free(item->item_use[i].use_param[j]);
                }
            }
            free(item->item_use[i].use_param);
        }
        free(item->item_use);
    }

    if (item->weapon_prop) {
        for (ae_size i = 0; i < item->weapon_prop_count; i++) {
            free(item->weapon_prop[i].type);
        }
        free(item->weapon_prop);
    }
    memset(item, 0, sizeof(*item));
}

/* ============================================================ */

typedef struct ae_item_entry {
    ae_u32 key;
    ae_bool used;
    ae_item_data_t item;
} ae_item_entry_t;

#define AE_ITEM_MAP_INIT_CAPACITY 64u

static ae_item_entry_t *g_slots;
static ae_size g_slot_capacity;
static ae_size g_item_count;

static ae_reliquary_main_prop_t *g_relic_main_props;
static ae_size g_relic_main_capacity;
static ae_size g_relic_main_count;

static ae_reliquary_affix_t *g_relic_affixes;
static ae_size g_relic_affix_capacity;
static ae_size g_relic_affix_count;

static ae_size item_slot_index(ae_u32 key, ae_size capacity)
{
    ae_u32 hash = key * 2654435761u;
    return (ae_size)(hash & (ae_u32)(capacity - 1u));
}

static ae_error_t item_map_grow(void)
{
    ae_size new_capacity = g_slot_capacity * 2u;
    ae_size bytes;
    ae_item_entry_t *grown;

    if (!ae_mul_overflow_uz(new_capacity, sizeof(*grown), &bytes)) {
        return AE_ERR_OVERFLOW;
    }
    grown = (ae_item_entry_t *)calloc(new_capacity, sizeof(*grown));
    if (!grown) {
        return AE_ERR_OUT_OF_MEMORY;
    }
    for (ae_size i = 0; i < g_slot_capacity; i++) {
        if (!g_slots[i].used) {
            continue;
        }
        ae_size index = item_slot_index(g_slots[i].key, new_capacity);
        while (grown[index].used) {
            index = (index + 1u) & (new_capacity - 1u);
        }
        grown[index] = g_slots[i];
    }
    free(g_slots);
    g_slots = grown;
    g_slot_capacity = new_capacity;
    return AE_OK;
}

static ae_error_t item_map_put(ae_u32 key, const ae_item_data_t *item)
{
    ae_size index;

    if (g_item_count + 1u > (g_slot_capacity * 7u) / 10u) {
        ae_error_t err = item_map_grow();
        if (err != AE_OK) {
            return err;
        }
    }
    index = item_slot_index(key, g_slot_capacity);
    for (;;) {
        ae_item_entry_t *entry = &g_slots[index];
        if (!entry->used) {
            entry->used = true;
            entry->key = key;
            entry->item = *item;
            g_item_count++;
            return AE_OK;
        }
        if (entry->key == key) {
            ae_gamedata_item_destroy(&entry->item);
            entry->item = *item;
            return AE_OK;
        }
        index = (index + 1u) & (g_slot_capacity - 1u);
    }
}

static ae_item_data_t *item_map_get(ae_u32 key)
{
    ae_size index;
    if (!g_slots) {
        return NULL;
    }
    index = item_slot_index(key, g_slot_capacity);
    for (;;) {
        ae_item_entry_t *entry = &g_slots[index];
        if (!entry->used) {
            return NULL;
        }
        if (entry->key == key) {
            return &entry->item;
        }
        index = (index + 1u) & (g_slot_capacity - 1u);
    }
}

/* ============================================================ */

ae_bool ae_gamedata_item_is_equip(const ae_item_data_t *item)
{
    return item &&
           (item->item_type == AE_ITEM_RELIQUARY ||
            item->item_type == AE_ITEM_WEAPON);
}

ae_bool ae_gamedata_item_can_add_relic_prop(const ae_item_data_t *item,
                                            ae_s32 level)
{
    if (!item || item->item_type != AE_ITEM_RELIQUARY) {
        return false;
    }
    for (ae_size i = 0; i < item->add_prop_levels_count; i++) {
        if (item->add_prop_levels[i] == level) {
            return true;
        }
    }
    return false;
}

ae_error_t ae_gamedata_init(void)
{
    if (g_slots) {
        return AE_ERR_ALREADY_EXISTS;
    }
    g_slots = (ae_item_entry_t *)calloc(AE_ITEM_MAP_INIT_CAPACITY,
                                        sizeof(*g_slots));
    if (!g_slots) {
        return AE_ERR_OUT_OF_MEMORY;
    }
    g_slot_capacity = AE_ITEM_MAP_INIT_CAPACITY;
    g_item_count = 0;

    g_relic_main_count = 0;
    g_relic_affix_count = 0;
    return AE_OK;
}

void ae_gamedata_destroy(void)
{
    if (g_slots) {
        for (ae_size i = 0; i < g_slot_capacity; i++) {
            if (g_slots[i].used) {
                ae_gamedata_item_destroy(&g_slots[i].item);
            }
        }
        free(g_slots);
    }
    free(g_relic_main_props);
    free(g_relic_affixes);
    g_slots = NULL;
    g_slot_capacity = 0;
    g_item_count = 0;
    g_relic_main_props = NULL;
    g_relic_main_capacity = 0;
    g_relic_main_count = 0;
    g_relic_affixes = NULL;
    g_relic_affix_capacity = 0;
    g_relic_affix_count = 0;
}

const ae_item_data_t *ae_gamedata_get_item(ae_u32 id)
{
    return item_map_get(id);
}

ae_size ae_gamedata_item_count(void)
{
    return g_item_count;
}

/* ============================================================ */
/* Reliquary prop registries                                    */
/* ============================================================ */

ae_error_t ae_gamedata_reliquary_main_prop_from_json(
    const cJSON *object, ae_reliquary_main_prop_t *out_data)
{
    if (!object || !out_data) {
        return AE_ERR_INVALID_ARG;
    }
    memset(out_data, 0, sizeof(*out_data));
    out_data->id = (ae_u32)get_s32(object, "id", 0);
    out_data->prop_depot_id = get_s32(object, "propDepotId", 0);
    out_data->fight_prop = ae_fight_prop_from_name(
        get_string_or_default(object, "propType", NULL));
    out_data->weight = get_s32(object, "weight", 0);
    return AE_OK;
}

ae_error_t ae_gamedata_reliquary_affix_from_json(
    const cJSON *object, ae_reliquary_affix_t *out_data)
{
    if (!object || !out_data) {
        return AE_ERR_INVALID_ARG;
    }
    memset(out_data, 0, sizeof(*out_data));
    out_data->id = (ae_u32)get_s32(object, "id", 0);
    out_data->depot_id = get_s32(object, "depotId", 0);
    out_data->group_id = get_s32(object, "groupId", 0);
    out_data->fight_prop = ae_fight_prop_from_name(
        get_string_or_default(object, "propType", NULL));
    out_data->prop_value = get_f32(object, "propValue", 0.0f);
    out_data->weight = get_s32(object, "weight", 0);
    out_data->upgrade_weight = get_s32(object, "upgradeWeight", 0);
    return AE_OK;
}

/* ============================================================ */

static ae_error_t relic_main_prop_append(
    const ae_reliquary_main_prop_t *data)
{
    if (g_relic_main_count == g_relic_main_capacity) {
        ae_size new_capacity =
            g_relic_main_capacity ? g_relic_main_capacity * 2u : 64u;
        ae_size bytes;
        ae_reliquary_main_prop_t *grown;

        if (!ae_mul_overflow_uz(new_capacity, sizeof(*grown), &bytes)) {
            return AE_ERR_OVERFLOW;
        }
        grown = (ae_reliquary_main_prop_t *)realloc(g_relic_main_props,
                                                    bytes);
        if (!grown) {
            return AE_ERR_OUT_OF_MEMORY;
        }
        g_relic_main_props = grown;
        g_relic_main_capacity = new_capacity;
    }
    g_relic_main_props[g_relic_main_count++] = *data;
    return AE_OK;
}

static ae_error_t relic_affix_append(const ae_reliquary_affix_t *data)
{
    if (g_relic_affix_count == g_relic_affix_capacity) {
        ae_size new_capacity =
            g_relic_affix_capacity ? g_relic_affix_capacity * 2u : 64u;
        ae_size bytes;
        ae_reliquary_affix_t *grown;

        if (!ae_mul_overflow_uz(new_capacity, sizeof(*grown), &bytes)) {
            return AE_ERR_OVERFLOW;
        }
        grown = (ae_reliquary_affix_t *)realloc(g_relic_affixes, bytes);
        if (!grown) {
            return AE_ERR_OUT_OF_MEMORY;
        }
        g_relic_affixes = grown;
        g_relic_affix_capacity = new_capacity;
    }
    g_relic_affixes[g_relic_affix_count++] = *data;
    return AE_OK;
}

static ae_error_t parse_reliquary_main_prop(const cJSON *object, void *user)
{
    ae_reliquary_main_prop_t data;
    ae_error_t err;

    (void)user;
    err = ae_gamedata_reliquary_main_prop_from_json(object, &data);
    if (err != AE_OK) {
        return err;
    }
    return relic_main_prop_append(&data);
}

static ae_error_t parse_reliquary_affix(const cJSON *object, void *user)
{
    ae_reliquary_affix_t data;
    ae_error_t err;

    (void)user;
    err = ae_gamedata_reliquary_affix_from_json(object, &data);
    if (err != AE_OK) {
        return err;
    }
    return relic_affix_append(&data);
}

/* ============================================================ */

const ae_reliquary_main_prop_t *ae_gamedata_get_reliquary_main_prop(
    ae_u32 id)
{
    for (ae_size i = 0; i < g_relic_main_count; i++) {
        if (g_relic_main_props[i].id == id) {
            return &g_relic_main_props[i];
        }
    }
    return NULL;
}

const ae_reliquary_affix_t *ae_gamedata_get_reliquary_affix(ae_u32 id)
{
    for (ae_size i = 0; i < g_relic_affix_count; i++) {
        if (g_relic_affixes[i].id == id) {
            return &g_relic_affixes[i];
        }
    }
    return NULL;
}

ae_size ae_gamedata_reliquary_main_prop_count(void)
{
    return g_relic_main_count;
}

ae_size ae_gamedata_reliquary_affix_count(void)
{
    return g_relic_affix_count;
}

void ae_gamedata_for_each_reliquary_main_prop(
    void (*fn)(const ae_reliquary_main_prop_t *data, void *user),
    void *user)
{
    if (!fn) {
        return;
    }
    for (ae_size i = 0; i < g_relic_main_count; i++) {
        fn(&g_relic_main_props[i], user);
    }
}

void ae_gamedata_for_each_reliquary_affix(
    void (*fn)(const ae_reliquary_affix_t *data, void *user),
    void *user)
{
    if (!fn) {
        return;
    }
    for (ae_size i = 0; i < g_relic_affix_count; i++) {
        fn(&g_relic_affixes[i], user);
    }
}

/* ============================================================ */
/* Registration and loaders                                     */
/* ============================================================ */

static ae_error_t parse_item(const cJSON *object, void *user)
{
    ae_item_data_t item;
    ae_error_t err;

    (void)user;
    err = ae_gamedata_item_from_json(object, &item);
    if (err != AE_OK) {
        return err;
    }
    err = ae_gamedata_item_on_load(&item);
    if (err != AE_OK) {
        ae_gamedata_item_destroy(&item);
        return err;
    }
    return item_map_put(item.id, &item);
}

static const char *const k_item_filenames[] = {
    "MaterialExcelConfigData.json",
    "WeaponExcelConfigData.json",
    "ReliquaryExcelConfigData.json",
    "HomeWorldFurnitureExcelConfigData.json",
};

static const ae_resource_def_t k_item_resource_def = {
    .type_name = "ItemData",
    .priority = AE_RESOURCE_LOAD_PRIORITY_NORMAL,
    .filenames = k_item_filenames,
    .filename_count = AE_ARRAY_SIZE(k_item_filenames),
    .load = ae_gamedata_load_item_excel,
};

static const char *const k_relic_main_prop_filenames[] = {
    "ReliquaryMainPropExcelConfigData.json",
};

static const ae_resource_def_t k_relic_main_prop_resource_def = {
    .type_name = "ReliquaryMainPropData",
    .priority = AE_RESOURCE_LOAD_PRIORITY_NORMAL,
    .filenames = k_relic_main_prop_filenames,
    .filename_count = AE_ARRAY_SIZE(k_relic_main_prop_filenames),
    .load = ae_gamedata_load_reliquary_main_prop_excel,
};

static const char *const k_relic_affix_filenames[] = {
    "ReliquaryAffixExcelConfigData.json",
};

static const ae_resource_def_t k_relic_affix_resource_def = {
    .type_name = "ReliquaryAffixData",
    .priority = AE_RESOURCE_LOAD_PRIORITY_NORMAL,
    .filenames = k_relic_affix_filenames,
    .filename_count = AE_ARRAY_SIZE(k_relic_affix_filenames),
    .load = ae_gamedata_load_reliquary_affix_excel,
};

ae_error_t ae_gamedata_register_all(void)
{
    ae_error_t first_err = AE_OK;
    ae_error_t err;

    err = ae_resource_loader_register(&k_item_resource_def);
    if (err != AE_OK) {
        first_err = err;
    }
    err = ae_resource_loader_register(&k_relic_main_prop_resource_def);
    if (err != AE_OK && first_err == AE_OK) {
        first_err = err;
    }
    err = ae_resource_loader_register(&k_relic_affix_resource_def);
    if (err != AE_OK && first_err == AE_OK) {
        first_err = err;
    }
    return first_err;
}

ae_error_t ae_gamedata_load_item_excel(const char *resources_dir,
                                       const struct ae_resource_def *def)
{
    ae_error_t first_err = AE_OK;

    if (!def) {
        return AE_ERR_INVALID_ARG;
    }
    for (ae_size i = 0; i < def->filename_count; i++) {
        ae_error_t err = ae_resource_loader_load_json_array(
            resources_dir, def->filenames[i], parse_item, NULL);
        if (err != AE_OK && first_err == AE_OK) {
            first_err = err;
        }
    }
    return first_err;
}

ae_error_t ae_gamedata_load_reliquary_main_prop_excel(
    const char *resources_dir, const struct ae_resource_def *def)
{
    ae_error_t first_err = AE_OK;

    if (!def) {
        return AE_ERR_INVALID_ARG;
    }
    for (ae_size i = 0; i < def->filename_count; i++) {
        ae_error_t err = ae_resource_loader_load_json_array(
            resources_dir, def->filenames[i], parse_reliquary_main_prop,
            NULL);
        if (err != AE_OK && first_err == AE_OK) {
            first_err = err;
        }
    }
    return first_err;
}

ae_error_t ae_gamedata_load_reliquary_affix_excel(
    const char *resources_dir, const struct ae_resource_def *def)
{
    ae_error_t first_err = AE_OK;

    if (!def) {
        return AE_ERR_INVALID_ARG;
    }
    for (ae_size i = 0; i < def->filename_count; i++) {
        ae_error_t err = ae_resource_loader_load_json_array(
            resources_dir, def->filenames[i], parse_reliquary_affix, NULL);
        if (err != AE_OK && first_err == AE_OK) {
            first_err = err;
        }
    }
    return first_err;
}

/* ============================================================ */