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
 * @file ae_inventory.h
 *
 * @brief Player inventory: the item model and the item store.
 *
 * The game item mirrors the reference runtime item: an item id, a
 * count, a registry-owned item-data pointer and the equipment state
 * (level, affixes, relic props). Relic creation consumes the depot
 * rolls and affix filters exactly like the reference constructor.
 *
 * The inventory mirrors the reference store: a guid-keyed item map in
 * front of per-item-type tabs. Two tab kinds exist - equipment tabs
 * keep a plain set and answer id lookups with NULL, material tabs key
 * by item id for stacking. Virtual currencies live as small balances
 * on the inventory until the player plumbing lands.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_INVENTORY_H
#define AE_INVENTORY_H

#include <aetheris/ae_error.h>
#include <aetheris/ae_gamedata.h>
#include <aetheris/ae_props.h>
#include <aetheris/ae_types.h>

/* ============================================================ */

/* Item id + count pair, mirrors ItemParamData. */
typedef struct ae_item_param {
    ae_u32 id;
    ae_s32 count;
} ae_item_param_t;

/*
 * A runtime item. `item_data` is a registry-owned pointer; the item
 * never frees it. Owned arrays (affixes, relic sub-stats) are heap
 * owned and released by `ae_game_item_destroy`.
 */
typedef struct ae_game_item {
    ae_s32 owner_id;
    ae_u32 item_id;
    ae_s32 count;
    ae_u64 guid; /* player-unique id */

    const ae_item_data_t *item_data;

    /* Equip state (weapon and relic). */
    ae_s32 level;
    ae_s32 exp;
    ae_s32 total_exp;
    ae_s32 promote_level;
    ae_bool locked;

    /* Weapon. */
    ae_s32 *affixes;
    ae_size affix_count;
    ae_s32 refinement;

    /* Relic. */
    ae_s32 main_prop_id;
    ae_s32 *append_prop_ids;
    ae_size append_prop_count;

    ae_s32 equip_character;
    ae_bool is_new;
} ae_game_item_t;

/**
 * ae_game_item_from_data - construct an item from item data.
 * @item: item to initialise (must not be live).
 * @data: registry-owned item data.
 * @count: requested stack count, clamped by the model rules.
 *
 * Mirrors the reference constructor for every item type: virtual
 * items keep the raw count, weapons become a fresh level-one copy
 * with their skill affixes, relics become a level-one copy with a
 * rolled main property and generated sub-stats, everything else is
 * clamped to the stack limit.
 *
 * Returns AE_OK or AE_ERR_OUT_OF_MEMORY / AE_ERR_OVERFLOW. The item
 * must be destroyed with `ae_game_item_destroy` afterwards.
 */
ae_error_t ae_game_item_from_data(ae_game_item_t *item,
                                  const ae_item_data_t *data,
                                  ae_s32 count);

/**
 * ae_game_item_from_id - construct an item by id.
 * @item: item to initialise.
 * @item_id: inventory item id.
 * @count: requested stack count.
 *
 * Returns AE_ERR_NOT_FOUND when the item id has no item data.
 */
ae_error_t ae_game_item_from_id(ae_game_item_t *item, ae_u32 item_id,
                                ae_s32 count);

/**
 * ae_game_item_destroy - release owned memory.
 */
void ae_game_item_destroy(ae_game_item_t *item);

/**
 * ae_game_item_get_type - the item's item type.
 */
ae_item_type_t ae_game_item_get_type(const ae_game_item_t *item);

/**
 * ae_game_item_get_equip_slot - the equip slot value of the item.
 */
ae_s32 ae_game_item_get_equip_slot(const ae_game_item_t *item);

/**
 * ae_game_item_is_equipped - bound to a character?
 */
ae_bool ae_game_item_is_equipped(const ae_game_item_t *item);

/**
 * ae_game_item_is_destroyable - not locked and not equipped?
 */
ae_bool ae_game_item_is_destroyable(const ae_game_item_t *item);

/**
 * ae_game_item_get_min_promote_level - minimum promote level by level.
 */
ae_s32 ae_game_item_get_min_promote_level(ae_s32 level);

/**
 * ae_game_item_add_append_props - roll @quantity relic sub-stats.
 */
void ae_game_item_add_append_props(ae_game_item_t *item, ae_s32 quantity);

/* ============================================================ */

/* Inventory tab kinds: equipment tabs are a set, material tabs are a
 * map keyed by item id. */
typedef enum ae_inventory_tab_kind {
    AE_TAB_EQUIP,
    AE_TAB_MATERIAL,
} ae_inventory_tab_kind_t;

typedef struct ae_inventory_tab {
    ae_inventory_tab_kind_t kind;
    ae_s32 max_capacity;
    /* Material tab: item id -> item. */
    struct ae_inventory_mat_slot {
        ae_u32 key;
        ae_u8 state; /* 0 empty, 1 used, 2 tombstone */
        ae_game_item_t *value;
    } * slots;
    ae_size slot_capacity;
    ae_size tab_size;
    /* Equip tab: item pointer array. */
    ae_game_item_t **equip;
    ae_size equip_count;
    ae_size equip_capacity;
} ae_inventory_tab_t;

typedef struct ae_inventory {
    ae_s32 owner_id;
    ae_u64 next_guid;
    /* Store: guid -> item, owns every item. */
    struct ae_inventory_item_slot {
        ae_u64 key;
        ae_u8 state; /* 0 empty, 1 used, 2 tombstone */
        ae_game_item_t *value;
    } * store;
    ae_size store_capacity;
    ae_size store_size;
    /* Tabs indexed by item-type value. */
    ae_inventory_tab_t tabs[AE_ITEM_FURNITURE + 1];
    /* Virtual balances: { id, value } pairs. */
    struct {
        ae_u32 id;
        ae_s32 value;
    } balances[8];
    ae_size balance_count;
} ae_inventory_t;

/**
 * ae_inventory_init - initialise an empty inventory.
 * @inv: inventory to initialise.
 * @owner_id: owning account uid.
 *
 * Tabs come up with the reference default capacity (2000 items per
 * type). The store starts empty and is ready for use.
 */
ae_error_t ae_inventory_init(ae_inventory_t *inv, ae_s32 owner_id);

/**
 * ae_inventory_destroy - release every stored item and the tabs.
 *
 * Safe on a zeroed inventory.
 */
void ae_inventory_destroy(ae_inventory_t *inv);

/**
 * ae_inventory_alloc_guid - hand out one fresh item guid.
 */
ae_u64 ae_inventory_alloc_guid(ae_inventory_t *inv);

/**
 * ae_inventory_get_item_by_guid - lookup by item guid.
 */
ae_game_item_t *ae_inventory_get_item_by_guid(ae_inventory_t *inv,
                                              ae_u64 guid);

/**
 * ae_inventory_get_first_item - first stored item with an id.
 */
ae_game_item_t *ae_inventory_get_first_item(ae_inventory_t *inv,
                                            ae_u32 item_id);

/**
 * ae_inventory_get_item_by_id - tab lookup of a stackable item.
 */
ae_game_item_t *ae_inventory_get_item_by_id(ae_inventory_t *inv,
                                            ae_u32 item_id);

/**
 * ae_inventory_get_item_count_by_id - stacked count of an item id.
 */
ae_s32 ae_inventory_get_item_count_by_id(ae_inventory_t *inv,
                                         ae_u32 item_id);

/**
 * ae_inventory_get_tab_count - live items in a type's tab.
 */
ae_s32 ae_inventory_get_tab_count(ae_inventory_t *inv, ae_item_type_t type);

/**
 * ae_inventory_add_item - add an item by id and count.
 *
 * Creates the item through the model rules, then stores it via
 * `ae_inventory_add_game_item`. Returns true when the item landed in
 * the inventory.
 */
ae_bool ae_inventory_add_item(ae_inventory_t *inv, ae_u32 item_id,
                              ae_s32 count);

/**
 * ae_inventory_add_game_item - put an item into the inventory.
 * @inv: inventory.
 * @item: heap-owned item; ownership transfers to the inventory on
 *        success, and the item is freed on failure.
 *
 * Mirrors the reference add flow: use-on-gain items are consumed
 * without being stored, weapon and relic stacks are normalised,
 * material tabs merge stacks up to the stack limit, and full tabs
 * reject the item. Returns the item now owned by the inventory (the
 * merged stack for materials), or NULL when nothing was stored.
 */
ae_game_item_t *ae_inventory_add_game_item(ae_inventory_t *inv,
                                           ae_game_item_t *item);

/**
 * ae_inventory_add_item_params - add a batch of item params.
 *
 * Returns the number of items that were stored.
 */
ae_size ae_inventory_add_item_params(ae_inventory_t *inv,
                                     const ae_item_param_t *params,
                                     ae_size count);

/**
 * ae_inventory_remove_item - remove a stored item by guid.
 */
ae_bool ae_inventory_remove_item(ae_inventory_t *inv, ae_u64 guid,
                                 ae_s32 count);

/**
 * ae_inventory_remove_item_by_id - remove count of the first item
 * with @item_id.
 */
ae_bool ae_inventory_remove_item_by_id(ae_inventory_t *inv,
                                       ae_u32 item_id, ae_s32 count);

/**
 * ae_inventory_has_item - check a player holds at least @count of an
 * item, or exactly @count when @enforce.
 */
ae_bool ae_inventory_has_item(ae_inventory_t *inv, ae_u32 item_id,
                              ae_s32 count, ae_bool enforce);

/**
 * ae_inventory_pay_item - spend an item, virtual or stack.
 *
 * Fails without removing anything when the player does not hold the
 * cost. Virtual currencies come from the balance table; anything else
 * is paid from the material tab.
 */
ae_bool ae_inventory_pay_item(ae_inventory_t *inv, ae_u32 item_id,
                              ae_s32 count);

/**
 * ae_inventory_set_balance - set a virtual currency balance.
 * @inv: inventory.
 * @item_id: virtual item id (primogems 201, mora 202, and friends).
 * @value: new balance.
 */
ae_error_t ae_inventory_set_balance(ae_inventory_t *inv, ae_u32 item_id,
                                    ae_s32 value);

/**
 * ae_inventory_get_balance - read a virtual currency balance.
 */
ae_s32 ae_inventory_get_balance(ae_inventory_t *inv, ae_u32 item_id);

/**
 * ae_inventory_virtual_count - currency balance or material stack.
 */
ae_s32 ae_inventory_virtual_count(ae_inventory_t *inv, ae_u32 item_id);

/* ============================================================ */

#endif /* AE_INVENTORY_H */