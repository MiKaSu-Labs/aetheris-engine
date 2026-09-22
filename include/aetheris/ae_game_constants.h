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
 * @file ae_game_constants.h
 *
 * @brief Server-wide game constants mirroring the reference
 *        GameConstants class.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_GAME_CONSTANTS_H
#define AE_GAME_CONSTANTS_H

#include <aetheris/ae_sparse_set.h>
#include <aetheris/ae_types.h>

/* ============================================================ */

#define AE_GAME_VERSION "4.0.0"
#define AE_GAME_VERSION_PARTS { 4, 0, 0 }

#define AE_GAME_DEFAULT_TEAMS 4
#define AE_GAME_MAX_TEAMS 10

#define AE_GAME_MAIN_CHARACTER_MALE 10000005
#define AE_GAME_MAIN_CHARACTER_FEMALE 10000007

#define AE_GAME_MAX_FRIENDS 60
#define AE_GAME_MAX_FRIEND_REQUESTS 50

/* The UID of the server console's "player". */
#define AE_GAME_SERVER_CONSOLE_UID 99

#define AE_GAME_BATTLE_PASS_MAX_LEVEL 50
#define AE_GAME_BATTLE_PASS_POINT_PER_LEVEL 1000
#define AE_GAME_BATTLE_PASS_POINT_PER_WEEK 10000
#define AE_GAME_BATTLE_PASS_LEVEL_PRICE 150
#define AE_GAME_BATTLE_PASS_CURRENT_INDEX 2

typedef struct ae_game_position {
    ae_s32 x;
    ae_s32 y;
    ae_s32 z;
} ae_game_position_t;

/*
 * The world position a new character starts at.
 * Returns a view into a static instance; do not free.
 */
const ae_game_position_t *ae_game_start_position(void);

/* The number of default ability hashes handed to avatars. */
ae_size ae_game_default_ability_count(void);

/*
 * Returns the i-th default ability hash (the abilityHash of the
 * i-th default ability string).  Returns 0 when out of range.
 */
ae_s32 ae_game_default_ability_hash(ae_size index);

/* Returns the default ability name hash (abilityHash("Default")). */
ae_s32 ae_game_default_ability_name(void);

/* The number of scene tags a scene has, and a view into the table. */
ae_size ae_game_scene_tag_count(ae_s32 scene_id);

/* Returns a view of the tags for a scene; NULL when the scene has
 * no configured tags. */
const ae_s32 *ae_game_scene_tags(ae_s32 scene_id);

/*
 * Illegal-item filters.  The caller must call ae_game_constants_init()
 * once before the first lookup and ae_game_constants_destroy() at
 * shutdown.  These return the parsed sparse sets (never NULL after
 * init).
 */
ae_error_t ae_game_constants_init(void);
void ae_game_constants_destroy(void);

const ae_sparse_set_t *ae_game_illegal_weapons(void);
const ae_sparse_set_t *ae_game_illegal_relics(void);
const ae_sparse_set_t *ae_game_illegal_items(void);

/* ============================================================ */

#endif /* AE_GAME_CONSTANTS_H */