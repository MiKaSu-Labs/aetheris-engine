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
 * @file test_ae_gamedata.c
 *
 * @brief Unit tests for the gamedata item registry.
 *
 * Item excel fixtures are written into a `test_resources` tree under
 * the working directory at runtime.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include "cJSON.h"

#include <aetheris/ae_gamedata.h>

#include "ae_test.h"

/* ============================================================ */

static void make_dir(const char *path)
{
#if defined(_WIN32)
    (void)_mkdir(path);
#else
    (void)mkdir(path, 0755);
#endif
}

static void write_file(const char *path, const char *text)
{
    FILE *fp = fopen(path, "wb");
    if (!fp) {
        AE_TEST_ASSERT(false);
        return;
    }
    fputs(text, fp);
    fclose(fp);
}

static void make_fixture_tree(void)
{
    make_dir("test_resources");
    make_dir("test_resources/Server");
    make_dir("test_resources/ExcelBinOutput");

    write_file(
        "test_resources/ExcelBinOutput/MaterialExcelConfigData.json",
        "["
        " {\"id\":2001,\"itemType\":\"ITEM_MATERIAL\",\"materialType\":"
        "\"MATERIAL_FOOD\",\"rankLevel\":2,\"stackLimit\":99,"
        "\"maxUseCount\":3,\"rank\":1,\"weight\":50,"
        "\"effectName\":\"Eff_Item_Apple\",\"icon\":\"UI_ItemIcon_100001\","
        "\"effectType\":\"Item_Material\",\"destroyRule\":\"RETURN\","
        "\"destroyReturnMaterial\":[102],\"destroyReturnMaterialCount\":[1],"
        "\"foodQuality\":\"FOOD_QUALITY_STRANGE\",\"satiationParams\":[5,10],"
        "\"useTarget\":\"ITEM_USE_TARGET_CUR_AVATAR\","
        "\"itemUse\":[{\"useOp\":\"ITEM_USE_ADD_EXP\",\"useParam\":"
        "[\"104001\",\"1\"]},{\"useOp\":\"ITEM_USE_GAIN_AVATAR\","
        "\"useParam\":[\"10000002\"]}],\"useOnGain\":true,"
        "\"nameTextMapHash\":200000001},"
        " {\"id\":2002,\"itemType\":\"ITEM_MATERIAL\",\"materialType\":"
        "\"MATERIAL_QUEST\",\"rankLevel\":1}"
        "]");

    write_file(
        "test_resources/ExcelBinOutput/WeaponExcelConfigData.json",
        "["
        " {\"id\":11301,\"itemType\":\"ITEM_WEAPON\",\"weaponPromoteId\":1,"
        "\"weaponBaseExp\":100,\"storyId\":0,\"avatarPromoteId\":1,"
        "\"awakenMaterial\":120,\"awakenCosts\":[1000,2000],"
        "\"skillAffix\":[1,2,3],\"weaponProp\":["
        "{\"propType\":\"FIGHT_PROP_BASE_ATTACK\",\"initValue\":23,"
        "\"type\":\"BaseAttack\"},"
        "{\"propType\":\"FIGHT_PROP_ATTACK\",\"initValue\":10,"
        "\"type\":\"Attack\"},"
        "{\"type\":\"Unknown\"}],\"nameTextMapHash\":300000001,"
        "\"rankLevel\":3,\"stackLimit\":1}"
        "]");

    write_file(
        "test_resources/ExcelBinOutput/ReliquaryExcelConfigData.json",
        "["
        " {\"id\":23311,\"itemType\":\"ITEM_RELIQUARY\","
        "\"equipType\":\"EQUIP_BRACER\",\"mainPropDepotId\":902,"
        "\"appendPropDepotId\":1001,\"appendPropNum\":4,\"setId\":15008,"
        "\"addPropLevels\":[4,8,12,16,20],\"baseConvExp\":10,"
        "\"maxLevel\":90,\"nameTextMapHash\":400000001,\"rankLevel\":4,"
        "\"stackLimit\":1}"
        "]");

    write_file(
        "test_resources/ExcelBinOutput/HomeWorldFurnitureExcelConfigData.json",
        "["
        " {\"id\":37001,\"itemType\":\"ITEM_FURNITURE\",\"comfort\":90,"
        "\"furnType\":[0,1,2],\"furnitureGadgetID\":[-1,5001],"
        "\"specialFurnitureType\":\"APARTMENT\",\"roomSceneId\":8,"
        "\"nameTextMapHash\":500000001,\"rankLevel\":2}"
        "]");
}

/* ============================================================ */

static void test_special_furniture_lookup(void)
{
    AE_TEST_ASSERT(ae_special_furniture_value(
                       AE_SPECIAL_FURNITURE_NOT_SPECIAL) == -1);
    AE_TEST_ASSERT(ae_special_furniture_from_value(-1) ==
                   AE_SPECIAL_FURNITURE_NOT_SPECIAL);
    AE_TEST_ASSERT(ae_special_furniture_from_value(8) ==
                   AE_SPECIAL_FURNITURE_PAIMON);
    AE_TEST_ASSERT(ae_special_furniture_from_value(1) ==
                   AE_SPECIAL_FURNITURE_NOT_SPECIAL);
    AE_TEST_ASSERT(ae_special_furniture_from_name("APARTMENT") ==
                   AE_SPECIAL_FURNITURE_APARTMENT);
    AE_TEST_ASSERT(ae_special_furniture_from_name("FARM_FIELD") ==
                   AE_SPECIAL_FURNITURE_FARM_FIELD);
    AE_TEST_ASSERT(ae_special_furniture_from_name("bogus") ==
                   AE_SPECIAL_FURNITURE_NOT_SPECIAL);
    AE_TEST_ASSERT(ae_special_furniture_from_name(NULL) ==
                   AE_SPECIAL_FURNITURE_NOT_SPECIAL);
}

/* ============================================================ */

static void test_item_from_json_material(void)
{
    const char *text =
        "{\"id\":2001,\"itemType\":\"ITEM_MATERIAL\",\"materialType\":"
        "\"MATERIAL_FOOD\",\"rankLevel\":2,\"stackLimit\":99,"
        "\"maxUseCount\":3,\"rank\":1,\"weight\":50,"
        "\"effectName\":\"Eff_Item_Apple\",\"icon\":\"UI_ItemIcon_100001\","
        "\"effectType\":\"Item_Material\",\"destroyRule\":\"RETURN\","
        "\"destroyReturnMaterial\":[102],\"destroyReturnMaterialCount\":[1],"
        "\"satiationParams\":[5,10],"
        "\"useTarget\":\"ITEM_USE_TARGET_CUR_AVATAR\","
        "\"itemUse\":[{\"useOp\":\"ITEM_USE_ADD_EXP\",\"useParam\":"
        "[\"104001\",\"1\"]},{\"useOp\":\"ITEM_USE_GAIN_AVATAR\","
        "\"useParam\":[\"10000002\"]}],\"useOnGain\":true,"
        "\"nameTextMapHash\":200000001}";
    cJSON *root = cJSON_Parse(text);
    ae_item_data_t item;

    AE_TEST_ASSERT(root != NULL);
    memset(&item, 0, sizeof(item));
    AE_TEST_ASSERT(ae_gamedata_item_from_json(root, &item) == AE_OK);

    AE_TEST_ASSERT(item.id == 2001);
    AE_TEST_ASSERT(item.stack_limit == 99);
    AE_TEST_ASSERT(item.max_use_count == 3);
    AE_TEST_ASSERT(item.rank_level == 2);
    AE_TEST_ASSERT(item.item_type == AE_ITEM_MATERIAL);
    AE_TEST_ASSERT(item.material_type == AE_MATERIAL_FOOD);
    AE_TEST_ASSERT(item.equip_type == AE_EQUIP_NONE);
    AE_TEST_ASSERT(item.use_target == AE_ITEM_USE_TARGET_CUR_AVATAR);
    AE_TEST_ASSERT(item.use_on_gain == true);
    AE_TEST_ASSERT(item.name_text_map_hash == 200000001);

    AE_TEST_ASSERT(item.effect_name != NULL);
    AE_TEST_ASSERT(strcmp(item.effect_name, "Eff_Item_Apple") == 0);
    AE_TEST_ASSERT(item.icon != NULL);
    AE_TEST_ASSERT(strcmp(item.icon, "UI_ItemIcon_100001") == 0);
    AE_TEST_ASSERT(item.effect_type != NULL);
    AE_TEST_ASSERT(strcmp(item.effect_type, "Item_Material") == 0);
    AE_TEST_ASSERT(item.destroy_rule != NULL);
    AE_TEST_ASSERT(strcmp(item.destroy_rule, "RETURN") == 0);

    AE_TEST_ASSERT(item.destroy_return_material_count == 1);
    AE_TEST_ASSERT(item.destroy_return_material[0] == 102);
    AE_TEST_ASSERT(item.destroy_return_material_amount_count == 1);
    AE_TEST_ASSERT(item.destroy_return_material_amount[0] == 1);
    AE_TEST_ASSERT(item.satiation_params_count == 2);
    AE_TEST_ASSERT(item.satiation_params[0] == 5);

    AE_TEST_ASSERT(item.item_use_count == 2);
    AE_TEST_ASSERT(item.item_use[0].use_op == AE_ITEM_USE_ADD_EXP);
    AE_TEST_ASSERT(item.item_use[0].use_param_count == 2);
    AE_TEST_ASSERT(strcmp(item.item_use[0].use_param[0], "104001") == 0);
    AE_TEST_ASSERT(strcmp(item.item_use[0].use_param[1], "1") == 0);
    AE_TEST_ASSERT(item.item_use[1].use_op == AE_ITEM_USE_GAIN_AVATAR);
    AE_TEST_ASSERT(item.item_use[1].use_param_count == 1);
    AE_TEST_ASSERT(strcmp(item.item_use[1].use_param[0], "10000002") == 0);

    ae_gamedata_item_destroy(&item);
    AE_TEST_ASSERT(item.effect_name == NULL);
    AE_TEST_ASSERT(item.stack_limit == 0);
    AE_TEST_ASSERT(item.item_use == NULL);
    cJSON_Delete(root);
}

static void test_item_from_json_unknown_names(void)
{
    const char *text =
        "{\"id\":9,\"itemType\":\"BOGUS_TYPE\",\"materialType\":\"\","
        "\"useTarget\":\"ITEM_USE_TARGET_NONE\",\"useOnGain\":false,"
        "\"stackLimit\":-5}";
    cJSON *root = cJSON_Parse(text);
    ae_item_data_t item;

    AE_TEST_ASSERT(root != NULL);
    memset(&item, 0, sizeof(item));
    AE_TEST_ASSERT(ae_gamedata_item_from_json(root, &item) == AE_OK);
    AE_TEST_ASSERT(item.id == 9);
    AE_TEST_ASSERT(item.stack_limit == -5);
    AE_TEST_ASSERT(item.item_type == AE_ITEM_NONE);
    AE_TEST_ASSERT(item.material_type == AE_MATERIAL_NONE);
    ae_gamedata_item_destroy(&item);
    cJSON_Delete(root);
}

/* ============================================================ */

static void test_on_load_weapon(void)
{
    const char *text =
        "{\"id\":11301,\"itemType\":\"ITEM_WEAPON\","
        "\"weaponPromoteId\":1,\"weaponBaseExp\":100,\"awakennulls\":0,"
        "\"awakenMaterial\":120,\"awakenCosts\":[1000,2000],"
        "\"skillAffix\":[1,2,3],\"weaponProp\":["
        "{\"propType\":\"FIGHT_PROP_BASE_ATTACK\",\"initValue\":23,"
        "\"type\":\"BaseAttack\"},"
        "{\"propType\":\"FIGHT_PROP_ATTACK\",\"initValue\":10,"
        "\"type\":\"Attack\"},"
        "{\"type\":\"Unknown\"}]}";
    cJSON *root = cJSON_Parse(text);
    ae_item_data_t item;

    AE_TEST_ASSERT(root != NULL);
    memset(&item, 0, sizeof(item));
    AE_TEST_ASSERT(ae_gamedata_item_from_json(root, &item) == AE_OK);
    AE_TEST_ASSERT(ae_gamedata_item_on_load(&item) == AE_OK);

    AE_TEST_ASSERT(item.equip_type == AE_EQUIP_WEAPON);
    AE_TEST_ASSERT(item.weapon_prop_count == 2);
    AE_TEST_ASSERT(item.weapon_prop[0].prop_type == AE_FIGHT_PROP_BASE_ATTACK);
    AE_TEST_ASSERT(item.weapon_prop[0].init_value > 22.99f);
    AE_TEST_ASSERT(item.weapon_prop[0].init_value < 23.01f);
    AE_TEST_ASSERT(strcmp(item.weapon_prop[0].type, "BaseAttack") == 0);
    AE_TEST_ASSERT(item.weapon_prop[1].prop_type == AE_FIGHT_PROP_ATTACK);
    AE_TEST_ASSERT(item.weapon_prop[1].init_value > 9.99f);
    AE_TEST_ASSERT(item.weapon_prop[1].init_value < 10.01f);
    AE_TEST_ASSERT(strcmp(item.weapon_prop[1].type, "Attack") == 0);

    AE_TEST_ASSERT(item.weapon_promote_id == 1);
    AE_TEST_ASSERT(item.weapon_base_exp == 100);
    AE_TEST_ASSERT(item.awaken_material == 120);
    AE_TEST_ASSERT(item.awaken_costs_count == 2);
    AE_TEST_ASSERT(item.awaken_costs[1] == 2000);
    AE_TEST_ASSERT(item.skill_affix_count == 3);
    AE_TEST_ASSERT(item.skill_affix[2] == 3);

    AE_TEST_ASSERT(ae_gamedata_item_is_equip(&item));
    ae_gamedata_item_destroy(&item);
    cJSON_Delete(root);
}

static void test_on_load_relic(void)
{
    const char *text =
        "{\"id\":23311,\"itemType\":\"ITEM_RELIQUARY\","
        "\"equipType\":\"EQUIP_BRACER\",\"mainPropDepotId\":902,"
        "\"appendPropDepotId\":1001,\"appendPropNum\":4,\"setId\":15008,"
        "\"addPropLevels\":[4,8,12,16,20],\"baseConvExp\":10,"
        "\"maxLevel\":90}";
    cJSON *root = cJSON_Parse(text);
    ae_item_data_t item;

    AE_TEST_ASSERT(root != NULL);
    memset(&item, 0, sizeof(item));
    AE_TEST_ASSERT(ae_gamedata_item_from_json(root, &item) == AE_OK);
    AE_TEST_ASSERT(ae_gamedata_item_on_load(&item) == AE_OK);

    /* Relics keep their excel equip type. */
    AE_TEST_ASSERT(item.equip_type == AE_EQUIP_BRACER);
    AE_TEST_ASSERT(item.main_prop_depot_id == 902);
    AE_TEST_ASSERT(item.append_prop_depot_id == 1001);
    AE_TEST_ASSERT(item.append_prop_num == 4);
    AE_TEST_ASSERT(item.set_id == 15008);
    AE_TEST_ASSERT(item.base_conv_exp == 10);
    AE_TEST_ASSERT(item.max_level == 90);
    AE_TEST_ASSERT(item.add_prop_levels_count == 5);
    AE_TEST_ASSERT(ae_gamedata_item_can_add_relic_prop(&item, 4));
    AE_TEST_ASSERT(ae_gamedata_item_can_add_relic_prop(&item, 20));
    AE_TEST_ASSERT(!ae_gamedata_item_can_add_relic_prop(&item, 5));
    AE_TEST_ASSERT(ae_gamedata_item_is_equip(&item));

    ae_gamedata_item_destroy(&item);
    cJSON_Delete(root);
}

static void test_on_load_furniture(void)
{
    const char *text =
        "{\"id\":37001,\"itemType\":\"ITEM_FURNITURE\",\"comfort\":90,"
        "\"furnType\":[0,1,2],\"furnitureGadgetID\":[-1,5001],"
        "\"specialFurnitureType\":\"APARTMENT\",\"roomSceneId\":8}";
    cJSON *root = cJSON_Parse(text);
    ae_item_data_t item;

    AE_TEST_ASSERT(root != NULL);
    memset(&item, 0, sizeof(item));
    AE_TEST_ASSERT(ae_gamedata_item_from_json(root, &item) == AE_OK);
    AE_TEST_ASSERT(ae_gamedata_item_on_load(&item) == AE_OK);

    AE_TEST_ASSERT(item.equip_type == AE_EQUIP_NONE);
    AE_TEST_ASSERT(item.comfort == 90);
    AE_TEST_ASSERT(item.furn_type_count == 2);
    AE_TEST_ASSERT(item.furn_type[0] == 1);
    AE_TEST_ASSERT(item.furn_type[1] == 2);
    AE_TEST_ASSERT(item.furniture_gadget_id_count == 1);
    AE_TEST_ASSERT(item.furniture_gadget_id[0] == 5001);
    AE_TEST_ASSERT(item.special_furniture_type ==
                   AE_SPECIAL_FURNITURE_APARTMENT);
    AE_TEST_ASSERT(item.room_scene_id == 8);
    AE_TEST_ASSERT(!ae_gamedata_item_is_equip(&item));
    AE_TEST_ASSERT(!ae_gamedata_item_can_add_relic_prop(&item, 4));

    ae_gamedata_item_destroy(&item);
    cJSON_Delete(root);
}

/* ============================================================ */

static void test_gamedata_load_and_lookup(void)
{
    ae_error_t err;
    const ae_item_data_t *item;

    ae_resource_loader_reset();
    AE_TEST_ASSERT(ae_gamedata_init() == AE_OK);
    AE_TEST_ASSERT(ae_gamedata_init() == AE_ERR_ALREADY_EXISTS);
    AE_TEST_ASSERT(ae_gamedata_register_all() == AE_OK);

    err = ae_resource_loader_load_all("test_resources", false);
    AE_TEST_ASSERT(err == AE_OK);
    AE_TEST_ASSERT(ae_gamedata_item_count() == 5);

    item = ae_gamedata_get_item(2001);
    AE_TEST_ASSERT(item != NULL);
    AE_TEST_ASSERT(item->stack_limit == 99);
    AE_TEST_ASSERT(item->item_type == AE_ITEM_MATERIAL);
    AE_TEST_ASSERT(item->material_type == AE_MATERIAL_FOOD);
    AE_TEST_ASSERT(!ae_gamedata_item_is_equip(item));

    item = ae_gamedata_get_item(2002);
    AE_TEST_ASSERT(item != NULL);
    AE_TEST_ASSERT(item->stack_limit == 1);
    AE_TEST_ASSERT(item->material_type == AE_MATERIAL_QUEST);

    item = ae_gamedata_get_item(11301);
    AE_TEST_ASSERT(item != NULL);
    AE_TEST_ASSERT(item->equip_type == AE_EQUIP_WEAPON);
    AE_TEST_ASSERT(item->weapon_prop_count == 2);
    AE_TEST_ASSERT(ae_gamedata_item_is_equip(item));

    item = ae_gamedata_get_item(23311);
    AE_TEST_ASSERT(item != NULL);
    AE_TEST_ASSERT(item->equip_type == AE_EQUIP_BRACER);
    AE_TEST_ASSERT(ae_gamedata_item_can_add_relic_prop(item, 4));
    AE_TEST_ASSERT(ae_gamedata_item_can_add_relic_prop(item, 20));
    AE_TEST_ASSERT(!ae_gamedata_item_can_add_relic_prop(item, 5));
    AE_TEST_ASSERT(ae_gamedata_item_is_equip(item));

    item = ae_gamedata_get_item(37001);
    AE_TEST_ASSERT(item != NULL);
    AE_TEST_ASSERT(item->special_furniture_type ==
                   AE_SPECIAL_FURNITURE_APARTMENT);
    AE_TEST_ASSERT(item->furn_type_count == 2);
    AE_TEST_ASSERT(item->furn_type[0] == 1);
    AE_TEST_ASSERT(item->room_scene_id == 8);
    AE_TEST_ASSERT(item->equip_type == AE_EQUIP_NONE);

    AE_TEST_ASSERT(ae_gamedata_get_item(999) == NULL);
    AE_TEST_ASSERT(ae_gamedata_get_item(0) == NULL);

    ae_gamedata_destroy();
    AE_TEST_ASSERT(ae_gamedata_item_count() == 0);
    AE_TEST_ASSERT(ae_gamedata_get_item(2001) == NULL);

    /* The registry can be re-initialised after a teardown. */
    AE_TEST_ASSERT(ae_gamedata_init() == AE_OK);
    ae_gamedata_destroy();
    ae_resource_loader_reset();
}

/* ============================================================ */

int main(void)
{
    make_fixture_tree();

    static const ae_test_case_t suite[] = {
        {"special_furniture_lookup", test_special_furniture_lookup},
        {"item_from_json_material", test_item_from_json_material},
        {"item_from_json_unknown_names", test_item_from_json_unknown_names},
        {"on_load_weapon", test_on_load_weapon},
        {"on_load_relic", test_on_load_relic},
        {"on_load_furniture", test_on_load_furniture},
        {"gamedata_load_and_lookup", test_gamedata_load_and_lookup},
    };

    ae_u32 failed = AE_TEST_SUITE(suite);

    return (int)failed;
}