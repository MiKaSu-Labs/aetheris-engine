/*
 *                ______ _______ _    _ ______ _____  _____  _____
 *          /\   |  ____|__   __| |  | |  ____|  __ \|_   _|/ ____|
 *         /  \  | |__     | |  | |__| | |__  | |__) | | | | (___
 *        / /\ \ |  __|    | |  |  __  |  __| |  _  /  | | |  \___ \
 *       / ____ \| |____   | |  |  |  | | |____| | \ \ _| |_ ____) |
 *      /_/    \_\______|  |_|  |_|  |_|______|_|  \_\_____|_____/
 *
 * @name Aetheris
 *
 * @file ae_inventory.c
 *
 * @brief Inventory runtime: item constructors and the store/tab model
 *        that owns every item handed to it.
 *
 * Item and inventory instances are built against `ae_gamedata.h`
 * item/reliquary registries and fed through the shared weighted depot
 * (`ae_gamedepot.h`) whenever a relic rolls its main property or
 * sub-stats.
 */

#include <aetheris/ae_inventory.h>
#include <aetheris/ae_gamedata.h>
#include <aetheris/ae_gamedepot.h>
#include <aetheris/ae_props.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================ */

/* Owner sentinel used before an item is placed into an inventory. */
#define AE_GAME_ITEM_NO_OWNER ((ae_s32)0)

/* Balance-registry virtual item id for the material stack clamp. */
#define AE_INVENTORY_VIRTUAL_PRIMOGEM ((ae_u32)201)
#define AE_INVENTORY_VIRTUAL_MORA ((ae_u32)202)

/* Slots: 0 empty, 1 used, 2 tombstone. */
#define AE_SLOT_EMPTY ((ae_u8)0)
#define AE_SLOT_USED ((ae_u8)1)
#define AE_SLOT_TOMBSTONE ((ae_u8)2)

ae_error_t ae_game_item_from_data(ae_game_item_t *item,
                                  const ae_item_data_t *data,
                                  ae_s32 count)
{
    ae_size i;
    ae_size affix_count;
    const ae_reliquary_main_prop_t *rolled;
    const ae_reliquary_affix_t *const *candidates;
    ae_size candidate_count;
    ae_size pick;

    if (!item || !data) {
        return AE_ERR_INVALID_ARG;
    }
    if (count <= 0) {
        return AE_ERR_INVALID_ARG;
    }
    memset(item, 0, sizeof(*item));
    item->owner_id = AE_GAME_ITEM_NO_OWNER;
    item->item_id = data->id;
    item->count = count;
    item->item_data = data;
    item->level = 1;
    item->is_new = true;

    switch (data->item_type) {
    case AE_ITEM_VIRTUAL:
        break; /* keep the raw count, no extra state */
    case AE_ITEM_MATERIAL:
        if (data->stack_limit > 0 && item->count > data->stack_limit) {
            item->count = data->stack_limit;
        }
        break;
    case AE_ITEM_RELIQUARY:
        /* Relic: roll the main property through the depot. */
        rolled = ae_gamedepot_get_random_relic_main_prop(
            data->main_prop_depot_id);
        if (rolled) {
            item->main_prop_id = (ae_s32)rolled->id;
        }
        /* Roll the requested number of sub-stats. */
        ae_gamedepot_get_relic_affix_list(data->append_prop_depot_id,
                                          &candidates,
                                          &candidate_count);
        if (candidate_count > 0) {
            ae_u32 *weights = (ae_u32 *)malloc(candidate_count *
                                               sizeof(ae_u32));
            if (!weights) {
                ae_game_item_destroy(item);
                return AE_ERR_OUT_OF_MEMORY;
            }
            for (i = 0; i < candidate_count; i++) {
                weights[i] = (ae_u32)candidates[i]->weight;
            }
            item->append_prop_count = 0;
            item->append_prop_ids = (ae_s32 *)malloc(
                (ae_size)data->append_prop_num * sizeof(ae_s32));
            if (!item->append_prop_ids) {
                free(weights);
                ae_game_item_destroy(item);
                return AE_ERR_OUT_OF_MEMORY;
            }
            for (i = 0; i < (ae_size)data->append_prop_num &&
                        i < candidate_count; i++) {
                pick = ae_gamedepot_weighted_index(weights,
                                                   candidate_count);
                if (pick == AE_GAMEDEPOT_NO_INDEX) {
                    break;
                }
                item->append_prop_ids[item->append_prop_count++] =
                    (ae_s32)candidates[pick]->id;
            }
            free(weights);
        }
        item->count = 1;
        break;
    case AE_ITEM_WEAPON:
        /* Weapon: fresh level-one copy with its skill affixes. */
        affix_count = data->skill_affix_count;
        if (affix_count > 0) {
            ae_size affix_bytes;
            if (!ae_mul_overflow_uz((ae_size)affix_count,
                                    sizeof(ae_s32),
                                    &affix_bytes)) {
                ae_game_item_destroy(item);
                return AE_ERR_OVERFLOW;
            }
            item->affixes = (ae_s32 *)malloc(affix_bytes);
            if (!item->affixes) {
                ae_game_item_destroy(item);
                return AE_ERR_OUT_OF_MEMORY;
            }
            for (i = 0; i < affix_count; i++) {
                item->affixes[i] = data->skill_affix[i];
            }
            item->affix_count = affix_count;
        }
        item->count = 1;
        break;
    default:
        item->level = 1;
        break;
    }
    return AE_OK;
}
