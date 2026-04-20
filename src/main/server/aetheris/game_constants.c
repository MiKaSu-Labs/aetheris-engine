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
 * @file game_constants.c
 *
 * @brief Defines and initializes core game constants for Aetheris.
 *
 * Contains version data, start position, default ability definitions,
 * scene tag mappings, and illegal item/weapon/relic sets. Also provides
 * initialization and cleanup routines for derived runtime constants such
 * as ability hashes and sparse-set-based rule tables.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include "game_constants.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "position.h"
#include "sparse_set.h"
#include "utils.h"

/* =========================================================================
 * Mutable statics
 * ====================================================================== */

char  ae_version[16]      = "4.0.0";
int   ae_version_parts[3] = {4, 0, 0};
bool  ae_debug            = false;

/* =========================================================================
 * Start position  (x=2747, y=194, z=-1719)
 * ====================================================================== */

ae_position_t ae_start_position; /* set in game_constants_init() */

/* =========================================================================
 * Default ability strings
 * ====================================================================== */

const char *AE_DEFAULT_ABILITY_STRINGS[] = {
    /* Core avatar abilities */
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

    /* Flat fish */
    "Ability_Avatar_Dive_Seahorse_Spray_Baihe",

    /* Sumeru mushroom jump */
    "Avatar_Trampoline_Jump_Controller",

    /* Activity */
    "ActivityAbility_Absorb_Shoot",
    "Activity_MagicWave_SkillButton",
};

const size_t AE_DEFAULT_ABILITY_STRINGS_LEN =
    sizeof(AE_DEFAULT_ABILITY_STRINGS) / sizeof(AE_DEFAULT_ABILITY_STRINGS[0]);

const char *AE_DEFAULT_TEAM_ABILITY_STRINGS[] = {
    "Ability_Avatar_Dive_Team", /* FONTAINE diving */
};

const size_t AE_DEFAULT_TEAM_ABILITY_STRINGS_LEN =
    sizeof(AE_DEFAULT_TEAM_ABILITY_STRINGS) /
    sizeof(AE_DEFAULT_TEAM_ABILITY_STRINGS[0]);

/* =========================================================================
 * Derived ability hashes  (populated in game_constants_init)
 * ====================================================================== */

int   *ae_default_ability_hashes     = NULL;
size_t ae_default_ability_hashes_len = 0;
int    ae_default_ability_name       = 0;

/* =========================================================================
 * Default custom scene tags
 *
 * Stored as a flat (scene_id, tag) pair array; sentinel = {-1, -1}.
 * ====================================================================== */

/* Helper macro to shorten the table definition */
#define _ST(s, t) {(s), (t)}
#define _SENTINEL  {-1, -1}

const ae_scene_tag_entry_t AE_DEFAULT_CUSTOM_SCENE_TAGS[] = {
    /* scene 3 */
    _ST(3, 152), _ST(3, 153), _ST(3, 1094), _ST(3, 1164), _ST(3, 1166),
    /* scene 4 */
    _ST(4, 106), _ST(4, 109), _ST(4, 117),
    /* scene 9  - IntStream.range(1000, 1086) */
    _ST(9,1000),_ST(9,1001),_ST(9,1002),_ST(9,1003),_ST(9,1004),
    _ST(9,1005),_ST(9,1006),_ST(9,1007),_ST(9,1008),_ST(9,1009),
    _ST(9,1010),_ST(9,1011),_ST(9,1012),_ST(9,1013),_ST(9,1014),
    _ST(9,1015),_ST(9,1016),_ST(9,1017),_ST(9,1018),_ST(9,1019),
    _ST(9,1020),_ST(9,1021),_ST(9,1022),_ST(9,1023),_ST(9,1024),
    _ST(9,1025),_ST(9,1026),_ST(9,1027),_ST(9,1028),_ST(9,1029),
    _ST(9,1030),_ST(9,1031),_ST(9,1032),_ST(9,1033),_ST(9,1034),
    _ST(9,1035),_ST(9,1036),_ST(9,1037),_ST(9,1038),_ST(9,1039),
    _ST(9,1040),_ST(9,1041),_ST(9,1042),_ST(9,1043),_ST(9,1044),
    _ST(9,1045),_ST(9,1046),_ST(9,1047),_ST(9,1048),_ST(9,1049),
    _ST(9,1050),_ST(9,1051),_ST(9,1052),_ST(9,1053),_ST(9,1054),
    _ST(9,1055),_ST(9,1056),_ST(9,1057),_ST(9,1058),_ST(9,1059),
    _ST(9,1060),_ST(9,1061),_ST(9,1062),_ST(9,1063),_ST(9,1064),
    _ST(9,1065),_ST(9,1066),_ST(9,1067),_ST(9,1068),_ST(9,1069),
    _ST(9,1070),_ST(9,1071),_ST(9,1072),_ST(9,1073),_ST(9,1074),
    _ST(9,1075),_ST(9,1076),_ST(9,1077),_ST(9,1078),_ST(9,1079),
    _ST(9,1080),_ST(9,1081),_ST(9,1082),_ST(9,1083),_ST(9,1084),
    _ST(9,1085),
    /* scene 10 - IntStream.range(1261, 1269) */
    _ST(10,1261),_ST(10,1262),_ST(10,1263),_ST(10,1264),
    _ST(10,1265),_ST(10,1266),_ST(10,1267),_ST(10,1268),
    /* sentinel */
    _SENTINEL,
};

#undef _ST
#undef _SENTINEL

/* =========================================================================
 * Illegal sparse sets  (populated in game_constants_init)
 * ====================================================================== */

ae_sparse_set_t *ae_illegal_weapons = NULL;
ae_sparse_set_t *ae_illegal_relics  = NULL;
ae_sparse_set_t *ae_illegal_items   = NULL;

/* =========================================================================
 * game_constants_init()
 * ====================================================================== */
void game_constants_init(void) {
    /* --- Start position ------------------------------------------------ */
    ae_start_position.x =  2747.0f;
    ae_start_position.y =   194.0f;
    ae_start_position.z = -1719.0f;

    /* --- Ability hashes ------------------------------------------------ */
    ae_default_ability_hashes_len = AE_DEFAULT_ABILITY_STRINGS_LEN;
    ae_default_ability_hashes =
        malloc(ae_default_ability_hashes_len * sizeof(int));

    for (size_t i = 0; i < ae_default_ability_hashes_len; i++) {
        ae_default_ability_hashes[i] =
            utils_ability_hash(AE_DEFAULT_ABILITY_STRINGS[i]);
    }

    ae_default_ability_name = utils_ability_hash("Default");

    /* --- Illegal sparse sets ------------------------------------------- */
    // ae_sparse_set_parse() implements the same DSL.
    ae_illegal_weapons = ae_sparse_set_parse(
        "10000-10008, 11411, 11506-11508, 12505, 12506, 12508, 12509,"
        "13503, 13506, 14411, 14503, 14505, 14508, 15504-15506");

    ae_illegal_relics = ae_sparse_set_parse(
        "20001, 23300-23340, 23383-23385, 78310-78554, 99310-99554");

    ae_illegal_items = ae_sparse_set_parse(
        "100086, 100087, 100100-101000, 101106-101110, 101306, 101500-104000,"
        "105001, 105004, 106000-107000, 107011, 108000, 109000-110000,"
        "115000-130000, 200200-200899, 220050, 220054");
}

/* =========================================================================
 * game_constants_free()
 * ====================================================================== */
void game_constants_free(void) {
    free(ae_default_ability_hashes);
    ae_default_ability_hashes     = NULL;
    ae_default_ability_hashes_len = 0;

    ae_sparse_set_free(ae_illegal_weapons); ae_illegal_weapons = NULL;
    ae_sparse_set_free(ae_illegal_relics);  ae_illegal_relics  = NULL;
    ae_sparse_set_free(ae_illegal_items);   ae_illegal_items   = NULL;
}
