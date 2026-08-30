/*
 *
 *           ______ _______ _    _ ______ _____  _____  _____
 *     /\   |  ____|__   __| |  | |  ____|  __ \|_   _|/ ____|
 *    /  \  | |__     | |  | |__| | |__  | |__) | | | | (___
 *   / /\ \ |  __|    | |  |  __  |  __| |  _  /  | |  \___ \
 *  / ____ \| |____   | |  | |  | | |____| | \ \ _| |_ ____) |
 * /_/    \_\______|  |_|  |_|  |_|______|_|  \_\_____|_____/
 *
 *                                                             v1.0
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

#include "aetheris.h"       /* ae_error_t      */
#include "position.h"       /* ae_position_t   */
#include "sparse_set.h"     /* ae_sparse_set_t */

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Mutable statics
 * ====================================================================== */

/**
 * Server version string, NUL-terminated. Must remain 15 characters or
 * fewer plus the terminator to fit this buffer; any code that writes to
 * it must not exceed that bound.
 */
extern char ae_version[16];

/** Version components parsed from ae_version: {major, minor, patch}. */
extern int ae_version_parts[3];

/**
 * Master gameplay debug flag. Like the flags in debug_constants.h, this
 * is intended to be set once at startup before worker threads run; no
 * synchronization is performed on reads elsewhere in the codebase.
 */
extern bool ae_debug;

/* =========================================================================
 * Compile-time integer constants
 * ====================================================================== */

#define AE_DEFAULT_TEAMS                4    /**< Teams created per session by default. */
#define AE_MAX_TEAMS                   10    /**< Hard upper bound on teams per session. */
#define AE_MAIN_CHARACTER_MALE   10000005    /**< Avatar ID for the male protagonist. */
#define AE_MAIN_CHARACTER_FEMALE 10000007    /**< Avatar ID for the female protagonist. */
#define AE_MAX_FRIENDS                 60    /**< Maximum size of a player's friend list. */
#define AE_MAX_FRIEND_REQUESTS         50    /**< Maximum pending outgoing friend requests. */
#define AE_SERVER_CONSOLE_UID          99    /**< UID reserved for the server console player. */
#define AE_BATTLE_PASS_MAX_LEVEL       50    /**< Highest purchasable battle pass level. */
#define AE_BATTLE_PASS_POINT_PER_LEVEL 1000  /**< Points required to advance one level. */
#define AE_BATTLE_PASS_POINT_PER_WEEK  10000 /**< Weekly point cap for battle pass progress. */
#define AE_BATTLE_PASS_LEVEL_PRICE     150   /**< Crystal cost to buy one battle pass level. */
#define AE_BATTLE_PASS_CURRENT_INDEX   2     /**< Index of the currently active battle pass. */

/* =========================================================================
 * Start position
 *
 * World-space coordinates used when spawning a player with no saved
 * position. Populated by game_constants_init(); do not read before that
 * function has returned AE_OK.
 * ====================================================================== */

/** Default player spawn point {x=2747, y=194, z=-1719}. */
extern ae_position_t ae_start_position;

/* =========================================================================
 * Default ability strings
 *
 * Compile-time string tables listing ability identifiers granted to every
 * avatar on session start. The corresponding hash array is computed in
 * game_constants_init() and freed by game_constants_free().
 * ====================================================================== */

/** Null-terminated ability identifiers applied to every avatar. */
extern const char *AE_DEFAULT_ABILITY_STRINGS[];

/** Number of entries in AE_DEFAULT_ABILITY_STRINGS. */
extern const size_t AE_DEFAULT_ABILITY_STRINGS_LEN;

/** Ability identifiers applied at the team level. */
extern const char *AE_DEFAULT_TEAM_ABILITY_STRINGS[];

/** Number of entries in AE_DEFAULT_TEAM_ABILITY_STRINGS. */
extern const size_t AE_DEFAULT_TEAM_ABILITY_STRINGS_LEN;

/* =========================================================================
 * Default ability hashes
 *
 * Hashed equivalents of AE_DEFAULT_ABILITY_STRINGS, computed once in
 * game_constants_init() and released in game_constants_free(). The
 * pointer is NULL until init runs successfully; callers must not read
 * it before then.
 *
 * Ownership: heap-allocated by this module. Do not free directly; call
 * game_constants_free().
 * ====================================================================== */

/** Hashed ability IDs; parallel to AE_DEFAULT_ABILITY_STRINGS. */
extern int *ae_default_ability_hashes;

/** Number of valid entries in ae_default_ability_hashes. */
extern size_t ae_default_ability_hashes_len;

/** Hash of the literal string "Default", used as a fallback ability name. */
extern int ae_default_ability_name;

/* =========================================================================
 * Default custom scene tags
 *
 * Flat table of (scene_id, tag) pairs applied to every new session.
 * Terminated by a sentinel entry with scene_id == -1 and tag == -1.
 * ====================================================================== */

/** One entry in the custom scene tag table. */
typedef struct {
    int scene_id; /**< Scene to which the tag applies; -1 marks the sentinel. */
    int tag;      /**< Tag value to set on the scene; -1 in the sentinel entry. */
} ae_scene_tag_entry_t;

/** Sentinel-terminated scene tag table; defined in game_constants.c. */
extern const ae_scene_tag_entry_t AE_DEFAULT_CUSTOM_SCENE_TAGS[];

/* =========================================================================
 * Illegal item / weapon / relic sparse sets
 *
 * Sparse sets of item IDs that players are not permitted to possess.
 * Populated by game_constants_init() and released by game_constants_free().
 * All three pointers are NULL until init runs successfully.
 *
 * Ownership: allocated by ae_sparse_set_parse() inside this module. Do
 * not free directly; call game_constants_free().
 * ====================================================================== */

extern ae_sparse_set_t *ae_illegal_weapons; /**< Weapon IDs blocked from player inventories. */
extern ae_sparse_set_t *ae_illegal_relics;  /**< Relic IDs blocked from player inventories. */
extern ae_sparse_set_t *ae_illegal_items;   /**< General item IDs blocked from player inventories. */

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

/**
 * @brief Initialize all derived game constants.
 *
 * Populates ae_start_position, computes ae_default_ability_hashes from
 * AE_DEFAULT_ABILITY_STRINGS, sets ae_default_ability_name, and builds
 * the ae_illegal_weapons, ae_illegal_relics, and ae_illegal_items sparse
 * sets by parsing their DSL range strings.
 *
 * Called once from ae_main(), skipped in SERVER_RUN_MODE_DISPATCH_ONLY
 * (a dispatch-only deployment has no use for this data). On any non-AE_OK
 * return, global state may be left partially populated; the caller must
 * treat this as fatal and terminate the process rather than retry, since
 * ae_main()'s own contract already requires this on any failure.
 *
 * Calling this a second time without an intervening game_constants_free()
 * leaks the previously allocated array and sparse sets.
 *
 * Not thread-safe. Call from the main thread only.
 *
 * @return AE_OK on success.
 * @return AE_ERR_GENERIC if computing the ability-hash array size would
 *         overflow, if that array cannot be allocated, or if any of the
 *         three illegal-item sparse sets cannot be built.
 */
ae_error_t game_constants_init(void);

/**
 * @brief Release all heap memory owned by this module.
 *
 * Frees ae_default_ability_hashes and the three illegal-item sparse sets,
 * then sets all released pointers to NULL and resets
 * ae_default_ability_hashes_len to 0.
 *
 * Idempotent: safe to call even if game_constants_init() never ran or
 * only partially succeeded, and safe to call more than once.
 *
 * Call during ae_shutdown(). Not thread-safe. Call from the main thread
 * only.
 */
void game_constants_free(void);

#ifdef __cplusplus
}
#endif

#endif /* GAME_CONSTANTS_H */
