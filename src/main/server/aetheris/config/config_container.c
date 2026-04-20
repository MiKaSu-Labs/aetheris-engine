/*
 * config_container.c
 */

#include "config_container.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aetheris.h"     /* ae_config, ae_logger, ae_load_config, ae_save_config */
#include "crypto.h"       /* crypto_create_session_key()                          */
#include "int_set.h"      /* ae_int_set_create_empty()                            */
#include "json_utils.h"   /* json_utils_has_field()                               */
#include "utils.h"        /* utils_base64_encode()                                */

/* =========================================================================
 * Internal: fill vision options array with defaults
 * ====================================================================== */
static void _fill_default_vision_options(ae_cfg_vision_options_t out[AE_CFG_VISION_OPTIONS_COUNT]) {
    static const ae_cfg_vision_options_t defaults[AE_CFG_VISION_OPTIONS_COUNT] = {
        {"VISION_LEVEL_NORMAL",        80,   20},
        {"VISION_LEVEL_LITTLE_REMOTE", 16,   40},
        {"VISION_LEVEL_REMOTE",        1000, 250},
        {"VISION_LEVEL_SUPER",         4000, 1000},
        {"VISION_LEVEL_NEARBY",        40,   20},
        {"VISION_LEVEL_SUPER_NEARBY",  20,   20},
    };
    memcpy(out, defaults, sizeof(defaults));
}

/* =========================================================================
 * Internal: fill default mail items
 * ====================================================================== */
static ae_cfg_mail_item_t *_create_default_mail_items(size_t *out_len) {
    ae_cfg_mail_item_t *items = malloc(2 * sizeof(ae_cfg_mail_item_t));
    if (!items) { *out_len = 0; return NULL; }
    items[0] = (ae_cfg_mail_item_t){13509, 1,     1};
    items[1] = (ae_cfg_mail_item_t){201,   99999, 1};
    *out_len = 2;
    return items;
}

/* =========================================================================
 * config_container_create_default()
 * ====================================================================== */
ae_config_t *config_container_create_default(void) {
    ae_config_t *c = calloc(1, sizeof(ae_config_t));
    if (!c) return NULL;

    /* --- folderStructure ------------------------------------------------ */
    strncpy(c->folder_structure.resources, "./resources/",   sizeof(c->folder_structure.resources) - 1);
    strncpy(c->folder_structure.data,      "./data/",        sizeof(c->folder_structure.data)      - 1);
    strncpy(c->folder_structure.packets,   "./packets/",     sizeof(c->folder_structure.packets)   - 1);
    strncpy(c->folder_structure.scripts,   "resources:Scripts/", sizeof(c->folder_structure.scripts) - 1);
    strncpy(c->folder_structure.plugins,   "./plugins/",     sizeof(c->folder_structure.plugins)   - 1);
    strncpy(c->folder_structure.cache,     "./cache/",       sizeof(c->folder_structure.cache)     - 1);

    /* --- databaseInfo --------------------------------------------------- */
    strncpy(c->database_info.server.connection_uri, "mongodb://localhost:27017", sizeof(c->database_info.server.connection_uri) - 1);
    strncpy(c->database_info.server.collection,     "aetheris",                  sizeof(c->database_info.server.collection)     - 1);
    strncpy(c->database_info.game.connection_uri,   "mongodb://localhost:27017", sizeof(c->database_info.game.connection_uri)   - 1);
    strncpy(c->database_info.game.collection,       "aetheris",                  sizeof(c->database_info.game.collection)       - 1);

    /* --- language ------------------------------------------------------- */
    /* Locale.getDefault() -> fall back to "en-US" at runtime if undetectable */
    strncpy(c->language.language, "en-US", sizeof(c->language.language) - 1);
    strncpy(c->language.fallback, "en-US", sizeof(c->language.fallback) - 1);
    strncpy(c->language.document, "EN",    sizeof(c->language.document) - 1);

    /* --- account -------------------------------------------------------- */
    c->account.auto_create                  = false;
    c->account.experimental_real_password   = false;
    c->account.default_permissions          = NULL;
    c->account.default_permissions_len      = 0;
    c->account.max_player                   = -1;

    /* --- server --------------------------------------------------------- */
    c->server.debug_whitelist = ae_int_set_create_empty();
    c->server.debug_blacklist = ae_int_set_create_empty();
    c->server.run_mode        = SERVER_RUN_MODE_HYBRID;
    c->server.log_commands    = false;
    c->server.fast_require    = true;

    /* server.http */
    c->server.http.start_immediately = false;
    strncpy(c->server.http.bind_address,   "0.0.0.0",   sizeof(c->server.http.bind_address)   - 1);
    c->server.http.bind_port = 443;
    strncpy(c->server.http.access_address, "127.0.0.1", sizeof(c->server.http.access_address) - 1);
    c->server.http.access_port = 0;
    /* http.encryption */
    c->server.http.encryption.use_encryption = true;
    c->server.http.encryption.use_in_routing = true;
    strncpy(c->server.http.encryption.keystore,          "./keystore.p12", sizeof(c->server.http.encryption.keystore)          - 1);
    strncpy(c->server.http.encryption.keystore_password, "123456",         sizeof(c->server.http.encryption.keystore_password) - 1);
    /* http.policies.cors */
    c->server.http.policies.cors.enabled = true;
    c->server.http.policies.cors.allowed_origins = malloc(2 * sizeof(char *));
    if (c->server.http.policies.cors.allowed_origins) {
        c->server.http.policies.cors.allowed_origins[0] = strdup("*");
        c->server.http.policies.cors.allowed_origins[1] = NULL;
        c->server.http.policies.cors.allowed_origins_len = 1;
    }
    /* http.files */
    strncpy(c->server.http.files.index_file, "./index.html", sizeof(c->server.http.files.index_file) - 1);
    strncpy(c->server.http.files.error_file, "./404.html",   sizeof(c->server.http.files.error_file) - 1);

    /* server.game */
    strncpy(c->server.game.bind_address,   "0.0.0.0",   sizeof(c->server.game.bind_address)   - 1);
    c->server.game.bind_port = 22102;
    strncpy(c->server.game.access_address, "127.0.0.1", sizeof(c->server.game.access_address) - 1);
    c->server.game.access_port                      = 0;
    c->server.game.use_unique_packet_key            = true;
    c->server.game.load_entities_for_player_range   = 300;
    c->server.game.enable_script_in_big_world       = true;
    c->server.game.enable_console                   = true;
    c->server.game.kcp_interval                     = 20;
    c->server.game.log_packets                      = SERVER_DEBUG_MODE_NONE;
    c->server.game.is_show_packet_payload           = false;
    c->server.game.is_show_loop_packets             = false;
    c->server.game.cache_scene_entities_every_run   = false;
    /* game.gameOptions */
    c->server.game.game_options.inventory_limits = (ae_cfg_inventory_limits_t){2000, 2000, 2000, 2000, 30000};
    c->server.game.game_options.avatar_limits    = (ae_cfg_avatar_limits_t){4, 4};
    c->server.game.game_options.scene_entity_limit      = 1000;
    c->server.game.game_options.watch_gacha_config      = false;
    c->server.game.game_options.enable_shop_items       = true;
    c->server.game.game_options.stamina_usage           = true;
    c->server.game.game_options.energy_usage            = true;
    c->server.game.game_options.fishhook_teleport       = true;
    c->server.game.game_options.trial_costumes          = false;
    c->server.game.game_options.force_legacy_drops      = true;
    c->server.game.game_options.questing.enabled        = true;
    c->server.game.game_options.resin_options           = (ae_cfg_resin_options_t){false, 160, 480};
    c->server.game.game_options.rates                   = (ae_cfg_rates_t){1.0f, 1.0f, 1.0f};
    /* game.gameOptions.handbook */
    c->server.game.game_options.handbook.enable         = false;
    c->server.game.game_options.handbook.allow_commands = true;
    c->server.game.game_options.handbook.limits         = (ae_cfg_handbook_limits_t){false, 3, 10, 25};
    c->server.game.game_options.handbook.server.enforced   = false;
    strncpy(c->server.game.game_options.handbook.server.address, "127.0.0.1",
            sizeof(c->server.game.game_options.handbook.server.address) - 1);
    c->server.game.game_options.handbook.server.port       = 443;
    c->server.game.game_options.handbook.server.can_change = true;
    /* game.joinOptions */
    c->server.game.join_options.welcome_emotes[0] = 2007;
    c->server.game.join_options.welcome_emotes[1] = 1002;
    c->server.game.join_options.welcome_emotes[2] = 4010;
    strncpy(c->server.game.join_options.welcome_message,
            "Welcome to an Aetheris server.",
            sizeof(c->server.game.join_options.welcome_message) - 1);
    strncpy(c->server.game.join_options.welcome_mail.title,
            "Welcome to Aetheris!",
            sizeof(c->server.game.join_options.welcome_mail.title) - 1);
    strncpy(c->server.game.join_options.welcome_mail.content,
            "Hi there!\r\nFirst of all, welcome to Aetheris. "
            "If you have any issues, please let us know!\r\n",
            sizeof(c->server.game.join_options.welcome_mail.content) - 1);
    strncpy(c->server.game.join_options.welcome_mail.sender, "Server",
            sizeof(c->server.game.join_options.welcome_mail.sender) - 1);
    c->server.game.join_options.welcome_mail.items =
        _create_default_mail_items(&c->server.game.join_options.welcome_mail.items_len);
    /* game.serverAccount */
    c->server.game.server_account.avatar_id       = 10000007;
    c->server.game.server_account.name_card_id    = 210001;
    c->server.game.server_account.adventure_rank  = 1;
    c->server.game.server_account.world_level     = 0;
    strncpy(c->server.game.server_account.nick_name,  "Server",              sizeof(c->server.game.server_account.nick_name)  - 1);
    strncpy(c->server.game.server_account.signature,  "Welcome to Aetheris!", sizeof(c->server.game.server_account.signature) - 1);
    /* game.visionOptions */
    _fill_default_vision_options(c->server.game.vision_options);

    /* server.dispatch */
    c->server.dispatch.regions      = NULL;
    c->server.dispatch.regions_len  = 0;
    strncpy(c->server.dispatch.dispatch_url, "ws://127.0.0.1:1111",
            sizeof(c->server.dispatch.dispatch_url) - 1);
    /* Generate a random 32-byte session key and its base64 representation */
    c->server.dispatch.encryption_key = malloc(32);
    if (c->server.dispatch.encryption_key) {
        crypto_create_session_key(c->server.dispatch.encryption_key, 32);
        c->server.dispatch.encryption_key_len = 32;
    }
    utils_base64_encode(c->server.dispatch.encryption_key, 32,
                        c->server.dispatch.dispatch_key,
                        sizeof(c->server.dispatch.dispatch_key));
    strncpy(c->server.dispatch.default_name, "Aetheris",
            sizeof(c->server.dispatch.default_name) - 1);
    c->server.dispatch.log_requests = SERVER_DEBUG_MODE_NONE;

    /* server.debugMode */
    c->server.debug_mode.server_logger_level    = AE_LOG_LEVEL_DEBUG;
    c->server.debug_mode.services_loggers_level = AE_LOG_LEVEL_INFO;
    c->server.debug_mode.log_packets            = SERVER_DEBUG_MODE_ALL;
    c->server.debug_mode.is_show_packet_payload = false;
    c->server.debug_mode.is_show_loop_packets   = false;
    c->server.debug_mode.log_requests           = SERVER_DEBUG_MODE_ALL;

    /* --- version -------------------------------------------------------- */
    c->version = AE_CONFIG_CONTAINER_VERSION;

    return c;
}

/* =========================================================================
 * config_container_free()
 * ====================================================================== */
void config_container_free(ae_config_t *c) {
    if (!c) return;

    /* account.defaultPermissions */
    if (c->account.default_permissions) {
        for (size_t i = 0; i < c->account.default_permissions_len; i++)
            free(c->account.default_permissions[i]);
        free(c->account.default_permissions);
    }

    /* server.debugWhitelist / Blacklist */
    ae_int_set_free(c->server.debug_whitelist);
    ae_int_set_free(c->server.debug_blacklist);

    /* server.http.policies.cors.allowedOrigins */
    if (c->server.http.policies.cors.allowed_origins) {
        for (size_t i = 0; i < c->server.http.policies.cors.allowed_origins_len; i++)
            free(c->server.http.policies.cors.allowed_origins[i]);
        free(c->server.http.policies.cors.allowed_origins);
    }

    /* server.game.joinOptions.welcomeMail.items */
    free(c->server.game.join_options.welcome_mail.items);

    /* server.dispatch.regions */
    free(c->server.dispatch.regions);

    /* server.dispatch.encryptionKey */
    free(c->server.dispatch.encryption_key);

    free(c);
}

/* =========================================================================
 * config_container_update()
 * Mirrors ConfigContainer.updateConfig()
 * ====================================================================== */
void config_container_update(ae_config_t *config) {
    /* Check for a legacy config (no "version" field in the JSON file) */
    if (!json_utils_has_field(AE_CONFIG_FILE, "version")) {
        ae_logger_info(ae_logger, "Updating legacy config...");
        ae_save_config(NULL);
    }

    int existing = config->version;
    int latest   = AE_CONFIG_CONTAINER_VERSION;

    if (existing == latest)
        return;

    /*
     * Build an updated config: start from defaults then copy the live
     * fields over the top, mirroring the reflection-based field copy.
     * In C we do a shallow struct copy then fix up owned heap pointers.
     */
    ae_config_t *updated = config_container_create_default();
    if (!updated) {
        ae_logger_error(ae_logger, "Failed to allocate updated configuration.");
        return;
    }

    /*
     * Overwrite the default values with the existing config's values.
     * Heap-owned fields (regions, encryption_key, cors origins, mail items,
     * permissions) are intentionally left as the fresh defaults because
     * a version migration may have changed their shape.
     * Non-heap fields are copied wholesale.
     */
    updated->folder_structure = config->folder_structure;
    updated->database_info    = config->database_info;
    updated->language         = config->language;
    updated->account.auto_create                = config->account.auto_create;
    updated->account.experimental_real_password = config->account.experimental_real_password;
    updated->account.max_player                 = config->account.max_player;
    updated->server.run_mode     = config->server.run_mode;
    updated->server.log_commands = config->server.log_commands;
    updated->server.fast_require = config->server.fast_require;
    updated->server.http         = config->server.http;  /* shallow, pointers stay as defaults */
    updated->server.game         = config->server.game;
    updated->server.dispatch.log_requests = config->server.dispatch.log_requests;
    updated->server.debug_mode   = config->server.debug_mode;
    updated->version             = latest;

    if (ae_save_config(updated) != AE_OK) {
        ae_logger_warn(ae_logger, "Failed to save the updated configuration.");
    } else {
        ae_load_config();
    }

    config_container_free(updated);
}
