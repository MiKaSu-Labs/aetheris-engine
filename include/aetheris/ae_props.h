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
 * @file ae_props.h
 *
 * @brief Server-side property and type enums with value and name
 *        lookups, mirroring the reference emu.grasscutter.game.props
 *        package.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_PROPS_H
#define AE_PROPS_H

#include <aetheris/ae_types.h>

/* ============================================================ */

/*
 * The types below mirror the reference enums 1:1.  Every enum is
 * paired with lookup helpers that return the canonical "None" member
 * when a value or name is unknown, so callers can test the result
 * directly without checking error codes.
 */

/* ------------------------------------------------------------------ */
/* ElementType                                                          */
/* ------------------------------------------------------------------ */

/* Numeric id of a fight property (see ae_fight_prop). */
typedef ae_s32 ae_fight_prop_id_t;

typedef enum ae_elem {
    AE_ELEM_NONE = 0,
    AE_ELEM_FIRE = 1,
    AE_ELEM_WATER = 2,
    AE_ELEM_GRASS = 3,
    AE_ELEM_ELECTRIC = 4,
    AE_ELEM_ICE = 5,
    AE_ELEM_FROZEN = 6,
    AE_ELEM_WIND = 7,
    AE_ELEM_ROCK = 8,
    AE_ELEM_ANTI_FIRE = 9,
    AE_ELEM_DEFAULT = 255
} ae_elem_t;

/*
 * Element metadata: the current/max stamina props for the element,
 * the team resonance unlock id, the config name used for hashing,
 * and the element depot index.
 */
typedef struct ae_elem_info {
    ae_fight_prop_id_t cur_energy_prop;
    ae_fight_prop_id_t max_energy_prop;
    ae_s32 team_resonance_id;
    const char *config_name; /* NULL when the element has no config */
    ae_s32 depot_index;
    ae_s32 config_hash; /* ability hash of config_name, or 0 */
} ae_elem_info_t;

ae_elem_t ae_elem_from_value(ae_s32 value);
ae_elem_t ae_elem_from_name(const char *name);
ae_s32 ae_elem_value(ae_elem_t elem);
ae_bool ae_elem_info_get(ae_elem_t elem, ae_elem_info_t *info);

/* ------------------------------------------------------------------ */
/* FightProperty                                                       */
/* ------------------------------------------------------------------ */

typedef enum ae_fight_prop {
    AE_FIGHT_PROP_NONE = 0,
    AE_FIGHT_PROP_BASE_HP = 1,
    AE_FIGHT_PROP_HP = 2,
    AE_FIGHT_PROP_HP_PERCENT = 3,
    AE_FIGHT_PROP_BASE_ATTACK = 4,
    AE_FIGHT_PROP_ATTACK = 5,
    AE_FIGHT_PROP_ATTACK_PERCENT = 6,
    AE_FIGHT_PROP_BASE_DEFENSE = 7,
    AE_FIGHT_PROP_DEFENSE = 8,
    AE_FIGHT_PROP_DEFENSE_PERCENT = 9,
    AE_FIGHT_PROP_BASE_SPEED = 10,
    AE_FIGHT_PROP_SPEED_PERCENT = 11,
    AE_FIGHT_PROP_HP_MP_PERCENT = 12,
    AE_FIGHT_PROP_ATTACK_MP_PERCENT = 13,
    AE_FIGHT_PROP_CRITICAL = 20,
    AE_FIGHT_PROP_ANTI_CRITICAL = 21,
    AE_FIGHT_PROP_CRITICAL_HURT = 22,
    AE_FIGHT_PROP_CHARGE_EFFICIENCY = 23,
    AE_FIGHT_PROP_ADD_HURT = 24,
    AE_FIGHT_PROP_SUB_HURT = 25,
    AE_FIGHT_PROP_HEAL_ADD = 26,
    AE_FIGHT_PROP_HEALED_ADD = 27,
    AE_FIGHT_PROP_ELEMENT_MASTERY = 28,
    AE_FIGHT_PROP_PHYSICAL_SUB_HURT = 29,
    AE_FIGHT_PROP_PHYSICAL_ADD_HURT = 30,
    AE_FIGHT_PROP_DEFENCE_IGNORE_RATIO = 31,
    AE_FIGHT_PROP_DEFENCE_IGNORE_DELTA = 32,
    AE_FIGHT_PROP_FIRE_ADD_HURT = 40,
    AE_FIGHT_PROP_ELEC_ADD_HURT = 41,
    AE_FIGHT_PROP_WATER_ADD_HURT = 42,
    AE_FIGHT_PROP_GRASS_ADD_HURT = 43,
    AE_FIGHT_PROP_WIND_ADD_HURT = 44,
    AE_FIGHT_PROP_ROCK_ADD_HURT = 45,
    AE_FIGHT_PROP_ICE_ADD_HURT = 46,
    AE_FIGHT_PROP_HIT_HEAD_ADD_HURT = 47,
    AE_FIGHT_PROP_FIRE_SUB_HURT = 50,
    AE_FIGHT_PROP_ELEC_SUB_HURT = 51,
    AE_FIGHT_PROP_WATER_SUB_HURT = 52,
    AE_FIGHT_PROP_GRASS_SUB_HURT = 53,
    AE_FIGHT_PROP_WIND_SUB_HURT = 54,
    AE_FIGHT_PROP_ROCK_SUB_HURT = 55,
    AE_FIGHT_PROP_ICE_SUB_HURT = 56,
    AE_FIGHT_PROP_EFFECT_HIT = 60,
    AE_FIGHT_PROP_EFFECT_RESIST = 61,
    AE_FIGHT_PROP_FREEZE_RESIST = 62,
    AE_FIGHT_PROP_TORPOR_RESIST = 63,
    AE_FIGHT_PROP_DIZZY_RESIST = 64,
    AE_FIGHT_PROP_FREEZE_SHORTEN = 65,
    AE_FIGHT_PROP_TORPOR_SHORTEN = 66,
    AE_FIGHT_PROP_DIZZY_SHORTEN = 67,
    AE_FIGHT_PROP_MAX_FIRE_ENERGY = 70,
    AE_FIGHT_PROP_MAX_ELEC_ENERGY = 71,
    AE_FIGHT_PROP_MAX_WATER_ENERGY = 72,
    AE_FIGHT_PROP_MAX_GRASS_ENERGY = 73,
    AE_FIGHT_PROP_MAX_WIND_ENERGY = 74,
    AE_FIGHT_PROP_MAX_ICE_ENERGY = 75,
    AE_FIGHT_PROP_MAX_ROCK_ENERGY = 76,
    AE_FIGHT_PROP_SKILL_CD_MINUS_RATIO = 80,
    AE_FIGHT_PROP_SHIELD_COST_MINUS_RATIO = 81,
    AE_FIGHT_PROP_CUR_FIRE_ENERGY = 1000,
    AE_FIGHT_PROP_CUR_ELEC_ENERGY = 1001,
    AE_FIGHT_PROP_CUR_WATER_ENERGY = 1002,
    AE_FIGHT_PROP_CUR_GRASS_ENERGY = 1003,
    AE_FIGHT_PROP_CUR_WIND_ENERGY = 1004,
    AE_FIGHT_PROP_CUR_ICE_ENERGY = 1005,
    AE_FIGHT_PROP_CUR_ROCK_ENERGY = 1006,
    AE_FIGHT_PROP_CUR_HP = 1010,
    AE_FIGHT_PROP_MAX_HP = 2000,
    AE_FIGHT_PROP_CUR_ATTACK = 2001,
    AE_FIGHT_PROP_CUR_DEFENSE = 2002,
    AE_FIGHT_PROP_CUR_SPEED = 2003,
    AE_FIGHT_PROP_NONEXTRA_ATTACK = 3000,
    AE_FIGHT_PROP_NONEXTRA_DEFENSE = 3001,
    AE_FIGHT_PROP_NONEXTRA_CRITICAL = 3002,
    AE_FIGHT_PROP_NONEXTRA_ANTI_CRITICAL = 3003,
    AE_FIGHT_PROP_NONEXTRA_CRITICAL_HURT = 3004,
    AE_FIGHT_PROP_NONEXTRA_CHARGE_EFFICIENCY = 3005,
    AE_FIGHT_PROP_NONEXTRA_ELEMENT_MASTERY = 3006,
    AE_FIGHT_PROP_NONEXTRA_PHYSICAL_SUB_HURT = 3007,
    AE_FIGHT_PROP_NONEXTRA_FIRE_ADD_HURT = 3008,
    AE_FIGHT_PROP_NONEXTRA_ELEC_ADD_HURT = 3009,
    AE_FIGHT_PROP_NONEXTRA_WATER_ADD_HURT = 3010,
    AE_FIGHT_PROP_NONEXTRA_GRASS_ADD_HURT = 3011,
    AE_FIGHT_PROP_NONEXTRA_WIND_ADD_HURT = 3012,
    AE_FIGHT_PROP_NONEXTRA_ROCK_ADD_HURT = 3013,
    AE_FIGHT_PROP_NONEXTRA_ICE_ADD_HURT = 3014,
    AE_FIGHT_PROP_NONEXTRA_FIRE_SUB_HURT = 3015,
    AE_FIGHT_PROP_NONEXTRA_ELEC_SUB_HURT = 3016,
    AE_FIGHT_PROP_NONEXTRA_WATER_SUB_HURT = 3017,
    AE_FIGHT_PROP_NONEXTRA_GRASS_SUB_HURT = 3018,
    AE_FIGHT_PROP_NONEXTRA_WIND_SUB_HURT = 3019,
    AE_FIGHT_PROP_NONEXTRA_ROCK_SUB_HURT = 3020,
    AE_FIGHT_PROP_NONEXTRA_ICE_SUB_HURT = 3021,
    AE_FIGHT_PROP_NONEXTRA_SKILL_CD_MINUS_RATIO = 3022,
    AE_FIGHT_PROP_NONEXTRA_SHIELD_COST_MINUS_RATIO = 3023,
    AE_FIGHT_PROP_NONEXTRA_PHYSICAL_ADD_HURT = 3024
} ae_fight_prop_t;

/* Compound property: result = base * (1 + percent) + flat. */
typedef struct ae_fight_compound_prop {
    ae_fight_prop_t result;
    ae_fight_prop_t base;
    ae_fight_prop_t percent;
    ae_fight_prop_t flat;
} ae_fight_compound_prop_t;

ae_fight_prop_t ae_fight_prop_from_id(ae_s32 id);
ae_fight_prop_t ae_fight_prop_from_name(const char *name);
ae_fight_prop_t ae_fight_prop_from_short_name(const char *name);
ae_s32 ae_fight_prop_id(ae_fight_prop_t prop);
ae_bool ae_fight_prop_is_percentage(ae_fight_prop_t prop);

/* Returns the compound formula for HP/ATK/DEF when one exists. */
ae_bool ae_fight_prop_compound_get(ae_fight_prop_t prop,
                                   ae_fight_compound_prop_t *out);

/* ------------------------------------------------------------------ */
/* PlayerProperty                                                      */
/* ------------------------------------------------------------------ */

typedef enum ae_player_prop {
    AE_PLAYER_PROP_NONE = 0,
    AE_PLAYER_PROP_EXP = 1001,
    AE_PLAYER_PROP_BREAK_LEVEL = 1002,
    AE_PLAYER_PROP_SATIATION_VAL = 1003,
    AE_PLAYER_PROP_SATIATION_PENALTY_TIME = 1004,
    AE_PLAYER_PROP_LEVEL = 4001,
    AE_PLAYER_PROP_LAST_CHANGE_AVATAR_TIME = 10001,
    AE_PLAYER_PROP_MAX_SPRING_VOLUME = 10002,
    AE_PLAYER_PROP_CUR_SPRING_VOLUME = 10003,
    AE_PLAYER_PROP_IS_SPRING_AUTO_USE = 10004,
    AE_PLAYER_PROP_SPRING_AUTO_USE_PERCENT = 10005,
    AE_PLAYER_PROP_IS_FLYABLE = 10006,
    AE_PLAYER_PROP_IS_WEATHER_LOCKED = 10007,
    AE_PLAYER_PROP_IS_GAME_TIME_LOCKED = 10008,
    AE_PLAYER_PROP_IS_TRANSFERABLE = 10009,
    AE_PLAYER_PROP_MAX_STAMINA = 10010,
    AE_PLAYER_PROP_CUR_PERSIST_STAMINA = 10011,
    AE_PLAYER_PROP_CUR_TEMPORARY_STAMINA = 10012,
    AE_PLAYER_PROP_PLAYER_LEVEL = 10013,
    AE_PLAYER_PROP_PLAYER_EXP = 10014,
    AE_PLAYER_PROP_PLAYER_HCOIN = 10015,
    AE_PLAYER_PROP_PLAYER_SCOIN = 10016,
    AE_PLAYER_PROP_PLAYER_MP_SETTING_TYPE = 10017,
    AE_PLAYER_PROP_IS_MP_MODE_AVAILABLE = 10018,
    AE_PLAYER_PROP_PLAYER_WORLD_LEVEL = 10019,
    AE_PLAYER_PROP_PLAYER_RESIN = 10020,
    AE_PLAYER_PROP_PLAYER_WAIT_SUB_HCOIN = 10022,
    AE_PLAYER_PROP_PLAYER_WAIT_SUB_SCOIN = 10023,
    AE_PLAYER_PROP_IS_ONLY_MP_WITH_PS_PLAYER = 10024,
    AE_PLAYER_PROP_PLAYER_MCOIN = 10025,
    AE_PLAYER_PROP_PLAYER_WAIT_SUB_MCOIN = 10026,
    AE_PLAYER_PROP_PLAYER_LEGENDARY_KEY = 10027,
    AE_PLAYER_PROP_IS_HAS_FIRST_SHARE = 10028,
    AE_PLAYER_PROP_PLAYER_FORGE_POINT = 10029,
    AE_PLAYER_PROP_CUR_CLIMATE_METER = 10035,
    AE_PLAYER_PROP_CUR_CLIMATE_TYPE = 10036,
    AE_PLAYER_PROP_CUR_CLIMATE_AREA_ID = 10037,
    AE_PLAYER_PROP_CUR_CLIMATE_AREA_CLIMATE_TYPE = 10038,
    AE_PLAYER_PROP_PLAYER_WORLD_LEVEL_LIMIT = 10039,
    AE_PLAYER_PROP_PLAYER_WORLD_LEVEL_ADJUST_CD = 10040,
    AE_PLAYER_PROP_PLAYER_LEGENDARY_DAILY_TASK_NUM = 10041,
    AE_PLAYER_PROP_PLAYER_HOME_COIN = 10042,
    AE_PLAYER_PROP_PLAYER_WAIT_SUB_HOME_COIN = 10043,
    AE_PLAYER_PROP_IS_AUTO_UNLOCK_SPECIFIC_EQUIP = 10044,
    AE_PLAYER_PROP_PLAYER_GCG_COIN = 10045,
    AE_PLAYER_PROP_PLAYER_WAIT_SUB_GCG_COIN = 10046,
    AE_PLAYER_PROP_PLAYER_ONLINE_TIME = 10047,
    AE_PLAYER_PROP_PLAYER_CAN_DIVE = 10048,
    AE_PLAYER_PROP_DIVE_MAX_STAMINA = 10049,
    AE_PLAYER_PROP_DIVE_CUR_STAMINA = 10050
} ae_player_prop_t;

typedef struct ae_player_prop_range {
    ae_s32 min;
    ae_s32 max;
    ae_bool dynamic; /* true when the range depends on other props */
} ae_player_prop_range_t;

ae_player_prop_t ae_player_prop_from_id(ae_s32 id);
ae_s32 ae_player_prop_id(ae_player_prop_t prop);
ae_bool ae_player_prop_range_get(ae_player_prop_t prop,
                                 ae_player_prop_range_t *range);

/* ------------------------------------------------------------------ */
/* Other simple tag enums with value/name lookups.                     */
/* ------------------------------------------------------------------ */

typedef enum ae_item_type {
    AE_ITEM_NONE = 0,
    AE_ITEM_VIRTUAL = 1,
    AE_ITEM_MATERIAL = 2,
    AE_ITEM_RELIQUARY = 3,
    AE_ITEM_WEAPON = 4,
    AE_ITEM_DISPLAY = 5,
    AE_ITEM_FURNITURE = 6
} ae_item_type_t;

typedef enum ae_material_type {
    AE_MATERIAL_NONE = 0,
    AE_MATERIAL_FOOD = 1,
    AE_MATERIAL_QUEST = 2,
    AE_MATERIAL_EXCHANGE = 4,
    AE_MATERIAL_CONSUME = 5,
    AE_MATERIAL_EXP_FRUIT = 6,
    AE_MATERIAL_AVATAR = 7,
    AE_MATERIAL_ADSORBATE = 8,
    AE_MATERIAL_CRICKET = 9,
    AE_MATERIAL_ELEM_CRYSTAL = 10,
    AE_MATERIAL_WEAPON_EXP_STONE = 11,
    AE_MATERIAL_CHEST = 12,
    AE_MATERIAL_RELIQUARY_MATERIAL = 13,
    AE_MATERIAL_AVATAR_MATERIAL = 14,
    AE_MATERIAL_NOTICE_ADD_HP = 15,
    AE_MATERIAL_SEA_LAMP = 16,
    AE_MATERIAL_SELECTABLE_CHEST = 17,
    AE_MATERIAL_FLYCLOAK = 18,
    AE_MATERIAL_NAMECARD = 19,
    AE_MATERIAL_TALENT = 20,
    AE_MATERIAL_WIDGET = 21,
    AE_MATERIAL_CHEST_BATCH_USE = 22,
    AE_MATERIAL_FAKE_ABSORBATE = 23,
    AE_MATERIAL_CONSUME_BATCH_USE = 24,
    AE_MATERIAL_WOOD = 25,
    AE_MATERIAL_FURNITURE_FORMULA = 27,
    AE_MATERIAL_CHANNELLER_SLAB_BUFF = 28,
    AE_MATERIAL_FURNITURE_SUITE_FORMULA = 29,
    AE_MATERIAL_COSTUME = 30,
    AE_MATERIAL_HOME_SEED = 31,
    AE_MATERIAL_FISH_BAIT = 32,
    AE_MATERIAL_FISH_ROD = 33,
    AE_MATERIAL_SUMO_BUFF = 34,
    AE_MATERIAL_FIREWORKS = 35,
    AE_MATERIAL_BGM = 36,
    AE_MATERIAL_SPICE_FOOD = 37,
    AE_MATERIAL_ACTIVITY_ROBOT = 38,
    AE_MATERIAL_ACTIVITY_GEAR = 39,
    AE_MATERIAL_ACTIVITY_JIGSAW = 40,
    AE_MATERIAL_ARANARA = 41,
    AE_MATERIAL_DESHRET_MANUAL = 46
} ae_material_type_t;

typedef enum ae_equip_type {
    AE_EQUIP_NONE = 0,
    AE_EQUIP_BRACER = 1,
    AE_EQUIP_NECKLACE = 2,
    AE_EQUIP_SHOES = 3,
    AE_EQUIP_RING = 4,
    AE_EQUIP_DRESS = 5,
    AE_EQUIP_WEAPON = 6
} ae_equip_type_t;

typedef enum ae_item_quality {
    AE_QUALITY_NONE = 0,
    AE_QUALITY_WHITE = 1,
    AE_QUALITY_GREEN = 2,
    AE_QUALITY_BLUE = 3,
    AE_QUALITY_PURPLE = 4,
    AE_QUALITY_ORANGE = 5,
    AE_QUALITY_ORANGE_SP = 105
} ae_item_quality_t;

typedef enum ae_entity_type {
    AE_ENTITY_NONE = 0,
    AE_ENTITY_AVATAR = 1,
    AE_ENTITY_MONSTER = 2,
    AE_ENTITY_BULLET = 3,
    AE_ENTITY_ATTACK_PHYSICAL_UNIT = 4,
    AE_ENTITY_AOE = 5,
    AE_ENTITY_CAMERA = 6,
    AE_ENTITY_ENVIRO_AREA = 7,
    AE_ENTITY_EQUIP = 8,
    AE_ENTITY_MONSTER_EQUIP = 9,
    AE_ENTITY_GRASS = 10,
    AE_ENTITY_LEVEL = 11,
    AE_ENTITY_NPC = 12,
    AE_ENTITY_TRANS_POINT_FIRST = 13,
    AE_ENTITY_TRANS_POINT_FIRST_GADGET = 14,
    AE_ENTITY_TRANS_POINT_SECOND = 15,
    AE_ENTITY_TRANS_POINT_SECOND_GADGET = 16,
    AE_ENTITY_DROP_ITEM = 17,
    AE_ENTITY_FIELD = 18,
    AE_ENTITY_GADGET = 19,
    AE_ENTITY_WATER = 20,
    AE_ENTITY_GATHER_POINT = 21,
    AE_ENTITY_GATHER_OBJECT = 22,
    AE_ENTITY_AIRFLOW_FIELD = 23,
    AE_ENTITY_SPEEDUP_FIELD = 24,
    AE_ENTITY_GEAR = 25,
    AE_ENTITY_CHEST = 26,
    AE_ENTITY_ENERGY_BALL = 27,
    AE_ENTITY_ELEM_CRYSTAL = 28,
    AE_ENTITY_TIMELINE = 29,
    AE_ENTITY_WORKTOP = 30,
    AE_ENTITY_TEAM = 31,
    AE_ENTITY_PLATFORM = 32,
    AE_ENTITY_AMBER_WIND = 33,
    AE_ENTITY_ENV_ANIMAL = 34,
    AE_ENTITY_SEAL_GADGET = 35,
    AE_ENTITY_TREE = 36,
    AE_ENTITY_BUSH = 37,
    AE_ENTITY_QUEST_GADGET = 38,
    AE_ENTITY_LIGHTNING = 39,
    AE_ENTITY_REWARD_POINT = 40,
    AE_ENTITY_REWARD_STATUE = 41,
    AE_ENTITY_MP_LEVEL = 42,
    AE_ENTITY_WIND_SEED = 43,
    AE_ENTITY_MP_PLAY_REWARD_POINT = 44,
    AE_ENTITY_VIEW_POINT = 45,
    AE_ENTITY_REMOTE_AVATAR = 46,
    AE_ENTITY_GENERAL_REWARD_POINT = 47,
    AE_ENTITY_PLAY_TEAM = 48,
    AE_ENTITY_OFFERING_GADGET = 49,
    AE_ENTITY_EYE_POINT = 50,
    AE_ENTITY_MIRACLE_RING = 51,
    AE_ENTITY_FOUNDATION = 52,
    AE_ENTITY_WIDGET_GADGET = 53,
    AE_ENTITY_VEHICLE = 54,
    AE_ENTITY_SUB_EQUIP = 55,
    AE_ENTITY_FISH_ROD = 56,
    AE_ENTITY_CUSTOM_TILE = 57,
    AE_ENTITY_FISH_POOL = 58,
    AE_ENTITY_CUSTOM_GADGET = 59,
    AE_ENTITY_BLACK_MUD = 60,
    AE_ENTITY_ROGUELIKE_OPERATOR_GADGET = 61,
    AE_ENTITY_NIGHT_CROW_GADGET = 62,
    AE_ENTITY_PROJECTOR = 63,
    AE_ENTITY_SCREEN = 64,
    AE_ENTITY_ECHO_SHELL = 65,
    AE_ENTITY_UI_INTERACT_GADGET = 66,
    AE_ENTITY_REGION = 98,
    AE_ENTITY_PLACE_HOLDER = 99
} ae_entity_type_t;

typedef enum ae_entity_id_type {
    AE_ENTITY_ID_AVATAR = 0x01,
    AE_ENTITY_ID_MONSTER = 0x02,
    AE_ENTITY_ID_NPC = 0x03,
    AE_ENTITY_ID_GADGET = 0x04,
    AE_ENTITY_ID_REGION = 0x05,
    AE_ENTITY_ID_WEAPON = 0x06,
    AE_ENTITY_ID_TEAM = 0x09,
    AE_ENTITY_ID_MP_LEVEL = 0x0b
} ae_entity_id_type_t;

typedef enum ae_weapon_type {
    AE_WEAPON_NONE = 0,
    AE_WEAPON_SWORD_ONE_HAND = 1,
    AE_WEAPON_CROSSBOW = 2,
    AE_WEAPON_STAFF = 3,
    AE_WEAPON_DOUBLE_DAGGER = 4,
    AE_WEAPON_KATANA = 5,
    AE_WEAPON_SHURIKEN = 6,
    AE_WEAPON_STICK = 7,
    AE_WEAPON_SPEAR = 8,
    AE_WEAPON_SHIELD_SMALL = 9,
    AE_WEAPON_CATALYST = 10,
    AE_WEAPON_CLAYMORE = 11,
    AE_WEAPON_BOW = 12,
    AE_WEAPON_POLE = 13
} ae_weapon_type_t;

typedef enum ae_scene_type {
    AE_SCENE_NONE = 0,
    AE_SCENE_WORLD = 1,
    AE_SCENE_DUNGEON = 2,
    AE_SCENE_ROOM = 3,
    AE_SCENE_HOME_WORLD = 4,
    AE_SCENE_HOME_ROOM = 5,
    AE_SCENE_ACTIVITY = 6
} ae_scene_type_t;

typedef enum ae_monster_type {
    AE_MONSTER_NONE = 0,
    AE_MONSTER_ORDINARY = 1,
    AE_MONSTER_BOSS = 2,
    AE_MONSTER_ENV_ANIMAL = 3,
    AE_MONSTER_LITTLE_MONSTER = 4,
    AE_MONSTER_FISH = 5
} ae_monster_type_t;

typedef enum ae_life_state {
    AE_LIFE_NONE = 0,
    AE_LIFE_ALIVE = 1,
    AE_LIFE_DEAD = 2,
    AE_LIFE_REVIVE = 3
} ae_life_state_t;

typedef enum ae_climate_type {
    AE_CLIMATE_NONE = 0,
    AE_CLIMATE_SUNNY = 1,
    AE_CLIMATE_CLOUDY = 2,
    AE_CLIMATE_RAIN = 3,
    AE_CLIMATE_THUNDERSTORM = 4,
    AE_CLIMATE_SNOW = 5,
    AE_CLIMATE_MIST = 6,
    AE_CLIMATE_DESERT = 7
} ae_climate_type_t;

/* Fetter/NPC affinity state. */
typedef enum ae_fetter_state {
    AE_FETTER_NONE = 0,
    AE_FETTER_NOT_OPEN = 1,
    AE_FETTER_OPEN = 1,
    AE_FETTER_FINISH = 3
} ae_fetter_state_t;

typedef enum ae_enter_reason {
    AE_ENTER_NONE = 0,
    AE_ENTER_LOGIN = 1,
    AE_ENTER_DUNGEON_REPLAY = 11,
    AE_ENTER_DUNGEON_REVIVE_ON_WAYPOINT = 12,
    AE_ENTER_DUNGEON_ENTER = 13,
    AE_ENTER_DUNGEON_QUIT = 14,
    AE_ENTER_GM = 21,
    AE_ENTER_QUEST_ROLLBACK = 31,
    AE_ENTER_REVIVAL = 32,
    AE_ENTER_PERSONAL_SCENE = 41,
    AE_ENTER_TRANS_POINT = 42,
    AE_ENTER_CLIENT_TRANSMIT = 43,
    AE_ENTER_FORCE_DRAG_BACK = 44,
    AE_ENTER_TEAM_KICK = 51,
    AE_ENTER_TEAM_JOIN = 52,
    AE_ENTER_TEAM_BACK = 53,
    AE_ENTER_MUIP = 54,
    AE_ENTER_DUNGEON_INVITE_ACCEPT = 55,
    AE_ENTER_LUA = 56,
    AE_ENTER_ACTIVITY_LOAD_TERRAIN = 57,
    AE_ENTER_HOST_FROM_SINGLE_TO_MP = 58,
    AE_ENTER_MP_PLAY = 59,
    AE_ENTER_ANCHOR_POINT = 60,
    AE_ENTER_LUA_SKIP_UI = 61,
    AE_ENTER_RELOAD_TERRAIN = 62,
    AE_ENTER_DRAFT_TRANSFER = 63,
    AE_ENTER_ENTER_HOME = 64,
    AE_ENTER_EXIT_HOME = 65,
    AE_ENTER_CHANGE_HOME_MODULE = 66,
    AE_ENTER_GALLERY = 67,
    AE_ENTER_HOME_SCENE_JUMP = 68,
    AE_ENTER_HIDE_AND_SEEK = 69
} ae_enter_reason_t;

typedef enum ae_grow_curve {
    AE_GROW_CURVE_NONE = 0,
    AE_GROW_CURVE_HP = 1,
    AE_GROW_CURVE_ATTACK = 2,
    AE_GROW_CURVE_STAMINA = 3,
    AE_GROW_CURVE_STRIKE = 4,
    AE_GROW_CURVE_ANTI_STRIKE = 5,
    AE_GROW_CURVE_ANTI_STRIKE1 = 6,
    AE_GROW_CURVE_ANTI_STRIKE2 = 7,
    AE_GROW_CURVE_ANTI_STRIKE3 = 8,
    AE_GROW_CURVE_STRIKE_HURT = 9,
    AE_GROW_CURVE_ELEMENT = 10,
    AE_GROW_CURVE_KILL_EXP = 11,
    AE_GROW_CURVE_DEFENSE = 12,
    AE_GROW_CURVE_ATTACK_BOMB = 13,
    AE_GROW_CURVE_HP_LITTLEMONSTER = 14,
    AE_GROW_CURVE_ELEMENT_MASTERY = 15,
    AE_GROW_CURVE_PROGRESSION = 16,
    AE_GROW_CURVE_DEFENDING = 17,
    AE_GROW_CURVE_MHP = 18,
    AE_GROW_CURVE_MATK = 19,
    AE_GROW_CURVE_TOWERATK = 20,
    AE_GROW_CURVE_HP_S5 = 21,
    AE_GROW_CURVE_HP_S4 = 22,
    AE_GROW_CURVE_HP_2 = 23,
    AE_GROW_CURVE_ATTACK_S5 = 31,
    AE_GROW_CURVE_ATTACK_S4 = 32,
    AE_GROW_CURVE_ATTACK_S3 = 33,
    AE_GROW_CURVE_STRIKE_S5 = 34,
    AE_GROW_CURVE_DEFENSE_S5 = 41,
    AE_GROW_CURVE_DEFENSE_S4 = 42,
    AE_GROW_CURVE_ATTACK_101 = 1101,
    AE_GROW_CURVE_ATTACK_102 = 1102,
    AE_GROW_CURVE_ATTACK_103 = 1103,
    AE_GROW_CURVE_ATTACK_104 = 1104,
    AE_GROW_CURVE_ATTACK_105 = 1105,
    AE_GROW_CURVE_ATTACK_201 = 1201,
    AE_GROW_CURVE_ATTACK_202 = 1202,
    AE_GROW_CURVE_ATTACK_203 = 1203,
    AE_GROW_CURVE_ATTACK_204 = 1204,
    AE_GROW_CURVE_ATTACK_205 = 1205,
    AE_GROW_CURVE_ATTACK_301 = 1301,
    AE_GROW_CURVE_ATTACK_302 = 1302,
    AE_GROW_CURVE_ATTACK_303 = 1303,
    AE_GROW_CURVE_ATTACK_304 = 1304,
    AE_GROW_CURVE_ATTACK_305 = 1305,
    AE_GROW_CURVE_CRITICAL_101 = 2101,
    AE_GROW_CURVE_CRITICAL_102 = 2102,
    AE_GROW_CURVE_CRITICAL_103 = 2103,
    AE_GROW_CURVE_CRITICAL_104 = 2104,
    AE_GROW_CURVE_CRITICAL_105 = 2105,
    AE_GROW_CURVE_CRITICAL_201 = 2201,
    AE_GROW_CURVE_CRITICAL_202 = 2202,
    AE_GROW_CURVE_CRITICAL_203 = 2203,
    AE_GROW_CURVE_CRITICAL_204 = 2204,
    AE_GROW_CURVE_CRITICAL_205 = 2205,
    AE_GROW_CURVE_CRITICAL_301 = 2301,
    AE_GROW_CURVE_CRITICAL_302 = 2302,
    AE_GROW_CURVE_CRITICAL_303 = 2303,
    AE_GROW_CURVE_CRITICAL_304 = 2304,
    AE_GROW_CURVE_CRITICAL_305 = 2305
} ae_grow_curve_t;

typedef enum ae_server_buff_type {
    AE_SERVER_BUFF_NONE = 0,
    AE_SERVER_BUFF_AVATAR = 1,
    AE_SERVER_BUFF_TEAM = 2,
    AE_SERVER_BUFF_TOWER = 3
} ae_server_buff_type_t;

typedef enum ae_item_use_op {
    AE_ITEM_USE_NONE = 0,
    AE_ITEM_USE_ACCEPT_QUEST = 1,
    AE_ITEM_USE_TRIGGER_ABILITY = 2,
    AE_ITEM_USE_GAIN_AVATAR = 3,
    AE_ITEM_USE_ADD_EXP = 4,
    AE_ITEM_USE_RELIVE_AVATAR = 5,
    AE_ITEM_USE_ADD_BIG_TALENT_POINT = 6,
    AE_ITEM_USE_ADD_PERSIST_STAMINA = 7,
    AE_ITEM_USE_ADD_TEMPORARY_STAMINA = 8,
    AE_ITEM_USE_ADD_CUR_STAMINA = 9,
    AE_ITEM_USE_ADD_CUR_HP = 10,
    AE_ITEM_USE_ADD_ELEM_ENERGY = 11,
    AE_ITEM_USE_ADD_ALL_ENERGY = 12,
    AE_ITEM_USE_ADD_DUNGEON_COND_TIME = 13,
    AE_ITEM_USE_ADD_WEAPON_EXP = 14,
    AE_ITEM_USE_ADD_SERVER_BUFF = 15,
    AE_ITEM_USE_DEL_SERVER_BUFF = 16,
    AE_ITEM_USE_UNLOCK_COOK_RECIPE = 17,
    AE_ITEM_USE_OPEN_RANDOM_CHEST = 20,
    AE_ITEM_USE_MAKE_GADGET = 24,
    AE_ITEM_USE_ADD_ITEM = 25,
    AE_ITEM_USE_GRANT_SELECT_REWARD = 26,
    AE_ITEM_USE_ADD_SELECT_ITEM = 27,
    AE_ITEM_USE_GAIN_FLYCLOAK = 28,
    AE_ITEM_USE_GAIN_NAME_CARD = 29,
    AE_ITEM_USE_UNLOCK_PAID_BATTLE_PASS_NORMAL = 30,
    AE_ITEM_USE_GAIN_CARD_PRODUCT = 31,
    AE_ITEM_USE_UNLOCK_FORGE = 32,
    AE_ITEM_USE_UNLOCK_COMBINE = 33,
    AE_ITEM_USE_UNLOCK_CODEX = 34,
    AE_ITEM_USE_CHEST_SELECT_ITEM = 35,
    AE_ITEM_USE_GAIN_RESIN_CARD_PRODUCT = 36,
    AE_ITEM_USE_ADD_RELIQUARY_EXP = 37,
    AE_ITEM_USE_UNLOCK_FURNITURE_FORMULA = 38,
    AE_ITEM_USE_UNLOCK_FURNITURE_SUITE = 39,
    AE_ITEM_USE_ADD_CHANNELLER_SLAB_BUFF = 40,
    AE_ITEM_USE_GAIN_COSTUME = 41,
    AE_ITEM_USE_ADD_TREASURE_MAP_BONUS_REGION_FRAGMENT = 42,
    AE_ITEM_USE_COMBINE_ITEM = 43,
    AE_ITEM_USE_UNLOCK_HOME_MODULE = 44,
    AE_ITEM_USE_UNLOCK_HOME_BGM = 45,
    AE_ITEM_USE_ADD_REGIONAL_PLAY_VAR = 46
} ae_item_use_op_t;

typedef enum ae_item_use_target {
    AE_ITEM_USE_TARGET_NONE = 0,
    AE_ITEM_USE_TARGET_CUR_AVATAR = 1,
    AE_ITEM_USE_TARGET_CUR_TEAM = 2,
    AE_ITEM_USE_TARGET_SPECIFY_AVATAR = 3,
    AE_ITEM_USE_TARGET_SPECIFY_ALIVE_AVATAR = 4,
    AE_ITEM_USE_TARGET_SPECIFY_DEAD_AVATAR = 5
} ae_item_use_target_t;

/* Refresh interval type for events and daily accounts. */
typedef enum ae_refresh_type {
    AE_REFRESH_NONE = 0,
    AE_REFRESH_INTERVAL = 1,
    AE_REFRESH_DAILY = 2,
    AE_REFRESH_WEEKLY = 3,
    AE_REFRESH_DAY_BEGIN_INTERVAL = 4
} ae_refresh_type_t;

typedef enum ae_activity_type {
    AE_ACTIVITY_NONE = 0,
    AE_ACTIVITY_TRIAL_AVATAR = 4,
    AE_ACTIVITY_PERSONAL_LINE = 8,
    AE_ACTIVITY_SALESMAN_MP = 1205,
    AE_ACTIVITY_SUMMER_TIME = 1600,
    AE_ACTIVITY_GENERAL_BANNER = 2100,
    AE_ACTIVITY_MUSIC_GAME = 2202,
    AE_ACTIVITY_PHOTO = 2603,
    AE_ACTIVITY_FUNGUS_FIGHTER = 3201,
    AE_ACTIVITY_EFFIGY_CHALLENGE_V2 = 3203
} ae_activity_type_t;

typedef enum ae_camp_target_type {
    AE_CAMP_TARGET_NONE = 0,
    AE_CAMP_TARGET_ALLIANCE = 1,
    AE_CAMP_TARGET_ENEMY = 2,
    AE_CAMP_TARGET_SELF = 3,
    AE_CAMP_TARGET_SELF_CAMP = 4,
    AE_CAMP_TARGET_ALL = 5,
    AE_CAMP_TARGET_ALL_EXCEPT_SELF = 6,
    AE_CAMP_TARGET_ALLIANCE_INCLUDE_SELF = 7
} ae_camp_target_type_t;

typedef enum ae_battle_pass_refresh_type {
    AE_BATTLE_PASS_REFRESH_DAILY = 0,
    AE_BATTLE_PASS_REFRESH_CYCLE_CROSS_SCHEDULE = 1,
    AE_BATTLE_PASS_REFRESH_SCHEDULE = 2,
    AE_BATTLE_PASS_REFRESH_CYCLE = 1
} ae_battle_pass_refresh_type_t;

typedef enum ae_battle_pass_mission_status {
    AE_BATTLE_PASS_MISSION_INVALID = 0,
    AE_BATTLE_PASS_MISSION_UNFINISHED = 1,
    AE_BATTLE_PASS_MISSION_FINISHED = 2,
    AE_BATTLE_PASS_MISSION_POINT_TAKEN = 3
} ae_battle_pass_mission_status_t;

/* Lookup helpers for the simple tag enums above. */
ae_item_type_t ae_item_type_from_value(ae_s32 value);
ae_item_type_t ae_item_type_from_name(const char *name);
ae_s32 ae_item_type_value(ae_item_type_t type);

ae_material_type_t ae_material_type_from_value(ae_s32 value);
ae_material_type_t ae_material_type_from_name(const char *name);
ae_s32 ae_material_type_value(ae_material_type_t type);

ae_equip_type_t ae_equip_type_from_value(ae_s32 value);
ae_equip_type_t ae_equip_type_from_name(const char *name);
ae_s32 ae_equip_type_value(ae_equip_type_t type);

ae_item_quality_t ae_item_quality_from_value(ae_s32 value);
ae_item_quality_t ae_item_quality_from_name(const char *name);
ae_s32 ae_item_quality_value(ae_item_quality_t type);

ae_entity_type_t ae_entity_type_from_value(ae_s32 value);
ae_entity_type_t ae_entity_type_from_name(const char *name);
ae_s32 ae_entity_type_value(ae_entity_type_t type);

/* Maps a packed entity id type nibble to the matching entity type. */
ae_entity_type_t ae_entity_type_from_id_type(ae_s32 id_type);

ae_weapon_type_t ae_weapon_type_from_value(ae_s32 value);
ae_weapon_type_t ae_weapon_type_from_name(const char *name);
ae_s32 ae_weapon_type_value(ae_weapon_type_t type);

ae_scene_type_t ae_scene_type_from_value(ae_s32 value);
ae_monster_type_t ae_monster_type_from_value(ae_s32 value);
ae_life_state_t ae_life_state_from_value(ae_s32 value);
ae_climate_type_t ae_climate_type_from_value(ae_s32 value);
ae_fetter_state_t ae_fetter_state_from_value(ae_s32 value);
ae_enter_reason_t ae_enter_reason_from_value(ae_s32 value);
ae_grow_curve_t ae_grow_curve_from_value(ae_s32 value);
ae_server_buff_type_t ae_server_buff_type_from_value(ae_s32 value);
ae_item_use_op_t ae_item_use_op_from_value(ae_s32 value);
ae_item_use_op_t ae_item_use_op_from_name(const char *name);
ae_item_use_target_t ae_item_use_target_from_value(ae_s32 value);
ae_item_use_target_t ae_item_use_target_from_name(const char *name);
ae_refresh_type_t ae_refresh_type_from_value(ae_s32 value);
ae_activity_type_t ae_activity_type_from_value(ae_s32 value);
ae_camp_target_type_t ae_camp_target_type_from_value(ae_s32 value);
ae_battle_pass_refresh_type_t
ae_battle_pass_refresh_type_from_value(ae_s32 value);
ae_battle_pass_mission_status_t
ae_battle_pass_mission_status_from_value(ae_s32 value);

/* ------------------------------------------------------------------ */
/* Utility hashing                                                      */
/* ------------------------------------------------------------------ */

/*
 * The "ability hash" used by the game data: a 32-bit rolling hash.
 * Matches the client-side ConfigHash computation for ability names.
 */
ae_s32 ae_ability_hash(const char *name);

/* ============================================================ */

#endif /* AE_PROPS_H */