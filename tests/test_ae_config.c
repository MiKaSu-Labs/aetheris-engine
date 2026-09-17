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
 * @file test_ae_config.c
 *
 * @brief Tests for the configuration model, cJSON round-trip, and
 *        schema-version migration.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aetheris/ae_config.h>

#include "ae_test.h"

/* ============================================================ */

static void test_defaults(void)
{
    ae_config_t cfg;

    ae_config_init(&cfg);

    AE_TEST_ASSERT(0 == strcmp(cfg.folder_structure.resources,
                               "./resources/"));
    AE_TEST_ASSERT(0 == strcmp(cfg.folder_structure.data, "./data/"));
    AE_TEST_ASSERT(0 == strcmp(cfg.folder_structure.packets,
                               "./packets/"));
    AE_TEST_ASSERT(0 == strcmp(cfg.folder_structure.plugins,
                               "./plugins/"));
    AE_TEST_ASSERT(0 == strcmp(cfg.folder_structure.cache, "./cache/"));

    AE_TEST_ASSERT(0 == strcmp(cfg.database_info.server.connection_uri,
                               "mongodb://localhost:27017"));
    AE_TEST_ASSERT(0 == strcmp(cfg.database_info.game.collection,
                               "grasscutter"));

    AE_TEST_ASSERT(0 == strcmp(cfg.language.language, "en_US"));
    AE_TEST_ASSERT(0 == strcmp(cfg.language.document, "EN"));

    AE_TEST_ASSERT(!cfg.account.auto_create);
    AE_TEST_ASSERT(!cfg.account.experimental_real_password);
    AE_TEST_ASSERT(cfg.account.max_player == -1);
    AE_TEST_ASSERT(cfg.account.default_permissions_count == 0);

    AE_TEST_ASSERT(cfg.server.run_mode == AE_CONFIG_RUN_HYBRID);
    AE_TEST_ASSERT(!cfg.server.log_commands);
    AE_TEST_ASSERT(cfg.server.fast_require);

    AE_TEST_ASSERT(!cfg.server.http.start_immediately);
    AE_TEST_ASSERT(0 == strcmp(cfg.server.http.bind_address, "0.0.0.0"));
    AE_TEST_ASSERT(cfg.server.http.bind_port == 443);
    AE_TEST_ASSERT(0 == strcmp(cfg.server.http.access_address,
                               "127.0.0.1"));
    AE_TEST_ASSERT(cfg.server.http.access_port == 0);
    AE_TEST_ASSERT(cfg.server.http.encryption.use_encryption);
    AE_TEST_ASSERT(cfg.server.http.encryption.use_in_routing);
    AE_TEST_ASSERT(0 == strcmp(cfg.server.http.encryption.keystore,
                               "./keystore.p12"));
    AE_TEST_ASSERT(cfg.server.http.policies.cors.enabled);
    AE_TEST_ASSERT(cfg.server.http.policies.cors.allowed_origins_count == 1);
    AE_TEST_ASSERT(0 == strcmp(
        cfg.server.http.policies.cors.allowed_origins[0], "*"));

    AE_TEST_ASSERT(0 == strcmp(cfg.server.game.bind_address, "0.0.0.0"));
    AE_TEST_ASSERT(cfg.server.game.bind_port == 22102);
    AE_TEST_ASSERT(cfg.server.game.use_unique_packet_key);
    AE_TEST_ASSERT(cfg.server.game.load_entities_for_player_range == 300);
    AE_TEST_ASSERT(cfg.server.game.enable_console);
    AE_TEST_ASSERT(cfg.server.game.kcp_interval == 20);
    AE_TEST_ASSERT(cfg.server.game.log_packets == AE_CONFIG_DEBUG_NONE);

    AE_TEST_ASSERT(cfg.server.game.game_options.inventory_limits.weapons
                   == 2000);
    AE_TEST_ASSERT(cfg.server.game.game_options.inventory_limits.all
                   == 30000);
    AE_TEST_ASSERT(cfg.server.game.game_options.avatar_limits.
                   single_player_team == 4);
    AE_TEST_ASSERT(cfg.server.game.game_options.scene_entity_limit
                   == 1000);
    AE_TEST_ASSERT(cfg.server.game.game_options.questing.enabled);
    AE_TEST_ASSERT(!cfg.server.game.game_options.resin_options.
                   resin_usage);
    AE_TEST_ASSERT(cfg.server.game.game_options.rates.mora == 1.0F);
    AE_TEST_ASSERT(!cfg.server.game.game_options.handbook.enable);
    AE_TEST_ASSERT(cfg.server.game.game_options.handbook.limits.
                   max_requests == 10);

    AE_TEST_ASSERT(cfg.server.game.join_options.welcome_emotes_count == 3);
    AE_TEST_ASSERT(cfg.server.game.join_options.welcome_emotes[0] == 2007);
    AE_TEST_ASSERT(0 == strcmp(cfg.server.game.join_options.
                               welcome_message,
                               "Welcome to a Grasscutter server."));
    AE_TEST_ASSERT(cfg.server.game.join_options.welcome_mail.items_count
                   == 2);
    AE_TEST_ASSERT(cfg.server.game.join_options.welcome_mail.items[0].
                   item_id == 13509);
    AE_TEST_ASSERT(cfg.server.game.join_options.welcome_mail.items[1].
                   item_count == 99999);

    AE_TEST_ASSERT(cfg.server.game.console_account.avatar_id == 10000007);
    AE_TEST_ASSERT(0 == strcmp(cfg.server.game.console_account.nick_name,
                               "Server"));

    AE_TEST_ASSERT(cfg.server.game.vision_options_count == 6);
    AE_TEST_ASSERT(0 == strcmp(cfg.server.game.vision_options[0].name,
                               "VISION_LEVEL_NORMAL"));
    AE_TEST_ASSERT(cfg.server.game.vision_options[0].vision_range == 80);
    AE_TEST_ASSERT(cfg.server.game.vision_options[0].grid_width == 20);
    AE_TEST_ASSERT(0 == strcmp(cfg.server.game.vision_options[5].name,
                               "VISION_LEVEL_SUPER_NEARBY"));

    AE_TEST_ASSERT(cfg.server.dispatch.regions_count == 0);
    AE_TEST_ASSERT(0 == strcmp(cfg.server.dispatch.dispatch_url,
                               "ws://127.0.0.1:1111"));
    AE_TEST_ASSERT(0 == strcmp(cfg.server.dispatch.default_name,
                               "Grasscutter"));
    AE_TEST_ASSERT(cfg.server.dispatch.log_requests == AE_CONFIG_DEBUG_NONE);

    AE_TEST_ASSERT(cfg.server.debug_mode.server_logger_level
                   == AE_CONFIG_LOG_DEBUG);
    AE_TEST_ASSERT(cfg.server.debug_mode.log_packets == AE_CONFIG_DEBUG_ALL);
    AE_TEST_ASSERT(cfg.server.debug_mode.log_requests == AE_CONFIG_DEBUG_ALL);

    AE_TEST_ASSERT(cfg.version == AE_CONFIG_VERSION);
}

static void test_load_overrides(void)
{
    static const char *json =
        "{"
        " \"folderStructure\": {\"resources\": \"./res/\"},"
        " \"language\": {\"language\": \"pt_BR\"},"
        " \"account\": {\"autoCreate\": true, \"maxPlayer\": 100},"
        " \"server\": {"
        "   \"runMode\": \"GAME_ONLY\","
        "   \"http\": {\"bindAddress\": \"10.1.2.3\","
        "             \"bindPort\": 8080,"
        "             \"encryption\": {\"useEncryption\": false},"
        "             \"policies\": {\"cors\": {\"allowedOrigins\":"
        "                 [\"http://a\", \"http://b\"]}}},"
        "   \"game\": {\"logPackets\": \"ALL\","
        "             \"kcpInterval\": 10,"
        "             \"gameOptions\": {\"rates\": {\"mora\": 2.5}},"
        "             \"serverAccount\": {\"nickName\": \"Root\"},"
        "             \"visionOptions\": [{\"name\": \"X\","
        "                 \"visionRange\": 9, \"gridWidth\": 4}]},"
        "   \"dispatch\": {\"dispatchUrl\": \"ws://x\"," 
        "     \"regions\": [{\"Name\": \"os_usa\", \"Title\": \"GC\","
        "                    \"Ip\": \"1.2.3.4\", \"Port\": 22102}]},"
        "   \"debugMode\": {\"serverLoggerLevel\": \"WARN\"}"
        " },"
        " \"version\": 3"
        "}";
    ae_config_t cfg;

    ae_config_init(&cfg);
    AE_TEST_ASSERT(AE_OK == ae_config_load_string(&cfg, json));

    AE_TEST_ASSERT(0 == strcmp(cfg.folder_structure.resources, "./res/"));
    AE_TEST_ASSERT(0 == strcmp(cfg.folder_structure.data, "./data/"));
    AE_TEST_ASSERT(0 == strcmp(cfg.language.language, "pt_BR"));
    AE_TEST_ASSERT(0 == strcmp(cfg.language.fallback, "en_US"));
    AE_TEST_ASSERT(cfg.account.auto_create);
    AE_TEST_ASSERT(cfg.account.max_player == 100);

    AE_TEST_ASSERT(cfg.server.run_mode == AE_CONFIG_RUN_GAME_ONLY);
    AE_TEST_ASSERT(0 == strcmp(cfg.server.http.bind_address, "10.1.2.3"));
    AE_TEST_ASSERT(cfg.server.http.bind_port == 8080);
    AE_TEST_ASSERT(!cfg.server.http.encryption.use_encryption);
    AE_TEST_ASSERT(cfg.server.http.policies.cors.allowed_origins_count == 2);
    AE_TEST_ASSERT(0 == strcmp(
        cfg.server.http.policies.cors.allowed_origins[1], "http://b"));

    AE_TEST_ASSERT(cfg.server.game.log_packets == AE_CONFIG_DEBUG_ALL);
    AE_TEST_ASSERT(cfg.server.game.kcp_interval == 10);
    AE_TEST_ASSERT(cfg.server.game.game_options.rates.mora == 2.5F);
    AE_TEST_ASSERT(0 == strcmp(cfg.server.game.console_account.nick_name,
                               "Root"));
    AE_TEST_ASSERT(cfg.server.game.vision_options_count == 1);
    AE_TEST_ASSERT(0 == strcmp(cfg.server.game.vision_options[0].name,
                               "X"));
    AE_TEST_ASSERT(cfg.server.game.vision_options[0].grid_width == 4);

    AE_TEST_ASSERT(cfg.server.dispatch.regions_count == 1);
    AE_TEST_ASSERT(0 == strcmp(cfg.server.dispatch.dispatch_url, "ws://x"));
    AE_TEST_ASSERT(0 == strcmp(cfg.server.dispatch.regions[0].name,
                               "os_usa"));
    AE_TEST_ASSERT(cfg.server.dispatch.regions[0].port == 22102);

    AE_TEST_ASSERT(cfg.server.debug_mode.server_logger_level
                   == AE_CONFIG_LOG_WARN);
    AE_TEST_ASSERT(cfg.version == AE_CONFIG_VERSION);
}

static void test_round_trip(void)
{
    ae_config_t a;
    ae_config_t b;
    char *t1 = NULL;
    char *t2 = NULL;

    ae_config_init(&a);
    ae_config_init(&b);

    AE_TEST_ASSERT(AE_OK == ae_config_save_string(&a, &t1, false));
    AE_TEST_ASSERT(t1 != NULL);
    AE_TEST_ASSERT(NULL != strstr(t1, "\"version\":14"));
    AE_TEST_ASSERT(NULL != strstr(t1, "\"folderStructure\":{"));
    AE_TEST_ASSERT(NULL != strstr(t1, "\"databaseInfo\":{"));
    AE_TEST_ASSERT(NULL != strstr(t1, "\"runMode\":\"HYBRID\""));
    AE_TEST_ASSERT(NULL != strstr(t1, "\"logPackets\":\"NONE\""));
    AE_TEST_ASSERT(NULL != strstr(t1, "\"bindPort\":443"));

    AE_TEST_ASSERT(AE_OK == ae_config_load_string(&b, t1));
    AE_TEST_ASSERT(AE_OK == ae_config_save_string(&b, &t2, false));
    AE_TEST_ASSERT(t2 != NULL);
    AE_TEST_ASSERT(0 == strcmp(t1, t2));

    free(t1);
    free(t2);
}

static void test_pretty_output(void)
{
    ae_config_t cfg;
    char *text = NULL;

    ae_config_init(&cfg);
    AE_TEST_ASSERT(AE_OK == ae_config_save_string(&cfg, &text, true));
    AE_TEST_ASSERT(text != NULL);
    AE_TEST_ASSERT(NULL != strstr(text, "\"bindPort\":\t443"));
    free(text);
}

static void test_unknown_and_malformed(void)
{
    ae_config_t cfg;

    ae_config_init(&cfg);

    AE_TEST_ASSERT(AE_OK ==
                   ae_config_load_string(&cfg, "{ \"totallyUnknown\": 42, "
                                          "\"server\": {\"nope\": 1} }"));
    AE_TEST_ASSERT(cfg.version == AE_CONFIG_VERSION);

    AE_TEST_ASSERT(AE_ERR_BAD_DATA ==
                   ae_config_load_string(&cfg, "{ broken json"));
    AE_TEST_ASSERT(AE_ERR_BAD_DATA == ae_config_load_string(&cfg, ""));

    ae_config_init(&cfg);
    AE_TEST_ASSERT(AE_OK ==
                   ae_config_load_string(&cfg, "null"));
    AE_TEST_ASSERT(cfg.server.run_mode == AE_CONFIG_RUN_HYBRID);
}

static void test_file_round_trip(void)
{
    ae_config_t cfg;
    const char *path = "build/config_roundtrip_test.json";

    ae_config_init(&cfg);
    cfg.server.run_mode = AE_CONFIG_RUN_DISPATCH_ONLY;
    cfg.server.game.game_options.rates.adventure_exp = 4.0F;

    AE_TEST_ASSERT(AE_OK == ae_config_save_file(&cfg, path));

    ae_config_init(&cfg);
    AE_TEST_ASSERT(AE_OK == ae_config_load_file(&cfg, path));
    AE_TEST_ASSERT(cfg.server.run_mode == AE_CONFIG_RUN_DISPATCH_ONLY);
    AE_TEST_ASSERT(cfg.server.game.game_options.rates.adventure_exp == 4.0F);

    AE_TEST_ASSERT(AE_ERR_IO ==
                   ae_config_load_file(&cfg, "no/such/dir/file.json"));
    remove(path);
}

static void test_migration(void)
{
    ae_config_t cfg;

    ae_config_init(&cfg);
    cfg.version = 13;

    AE_TEST_ASSERT(ae_config_update(&cfg));
    AE_TEST_ASSERT(cfg.version == AE_CONFIG_VERSION);
    AE_TEST_ASSERT(!ae_config_update(&cfg));

    AE_TEST_ASSERT(!ae_config_update(NULL));
}

static void test_null_and_invalid(void)
{
    ae_config_t cfg;
    char *text = NULL;

    ae_config_init(&cfg);

    AE_TEST_ASSERT(AE_ERR_INVALID_ARG ==
                   ae_config_load_string(&cfg, NULL));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG ==
                   ae_config_load_string(NULL, "{}"));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG ==
                   ae_config_save_string(&cfg, NULL, false));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG ==
                   ae_config_save_string(NULL, &text, false));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG ==
                   ae_config_load_file(&cfg, NULL));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG ==
                   ae_config_save_file(&cfg, NULL));
    AE_TEST_ASSERT(text == NULL);
}

/* ============================================================ */

static const ae_test_case_t test_cases[] = {
    {"Defaults", test_defaults},
    {"Load overrides", test_load_overrides},
    {"Save/load round trip", test_round_trip},
    {"Pretty output", test_pretty_output},
    {"Unknown keys and malformed JSON", test_unknown_and_malformed},
    {"File round trip", test_file_round_trip},
    {"Version migration", test_migration},
    {"NULL and invalid arguments", test_null_and_invalid},
};

int main(void)
{
    ae_u32 failed = AE_TEST_SUITE(test_cases);

    printf("config: %u assertions, %u failed\n",
           ae_test_run_count(), ae_test_fail_count());

    return failed != 0 ? 1 : 0;
}

/* ============================================================ */