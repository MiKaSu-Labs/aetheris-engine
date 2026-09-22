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
 * @file test_ae_game_constants.c
 *
 * @brief Unit tests for the game constants and illegal-item filters.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <aetheris/ae_game_constants.h>

#include "ae_test.h"

/* ============================================================ */

static void test_version_and_start_position(void) {
    const ae_game_position_t *sp = ae_game_start_position();

    AE_TEST_ASSERT(sp != NULL);
    AE_TEST_ASSERT(sp->x == 2747);
    AE_TEST_ASSERT(sp->y == 194);
    AE_TEST_ASSERT(sp->z == -1719);
}

static void test_default_ability_hashes(void) {
    static const ae_s32 expected[] = {
        -1988905289, -1189338119, -523440627, 100636247,  1564404322,
        497711942,   825255509,   1771196189, -1626113073, 1302367693,
        -1368383809, -255004597,  804339155,  -1045959981, -1201541048,
        -788401632,  2029613654,  -39184011,  1235858629, 1232926874};

    AE_TEST_ASSERT(ae_game_default_ability_count() == 20);
    for (ae_size i = 0; i < 20; i++) {
        AE_TEST_ASSERT(ae_game_default_ability_hash(i) == expected[i]);
    }

    AE_TEST_ASSERT(ae_game_default_ability_hash(20) == 0);
    AE_TEST_ASSERT(ae_game_default_ability_hash(1000) == 0);
    AE_TEST_ASSERT(ae_game_default_ability_name() == 1178079449);
}

static void test_scene_tags(void) {
    static const ae_s32 scene3[] = {152, 153, 1094, 1164, 1166};
    static const ae_s32 scene4[] = {106, 109, 117};

    AE_TEST_ASSERT(ae_game_scene_tag_count(3) == 5);
    AE_TEST_ASSERT(ae_game_scene_tag_count(4) == 3);
    AE_TEST_ASSERT(ae_game_scene_tag_count(9) == 86);
    AE_TEST_ASSERT(ae_game_scene_tag_count(10) == 8);
    AE_TEST_ASSERT(ae_game_scene_tag_count(99) == 0);

    const ae_s32 *tags = ae_game_scene_tags(3);
    AE_TEST_ASSERT(tags != NULL);
    AE_TEST_ASSERT(tags[0] == scene3[0] && tags[4] == scene3[4]);

    tags = ae_game_scene_tags(4);
    AE_TEST_ASSERT(tags != NULL);
    AE_TEST_ASSERT(tags[0] == scene4[0] && tags[2] == scene4[2]);

    tags = ae_game_scene_tags(9);
    AE_TEST_ASSERT(tags != NULL);
    AE_TEST_ASSERT(tags[0] == 1000);
    AE_TEST_ASSERT(tags[85] == 1085);

    tags = ae_game_scene_tags(10);
    AE_TEST_ASSERT(tags != NULL);
    AE_TEST_ASSERT(tags[0] == 1261);
    AE_TEST_ASSERT(tags[7] == 1268);

    AE_TEST_ASSERT(ae_game_scene_tags(99) == NULL);
}

static void test_illegal_sets(void) {
    const ae_sparse_set_t *weapons;
    const ae_sparse_set_t *relics;
    const ae_sparse_set_t *items;

    AE_TEST_ASSERT(ae_game_constants_init() == AE_OK);
    /* Re-init is idempotent. */
    AE_TEST_ASSERT(ae_game_constants_init() == AE_OK);

    weapons = ae_game_illegal_weapons();
    relics = ae_game_illegal_relics();
    items = ae_game_illegal_items();
    AE_TEST_ASSERT(weapons != NULL);
    AE_TEST_ASSERT(relics != NULL);
    AE_TEST_ASSERT(items != NULL);

    /* Weapons: single values and ranges. */
    AE_TEST_ASSERT(ae_sparse_set_contains(weapons, 10000));
    AE_TEST_ASSERT(ae_sparse_set_contains(weapons, 10008));
    AE_TEST_ASSERT(ae_sparse_set_contains(weapons, 11411));
    AE_TEST_ASSERT(ae_sparse_set_contains(weapons, 11506));
    AE_TEST_ASSERT(ae_sparse_set_contains(weapons, 12505));
    AE_TEST_ASSERT(ae_sparse_set_contains(weapons, 15506));
    AE_TEST_ASSERT(!ae_sparse_set_contains(weapons, 10009));
    AE_TEST_ASSERT(!ae_sparse_set_contains(weapons, 20000));

    /* Relics. */
    AE_TEST_ASSERT(ae_sparse_set_contains(relics, 20001));
    AE_TEST_ASSERT(ae_sparse_set_contains(relics, 23300));
    AE_TEST_ASSERT(ae_sparse_set_contains(relics, 23340));
    AE_TEST_ASSERT(ae_sparse_set_contains(relics, 23383));
    AE_TEST_ASSERT(ae_sparse_set_contains(relics, 78310));
    AE_TEST_ASSERT(ae_sparse_set_contains(relics, 99554));
    AE_TEST_ASSERT(!ae_sparse_set_contains(relics, 23341));
    AE_TEST_ASSERT(!ae_sparse_set_contains(relics, 23382));

    /* Items. */
    AE_TEST_ASSERT(ae_sparse_set_contains(items, 100086));
    AE_TEST_ASSERT(ae_sparse_set_contains(items, 100100));
    AE_TEST_ASSERT(ae_sparse_set_contains(items, 101000));
    AE_TEST_ASSERT(ae_sparse_set_contains(items, 101306));
    AE_TEST_ASSERT(ae_sparse_set_contains(items, 104000));
    AE_TEST_ASSERT(ae_sparse_set_contains(items, 107011));
    AE_TEST_ASSERT(ae_sparse_set_contains(items, 129999));
    AE_TEST_ASSERT(ae_sparse_set_contains(items, 200899));
    AE_TEST_ASSERT(ae_sparse_set_contains(items, 220050));
    AE_TEST_ASSERT(!ae_sparse_set_contains(items, 100099));
    AE_TEST_ASSERT(!ae_sparse_set_contains(items, 101305));
    AE_TEST_ASSERT(!ae_sparse_set_contains(items, 220053));

    ae_game_constants_destroy();
    ae_game_constants_destroy(); /* idempotent */
}

/* ============================================================ */

int main(void) {
    static const ae_test_case_t suite[] = {
        {"version_and_start_position", test_version_and_start_position},
        {"default_ability_hashes", test_default_ability_hashes},
        {"scene_tags", test_scene_tags},
        {"illegal_sets", test_illegal_sets},
    };

    ae_u32 failed = AE_TEST_SUITE(suite);

    return (int)failed;
}