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
 * @file test_ae_props.c
 *
 * @brief Unit tests for the props/enum lookup module.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <aetheris/ae_props.h>

#include "ae_test.h"

/* ============================================================ */

static void test_fight_prop_id_lookup(void) {
    AE_TEST_ASSERT(ae_fight_prop_from_id(0) == AE_FIGHT_PROP_NONE);
    AE_TEST_ASSERT(ae_fight_prop_from_id(1) == AE_FIGHT_PROP_BASE_HP);
    AE_TEST_ASSERT(ae_fight_prop_from_id(23) == AE_FIGHT_PROP_CHARGE_EFFICIENCY);
    AE_TEST_ASSERT(ae_fight_prop_from_id(28) == AE_FIGHT_PROP_ELEMENT_MASTERY);
    AE_TEST_ASSERT(ae_fight_prop_from_id(1010) == AE_FIGHT_PROP_CUR_HP);
    AE_TEST_ASSERT(ae_fight_prop_from_id(2000) == AE_FIGHT_PROP_MAX_HP);
    AE_TEST_ASSERT(ae_fight_prop_from_id(3024) ==
                   AE_FIGHT_PROP_NONEXTRA_PHYSICAL_ADD_HURT);
    AE_TEST_ASSERT(ae_fight_prop_from_id(9999) == AE_FIGHT_PROP_NONE);
    AE_TEST_ASSERT(ae_fight_prop_from_id(-1) == AE_FIGHT_PROP_NONE);

    AE_TEST_ASSERT(ae_fight_prop_from_name("FIGHT_PROP_HP") == AE_FIGHT_PROP_HP);
    AE_TEST_ASSERT(ae_fight_prop_from_name("FIGHT_PROP_CRITICAL_HURT") ==
                   AE_FIGHT_PROP_CRITICAL_HURT);
    AE_TEST_ASSERT(ae_fight_prop_from_name("bogus") == AE_FIGHT_PROP_NONE);
    AE_TEST_ASSERT(ae_fight_prop_from_name(NULL) == AE_FIGHT_PROP_NONE);
}

static void test_fight_prop_short_names(void) {
    AE_TEST_ASSERT(ae_fight_prop_from_short_name("hp") == AE_FIGHT_PROP_HP);
    AE_TEST_ASSERT(ae_fight_prop_from_short_name("atk%") ==
                   AE_FIGHT_PROP_ATTACK_PERCENT);
    AE_TEST_ASSERT(ae_fight_prop_from_short_name("em") ==
                   AE_FIGHT_PROP_ELEMENT_MASTERY);
    AE_TEST_ASSERT(ae_fight_prop_from_short_name("er") ==
                   AE_FIGHT_PROP_CHARGE_EFFICIENCY);
    AE_TEST_ASSERT(ae_fight_prop_from_short_name("cdmg") ==
                   AE_FIGHT_PROP_CRITICAL_HURT);
    AE_TEST_ASSERT(ae_fight_prop_from_short_name("crate") ==
                   AE_FIGHT_PROP_CRITICAL);
    AE_TEST_ASSERT(ae_fight_prop_from_short_name("phys%") ==
                   AE_FIGHT_PROP_PHYSICAL_ADD_HURT);
    AE_TEST_ASSERT(ae_fight_prop_from_short_name("dendro%") ==
                   AE_FIGHT_PROP_GRASS_ADD_HURT);
    AE_TEST_ASSERT(ae_fight_prop_from_short_name("resall") ==
                   AE_FIGHT_PROP_SUB_HURT);
    AE_TEST_ASSERT(ae_fight_prop_from_short_name("resphys") ==
                   AE_FIGHT_PROP_PHYSICAL_SUB_HURT);
    AE_TEST_ASSERT(ae_fight_prop_from_short_name("unknown") ==
                   AE_FIGHT_PROP_NONE);
}

static void test_fight_prop_is_percentage(void) {
    AE_TEST_ASSERT(!ae_fight_prop_is_percentage(AE_FIGHT_PROP_BASE_HP));
    AE_TEST_ASSERT(!ae_fight_prop_is_percentage(AE_FIGHT_PROP_HP));
    AE_TEST_ASSERT(!ae_fight_prop_is_percentage(AE_FIGHT_PROP_ATTACK));
    AE_TEST_ASSERT(!ae_fight_prop_is_percentage(AE_FIGHT_PROP_DEFENSE));
    AE_TEST_ASSERT(!ae_fight_prop_is_percentage(AE_FIGHT_PROP_CUR_HP));
    AE_TEST_ASSERT(!ae_fight_prop_is_percentage(AE_FIGHT_PROP_MAX_HP));
    AE_TEST_ASSERT(!ae_fight_prop_is_percentage(AE_FIGHT_PROP_CUR_ATTACK));
    AE_TEST_ASSERT(!ae_fight_prop_is_percentage(AE_FIGHT_PROP_CUR_DEFENSE));
    AE_TEST_ASSERT(!ae_fight_prop_is_percentage(
        AE_FIGHT_PROP_CUR_FIRE_ENERGY));

    AE_TEST_ASSERT(ae_fight_prop_is_percentage(AE_FIGHT_PROP_HP_PERCENT));
    AE_TEST_ASSERT(ae_fight_prop_is_percentage(AE_FIGHT_PROP_CRITICAL));
    AE_TEST_ASSERT(ae_fight_prop_is_percentage(
        AE_FIGHT_PROP_CHARGE_EFFICIENCY));
    AE_TEST_ASSERT(ae_fight_prop_is_percentage(
        AE_FIGHT_PROP_ELEMENT_MASTERY));
    AE_TEST_ASSERT(ae_fight_prop_is_percentage(AE_FIGHT_PROP_ANTI_CRITICAL));
}

static void test_fight_prop_compound(void) {
    ae_fight_compound_prop_t cp;

    AE_TEST_ASSERT(ae_fight_prop_compound_get(AE_FIGHT_PROP_MAX_HP, &cp));
    AE_TEST_ASSERT(cp.result == AE_FIGHT_PROP_MAX_HP);
    AE_TEST_ASSERT(cp.base == AE_FIGHT_PROP_BASE_HP);
    AE_TEST_ASSERT(cp.percent == AE_FIGHT_PROP_HP_PERCENT);
    AE_TEST_ASSERT(cp.flat == AE_FIGHT_PROP_HP);

    AE_TEST_ASSERT(ae_fight_prop_compound_get(AE_FIGHT_PROP_CUR_ATTACK, &cp));
    AE_TEST_ASSERT(cp.base == AE_FIGHT_PROP_BASE_ATTACK);
    AE_TEST_ASSERT(cp.percent == AE_FIGHT_PROP_ATTACK_PERCENT);
    AE_TEST_ASSERT(cp.flat == AE_FIGHT_PROP_ATTACK);

    AE_TEST_ASSERT(ae_fight_prop_compound_get(AE_FIGHT_PROP_CUR_DEFENSE, &cp));
    AE_TEST_ASSERT(cp.base == AE_FIGHT_PROP_BASE_DEFENSE);
    AE_TEST_ASSERT(cp.percent == AE_FIGHT_PROP_DEFENSE_PERCENT);
    AE_TEST_ASSERT(cp.flat == AE_FIGHT_PROP_DEFENSE);

    AE_TEST_ASSERT(!ae_fight_prop_compound_get(AE_FIGHT_PROP_CRITICAL, &cp));
    AE_TEST_ASSERT(!ae_fight_prop_compound_get(AE_FIGHT_PROP_NONE, &cp));
    AE_TEST_ASSERT(!ae_fight_prop_compound_get(AE_FIGHT_PROP_MAX_HP, NULL));
}

static void test_element_props(void) {
    ae_elem_info_t info;

    AE_TEST_ASSERT(ae_elem_from_value(0) == AE_ELEM_NONE);
    AE_TEST_ASSERT(ae_elem_from_value(1) == AE_ELEM_FIRE);
    AE_TEST_ASSERT(ae_elem_from_value(3) == AE_ELEM_GRASS);
    AE_TEST_ASSERT(ae_elem_from_value(8) == AE_ELEM_ROCK);
    AE_TEST_ASSERT(ae_elem_from_value(255) == AE_ELEM_DEFAULT);
    AE_TEST_ASSERT(ae_elem_from_value(77) == AE_ELEM_NONE);

    AE_TEST_ASSERT(ae_elem_from_name("Fire") == AE_ELEM_FIRE);
    AE_TEST_ASSERT(ae_elem_from_name("Water") == AE_ELEM_WATER);
    AE_TEST_ASSERT(ae_elem_from_name("Electric") == AE_ELEM_ELECTRIC);
    AE_TEST_ASSERT(ae_elem_from_name("Ice") == AE_ELEM_ICE);
    AE_TEST_ASSERT(ae_elem_from_name("Wind") == AE_ELEM_WIND);
    AE_TEST_ASSERT(ae_elem_from_name("Rock") == AE_ELEM_ROCK);
    AE_TEST_ASSERT(ae_elem_from_name("Default") == AE_ELEM_DEFAULT);
    AE_TEST_ASSERT(ae_elem_from_name("Frozen") == AE_ELEM_FROZEN);
    AE_TEST_ASSERT(ae_elem_from_name("nope") == AE_ELEM_NONE);

    AE_TEST_ASSERT(ae_elem_value(AE_ELEM_FIRE) == 1);
    AE_TEST_ASSERT(ae_elem_value(AE_ELEM_DEFAULT) == 255);

    /* Fire energy props. */
    AE_TEST_ASSERT(ae_elem_info_get(AE_ELEM_FIRE, &info));
    AE_TEST_ASSERT(info.cur_energy_prop == 1000);
    AE_TEST_ASSERT(info.max_energy_prop == 70);
    AE_TEST_ASSERT(info.team_resonance_id == 10101);
    AE_TEST_ASSERT(info.depot_index == 1);
    AE_TEST_ASSERT(info.config_name != NULL);

    /* Ice uses the ice energy props and its own resonance. */
    AE_TEST_ASSERT(ae_elem_info_get(AE_ELEM_ICE, &info));
    AE_TEST_ASSERT(info.cur_energy_prop == 1005);
    AE_TEST_ASSERT(info.max_energy_prop == 75);
    AE_TEST_ASSERT(info.team_resonance_id == 10601);

    /* Wind/rock energy props. */
    AE_TEST_ASSERT(ae_elem_info_get(AE_ELEM_WIND, &info));
    AE_TEST_ASSERT(info.cur_energy_prop == 1004);
    AE_TEST_ASSERT(info.max_energy_prop == 74);
    AE_TEST_ASSERT(ae_elem_info_get(AE_ELEM_ROCK, &info));
    AE_TEST_ASSERT(info.cur_energy_prop == 1006);
    AE_TEST_ASSERT(info.max_energy_prop == 76);

    /* No-resonance elements. */
    AE_TEST_ASSERT(ae_elem_info_get(AE_ELEM_FROZEN, &info));
    AE_TEST_ASSERT(info.team_resonance_id == 0);
    AE_TEST_ASSERT(info.config_name == NULL);

    /* All-different resonance on Default element. */
    AE_TEST_ASSERT(ae_elem_info_get(AE_ELEM_DEFAULT, &info));
    AE_TEST_ASSERT(info.team_resonance_id == 10801);

    /* Unknown element gets the NONE metadata. */
    AE_TEST_ASSERT(!ae_elem_info_get((ae_elem_t)77, &info));
    AE_TEST_ASSERT(!ae_elem_info_get(AE_ELEM_FIRE, NULL));
}

static void test_player_props(void) {
    ae_player_prop_range_t range;

    AE_TEST_ASSERT(ae_player_prop_from_id(1001) == AE_PLAYER_PROP_EXP);
    AE_TEST_ASSERT(ae_player_prop_from_id(4001) == AE_PLAYER_PROP_LEVEL);
    AE_TEST_ASSERT(ae_player_prop_from_id(10013) == AE_PLAYER_PROP_PLAYER_LEVEL);
    AE_TEST_ASSERT(ae_player_prop_from_id(10020) == AE_PLAYER_PROP_PLAYER_RESIN);
    AE_TEST_ASSERT(ae_player_prop_from_id(10050) ==
                   AE_PLAYER_PROP_DIVE_CUR_STAMINA);
    AE_TEST_ASSERT(ae_player_prop_from_id(0) == AE_PLAYER_PROP_NONE);
    AE_TEST_ASSERT(ae_player_prop_from_id(999) == AE_PLAYER_PROP_NONE);

    AE_TEST_ASSERT(ae_player_prop_id(AE_PLAYER_PROP_PLAYER_LEVEL) == 10013);

    /* Bounded props. */
    AE_TEST_ASSERT(ae_player_prop_range_get(AE_PLAYER_PROP_PLAYER_LEVEL,
                                            &range));
    AE_TEST_ASSERT(range.min == 1);
    AE_TEST_ASSERT(range.max == 60);
    AE_TEST_ASSERT(!range.dynamic);

    AE_TEST_ASSERT(ae_player_prop_range_get(AE_PLAYER_PROP_PLAYER_RESIN,
                                            &range));
    AE_TEST_ASSERT(range.min == 0);
    AE_TEST_ASSERT(range.max == 2000);

    AE_TEST_ASSERT(ae_player_prop_range_get(AE_PLAYER_PROP_MAX_STAMINA,
                                            &range));
    AE_TEST_ASSERT(range.max == 24000);

    AE_TEST_ASSERT(ae_player_prop_range_get(AE_PLAYER_PROP_MAX_SPRING_VOLUME,
                                            &range));
    AE_TEST_ASSERT(range.max == 8500000);

    AE_TEST_ASSERT(ae_player_prop_range_get(AE_PLAYER_PROP_PLAYER_WORLD_LEVEL,
                                            &range));
    AE_TEST_ASSERT(range.min == 0);
    AE_TEST_ASSERT(range.max == 8);

    /* Dynamic ranges are flagged. */
    AE_TEST_ASSERT(ae_player_prop_range_get(AE_PLAYER_PROP_CUR_SPRING_VOLUME,
                                            &range));
    AE_TEST_ASSERT(range.dynamic);
    AE_TEST_ASSERT(ae_player_prop_range_get(AE_PLAYER_PROP_CUR_PERSIST_STAMINA,
                                            &range));
    AE_TEST_ASSERT(range.dynamic);

    /* Unknown prop. */
    AE_TEST_ASSERT(!ae_player_prop_range_get((ae_player_prop_t)777, &range));
    AE_TEST_ASSERT(!ae_player_prop_range_get(AE_PLAYER_PROP_NONE, NULL));
}

static void test_simple_enums(void) {
    AE_TEST_ASSERT(ae_item_type_from_value(2) == AE_ITEM_MATERIAL);
    AE_TEST_ASSERT(ae_item_type_from_value(4) == AE_ITEM_WEAPON);
    AE_TEST_ASSERT(ae_item_type_from_value(3) == AE_ITEM_RELIQUARY);
    AE_TEST_ASSERT(ae_item_type_from_value(6) == AE_ITEM_FURNITURE);
    AE_TEST_ASSERT(ae_item_type_from_value(99) == AE_ITEM_NONE);
    AE_TEST_ASSERT(ae_item_type_from_name("ITEM_VIRTUAL") == AE_ITEM_VIRTUAL);
    AE_TEST_ASSERT(ae_item_type_value(AE_ITEM_WEAPON) == 4);

    AE_TEST_ASSERT(ae_material_type_from_value(11) ==
                   AE_MATERIAL_WEAPON_EXP_STONE);
    AE_TEST_ASSERT(ae_material_type_from_value(41) == AE_MATERIAL_ARANARA);
    AE_TEST_ASSERT(ae_material_type_from_value(46) == AE_MATERIAL_DESHRET_MANUAL);
    AE_TEST_ASSERT(ae_material_type_from_value(3) == AE_MATERIAL_NONE);
    AE_TEST_ASSERT(ae_material_type_from_name("MATERIAL_CONSUME") ==
                   AE_MATERIAL_CONSUME);
    AE_TEST_ASSERT(ae_material_type_value(AE_MATERIAL_NAMECARD) == 19);

    AE_TEST_ASSERT(ae_equip_type_from_value(6) == AE_EQUIP_WEAPON);
    AE_TEST_ASSERT(ae_equip_type_from_value(1) == AE_EQUIP_BRACER);
    AE_TEST_ASSERT(ae_equip_type_from_name("EQUIP_DRESS") == AE_EQUIP_DRESS);

    AE_TEST_ASSERT(ae_item_quality_from_value(5) == AE_QUALITY_ORANGE);
    AE_TEST_ASSERT(ae_item_quality_from_value(105) == AE_QUALITY_ORANGE_SP);
    AE_TEST_ASSERT(ae_item_quality_from_name("QUALITY_PURPLE") ==
                   AE_QUALITY_PURPLE);

    AE_TEST_ASSERT(ae_entity_type_from_value(1) == AE_ENTITY_AVATAR);
    AE_TEST_ASSERT(ae_entity_type_from_value(2) == AE_ENTITY_MONSTER);
    AE_TEST_ASSERT(ae_entity_type_from_value(12) == AE_ENTITY_NPC);
    AE_TEST_ASSERT(ae_entity_type_from_value(19) == AE_ENTITY_GADGET);
    AE_TEST_ASSERT(ae_entity_type_from_value(98) == AE_ENTITY_REGION);
    AE_TEST_ASSERT(ae_entity_type_from_value(99) == AE_ENTITY_PLACE_HOLDER);
    AE_TEST_ASSERT(ae_entity_type_from_value(200) == AE_ENTITY_NONE);
    AE_TEST_ASSERT(ae_entity_type_from_name("Monster") == AE_ENTITY_MONSTER);
    AE_TEST_ASSERT(ae_entity_type_from_name("PlaceHolder") ==
                   AE_ENTITY_PLACE_HOLDER);

    AE_TEST_ASSERT(ae_entity_type_from_id_type(0x01) == AE_ENTITY_AVATAR);
    AE_TEST_ASSERT(ae_entity_type_from_id_type(0x03) == AE_ENTITY_NPC);
    AE_TEST_ASSERT(ae_entity_type_from_id_type(0x05) == AE_ENTITY_REGION);
    AE_TEST_ASSERT(ae_entity_type_from_id_type(0x06) == AE_ENTITY_EQUIP);
    AE_TEST_ASSERT(ae_entity_type_from_id_type(0x09) == AE_ENTITY_TEAM);
    AE_TEST_ASSERT(ae_entity_type_from_id_type(0x0b) == AE_ENTITY_MP_LEVEL);
    AE_TEST_ASSERT(ae_entity_type_from_id_type(0x55) == AE_ENTITY_NONE);

    AE_TEST_ASSERT(ae_weapon_type_from_value(1) == AE_WEAPON_SWORD_ONE_HAND);
    AE_TEST_ASSERT(ae_weapon_type_from_value(10) == AE_WEAPON_CATALYST);
    AE_TEST_ASSERT(ae_weapon_type_from_value(13) == AE_WEAPON_POLE);
    AE_TEST_ASSERT(ae_weapon_type_from_value(77) == AE_WEAPON_NONE);
    AE_TEST_ASSERT(ae_weapon_type_from_name("WEAPON_CLAYMORE") ==
                   AE_WEAPON_CLAYMORE);

    AE_TEST_ASSERT(ae_scene_type_from_value(2) == AE_SCENE_DUNGEON);
    AE_TEST_ASSERT(ae_scene_type_from_value(9) == AE_SCENE_NONE);
    AE_TEST_ASSERT(ae_monster_type_from_value(2) == AE_MONSTER_BOSS);
    AE_TEST_ASSERT(ae_monster_type_from_value(3) == AE_MONSTER_ENV_ANIMAL);
    AE_TEST_ASSERT(ae_monster_type_from_value(9) == AE_MONSTER_NONE);
    AE_TEST_ASSERT(ae_life_state_from_value(2) == AE_LIFE_DEAD);
    AE_TEST_ASSERT(ae_life_state_from_value(1) == AE_LIFE_ALIVE);
    AE_TEST_ASSERT(ae_life_state_from_value(7) == AE_LIFE_NONE);
    AE_TEST_ASSERT(ae_climate_type_from_value(5) == AE_CLIMATE_SNOW);
    AE_TEST_ASSERT(ae_climate_type_from_value(6) == AE_CLIMATE_MIST);
    AE_TEST_ASSERT(ae_climate_type_from_value(9) == AE_CLIMATE_NONE);
    AE_TEST_ASSERT(ae_fetter_state_from_value(3) == AE_FETTER_FINISH);
    AE_TEST_ASSERT(ae_fetter_state_from_value(2) == AE_FETTER_NONE);
    AE_TEST_ASSERT(ae_enter_reason_from_value(1) == AE_ENTER_LOGIN);
    AE_TEST_ASSERT(ae_enter_reason_from_value(42) == AE_ENTER_TRANS_POINT);
    AE_TEST_ASSERT(ae_enter_reason_from_value(63) == AE_ENTER_DRAFT_TRANSFER);
    AE_TEST_ASSERT(ae_enter_reason_from_value(99) == AE_ENTER_NONE);
    AE_TEST_ASSERT(ae_grow_curve_from_value(12) == AE_GROW_CURVE_DEFENSE);
    AE_TEST_ASSERT(ae_grow_curve_from_value(1104) ==
                   AE_GROW_CURVE_ATTACK_104);
    AE_TEST_ASSERT(ae_grow_curve_from_value(2305) ==
                   AE_GROW_CURVE_CRITICAL_305);
    AE_TEST_ASSERT(ae_grow_curve_from_value(999) == AE_GROW_CURVE_NONE);
    AE_TEST_ASSERT(ae_server_buff_type_from_value(1) == AE_SERVER_BUFF_AVATAR);
    AE_TEST_ASSERT(ae_server_buff_type_from_value(3) == AE_SERVER_BUFF_TOWER);
    AE_TEST_ASSERT(ae_item_use_op_from_value(2) ==
                   AE_ITEM_USE_TRIGGER_ABILITY);
    AE_TEST_ASSERT(ae_item_use_op_from_value(46) ==
                   AE_ITEM_USE_ADD_REGIONAL_PLAY_VAR);
    AE_TEST_ASSERT(ae_item_use_op_from_value(99) == AE_ITEM_USE_NONE);
    AE_TEST_ASSERT(ae_item_use_target_from_value(4) ==
                   AE_ITEM_USE_TARGET_SPECIFY_ALIVE_AVATAR);
    AE_TEST_ASSERT(ae_refresh_type_from_value(3) == AE_REFRESH_WEEKLY);
    AE_TEST_ASSERT(ae_refresh_type_from_value(4) == AE_REFRESH_DAY_BEGIN_INTERVAL);
    AE_TEST_ASSERT(ae_refresh_type_from_value(8) == AE_REFRESH_NONE);
    AE_TEST_ASSERT(ae_activity_type_from_value(2202) == AE_ACTIVITY_MUSIC_GAME);
    AE_TEST_ASSERT(ae_activity_type_from_value(3203) ==
                   AE_ACTIVITY_EFFIGY_CHALLENGE_V2);
    AE_TEST_ASSERT(ae_activity_type_from_value(5) == AE_ACTIVITY_NONE);
    AE_TEST_ASSERT(ae_camp_target_type_from_value(4) == AE_CAMP_TARGET_SELF_CAMP);
    AE_TEST_ASSERT(ae_camp_target_type_from_value(7) ==
                   AE_CAMP_TARGET_ALLIANCE_INCLUDE_SELF);
    AE_TEST_ASSERT(ae_camp_target_type_from_value(9) == AE_CAMP_TARGET_NONE);
    AE_TEST_ASSERT(ae_battle_pass_refresh_type_from_value(2) ==
                   AE_BATTLE_PASS_REFRESH_SCHEDULE);
    AE_TEST_ASSERT(ae_battle_pass_refresh_type_from_value(7) ==
                   AE_BATTLE_PASS_REFRESH_DAILY);
    AE_TEST_ASSERT(ae_battle_pass_mission_status_from_value(3) ==
                   AE_BATTLE_PASS_MISSION_POINT_TAKEN);
    AE_TEST_ASSERT(ae_battle_pass_mission_status_from_value(9) ==
                   AE_BATTLE_PASS_MISSION_INVALID);
}

static void test_ability_hash(void) {
    AE_TEST_ASSERT(ae_ability_hash(NULL) == 0);
    AE_TEST_ASSERT(ae_ability_hash("") == 0);

    /* Known values from the reference implementation. */
    AE_TEST_ASSERT(ae_ability_hash("TeamResonance_Fire_Lv2") == -214370155);
    AE_TEST_ASSERT(ae_ability_hash("TeamResonance_Water_Lv2") == 512325970);
    AE_TEST_ASSERT(ae_ability_hash("TeamResonance_Grass_Lv2") == -457235075);
    AE_TEST_ASSERT(ae_ability_hash("TeamResonance_Electric_Lv2") == 663807720);
    AE_TEST_ASSERT(ae_ability_hash("TeamResonance_Ice_Lv2") == 554760522);
    AE_TEST_ASSERT(ae_ability_hash("TeamResonance_Wind_Lv2") == -514075341);
    AE_TEST_ASSERT(ae_ability_hash("TeamResonance_Rock_Lv2") == 1958705752);
    AE_TEST_ASSERT(ae_ability_hash("TeamResonance_AllDifferent") == 127390306);
    AE_TEST_ASSERT(ae_ability_hash("Default") != 0);
}

/* ============================================================ */

int main(void) {
    static const ae_test_case_t suite[] = {
        {"fight_prop_id_lookup", test_fight_prop_id_lookup},
        {"fight_prop_short_names", test_fight_prop_short_names},
        {"fight_prop_is_percentage", test_fight_prop_is_percentage},
        {"fight_prop_compound", test_fight_prop_compound},
        {"element_props", test_element_props},
        {"player_props", test_player_props},
        {"simple_enums", test_simple_enums},
        {"ability_hash", test_ability_hash},
    };

    ae_u32 failed = AE_TEST_SUITE(suite);

    return (int)failed;
}