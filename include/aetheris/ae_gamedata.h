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
 * @file ae_gamedata.h
 *
 * @brief Static game data registries loaded from the resource tree.
 *
 * The registry mirrors the reference gamedata maps: parsed resource
 * objects live in an id-keyed hash table and hand out const pointers
 * to consumers. Ownership of every heap field stays with the registry
 * until `ae_gamedata_destroy`.
 *
 * Item data is the first registry; avatar and equipment registries are
 * added in later phases. Each registry registers its resource
 * definitions with the resource loader on init.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_GAMEDATA_H
#define AE_GAMEDATA_H

#include <aetheris/ae_common.h>
#include <aetheris/ae_error.h>
#include <aetheris/ae_props.h>
#include <aetheris/ae_resource_loader.h>

/* ============================================================ */

/* Opaque cJSON tag forward declaration: the parse entry point only
 * receives element pointers. */
typedef struct cJSON cJSON;

/* Housing furniture specialisation, mirrors SpecialFurnitureType. */
typedef enum ae_special_furniture_type {
    AE_SPECIAL_FURNITURE_NOT_SPECIAL = -1,
    AE_SPECIAL_FURNITURE_FARM_FIELD = 2,
    AE_SPECIAL_FURNITURE_TELEPORT_POINT = 3,
    AE_SPECIAL_FURNITURE_NPC = 5,
    AE_SPECIAL_FURNITURE_APARTMENT = 6,
    AE_SPECIAL_FURNITURE_FURNITURE_SUITE = 7,
    AE_SPECIAL_FURNITURE_PAIMON = 8,
} ae_special_furniture_type_t;

ae_special_furniture_type_t ae_special_furniture_from_value(ae_s32 value);
ae_special_furniture_type_t ae_special_furniture_from_name(
    const char *name);
ae_s32 ae_special_furniture_value(
    ae_special_furniture_type_t type);

/* Weapon stat entry, mirrors ItemData.WeaponProperty. All pointer
 * members are heap owned. */
typedef struct ae_weapon_prop {
    ae_fight_prop_t prop_type;
    float init_value;
    char *type;
} ae_weapon_prop_t;

/* Raw item-use action descriptor, mirrors ItemUseData. */
typedef struct ae_item_use_data {
    ae_item_use_op_t use_op;
    char **use_param;
    ae_size use_param_count;
} ae_item_use_data_t;

/*
 * Parsed ItemExcelConfigData entry. Integer/string lists are heap
 * owned arrays bound by their count fields; absent optional fields
 * are NULL / zero. Callers must never free members themselves; the
 * registry tears items down in `ae_gamedata_destroy` and helpers like
 * `ae_gamedata_item_destroy` exist for single-item lifetimes.
 */
typedef struct ae_item_data {
    ae_u32 id;
    ae_s32 stack_limit;
    ae_s32 max_use_count;
    ae_s32 rank_level;
    char *effect_name;
    ae_s32 rank;
    ae_s32 weight;
    ae_s32 gadget_id;
    char *icon;
    ae_s32 *destroy_return_material;
    ae_size destroy_return_material_count;
    ae_s32 *destroy_return_material_amount;
    ae_size destroy_return_material_amount_count;
    ae_item_type_t item_type;
    ae_material_type_t material_type;
    ae_equip_type_t equip_type;
    char *effect_type;
    char *destroy_rule;
    char *food_quality;
    ae_s32 *satiation_params;
    ae_size satiation_params_count;
    ae_item_use_target_t use_target;
    ae_item_use_data_t *item_use;
    ae_size item_use_count;
    ae_bool use_on_gain;
    ae_s32 main_prop_depot_id;
    ae_s32 append_prop_depot_id;
    ae_s32 append_prop_num;
    ae_s32 set_id;
    ae_s32 *add_prop_levels;
    ae_size add_prop_levels_count;
    ae_s32 base_conv_exp;
    ae_s32 max_level;
    ae_s32 weapon_promote_id;
    ae_s32 weapon_base_exp;
    ae_s32 story_id;
    ae_s32 avatar_promote_id;
    ae_s32 awaken_material;
    ae_s32 *awaken_costs;
    ae_size awaken_costs_count;
    ae_s32 *skill_affix;
    ae_size skill_affix_count;
    ae_weapon_prop_t *weapon_prop;
    ae_size weapon_prop_count;
    ae_u64 name_text_map_hash;
    ae_s32 comfort;
    ae_s32 *furn_type;
    ae_size furn_type_count;
    ae_s32 *furniture_gadget_id;
    ae_size furniture_gadget_id_count;
    ae_special_furniture_type_t special_furniture_type;
    ae_s32 room_scene_id;
} ae_item_data_t;

/**
 * ae_gamedata_item_from_json - parse one excel array element.
 * @object: cJSON value from a resource file element.
 * @out_item: receives the parsed item; zeroed first, so a partial
 *            allocation failure leaves a destroyable struct.
 *
 * Returns AE_OK or AE_ERR_OUT_OF_MEMORY. On failure the caller owns a
 * partially-filled struct and must call `ae_gamedata_item_destroy`.
 */
ae_error_t ae_gamedata_item_from_json(const cJSON *object,
                                      ae_item_data_t *out_item);

/**
 * ae_gamedata_item_on_load - post-parse fix-ups.
 * @item: item to complete.
 *
 * Mirrors the reference onLoad: weapons force equip type, relics keep
 * their equip type and gain relic-prop helper data, weapons drop
 * entries without a fight prop, furniture filters non-positive
 * lists, and material type falls back to none.
 */
ae_error_t ae_gamedata_item_on_load(ae_item_data_t *item);

/**
 * ae_gamedata_item_destroy - release one item's owned memory.
 * @item: item to destroy. Safe on a zeroed struct.
 */
void ae_gamedata_item_destroy(ae_item_data_t *item);

/**
 * ae_gamedata_item_is_equip - relic or weapon predicate.
 */
ae_bool ae_gamedata_item_is_equip(const ae_item_data_t *item);

/**
 * ae_gamedata_item_can_add_relic_prop - add-prop level membership.
 * @item: relic item.
 * @level: avatar level to test.
 *
 * True only for relics whose add-prop levels contain @level.
 */
ae_bool ae_gamedata_item_can_add_relic_prop(const ae_item_data_t *item,
                                            ae_s32 level);

/*
 * Reliquary main-property data, mirrors ReliquaryMainPropData. A
 * property line lists the base stat a relic can roll and how often
 * the depot picks it.
 */
typedef struct ae_reliquary_main_prop {
    ae_u32 id;
    ae_s32 prop_depot_id;
    ae_fight_prop_t fight_prop;
    ae_s32 weight;
} ae_reliquary_main_prop_t;

/*
 * Reliquary sub-stat data, mirrors ReliquaryAffixData. One row is one
 * candidate append stat; the upgrade weight weights a re-roll of an
 * existing stat.
 */
typedef struct ae_reliquary_affix {
    ae_u32 id;
    ae_s32 depot_id;
    ae_s32 group_id;
    ae_fight_prop_t fight_prop;
    float prop_value;
    ae_s32 weight;
    ae_s32 upgrade_weight;
} ae_reliquary_affix_t;

/**
 * ae_gamedata_reliquary_main_prop_from_json - parse one excel element.
 * @object: cJSON value from ReliquaryMainPropExcelConfigData.
 * @out_data: receives the parsed property; zeroed first.
 */
ae_error_t ae_gamedata_reliquary_main_prop_from_json(
    const cJSON *object, ae_reliquary_main_prop_t *out_data);

/**
 * ae_gamedata_reliquary_affix_from_json - parse one excel element.
 * @object: cJSON value from ReliquaryAffixExcelConfigData.
 * @out_data: receives the parsed affix; zeroed first.
 */
ae_error_t ae_gamedata_reliquary_affix_from_json(
    const cJSON *object, ae_reliquary_affix_t *out_data);

/**
 * ae_gamedata_get_reliquary_main_prop - main-prop lookup by id.
 */
const ae_reliquary_main_prop_t *ae_gamedata_get_reliquary_main_prop(
    ae_u32 id);

/**
 * ae_gamedata_get_reliquary_affix - affix lookup by id.
 */
const ae_reliquary_affix_t *ae_gamedata_get_reliquary_affix(ae_u32 id);

/**
 * ae_gamedata_reliquary_main_prop_count - number of parsed props.
 */
ae_size ae_gamedata_reliquary_main_prop_count(void);

/**
 * ae_gamedata_reliquary_affix_count - number of parsed affixes.
 */
ae_size ae_gamedata_reliquary_affix_count(void);

/**
 * ae_gamedata_for_each_reliquary_main_prop - visit all parsed props.
 * @fn: visitor, may be NULL for a pure count.
 * @user: context passed to every visitor call.
 */
void ae_gamedata_for_each_reliquary_main_prop(
    void (*fn)(const ae_reliquary_main_prop_t *data, void *user),
    void *user);

/**
 * ae_gamedata_for_each_reliquary_affix - visit all parsed affixes.
 */
void ae_gamedata_for_each_reliquary_affix(
    void (*fn)(const ae_reliquary_affix_t *data, void *user),
    void *user);

/**
 * ae_gamedata_init - initialise the gamedata registries.
 *
 * Returns AE_ERR_ALREADY_EXISTS when the registry is already live.
 */
ae_error_t ae_gamedata_init(void);

/**
 * ae_gamedata_destroy - release every registry entry.
 *
 * Safe to call on a never-initialised registry; after this the
 * registry can be initialised again.
 */
void ae_gamedata_destroy(void);

/**
 * ae_gamedata_get_item - item lookup by id.
 * @id: item id.
 *
 * Returns a pointer owned by the registry, valid until
 * `ae_gamedata_destroy`, or NULL when unknown.
 */
const ae_item_data_t *ae_gamedata_get_item(ae_u32 id);

/**
 * ae_gamedata_item_count - number of parsed items.
 */
ae_size ae_gamedata_item_count(void);

/**
 * ae_gamedata_register_all - register every gamedata resource def.
 *
 * Registers the item excel definitions with the resource loader.
 */
ae_error_t ae_gamedata_register_all(void);

/**
 * ae_gamedata_load_item_excel - load one item excel family.
 * @resources_dir: base directory of the resource tree.
 * @def: item definition describing the excel files to load.
 *
 * Resource-definition loader for the item registry; load-all drives
 * it after registration.
 */
ae_error_t ae_gamedata_load_item_excel(const char *resources_dir,
                                       const struct ae_resource_def *def);

/**
 * ae_gamedata_load_reliquary_main_prop_excel - load the depot excel.
 * @resources_dir: base directory of the resource tree.
 * @def: definition describing the reliquary main-prop excel to load.
 *
 * Resource-definition loader for the reliquary main-prop registry.
 */
ae_error_t ae_gamedata_load_reliquary_main_prop_excel(
    const char *resources_dir, const struct ae_resource_def *def);

/**
 * ae_gamedata_load_reliquary_affix_excel - load the affix excel.
 * @resources_dir: base directory of the resource tree.
 * @def: definition describing the reliquary affix excel to load.
 *
 * Resource-definition loader for the reliquary affix registry.
 */
ae_error_t ae_gamedata_load_reliquary_affix_excel(
    const char *resources_dir, const struct ae_resource_def *def);

/* ============================================================ */

#endif /* AE_GAMEDATA_H */