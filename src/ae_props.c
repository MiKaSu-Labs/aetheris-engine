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
 * @file ae_props.c
 *
 * @brief Property and type enum lookup tables.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <aetheris/ae_props.h>

#include <string.h>

/* ============================================================ */

/*
 * All tag enums share the same sparse value->member mapping.  The
 * tables are declared once here and referenced by the per-enum
 * lookup helpers below.
 */

typedef struct ae_prop_table {
    ae_s32 value;
    const char *name;
} ae_prop_table_t;

/* Linear search returning the offset of the first match, or -1. */
static ae_s32 prop_table_value(const ae_prop_table_t *table,
                               ae_size count, ae_s32 value) {
    for (ae_size i = 0; i < count; i++) {
        if (table[i].value == value) {
            return (ae_s32)i;
        }
    }
    return -1;
}

/* Exact, case-sensitive name lookup. */
static ae_s32 prop_table_name(const ae_prop_table_t *table,
                              ae_size count, const char *name) {
    if (name == NULL) {
        return -1;
    }
    for (ae_size i = 0; i < count; i++) {
        if (table[i].name != NULL && strcmp(table[i].name, name) == 0) {
            return (ae_s32)i;
        }
    }
    return -1;
}

/* ------------------------------------------------------------------ */
/* FightProperty                                                       */
/* ------------------------------------------------------------------ */

static const ae_prop_table_t fight_prop_table[] = {
    {0, "FIGHT_PROP_NONE"},
    {1, "FIGHT_PROP_BASE_HP"},
    {2, "FIGHT_PROP_HP"},
    {3, "FIGHT_PROP_HP_PERCENT"},
    {4, "FIGHT_PROP_BASE_ATTACK"},
    {5, "FIGHT_PROP_ATTACK"},
    {6, "FIGHT_PROP_ATTACK_PERCENT"},
    {7, "FIGHT_PROP_BASE_DEFENSE"},
    {8, "FIGHT_PROP_DEFENSE"},
    {9, "FIGHT_PROP_DEFENSE_PERCENT"},
    {10, "FIGHT_PROP_BASE_SPEED"},
    {11, "FIGHT_PROP_SPEED_PERCENT"},
    {12, "FIGHT_PROP_HP_MP_PERCENT"},
    {13, "FIGHT_PROP_ATTACK_MP_PERCENT"},
    {20, "FIGHT_PROP_CRITICAL"},
    {21, "FIGHT_PROP_ANTI_CRITICAL"},
    {22, "FIGHT_PROP_CRITICAL_HURT"},
    {23, "FIGHT_PROP_CHARGE_EFFICIENCY"},
    {24, "FIGHT_PROP_ADD_HURT"},
    {25, "FIGHT_PROP_SUB_HURT"},
    {26, "FIGHT_PROP_HEAL_ADD"},
    {27, "FIGHT_PROP_HEALED_ADD"},
    {28, "FIGHT_PROP_ELEMENT_MASTERY"},
    {29, "FIGHT_PROP_PHYSICAL_SUB_HURT"},
    {30, "FIGHT_PROP_PHYSICAL_ADD_HURT"},
    {31, "FIGHT_PROP_DEFENCE_IGNORE_RATIO"},
    {32, "FIGHT_PROP_DEFENCE_IGNORE_DELTA"},
    {40, "FIGHT_PROP_FIRE_ADD_HURT"},
    {41, "FIGHT_PROP_ELEC_ADD_HURT"},
    {42, "FIGHT_PROP_WATER_ADD_HURT"},
    {43, "FIGHT_PROP_GRASS_ADD_HURT"},
    {44, "FIGHT_PROP_WIND_ADD_HURT"},
    {45, "FIGHT_PROP_ROCK_ADD_HURT"},
    {46, "FIGHT_PROP_ICE_ADD_HURT"},
    {47, "FIGHT_PROP_HIT_HEAD_ADD_HURT"},
    {50, "FIGHT_PROP_FIRE_SUB_HURT"},
    {51, "FIGHT_PROP_ELEC_SUB_HURT"},
    {52, "FIGHT_PROP_WATER_SUB_HURT"},
    {53, "FIGHT_PROP_GRASS_SUB_HURT"},
    {54, "FIGHT_PROP_WIND_SUB_HURT"},
    {55, "FIGHT_PROP_ROCK_SUB_HURT"},
    {56, "FIGHT_PROP_ICE_SUB_HURT"},
    {60, "FIGHT_PROP_EFFECT_HIT"},
    {61, "FIGHT_PROP_EFFECT_RESIST"},
    {62, "FIGHT_PROP_FREEZE_RESIST"},
    {63, "FIGHT_PROP_TORPOR_RESIST"},
    {64, "FIGHT_PROP_DIZZY_RESIST"},
    {65, "FIGHT_PROP_FREEZE_SHORTEN"},
    {66, "FIGHT_PROP_TORPOR_SHORTEN"},
    {67, "FIGHT_PROP_DIZZY_SHORTEN"},
    {70, "FIGHT_PROP_MAX_FIRE_ENERGY"},
    {71, "FIGHT_PROP_MAX_ELEC_ENERGY"},
    {72, "FIGHT_PROP_MAX_WATER_ENERGY"},
    {73, "FIGHT_PROP_MAX_GRASS_ENERGY"},
    {74, "FIGHT_PROP_MAX_WIND_ENERGY"},
    {75, "FIGHT_PROP_MAX_ICE_ENERGY"},
    {76, "FIGHT_PROP_MAX_ROCK_ENERGY"},
    {80, "FIGHT_PROP_SKILL_CD_MINUS_RATIO"},
    {81, "FIGHT_PROP_SHIELD_COST_MINUS_RATIO"},
    {1000, "FIGHT_PROP_CUR_FIRE_ENERGY"},
    {1001, "FIGHT_PROP_CUR_ELEC_ENERGY"},
    {1002, "FIGHT_PROP_CUR_WATER_ENERGY"},
    {1003, "FIGHT_PROP_CUR_GRASS_ENERGY"},
    {1004, "FIGHT_PROP_CUR_WIND_ENERGY"},
    {1005, "FIGHT_PROP_CUR_ICE_ENERGY"},
    {1006, "FIGHT_PROP_CUR_ROCK_ENERGY"},
    {1010, "FIGHT_PROP_CUR_HP"},
    {2000, "FIGHT_PROP_MAX_HP"},
    {2001, "FIGHT_PROP_CUR_ATTACK"},
    {2002, "FIGHT_PROP_CUR_DEFENSE"},
    {2003, "FIGHT_PROP_CUR_SPEED"},
    {3000, "FIGHT_PROP_NONEXTRA_ATTACK"},
    {3001, "FIGHT_PROP_NONEXTRA_DEFENSE"},
    {3002, "FIGHT_PROP_NONEXTRA_CRITICAL"},
    {3003, "FIGHT_PROP_NONEXTRA_ANTI_CRITICAL"},
    {3004, "FIGHT_PROP_NONEXTRA_CRITICAL_HURT"},
    {3005, "FIGHT_PROP_NONEXTRA_CHARGE_EFFICIENCY"},
    {3006, "FIGHT_PROP_NONEXTRA_ELEMENT_MASTERY"},
    {3007, "FIGHT_PROP_NONEXTRA_PHYSICAL_SUB_HURT"},
    {3008, "FIGHT_PROP_NONEXTRA_FIRE_ADD_HURT"},
    {3009, "FIGHT_PROP_NONEXTRA_ELEC_ADD_HURT"},
    {3010, "FIGHT_PROP_NONEXTRA_WATER_ADD_HURT"},
    {3011, "FIGHT_PROP_NONEXTRA_GRASS_ADD_HURT"},
    {3012, "FIGHT_PROP_NONEXTRA_WIND_ADD_HURT"},
    {3013, "FIGHT_PROP_NONEXTRA_ROCK_ADD_HURT"},
    {3014, "FIGHT_PROP_NONEXTRA_ICE_ADD_HURT"},
    {3015, "FIGHT_PROP_NONEXTRA_FIRE_SUB_HURT"},
    {3016, "FIGHT_PROP_NONEXTRA_ELEC_SUB_HURT"},
    {3017, "FIGHT_PROP_NONEXTRA_WATER_SUB_HURT"},
    {3018, "FIGHT_PROP_NONEXTRA_GRASS_SUB_HURT"},
    {3019, "FIGHT_PROP_NONEXTRA_WIND_SUB_HURT"},
    {3020, "FIGHT_PROP_NONEXTRA_ROCK_SUB_HURT"},
    {3021, "FIGHT_PROP_NONEXTRA_ICE_SUB_HURT"},
    {3022, "FIGHT_PROP_NONEXTRA_SKILL_CD_MINUS_RATIO"},
    {3023, "FIGHT_PROP_NONEXTRA_SHIELD_COST_MINUS_RATIO"},
    {3024, "FIGHT_PROP_NONEXTRA_PHYSICAL_ADD_HURT"}};

/* Short aliases used by relic/command tooling (value, name). */
static const ae_prop_table_t fight_prop_short_table[] = {
    {2, "hp"},      {5, "atk"},       {8, "def"},
    {3, "hp%"},     {6, "atk%"},      {9, "def%"},
    {28, "em"},     {23, "er"},       {26, "hb"},
    {26, "heal"},   {22, "cd"},       {22, "cdmg"},
    {20, "cr"},     {20, "crate"},    {30, "phys%"},
    {43, "dendro%"}, {45, "geo%"},     {44, "anemo%"},
    {42, "hydro%"}, {46, "cryo%"},    {41, "electro%"},
    {40, "pyro%"},  {2000, "maxhp"},  {24, "dmg"},
    {80, "cdr"},    {27, "heali"},    {81, "shield"},
    {31, "defi"},   {25, "resall"},   {54, "resanemo"},
    {56, "rescryo"},{53, "resdendro"},{51, "reselectro"},
    {55, "resgeo"}, {52, "reshydro"}, {50, "respyro"},
    {29, "resphys"}};

/* Flat (non-percentage) properties. */
static const ae_s32 flat_props[] = {1,  2,    4,    5,    7,    8,
                                    27, 1000, 1001, 1002, 1003, 1004,
                                    1005, 1006, 1010, 2000, 2001, 2002};

/* Compound property formulas for cur HP/ATK/DEF. */
static const ae_fight_compound_prop_t compound_properties[] = {
    {2000, 1, 3, 2},   /* MAX_HP      = BASE_HP      * (1 + HP%)      + HP */
    {2001, 4, 6, 5},   /* CUR_ATTACK  = BASE_ATTACK  * (1 + ATK%)     + ATK */
    {2002, 7, 9, 8}};  /* CUR_DEFENSE = BASE_DEFENSE * (1 + DEF%)     + DEF */

ae_fight_prop_t ae_fight_prop_from_id(ae_s32 id) {
    ae_s32 idx = prop_table_value(fight_prop_table,
                                  sizeof(fight_prop_table) /
                                      sizeof(fight_prop_table[0]),
                                  id);
    if (idx < 0) {
        return AE_FIGHT_PROP_NONE;
    }
    return (ae_fight_prop_t)fight_prop_table[idx].value;
}

ae_fight_prop_t ae_fight_prop_from_name(const char *name) {
    ae_s32 idx = prop_table_name(fight_prop_table,
                                 sizeof(fight_prop_table) /
                                     sizeof(fight_prop_table[0]),
                                 name);
    if (idx < 0) {
        return AE_FIGHT_PROP_NONE;
    }
    return (ae_fight_prop_t)fight_prop_table[idx].value;
}

ae_fight_prop_t ae_fight_prop_from_short_name(const char *name) {
    ae_s32 idx = prop_table_name(fight_prop_short_table,
                                 sizeof(fight_prop_short_table) /
                                     sizeof(fight_prop_short_table[0]),
                                 name);
    if (idx < 0) {
        return AE_FIGHT_PROP_NONE;
    }
    return (ae_fight_prop_t)fight_prop_short_table[idx].value;
}

ae_s32 ae_fight_prop_id(ae_fight_prop_t prop) {
    return (ae_s32)prop;
}

ae_bool ae_fight_prop_is_percentage(ae_fight_prop_t prop) {
    ae_s32 id = (ae_s32)prop;
    for (ae_size i = 0; i < sizeof(flat_props) / sizeof(flat_props[0]); i++) {
        if (flat_props[i] == id) {
            return false;
        }
    }
    return true;
}

ae_bool ae_fight_prop_compound_get(ae_fight_prop_t prop,
                                   ae_fight_compound_prop_t *out) {
    if (out == NULL) {
        return false;
    }
    ae_s32 id = (ae_s32)prop;
    for (ae_size i = 0;
         i < sizeof(compound_properties) / sizeof(compound_properties[0]);
         i++) {
        if ((ae_s32)compound_properties[i].result == id) {
            *out = compound_properties[i];
            return true;
        }
    }
    return false;
}

/* ------------------------------------------------------------------ */
/* ElementType                                                         */
/* ------------------------------------------------------------------ */

static const ae_prop_table_t elem_table[] = {
    {0, "None"},     {1, "Fire"},       {2, "Water"}, {3, "Grass"},
    {4, "Electric"}, {5, "Ice"},        {6, "Frozen"}, {7, "Wind"},
    {8, "Rock"},     {9, "AntiFire"},   {255, "Default"}};

/*
 * Element metadata keyed by element value.  Fields:
 *   cur_energy_prop, max_energy_prop, team_resonance_id, config_name,
 *   depot_index, (config_hash computed on demand from config_name)
 * Indexes align with AE_ELEM_* values except 255 which is last.
 */
static const ae_elem_info_t elem_info_table[] = {
    {1004, 74, 0, NULL, 0, 0},
    {1000, 70, 10101, "TeamResonance_Fire_Lv2", 1, 0},
    {1001, 71, 10201, "TeamResonance_Water_Lv2", 2, 0},
    {1002, 72, 10501, "TeamResonance_Grass_Lv2", 7, 0},
    {1003, 73, 10401, "TeamResonance_Electric_Lv2", 6, 0},
    {1005, 75, 10601, "TeamResonance_Ice_Lv2", 4, 0},
    {1005, 75, 0, NULL, 0, 0},
    {1004, 74, 10301, "TeamResonance_Wind_Lv2", 3, 0},
    {1006, 76, 10701, "TeamResonance_Rock_Lv2", 5, 0},
    {1000, 70, 0, NULL, 0, 0},
    {1000, 70, 10801, "TeamResonance_AllDifferent", 1, 0}};

typedef struct ae_elem_index {
    ae_s32 value;
    ae_size index;
} ae_elem_index_t;

static const ae_elem_index_t elem_index_table[] = {
    {0, 0},   {1, 1},  {2, 2}, {3, 3},   {4, 4},  {5, 5},
    {6, 6},   {7, 7},  {8, 8}, {9, 9},   {255, 10}};

ae_elem_t ae_elem_from_value(ae_s32 value) {
    ae_s32 idx = prop_table_value(elem_table,
                                  sizeof(elem_table) / sizeof(elem_table[0]),
                                  value);
    if (idx < 0) {
        return AE_ELEM_NONE;
    }
    return (ae_elem_t)elem_table[idx].value;
}

ae_elem_t ae_elem_from_name(const char *name) {
    ae_s32 idx = prop_table_name(elem_table,
                                 sizeof(elem_table) / sizeof(elem_table[0]),
                                 name);
    if (idx < 0) {
        return AE_ELEM_NONE;
    }
    return (ae_elem_t)elem_table[idx].value;
}

ae_s32 ae_elem_value(ae_elem_t elem) {
    return (ae_s32)elem;
}

ae_bool ae_elem_info_get(ae_elem_t elem, ae_elem_info_t *info) {
    if (info == NULL) {
        return false;
    }
    ae_s32 val = (ae_s32)elem;
    for (ae_size i = 0;
         i < sizeof(elem_index_table) / sizeof(elem_index_table[0]); i++) {
        if (elem_index_table[i].value == val) {
            ae_size table_index = elem_index_table[i].index;
            *info = elem_info_table[table_index];
            info->config_hash = info->config_name != NULL
                                    ? ae_ability_hash(info->config_name)
                                    : 0;
            return true;
        }
    }
    info->cur_energy_prop = 0;
    info->max_energy_prop = 0;
    info->team_resonance_id = 0;
    info->config_name = NULL;
    info->depot_index = 0;
    info->config_hash = 0;
    return false;
}

/* ------------------------------------------------------------------ */
/* PlayerProperty                                                      */
/* ------------------------------------------------------------------ */

/* min/max bounds with INT32_MIN/MAX encoding an unbounded slider. */
typedef struct ae_player_prop_row {
    ae_s32 id;
    ae_s32 min;
    ae_s32 max;
    ae_bool dynamic;
} ae_player_prop_row_t;

static const ae_player_prop_row_t player_prop_table[] = {
    {0, INT32_MIN, INT32_MAX, false},
    {1001, 0, INT32_MAX, false},           /* EXP */
    {1002, INT32_MIN, INT32_MAX, false},
    {1003, INT32_MIN, INT32_MAX, false},
    {1004, INT32_MIN, INT32_MAX, false},
    {4001, 0, 90, false},                  /* LEVEL */
    {10001, INT32_MIN, INT32_MAX, false},
    {10002, 0, 8500000, false},            /* MAX_SPRING_VOLUME */
    {10003, INT32_MIN, INT32_MAX, true},   /* CUR_SPRING_VOLUME (dyn) */
    {10004, 0, 1, false},
    {10005, 0, 100, false},
    {10006, 0, 1, false},
    {10007, 0, 1, false},
    {10008, 0, 1, false},
    {10009, 0, 1, false},
    {10010, 0, 24000, false},              /* MAX_STAMINA */
    {10011, INT32_MIN, INT32_MAX, true},   /* CUR_PERSIST_STAMINA (dyn) */
    {10012, INT32_MIN, INT32_MAX, false},
    {10013, 1, 60, false},                 /* PLAYER_LEVEL */
    {10014, 0, INT32_MAX, false},          /* PLAYER_EXP */
    {10015, INT32_MIN, INT32_MAX, false},  /* HCOIN (may be negative) */
    {10016, 0, INT32_MAX, false},          /* SCOIN */
    {10017, 0, 2, false},                  /* MP_SETTING_TYPE */
    {10018, 0, 1, false},
    {10019, 0, 8, false},                  /* WORLD_LEVEL */
    {10020, 0, 2000, false},               /* RESIN */
    {10022, INT32_MIN, INT32_MAX, false},
    {10023, INT32_MIN, INT32_MAX, false},
    {10024, 0, 1, false},
    {10025, INT32_MIN, INT32_MAX, false},  /* MCOIN (may be negative) */
    {10026, INT32_MIN, INT32_MAX, false},
    {10027, 0, INT32_MAX, false},          /* LEGENDARY_KEY */
    {10028, INT32_MIN, INT32_MAX, false},
    {10029, 0, 300000, false},             /* FORGE_POINT */
    {10035, INT32_MIN, INT32_MAX, false},
    {10036, INT32_MIN, INT32_MAX, false},
    {10037, INT32_MIN, INT32_MAX, false},
    {10038, INT32_MIN, INT32_MAX, false},
    {10039, 0, 8, false},                  /* WORLD_LEVEL_LIMIT */
    {10040, INT32_MIN, INT32_MAX, false},
    {10041, INT32_MIN, INT32_MAX, false},
    {10042, 0, INT32_MAX, false},          /* HOME_COIN */
    {10043, INT32_MIN, INT32_MAX, false},
    {10044, INT32_MIN, INT32_MAX, false},
    {10045, INT32_MIN, INT32_MAX, false},
    {10046, INT32_MIN, INT32_MAX, false},
    {10047, INT32_MIN, INT32_MAX, false},
    {10048, 0, 1, false},
    {10049, 0, 10000, false},              /* DIVE_MAX_STAMINA */
    {10050, 0, 10000, false}};             /* DIVE_CUR_STAMINA */

ae_player_prop_t ae_player_prop_from_id(ae_s32 id) {
    for (ae_size i = 0;
         i < sizeof(player_prop_table) / sizeof(player_prop_table[0]); i++) {
        if (player_prop_table[i].id == id) {
            return (ae_player_prop_t)id;
        }
    }
    return AE_PLAYER_PROP_NONE;
}

ae_s32 ae_player_prop_id(ae_player_prop_t prop) {
    return (ae_s32)prop;
}

ae_bool ae_player_prop_range_get(ae_player_prop_t prop,
                                 ae_player_prop_range_t *range) {
    if (range == NULL) {
        return false;
    }
    ae_s32 id = (ae_s32)prop;
    for (ae_size i = 0;
         i < sizeof(player_prop_table) / sizeof(player_prop_table[0]); i++) {
        if (player_prop_table[i].id == id) {
            range->min = player_prop_table[i].min;
            range->max = player_prop_table[i].max;
            range->dynamic = player_prop_table[i].dynamic;
            return true;
        }
    }
    range->min = INT32_MIN;
    range->max = INT32_MAX;
    range->dynamic = false;
    return false;
}

/* ------------------------------------------------------------------ */
/* Simple tag enums                                                    */
/* ------------------------------------------------------------------ */

static const ae_prop_table_t item_type_table[] = {
    {0, "ITEM_NONE"},     {1, "ITEM_VIRTUAL"},  {2, "ITEM_MATERIAL"},
    {3, "ITEM_RELIQUARY"},{4, "ITEM_WEAPON"},   {5, "ITEM_DISPLAY"},
    {6, "ITEM_FURNITURE"}};

static const ae_prop_table_t material_type_table[] = {
    {0, "MATERIAL_NONE"},
    {1, "MATERIAL_FOOD"},
    {2, "MATERIAL_QUEST"},
    {4, "MATERIAL_EXCHANGE"},
    {5, "MATERIAL_CONSUME"},
    {6, "MATERIAL_EXP_FRUIT"},
    {7, "MATERIAL_AVATAR"},
    {8, "MATERIAL_ADSORBATE"},
    {9, "MATERIAL_CRICKET"},
    {10, "MATERIAL_ELEM_CRYSTAL"},
    {11, "MATERIAL_WEAPON_EXP_STONE"},
    {12, "MATERIAL_CHEST"},
    {13, "MATERIAL_RELIQUARY_MATERIAL"},
    {14, "MATERIAL_AVATAR_MATERIAL"},
    {15, "MATERIAL_NOTICE_ADD_HP"},
    {16, "MATERIAL_SEA_LAMP"},
    {17, "MATERIAL_SELECTABLE_CHEST"},
    {18, "MATERIAL_FLYCLOAK"},
    {19, "MATERIAL_NAMECARD"},
    {20, "MATERIAL_TALENT"},
    {21, "MATERIAL_WIDGET"},
    {22, "MATERIAL_CHEST_BATCH_USE"},
    {23, "MATERIAL_FAKE_ABSORBATE"},
    {24, "MATERIAL_CONSUME_BATCH_USE"},
    {25, "MATERIAL_WOOD"},
    {27, "MATERIAL_FURNITURE_FORMULA"},
    {28, "MATERIAL_CHANNELLER_SLAB_BUFF"},
    {29, "MATERIAL_FURNITURE_SUITE_FORMULA"},
    {30, "MATERIAL_COSTUME"},
    {31, "MATERIAL_HOME_SEED"},
    {32, "MATERIAL_FISH_BAIT"},
    {33, "MATERIAL_FISH_ROD"},
    {34, "MATERIAL_SUMO_BUFF"},
    {35, "MATERIAL_FIREWORKS"},
    {36, "MATERIAL_BGM"},
    {37, "MATERIAL_SPICE_FOOD"},
    {38, "MATERIAL_ACTIVITY_ROBOT"},
    {39, "MATERIAL_ACTIVITY_GEAR"},
    {40, "MATERIAL_ACTIVITY_JIGSAW"},
    {41, "MATERIAL_ARANARA"},
    {46, "MATERIAL_DESHRET_MANUAL"}};

static const ae_prop_table_t equip_type_table[] = {
    {0, "EQUIP_NONE"},   {1, "EQUIP_BRACER"},  {2, "EQUIP_NECKLACE"},
    {3, "EQUIP_SHOES"},  {4, "EQUIP_RING"},    {5, "EQUIP_DRESS"},
    {6, "EQUIP_WEAPON"}};

static const ae_prop_table_t item_quality_table[] = {
    {0, "QUALITY_NONE"},    {1, "QUALITY_WHITE"},  {2, "QUALITY_GREEN"},
    {3, "QUALITY_BLUE"},    {4, "QUALITY_PURPLE"}, {5, "QUALITY_ORANGE"},
    {105, "QUALITY_ORANGE_SP"}};

static const ae_prop_table_t entity_type_table[] = {
    {0, "None"},             {1, "Avatar"},       {2, "Monster"},
    {3, "Bullet"},           {4, "AttackPhyisicalUnit"}, {5, "AOE"},
    {6, "Camera"},           {7, "EnviroArea"},   {8, "Equip"},
    {9, "MonsterEquip"},     {10, "Grass"},       {11, "Level"},
    {12, "NPC"},             {13, "TransPointFirst"}, {14, "TransPointFirstGadget"},
    {15, "TransPointSecond"},{16, "TransPointSecondGadget"}, {17, "DropItem"},
    {18, "Field"},           {19, "Gadget"},      {20, "Water"},
    {21, "GatherPoint"},     {22, "GatherObject"},{23, "AirflowField"},
    {24, "SpeedupField"},    {25, "Gear"},        {26, "Chest"},
    {27, "EnergyBall"},      {28, "ElemCrystal"}, {29, "Timeline"},
    {30, "Worktop"},         {31, "Team"},        {32, "Platform"},
    {33, "AmberWind"},       {34, "EnvAnimal"},   {35, "SealGadget"},
    {36, "Tree"},            {37, "Bush"},        {38, "QuestGadget"},
    {39, "Lightning"},       {40, "RewardPoint"}, {41, "RewardStatue"},
    {42, "MPLevel"},         {43, "WindSeed"},    {44, "MpPlayRewardPoint"},
    {45, "ViewPoint"},       {46, "RemoteAvatar"},{47, "GeneralRewardPoint"},
    {48, "PlayTeam"},        {49, "OfferingGadget"},{50, "EyePoint"},
    {51, "MiracleRing"},     {52, "Foundation"},  {53, "WidgetGadget"},
    {54, "Vehicle"},         {55, "SubEquip"},    {56, "FishRod"},
    {57, "CustomTile"},      {58, "FishPool"},    {59, "CustomGadget"},
    {60, "BlackMud"},        {61, "RoguelikeOperatorGadget"}, {62, "NightCrowGadget"},
    {63, "Projector"},       {64, "Screen"},      {65, "EchoShell"},
    {66, "UIInteractGadget"},{98, "Region"},      {99, "PlaceHolder"}};

/* EntityIdType id nibble -> canonical entity type (see lookup below). */

static const ae_prop_table_t weapon_type_table[] = {
    {0, "WEAPON_NONE"},        {1, "WEAPON_SWORD_ONE_HAND"},
    {2, "WEAPON_CROSSBOW"},    {3, "WEAPON_STAFF"},
    {4, "WEAPON_DOUBLE_DAGGER"},{5, "WEAPON_KATANA"},
    {6, "WEAPON_SHURIKEN"},    {7, "WEAPON_STICK"},
    {8, "WEAPON_SPEAR"},       {9, "WEAPON_SHIELD_SMALL"},
    {10, "WEAPON_CATALYST"},   {11, "WEAPON_CLAYMORE"},
    {12, "WEAPON_BOW"},        {13, "WEAPON_POLE"}};

static const ae_prop_table_t scene_type_table[] = {
    {0, "SCENE_NONE"},    {1, "SCENE_WORLD"},      {2, "SCENE_DUNGEON"},
    {3, "SCENE_ROOM"},    {4, "SCENE_HOME_WORLD"}, {5, "SCENE_HOME_ROOM"},
    {6, "SCENE_ACTIVITY"}};

static const ae_prop_table_t monster_type_table[] = {
    {0, "MONSTER_NONE"},    {1, "MONSTER_ORDINARY"},   {2, "MONSTER_BOSS"},
    {3, "MONSTER_ENV_ANIMAL"},{4, "MONSTER_LITTLE_MONSTER"},{5, "MONSTER_FISH"}};

static const ae_prop_table_t life_state_table[] = {
    {0, "LIFE_NONE"}, {1, "LIFE_ALIVE"}, {2, "LIFE_DEAD"}, {3, "LIFE_REVIVE"}};

static const ae_prop_table_t climate_type_table[] = {
    {0, "CLIMATE_NONE"},      {1, "CLIMATE_SUNNY"},
    {2, "CLIMATE_CLOUDY"},    {3, "CLIMATE_RAIN"},
    {4, "CLIMATE_THUNDERSTORM"},{5, "CLIMATE_SNOW"},
    {6, "CLIMATE_MIST"},      {7, "CLIMATE_DESERT"}};

static const ae_prop_table_t fetter_state_table[] = {
    {0, "NONE"}, {1, "OPEN"}, {3, "FINISH"}};

static const ae_prop_table_t enter_reason_table[] = {
    {0, "None"},             {1, "Login"},          {11, "DungeonReplay"},
    {12, "DungeonReviveOnWaypoint"}, {13, "DungeonEnter"}, {14, "DungeonQuit"},
    {21, "Gm"},              {31, "QuestRollback"}, {32, "Revival"},
    {41, "PersonalScene"},   {42, "TransPoint"},    {43, "ClientTransmit"},
    {44, "ForceDragBack"},   {51, "TeamKick"},      {52, "TeamJoin"},
    {53, "TeamBack"},        {54, "Muip"},          {55, "DungeonInviteAccept"},
    {56, "Lua"},             {57, "ActivityLoadTerrain"},
    {58, "HostFromSingleToMp"},{59, "MpPlay"},      {60, "AnchorPoint"},
    {61, "LuaSkipUi"},       {62, "ReloadTerrain"}, {63, "DraftTransfer"},
    {64, "EnterHome"},       {65, "ExitHome"},      {66, "ChangeHomeModule"},
    {67, "Gallery"},         {68, "HomeSceneJump"}, {69, "HideAndSeek"}};

static const ae_prop_table_t grow_curve_table[] = {
    {0, "GROW_CURVE_NONE"},
    {1, "GROW_CURVE_HP"},          {2, "GROW_CURVE_ATTACK"},
    {3, "GROW_CURVE_STAMINA"},     {4, "GROW_CURVE_STRIKE"},
    {5, "GROW_CURVE_ANTI_STRIKE"}, {6, "GROW_CURVE_ANTI_STRIKE1"},
    {7, "GROW_CURVE_ANTI_STRIKE2"},{8, "GROW_CURVE_ANTI_STRIKE3"},
    {9, "GROW_CURVE_STRIKE_HURT"}, {10, "GROW_CURVE_ELEMENT"},
    {11, "GROW_CURVE_KILL_EXP"},   {12, "GROW_CURVE_DEFENSE"},
    {13, "GROW_CURVE_ATTACK_BOMB"},{14, "GROW_CURVE_HP_LITTLEMONSTER"},
    {15, "GROW_CURVE_ELEMENT_MASTERY"},{16, "GROW_CURVE_PROGRESSION"},
    {17, "GROW_CURVE_DEFENDING"},  {18, "GROW_CURVE_MHP"},
    {19, "GROW_CURVE_MATK"},       {20, "GROW_CURVE_TOWERATK"},
    {21, "GROW_CURVE_HP_S5"},      {22, "GROW_CURVE_HP_S4"},
    {23, "GROW_CURVE_HP_2"},       {31, "GROW_CURVE_ATTACK_S5"},
    {32, "GROW_CURVE_ATTACK_S4"},  {33, "GROW_CURVE_ATTACK_S3"},
    {34, "GROW_CURVE_STRIKE_S5"},  {41, "GROW_CURVE_DEFENSE_S5"},
    {42, "GROW_CURVE_DEFENSE_S4"},
    {1101, "GROW_CURVE_ATTACK_101"},{1102, "GROW_CURVE_ATTACK_102"},
    {1103, "GROW_CURVE_ATTACK_103"},{1104, "GROW_CURVE_ATTACK_104"},
    {1105, "GROW_CURVE_ATTACK_105"},
    {1201, "GROW_CURVE_ATTACK_201"},{1202, "GROW_CURVE_ATTACK_202"},
    {1203, "GROW_CURVE_ATTACK_203"},{1204, "GROW_CURVE_ATTACK_204"},
    {1205, "GROW_CURVE_ATTACK_205"},
    {1301, "GROW_CURVE_ATTACK_301"},{1302, "GROW_CURVE_ATTACK_302"},
    {1303, "GROW_CURVE_ATTACK_303"},{1304, "GROW_CURVE_ATTACK_304"},
    {1305, "GROW_CURVE_ATTACK_305"},
    {2101, "GROW_CURVE_CRITICAL_101"},{2102, "GROW_CURVE_CRITICAL_102"},
    {2103, "GROW_CURVE_CRITICAL_103"},{2104, "GROW_CURVE_CRITICAL_104"},
    {2105, "GROW_CURVE_CRITICAL_105"},
    {2201, "GROW_CURVE_CRITICAL_201"},{2202, "GROW_CURVE_CRITICAL_202"},
    {2203, "GROW_CURVE_CRITICAL_203"},{2204, "GROW_CURVE_CRITICAL_204"},
    {2205, "GROW_CURVE_CRITICAL_205"},
    {2301, "GROW_CURVE_CRITICAL_301"},{2302, "GROW_CURVE_CRITICAL_302"},
    {2303, "GROW_CURVE_CRITICAL_303"},{2304, "GROW_CURVE_CRITICAL_304"},
    {2305, "GROW_CURVE_CRITICAL_305"}};

static const ae_prop_table_t server_buff_type_table[] = {
    {0, "SERVER_BUFF_NONE"},{1, "SERVER_BUFF_AVATAR"},
    {2, "SERVER_BUFF_TEAM"},{3, "SERVER_BUFF_TOWER"}};

static const ae_prop_table_t item_use_op_table[] = {
    {0, "ITEM_USE_NONE"}, {1, "ITEM_USE_ACCEPT_QUEST"},
    {2, "ITEM_USE_TRIGGER_ABILITY"},{3, "ITEM_USE_GAIN_AVATAR"},
    {4, "ITEM_USE_ADD_EXP"},{5, "ITEM_USE_RELIVE_AVATAR"},
    {6, "ITEM_USE_ADD_BIG_TALENT_POINT"},
    {7, "ITEM_USE_ADD_PERSIST_STAMINA"},
    {8, "ITEM_USE_ADD_TEMPORARY_STAMINA"},{9, "ITEM_USE_ADD_CUR_STAMINA"},
    {10, "ITEM_USE_ADD_CUR_HP"},{11, "ITEM_USE_ADD_ELEM_ENERGY"},
    {12, "ITEM_USE_ADD_ALL_ENERGY"},{13, "ITEM_USE_ADD_DUNGEON_COND_TIME"},
    {14, "ITEM_USE_ADD_WEAPON_EXP"},{15, "ITEM_USE_ADD_SERVER_BUFF"},
    {16, "ITEM_USE_DEL_SERVER_BUFF"},{17, "ITEM_USE_UNLOCK_COOK_RECIPE"},
    {20, "ITEM_USE_OPEN_RANDOM_CHEST"},{24, "ITEM_USE_MAKE_GADGET"},
    {25, "ITEM_USE_ADD_ITEM"},{26, "ITEM_USE_GRANT_SELECT_REWARD"},
    {27, "ITEM_USE_ADD_SELECT_ITEM"},{28, "ITEM_USE_GAIN_FLYCLOAK"},
    {29, "ITEM_USE_GAIN_NAME_CARD"},{30, "ITEM_USE_UNLOCK_PAID_BATTLE_PASS_NORMAL"},
    {31, "ITEM_USE_GAIN_CARD_PRODUCT"},{32, "ITEM_USE_UNLOCK_FORGE"},
    {33, "ITEM_USE_UNLOCK_COMBINE"},{34, "ITEM_USE_UNLOCK_CODEX"},
    {35, "ITEM_USE_CHEST_SELECT_ITEM"},{36, "ITEM_USE_GAIN_RESIN_CARD_PRODUCT"},
    {37, "ITEM_USE_ADD_RELIQUARY_EXP"},{38, "ITEM_USE_UNLOCK_FURNITURE_FORMULA"},
    {39, "ITEM_USE_UNLOCK_FURNITURE_SUITE"},
    {40, "ITEM_USE_ADD_CHANNELLER_SLAB_BUFF"},{41, "ITEM_USE_GAIN_COSTUME"},
    {42, "ITEM_USE_ADD_TREASURE_MAP_BONUS_REGION_FRAGMENT"},
    {43, "ITEM_USE_COMBINE_ITEM"},{44, "ITEM_USE_UNLOCK_HOME_MODULE"},
    {45, "ITEM_USE_UNLOCK_HOME_BGM"},{46, "ITEM_USE_ADD_REGIONAL_PLAY_VAR"}};

static const ae_prop_table_t item_use_target_table[] = {
    {0, "ITEM_USE_TARGET_NONE"},
    {1, "ITEM_USE_TARGET_CUR_AVATAR"},
    {2, "ITEM_USE_TARGET_CUR_TEAM"},
    {3, "ITEM_USE_TARGET_SPECIFY_AVATAR"},
    {4, "ITEM_USE_TARGET_SPECIFY_ALIVE_AVATAR"},
    {5, "ITEM_USE_TARGET_SPECIFY_DEAD_AVATAR"}};

static const ae_prop_table_t refresh_type_table[] = {
    {0, "REFRESH_NONE"},        {1, "REFRESH_INTERVAL"},
    {2, "REFRESH_DAILY"},       {3, "REFRESH_WEEKlY"},
    {4, "REFRESH_DAYBEGIN_INTERVAL"}};

static const ae_prop_table_t activity_type_table[] = {
    {0, "NONE"},
    {4, "NEW_ACTIVITY_TRIAL_AVATAR"},
    {8, "NEW_ACTIVITY_PERSONAL_LIINE"},
    {1205, "NEW_ACTIVITY_SALESMAN_MP"},
    {1600, "NEW_ACTIVITY_SUMMER_TIME"},
    {2100, "NEW_ACTIVITY_GENERAL_BANNER"},
    {2202, "NEW_ACTIVITY_MUSIC_GAME"},
    {2603, "NEW_ACTIVITY_PHOTO"},
    {3201, "NEW_ACTIVITY_FUNGUS_FIGHTER"},
    {3203, "NEW_ACTIVITY_EFFIGY_CHALLENGE_V2"}};

static const ae_prop_table_t camp_target_type_table[] = {
    {0, "None"}, {1, "Alliance"},       {2, "Enemy"},       {3, "Self"},
    {4, "SelfCamp"},{5, "All"},         {6, "AllExceptSelf"},
    {7, "AllianceIncludeSelf"}};

static const ae_prop_table_t battle_pass_refresh_type_table[] = {
    {0, "BATTLE_PASS_MISSION_REFRESH_DAILY"},
    {1, "BATTLE_PASS_MISSION_REFRESH_CYCLE_CROSS_SCHEDULE"},
    {2, "BATTLE_PASS_MISSION_REFRESH_SCHEDULE"}};

static const ae_prop_table_t battle_pass_mission_status_table[] = {
    {0, "MISSION_STATUS_INVALID"},
    {1, "MISSION_STATUS_UNFINISHED"},
    {2, "MISSION_STATUS_FINISHED"},
    {3, "MISSION_STATUS_POINT_TAKEN"}};

#define AE_PROP_TABLE_C(name)                                    \
    ((sizeof(name) / sizeof((name)[0])))

#define AE_DEFINE_VALUE_LOOKUP(ret_type, name, table)            \
    ret_type name(ae_s32 value) {                                \
        ae_s32 idx = prop_table_value(                           \
            table, AE_PROP_TABLE_C(table), value);               \
        if (idx < 0) {                                           \
            return (ret_type)0;                                  \
        }                                                        \
        return (ret_type)table[idx].value;                       \
    }

#define AE_DEFINE_NAMED_LOOKUP(ret_type, name, table)            \
    ret_type name(const char *name_str) {                        \
        ae_s32 idx = prop_table_name(                            \
            table, AE_PROP_TABLE_C(table), name_str);            \
        if (idx < 0) {                                           \
            return (ret_type)0;                                  \
        }                                                        \
        return (ret_type)table[idx].value;                       \
    }

#define AE_DEFINE_VALUE_GETTER(ret_type, name, table)            \
    ae_s32 name(ret_type value) {                                \
        return (ae_s32)value;                                    \
    }

AE_DEFINE_VALUE_LOOKUP(ae_item_type_t, ae_item_type_from_value,
                       item_type_table)
AE_DEFINE_NAMED_LOOKUP(ae_item_type_t, ae_item_type_from_name,
                       item_type_table)
AE_DEFINE_VALUE_GETTER(ae_item_type_t, ae_item_type_value,
                       item_type_table)

AE_DEFINE_VALUE_LOOKUP(ae_material_type_t, ae_material_type_from_value,
                       material_type_table)
AE_DEFINE_NAMED_LOOKUP(ae_material_type_t, ae_material_type_from_name,
                       material_type_table)
AE_DEFINE_VALUE_GETTER(ae_material_type_t, ae_material_type_value,
                       material_type_table)

AE_DEFINE_VALUE_LOOKUP(ae_equip_type_t, ae_equip_type_from_value,
                       equip_type_table)
AE_DEFINE_NAMED_LOOKUP(ae_equip_type_t, ae_equip_type_from_name,
                       equip_type_table)
AE_DEFINE_VALUE_GETTER(ae_equip_type_t, ae_equip_type_value,
                       equip_type_table)

AE_DEFINE_VALUE_LOOKUP(ae_item_quality_t, ae_item_quality_from_value,
                       item_quality_table)
AE_DEFINE_NAMED_LOOKUP(ae_item_quality_t, ae_item_quality_from_name,
                       item_quality_table)
AE_DEFINE_VALUE_GETTER(ae_item_quality_t, ae_item_quality_value,
                       item_quality_table)

AE_DEFINE_VALUE_LOOKUP(ae_entity_type_t, ae_entity_type_from_value,
                       entity_type_table)
AE_DEFINE_NAMED_LOOKUP(ae_entity_type_t, ae_entity_type_from_name,
                       entity_type_table)
AE_DEFINE_VALUE_GETTER(ae_entity_type_t, ae_entity_type_value,
                       entity_type_table)

/*
 * Maps the id nibble of an entity ("EntityIdType") to the matching
 * entity type, mirroring EntityIdType.toEntityType().
 */
ae_entity_type_t ae_entity_type_from_id_type(ae_s32 id_type) {
    ae_entity_type_t mapping = AE_ENTITY_NONE;
    switch (id_type) {
        case 0x01: mapping = AE_ENTITY_AVATAR; break;
        case 0x02: mapping = AE_ENTITY_MONSTER; break;
        case 0x03: mapping = AE_ENTITY_NPC; break;
        case 0x04: mapping = AE_ENTITY_GADGET; break;
        case 0x05: mapping = AE_ENTITY_REGION; break;
        case 0x06: mapping = AE_ENTITY_EQUIP; break;
        case 0x09: mapping = AE_ENTITY_TEAM; break;
        case 0x0b: mapping = AE_ENTITY_MP_LEVEL; break;
        default: mapping = AE_ENTITY_NONE; break;
    }
    return mapping;
}

AE_DEFINE_VALUE_LOOKUP(ae_weapon_type_t, ae_weapon_type_from_value,
                       weapon_type_table)
AE_DEFINE_NAMED_LOOKUP(ae_weapon_type_t, ae_weapon_type_from_name,
                       weapon_type_table)
AE_DEFINE_VALUE_GETTER(ae_weapon_type_t, ae_weapon_type_value,
                       weapon_type_table)

AE_DEFINE_VALUE_LOOKUP(ae_scene_type_t, ae_scene_type_from_value,
                       scene_type_table)
AE_DEFINE_VALUE_LOOKUP(ae_monster_type_t, ae_monster_type_from_value,
                       monster_type_table)
AE_DEFINE_VALUE_LOOKUP(ae_life_state_t, ae_life_state_from_value,
                       life_state_table)
AE_DEFINE_VALUE_LOOKUP(ae_climate_type_t, ae_climate_type_from_value,
                       climate_type_table)
AE_DEFINE_VALUE_LOOKUP(ae_fetter_state_t, ae_fetter_state_from_value,
                       fetter_state_table)
AE_DEFINE_VALUE_LOOKUP(ae_enter_reason_t, ae_enter_reason_from_value,
                       enter_reason_table)
AE_DEFINE_VALUE_LOOKUP(ae_grow_curve_t, ae_grow_curve_from_value,
                       grow_curve_table)
AE_DEFINE_VALUE_LOOKUP(ae_server_buff_type_t, ae_server_buff_type_from_value,
                       server_buff_type_table)
AE_DEFINE_VALUE_LOOKUP(ae_item_use_op_t, ae_item_use_op_from_value,
                       item_use_op_table)
AE_DEFINE_NAMED_LOOKUP(ae_item_use_op_t, ae_item_use_op_from_name,
                       item_use_op_table)
AE_DEFINE_VALUE_LOOKUP(ae_item_use_target_t, ae_item_use_target_from_value,
                       item_use_target_table)
AE_DEFINE_NAMED_LOOKUP(ae_item_use_target_t, ae_item_use_target_from_name,
                       item_use_target_table)
AE_DEFINE_VALUE_LOOKUP(ae_refresh_type_t, ae_refresh_type_from_value,
                       refresh_type_table)
AE_DEFINE_VALUE_LOOKUP(ae_activity_type_t, ae_activity_type_from_value,
                       activity_type_table)
AE_DEFINE_VALUE_LOOKUP(ae_camp_target_type_t, ae_camp_target_type_from_value,
                       camp_target_type_table)
AE_DEFINE_VALUE_LOOKUP(ae_battle_pass_refresh_type_t,
                       ae_battle_pass_refresh_type_from_value,
                       battle_pass_refresh_type_table)
AE_DEFINE_VALUE_LOOKUP(ae_battle_pass_mission_status_t,
                       ae_battle_pass_mission_status_from_value,
                       battle_pass_mission_status_table)

/* ============================================================ */

ae_s32 ae_ability_hash(const char *name) {
    ae_u32 hash = 0;
    if (name == NULL) {
        return 0;
    }
    for (const char *p = name; *p != '\0'; p++) {
        hash = (ae_u8)*p + 131u * hash;
    }
    return (ae_s32)hash;
}

/* ============================================================ */