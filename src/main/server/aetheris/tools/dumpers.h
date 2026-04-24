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
 * @file dumpers.h
 *
 * @brief Data dump utilities.
 *        Exports server game data (commands, avatars, items, scenes,
 *        entities, quests, areas) to CSV and JSON files for use by
 *        external tools such as the web GM handbook.
 *        See src/handbook/data/README.md for attributions.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef DUMPERS_H
#define DUMPERS_H

#include <stdbool.h>
#include <stddef.h>

#include "command.h"       /* ae_target_requirement_t  */
#include "item_type.h"     /* ae_item_type_t           */
#include "scene_type.h"    /* ae_scene_type_t          */

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * ae_dump_quality_t
 *
 * Represents the rarity tier of an avatar or item.
 * ====================================================================== */
typedef enum {
    AE_QUALITY_LEGENDARY,
    AE_QUALITY_EPIC,
    AE_QUALITY_RARE,
    AE_QUALITY_UNCOMMON,
    AE_QUALITY_COMMON,
    AE_QUALITY_UNKNOWN,
} ae_dump_quality_t;

/*
 * Converts a numeric rank level to ae_dump_quality_t.
 *   0       , AE_QUALITY_UNKNOWN
 *   1       , AE_QUALITY_COMMON
 *   2       , AE_QUALITY_UNCOMMON
 *   3       , AE_QUALITY_RARE
 *   4       , AE_QUALITY_EPIC
 *   5+      , AE_QUALITY_LEGENDARY
 */
ae_dump_quality_t ae_dump_quality_from_rank(int rank_level);

/* Returns the string name of a quality value (e.g. "LEGENDARY"). */
const char *ae_dump_quality_str(ae_dump_quality_t quality);

/* =========================================================================
 * Info structs
 *
 * Plain data containers used during dump construction.
 * All heap-allocated string fields are owned by the struct;
 * free with the matching ae_*_info_free() function.
 * ====================================================================== */

typedef struct {
    char                  **names;        /* label + aliases, NULL-terminated */
    size_t                  names_len;
    char                   *description;  /* heap-allocated                   */
    char                  **usage;        /* NULL-terminated                  */
    size_t                  usage_len;
    char                  **permissions;  /* NULL-terminated                  */
    size_t                  permissions_len;
    ae_target_requirement_t target;
} ae_command_info_t;

typedef struct {
    char             *name;     /* heap-allocated */
    ae_dump_quality_t quality;
} ae_avatar_info_t;

typedef struct {
    int               id;
    char             *name;     /* heap-allocated */
    ae_dump_quality_t quality;
    ae_item_type_t    type;
    char             *icon;     /* heap-allocated, prefix stripped */
} ae_item_info_t;

typedef struct {
    char            *identifier; /* heap-allocated */
    ae_scene_type_t  type;
} ae_scene_info_t;

typedef struct {
    char *name;     /* heap-allocated, localised display name */
    char *internal; /* heap-allocated, internal monster name  */
} ae_entity_info_t;

typedef struct {
    char *title; /* heap-allocated */
} ae_main_quest_info_t;

typedef struct {
    char *description; /* heap-allocated */
    int   main_quest;
} ae_quest_info_t;

typedef struct {
    int   parent;
    char *name; /* heap-allocated */
} ae_area_info_t;

/* =========================================================================
 * Dump functions
 *
 * Each function reloads resources, builds the data set, and writes the
 * result to a file in the current working directory.
 * Returns 0 on success, -1 on failure.
 * ====================================================================== */

/*
 * Dumps all registered commands to 'commands.json'.
 * 'locale' selects the language used for command descriptions.
 */
int ae_dump_commands(const char *locale);

/*
 * Dumps all avatars to 'avatars.csv'.
 * 'locale' selects the language used for avatar names.
 */
int ae_dump_avatars(const char *locale);

/*
 * Dumps all items to 'items.csv', deduplicating by name.
 * 'locale' selects the language used for item names.
 */
int ae_dump_items(const char *locale);

/*
 * Dumps all scenes to 'scenes.csv'.
 */
int ae_dump_scenes(void);

/*
 * Dumps all monster entities to 'entities.csv'.
 * 'locale' selects the language used for entity names.
 */
int ae_dump_entities(const char *locale);

/*
 * Dumps all quests to 'quests.csv' and main quests to 'mainquests.csv'.
 * 'locale' selects the language used for quest descriptions and titles.
 */
int ae_dump_quests(const char *locale);

/*
 * Dumps all world areas to 'areas.csv'.
 * 'locale' selects the language used for area names.
 */
int ae_dump_areas(const char *locale);

#ifdef __cplusplus
}
#endif

#endif /* DUMPERS_H */
