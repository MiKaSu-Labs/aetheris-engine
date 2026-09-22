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
 * @file ae_game_constants.c
 *
 * @brief Server-wide game constants.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <aetheris/ae_game_constants.h>

#include <aetheris/ae_props.h>

#include <string.h>

/* ============================================================ */

static const ae_game_position_t start_position = {2747, 194, -1719};

/* Default entity ability names; hashes mirror abilityHash(). */
static const char *const default_ability_strings[] = {
    "Avatar_DefaultAbility_VisionReplaceDieInvincible",
    "Avatar_DefaultAbility_AvartarInShaderChange",
    "Avatar_SprintBS_Invincible",
    "Avatar_Freeze_Duration_Reducer",
    "Avatar_Attack_ReviveEnergy",
    "Avatar_Component_Initializer",
    "Avatar_FallAnthem_Achievement_Listener",
    "GrapplingHookSkill_Ability",
    "SceneAbility_DiveVolume",
    "Avatar_PlayerBoy_DiveStamina_Reduction",
    "Avatar_PlayerGirl_DiveStamina_Reduction",
    /* Team */
    "Ability_Avatar_Dive_Team",
    /* Crab Shield */
    "Ability_Avatar_Dive_CrabShield",
    "Avatar_Absorb_TrackingMissile",
    /* Seal Echo */
    "Ability_Avatar_Dive_SealEcho",
    /* Sword Fish 2 charge */
    "Avatar_Absorb_SwordFishSlash",
    /* Flat fish thingy */
    "Ability_Avatar_Dive_Seahorse_Spray_Baihe",
    /* Sumeru mushroom jump */
    "Avatar_Trampoline_Jump_Controller",
    /* Activity */
    "ActivityAbility_Absorb_Shoot",
    "Activity_MagicWave_SkillButton"};

#define DEFAULT_ABILITY_COUNT \
    (sizeof(default_ability_strings) / sizeof(default_ability_strings[0]))

/* Scene -> custom tags, mirroring DEFAULT_CUSTOM_SCENE_TAGS. */
typedef struct ae_game_scene_tags {
    ae_s32 scene_id;
    const ae_s32 *tags;
    ae_size count;
} ae_game_scene_tags_t;

static const ae_s32 scene_3_tags[] = {152, 153, 1094, 1164, 1166};
static const ae_s32 scene_4_tags[] = {106, 109, 117};
static const ae_s32 scene_9_tags[] = {1000, 1001, 1002, 1003, 1004, 1005, 1006,
                                      1007, 1008, 1009, 1010, 1011, 1012, 1013,
                                      1014, 1015, 1016, 1017, 1018, 1019, 1020,
                                      1021, 1022, 1023, 1024, 1025, 1026, 1027,
                                      1028, 1029, 1030, 1031, 1032, 1033, 1034,
                                      1035, 1036, 1037, 1038, 1039, 1040, 1041,
                                      1042, 1043, 1044, 1045, 1046, 1047, 1048,
                                      1049, 1050, 1051, 1052, 1053, 1054, 1055,
                                      1056, 1057, 1058, 1059, 1060, 1061, 1062,
                                      1063, 1064, 1065, 1066, 1067, 1068, 1069,
                                      1070, 1071, 1072, 1073, 1074, 1075, 1076,
                                      1077, 1078, 1079, 1080, 1081, 1082, 1083,
                                      1084, 1085};
static const ae_s32 scene_10_tags[] = {1261, 1262, 1263, 1264, 1265,
                                       1266, 1267, 1268};

static const ae_game_scene_tags_t scene_tags_table[] = {
    {3, scene_3_tags, sizeof(scene_3_tags) / sizeof(scene_3_tags[0])},
    {4, scene_4_tags, sizeof(scene_4_tags) / sizeof(scene_4_tags[0])},
    {9, scene_9_tags, sizeof(scene_9_tags) / sizeof(scene_9_tags[0])},
    {10, scene_10_tags, sizeof(scene_10_tags) / sizeof(scene_10_tags[0])}};

/* Illegal-item filters. */
static ae_sparse_set_t illegal_weapons_set;
static ae_sparse_set_t illegal_relics_set;
static ae_sparse_set_t illegal_items_set;

static ae_bool constants_initialized = false;

const ae_game_position_t *ae_game_start_position(void) {
    return &start_position;
}

ae_size ae_game_default_ability_count(void) {
    return DEFAULT_ABILITY_COUNT;
}

ae_s32 ae_game_default_ability_hash(ae_size index) {
    if (index >= DEFAULT_ABILITY_COUNT) {
        return 0;
    }
    return ae_ability_hash(default_ability_strings[index]);
}

ae_s32 ae_game_default_ability_name(void) {
    return ae_ability_hash("Default");
}

ae_size ae_game_scene_tag_count(ae_s32 scene_id) {
    for (ae_size i = 0;
         i < sizeof(scene_tags_table) / sizeof(scene_tags_table[0]); i++) {
        if (scene_tags_table[i].scene_id == scene_id) {
            return scene_tags_table[i].count;
        }
    }
    return 0;
}

const ae_s32 *ae_game_scene_tags(ae_s32 scene_id) {
    for (ae_size i = 0;
         i < sizeof(scene_tags_table) / sizeof(scene_tags_table[0]); i++) {
        if (scene_tags_table[i].scene_id == scene_id) {
            return scene_tags_table[i].tags;
        }
    }
    return NULL;
}

ae_error_t ae_game_constants_init(void) {
    ae_error_t err;

    if (constants_initialized) {
        return AE_OK;
    }

    err = ae_sparse_set_init(&illegal_weapons_set,
                             "10000-10008, 11411, 11506-11508, 12505, "
                             "12506, 12508, 12509, 13503, 13506, 14411, "
                             "14503, 14505, 14508, 15504-15506",
                             NULL, 0);
    if (err != AE_OK) {
        return err;
    }

    err = ae_sparse_set_init(&illegal_relics_set,
                             "20001, 23300-23340, 23383-23385, "
                             "78310-78554, 99310-99554",
                             NULL, 0);
    if (err != AE_OK) {
        ae_sparse_set_destroy(&illegal_weapons_set);
        return err;
    }

    err = ae_sparse_set_init(&illegal_items_set,
                             "100086, 100087, 100100-101000, "
                             "101106-101110, 101306, 101500-104000, "
                             "105001, 105004, 106000-107000, 107011, "
                             "108000, 109000-110000, 115000-130000, "
                             "200200-200899, 220050, 220054",
                             NULL, 0);
    if (err != AE_OK) {
        ae_sparse_set_destroy(&illegal_weapons_set);
        ae_sparse_set_destroy(&illegal_relics_set);
        return err;
    }

    constants_initialized = true;
    return AE_OK;
}

void ae_game_constants_destroy(void) {
    if (!constants_initialized) {
        return;
    }
    ae_sparse_set_destroy(&illegal_weapons_set);
    ae_sparse_set_destroy(&illegal_relics_set);
    ae_sparse_set_destroy(&illegal_items_set);
    constants_initialized = false;
}

const ae_sparse_set_t *ae_game_illegal_weapons(void) {
    return &illegal_weapons_set;
}

const ae_sparse_set_t *ae_game_illegal_relics(void) {
    return &illegal_relics_set;
}

const ae_sparse_set_t *ae_game_illegal_items(void) {
    return &illegal_items_set;
}

/* ============================================================ */