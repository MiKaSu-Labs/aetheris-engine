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
 * @file ae_gamedepot.c
 *
 * @brief Derived relic rolling depots built from the gamedata maps.
 *
 * Main properties are bucketed by prop depot id in build order; each
 * bucket carries the candidate array plus an equal-length array of
 * weights so a roll is a weighted pick over the bucket. Affixes are
 * bucketed by depot id with no weights (the item model weights them
 * at pick time). Depots hold registries borrowed from gamedata, so
 * destroy never frees the pointed-to records.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdlib.h>
#include <string.h>

#include <aetheris/ae_common.h>
#include <aetheris/ae_error.h>
#include <aetheris/ae_gamedata.h>
#include <aetheris/ae_gamedepot.h>
#include <aetheris/ae_mt64.h>

/* ============================================================ */

typedef struct ae_relic_main_prop_bucket {
    ae_s32 depot_id;
    const ae_reliquary_main_prop_t **props;
    ae_u32 *weights;
    ae_size count;
    ae_size capacity;
} ae_relic_main_prop_bucket_t;

typedef struct ae_relic_affix_bucket {
    ae_s32 depot_id;
    const ae_reliquary_affix_t **affixes;
    ae_size count;
    ae_size capacity;
} ae_relic_affix_bucket_t;

static ae_relic_main_prop_bucket_t *g_main_prop_buckets;
static ae_size g_main_prop_bucket_count;
static ae_size g_main_prop_bucket_capacity;

static ae_relic_affix_bucket_t *g_affix_buckets;
static ae_size g_affix_bucket_count;
static ae_size g_affix_bucket_capacity;

static ae_mt64_t g_rng;
static ae_bool g_rng_ready;

/* ============================================================ */

static ae_error_t grow_main_prop_buckets(void)
{
    ae_size new_capacity =
        g_main_prop_bucket_capacity ? g_main_prop_bucket_capacity * 2u : 16u;
    ae_size bytes;
    ae_relic_main_prop_bucket_t *grown;

    if (!ae_mul_overflow_uz(new_capacity, sizeof(*grown), &bytes)) {
        return AE_ERR_OVERFLOW;
    }
    grown = (ae_relic_main_prop_bucket_t *)realloc(g_main_prop_buckets,
                                                   bytes);
    if (!grown) {
        return AE_ERR_OUT_OF_MEMORY;
    }
    g_main_prop_buckets = grown;
    g_main_prop_bucket_capacity = new_capacity;
    return AE_OK;
}

static ae_error_t grow_affix_buckets(void)
{
    ae_size new_capacity =
        g_affix_bucket_capacity ? g_affix_bucket_capacity * 2u : 16u;
    ae_size bytes;
    ae_relic_affix_bucket_t *grown;

    if (!ae_mul_overflow_uz(new_capacity, sizeof(*grown), &bytes)) {
        return AE_ERR_OVERFLOW;
    }
    grown = (ae_relic_affix_bucket_t *)realloc(g_affix_buckets, bytes);
    if (!grown) {
        return AE_ERR_OUT_OF_MEMORY;
    }
    g_affix_buckets = grown;
    g_affix_bucket_capacity = new_capacity;
    return AE_OK;
}

static ae_relic_main_prop_bucket_t *main_prop_bucket_for(ae_s32 depot_id)
{
    ae_size i;
    for (i = 0; i < g_main_prop_bucket_count; i++) {
        if (g_main_prop_buckets[i].depot_id == depot_id) {
            return &g_main_prop_buckets[i];
        }
    }
    return NULL;
}

static ae_relic_affix_bucket_t *affix_bucket_for(ae_s32 depot_id)
{
    ae_size i;
    for (i = 0; i < g_affix_bucket_count; i++) {
        if (g_affix_buckets[i].depot_id == depot_id) {
            return &g_affix_buckets[i];
        }
    }
    return NULL;
}

/* ============================================================ */

static ae_error_t main_prop_bucket_add(ae_s32 depot_id,
                                       const ae_reliquary_main_prop_t *prop,
                                       ae_u32 weight)
{
    ae_relic_main_prop_bucket_t *bucket = main_prop_bucket_for(depot_id);
    size_t bytes;

    if (!bucket) {
        if (g_main_prop_bucket_count == g_main_prop_bucket_capacity) {
            ae_error_t err = grow_main_prop_buckets();
            if (err != AE_OK) {
                return err;
            }
        }
        bucket = &g_main_prop_buckets[g_main_prop_bucket_count++];
        memset(bucket, 0, sizeof(*bucket));
        bucket->depot_id = depot_id;
    }
    if (bucket->count == bucket->capacity) {
        ae_size new_capacity =
            bucket->capacity ? bucket->capacity * 2u : 8u;
        const ae_reliquary_main_prop_t **grown_props;
        ae_u32 *grown_weights;

        if (!ae_mul_overflow_uz(new_capacity, sizeof(*grown_props),
                                &bytes)) {
            return AE_ERR_OVERFLOW;
        }
        grown_props = (const ae_reliquary_main_prop_t **)realloc(
            bucket->props, bytes);
        if (!grown_props) {
            return AE_ERR_OUT_OF_MEMORY;
        }
        bucket->props = grown_props;

        if (!ae_mul_overflow_uz(new_capacity, sizeof(*grown_weights),
                                &bytes)) {
            return AE_ERR_OVERFLOW;
        }
        grown_weights =
            (ae_u32 *)realloc(bucket->weights, bytes);
        if (!grown_weights) {
            return AE_ERR_OUT_OF_MEMORY;
        }
        bucket->weights = grown_weights;
        bucket->capacity = new_capacity;
    }
    bucket->props[bucket->count] = prop;
    bucket->weights[bucket->count] = weight;
    bucket->count++;
    return AE_OK;
}

static ae_error_t affix_bucket_add(ae_s32 depot_id,
                                   const ae_reliquary_affix_t *affix)
{
    ae_relic_affix_bucket_t *bucket = affix_bucket_for(depot_id);
    size_t bytes;

    if (!bucket) {
        if (g_affix_bucket_count == g_affix_bucket_capacity) {
            ae_error_t err = grow_affix_buckets();
            if (err != AE_OK) {
                return err;
            }
        }
        bucket = &g_affix_buckets[g_affix_bucket_count++];
        memset(bucket, 0, sizeof(*bucket));
        bucket->depot_id = depot_id;
    }
    if (bucket->count == bucket->capacity) {
        ae_size new_capacity = bucket->capacity ? bucket->capacity * 2u : 8u;
        const ae_reliquary_affix_t **grown;

        if (!ae_mul_overflow_uz(new_capacity, sizeof(*grown), &bytes)) {
            return AE_ERR_OVERFLOW;
        }
        grown = (const ae_reliquary_affix_t **)realloc(bucket->affixes,
                                                       bytes);
        if (!grown) {
            return AE_ERR_OUT_OF_MEMORY;
        }
        bucket->affixes = grown;
        bucket->capacity = new_capacity;
    }
    bucket->affixes[bucket->count++] = affix;
    return AE_OK;
}

/* ============================================================ */

static void bucket_depot_foreach_main_prop(
    const ae_reliquary_main_prop_t *data, void *user)
{
    ae_error_t *first_err = (ae_error_t *)user;

    if (*first_err != AE_OK) {
        return;
    }
    if (data->weight <= 0 || data->prop_depot_id <= 0) {
        return;
    }
    *first_err = main_prop_bucket_add(data->prop_depot_id, data,
                                      (ae_u32)data->weight);
}

static void bucket_depot_foreach_affix(const ae_reliquary_affix_t *data,
                                       void *user)
{
    ae_error_t *first_err = (ae_error_t *)user;

    if (*first_err != AE_OK) {
        return;
    }
    if (data->weight <= 0 || data->depot_id <= 0) {
        return;
    }
    *first_err = affix_bucket_add(data->depot_id, data);
}

static void clear_main_prop_buckets(void)
{
    ae_size i;
    for (i = 0; i < g_main_prop_bucket_count; i++) {
        free(g_main_prop_buckets[i].props);
        free(g_main_prop_buckets[i].weights);
    }
    free(g_main_prop_buckets);
    g_main_prop_buckets = NULL;
    g_main_prop_bucket_count = 0;
    g_main_prop_bucket_capacity = 0;
}

static void clear_affix_buckets(void)
{
    ae_size i;
    for (i = 0; i < g_affix_bucket_count; i++) {
        free(g_affix_buckets[i].affixes);
    }
    free(g_affix_buckets);
    g_affix_buckets = NULL;
    g_affix_bucket_count = 0;
    g_affix_bucket_capacity = 0;
}

/* ============================================================ */

ae_error_t ae_gamedepot_init(void)
{
    ae_error_t first_err = AE_OK;

    clear_main_prop_buckets();
    clear_affix_buckets();

    ae_gamedata_for_each_reliquary_main_prop(
        bucket_depot_foreach_main_prop, &first_err);
    if (first_err != AE_OK) {
        return first_err;
    }
    ae_gamedata_for_each_reliquary_affix(bucket_depot_foreach_affix,
                                         &first_err);
    if (first_err != AE_OK) {
        return first_err;
    }

    if (!g_rng_ready) {
        ae_mt64_init(&g_rng, 0x9E3779B97F4A7C15ULL);
        g_rng_ready = true;
    }
    return AE_OK;
}

void ae_gamedepot_destroy(void)
{
    clear_main_prop_buckets();
    clear_affix_buckets();
    g_rng_ready = false;
}

void ae_gamedepot_set_seed(ae_u64 seed)
{
    ae_mt64_set_seed(&g_rng, seed);
    g_rng_ready = true;
}

/* ============================================================ */

static double rng_unit_double(void)
{
    return (double)(ae_mt64_next(&g_rng) >> 11) *
           (1.0 / 9007199254740992.0);
}

static ae_size pick_weighted(const ae_u32 *weights, ae_size count,
                             double *out_total)
{
    double total;
    double value;
    ae_size i;

    total = 0.0;
    for (i = 0; i < count; i++) {
        total += (double)weights[i];
    }
    if (out_total) {
        *out_total = total;
    }
    if (total <= 0.0) {
        return AE_GAMEDEPOT_NO_INDEX;
    }
    value = rng_unit_double() * total;
    for (i = 0; i < count; i++) {
        value -= (double)weights[i];
        if (value < 0.0) {
            return i;
        }
    }
    return count - 1u;
}

ae_size ae_gamedepot_weighted_index(const ae_u32 *weights, ae_size count)
{
    if (!weights || count == 0) {
        return AE_GAMEDEPOT_NO_INDEX;
    }
    return pick_weighted(weights, count, NULL);
}

/* ============================================================ */

const ae_reliquary_main_prop_t *ae_gamedepot_get_random_relic_main_prop(
    ae_s32 depot)
{
    ae_relic_main_prop_bucket_t *bucket = main_prop_bucket_for(depot);
    ae_size index;

    if (!bucket || bucket->count == 0) {
        return NULL;
    }
    index = pick_weighted(bucket->weights, bucket->count, NULL);
    if (index == AE_GAMEDEPOT_NO_INDEX) {
        return NULL;
    }
    return bucket->props[index];
}

ae_error_t ae_gamedepot_get_relic_main_prop_list(
    ae_s32 depot, const ae_reliquary_main_prop_t *const **out_list,
    ae_size *out_count)
{
    ae_relic_main_prop_bucket_t *bucket;

    if (!out_list || !out_count) {
        return AE_ERR_INVALID_ARG;
    }
    *out_list = NULL;
    *out_count = 0;
    bucket = main_prop_bucket_for(depot);
    if (bucket) {
        *out_list = bucket->props;
        *out_count = bucket->count;
    }
    return AE_OK;
}

ae_error_t ae_gamedepot_get_relic_affix_list(
    ae_s32 depot, const ae_reliquary_affix_t *const **out_list,
    ae_size *out_count)
{
    ae_relic_affix_bucket_t *bucket;

    if (!out_list || !out_count) {
        return AE_ERR_INVALID_ARG;
    }
    *out_list = NULL;
    *out_count = 0;
    bucket = affix_bucket_for(depot);
    if (bucket) {
        *out_list = bucket->affixes;
        *out_count = bucket->count;
    }
    return AE_OK;
}

/* ============================================================ */