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
 * @file game_constants.h
 *
 * @brief Core game constants and runtime-initialized static data.
 *
 * Declares global configuration values, compile-time gameplay constants,
 * default ability definitions, scene tag mappings, and illegal item sets.
 * Also provides initialization and cleanup functions for derived data such
 * as ability hashes and sparse set structures.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

#include <stdbool.h>
#include <stddef.h>

#include "position.h"      /* ae_position_t                  */
#include "sparse_set.h"    /* ae_sparse_set_t                */

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Mutable statics
 * ====================================================================== */

extern char  ae_version[16];        /* "4.0.0"          */
extern int   ae_version_parts[3];   /* {4, 0, 0}        */
extern bool  ae_debug;              /* false            */

/* =========================================================================
 * Compile-time integer constants
 * ====================================================================== */

#define AE_DEFAULT_TEAMS                4
#define AE_MAX_TEAMS                   10
#define AE_MAIN_CHARACTER_MALE   10000005
#define AE_MAIN_CHARACTER_FEMALE 10000007
#define AE_MAX_FRIENDS                 60
#define AE_MAX_FRIEND_REQUESTS         50
#define AE_SERVER_CONSOLE_UID          99   /* UID of the server console "player" */
#define AE_BATTLE_PASS_MAX_LEVEL       50
#define AE_BATTLE_PASS_POINT_PER_LEVEL 1000
#define AE_BATTLE_PASS_POINT_PER_WEEK  10000
#define AE_BATTLE_PASS_LEVEL_PRICE     150
#define AE_BATTLE_PASS_CURRENT_INDEX   2

/* =========================================================================
 * Start position
 * Initialised in game_constants_init().
 * ====================================================================== */

extern ae_position_t ae_start_position; /* {2747.0f, 194.0f, -1719.0f} */

/* =========================================================================
 * Default ability strings
 * ====================================================================== */

extern const char *AE_DEFAULT_ABILITY_STRINGS[];
extern const size_t AE_DEFAULT_ABILITY_STRINGS_LEN;

extern const char *AE_DEFAULT_TEAM_ABILITY_STRINGS[];
extern const size_t AE_DEFAULT_TEAM_ABILITY_STRINGS_LEN;

/* =========================================================================
 * Default ability hashes
 * Populated by game_constants_init().
 * ====================================================================== */

extern int    *ae_default_ability_hashes;    /* heap-allocated array        */
extern size_t  ae_default_ability_hashes_len;
extern int     ae_default_ability_name;      /* Utils.abilityHash("Default") */

/* =========================================================================
 * Default custom scene tags
 *
 * Flattened into a table of (scene_id, tag) pairs; terminated by a
 * sentinel entry with scene_id == -1.
 * ====================================================================== */

typedef struct {
    int scene_id;
    int tag;
} ae_scene_tag_entry_t;

extern const ae_scene_tag_entry_t AE_DEFAULT_CUSTOM_SCENE_TAGS[]; /* sentinel-terminated */

/* =========================================================================
 * Illegal item / weapon / relic sparse sets
 * Populated by game_constants_init().
 * ====================================================================== */

extern ae_sparse_set_t *ae_illegal_weapons;
extern ae_sparse_set_t *ae_illegal_relics;
extern ae_sparse_set_t *ae_illegal_items;

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

/**
 * game_constants_init()
 * Computes all derived values (ability hashes, sparse sets, start position).
 * Must be called once during ae_init(), before any gameplay code runs.
 */
void game_constants_init(void);

/**
 * game_constants_free()
 * Releases heap memory owned by this module (hashes array, sparse sets).
 * Call during ae_shutdown().
 */
void game_constants_free(void);

#ifdef __cplusplus
}
#endif

#endif /* GAME_CONSTANTS_H */
