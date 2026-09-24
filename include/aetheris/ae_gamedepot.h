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
 * @file ae_gamedepot.h
 *
 * @brief Derived relic rolling depots built from the gamedata maps.
 *
 * Mirrors the reference depot builder: the battery of reliquary main
 * properties and affixes is bucketed by depot id, and a per-depot
 * weighted list answers random main-prop rolls for relic creation.
 * The weighted selection shares one seeder so tests can drive relic
 * rolls deterministically.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_GAMEDEPOT_H
#define AE_GAMEDEPOT_H

#include <aetheris/ae_error.h>
#include <aetheris/ae_gamedata.h>
#include <aetheris/ae_types.h>

/* ============================================================ */

#define AE_GAMEDEPOT_NO_INDEX ((ae_size)-1)

/**
 * ae_gamedepot_init - build the relic depots from gamedata.
 *
 * Must run after the reliquary registries are loaded. Safe to call
 * again to rebuild after a gamedata reload.
 */
ae_error_t ae_gamedepot_init(void);

/**
 * ae_gamedepot_destroy - release every depot.
 *
 * Safe on an uninitialised depot; the gamedata maps are untouched.
 */
void ae_gamedepot_destroy(void);

/**
 * ae_gamedepot_set_seed - reseed the shared weighted-pick generator.
 * @seed: 64-bit seed value.
 *
 * Seeding drives both the depot random rolls and
 * `ae_gamedepot_weighted_index`, making relic generation reproducible
 * for tests.
 */
void ae_gamedepot_set_seed(ae_u64 seed);

/**
 * ae_gamedepot_get_random_relic_main_prop - roll a main property.
 * @depot: prop depot id from the relic item data.
 *
 * Returns a pointer owned by the gamedata registry, or NULL when the
 * depot is empty or unknown.
 */
const ae_reliquary_main_prop_t *ae_gamedepot_get_random_relic_main_prop(
    ae_s32 depot);

/**
 * ae_gamedepot_get_relic_main_prop_list - list the candidates in a
 * main-prop depot.
 * @depot: prop depot id.
 * @out_list: receives the candidate pointer array (registry owned).
 * @out_count: receives the candidate count.
 *
 * A missing or empty depot yields a NULL list with count 0 and AE_OK.
 */
ae_error_t ae_gamedepot_get_relic_main_prop_list(
    ae_s32 depot, const ae_reliquary_main_prop_t *const **out_list,
    ae_size *out_count);

/**
 * ae_gamedepot_get_relic_affix_list - list the affixes in a depot.
 * @depot: affix depot id from the relic item data.
 * @out_list: receives the candidate pointer array (registry owned).
 * @out_count: receives the candidate count.
 *
 * A missing or empty depot yields a NULL list with count 0 and AE_OK.
 */
ae_error_t ae_gamedepot_get_relic_affix_list(
    ae_s32 depot, const ae_reliquary_affix_t *const **out_list,
    ae_size *out_count);

/**
 * ae_gamedepot_weighted_index - pick an index from a weight array.
 * @weights: element weights, matched index by index to a candidate
 *           list.
 * @count: number of weights.
 *
 * Non-positive weights never win. Returns AE_GAMEDEPOT_NO_INDEX when
 * there is nothing pickable. Uses the shared seeder, so seed before
 * calling for deterministic output.
 */
ae_size ae_gamedepot_weighted_index(const ae_u32 *weights, ae_size count);

/* ============================================================ */

#endif /* AE_GAMEDEPOT_H */