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
 * @file ae_config.h
 *
 * @brief Server configuration model with cJSON load/save and
 *        schema-version migration.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_CONFIG_H
#define AE_CONFIG_H

#include <aetheris/ae_error.h>
#include <aetheris/ae_types.h>

/* ============================================================ */

#define AE_CONFIG_VERSION 14

/* Bounded string/array sizes used by the (struct-based) model. */
#define AE_CONFIG_STR_LEN 256
#define AE_CONFIG_NAME_LEN 128
#define AE_CONFIG_MAX_ORIGINS 8
#define AE_CONFIG_MAX_PERMISSIONS 32
#define AE_CONFIG_MAX_REGIONS 16
#define AE_CONFIG_MAX_MAIL_ITEMS 8
#define AE_CONFIG_MAX_VISION 8

/* Run-mode and packet/http logging levels. */
typedef enum ae_config_run_mode {
    AE_CONFIG_RUN_HYBRID = 0,
    AE_CONFIG_RUN_DISPATCH_ONLY,
    AE_CONFIG_RUN_GAME_ONLY
} ae_config_run_mode_t;

typedef enum ae_config_debug_mode {
    AE_CONFIG_DEBUG_ALL = 0,
    AE_CONFIG_DEBUG_MISSING,
    AE_CONFIG_DEBUG_WHITELIST,
    AE_CONFIG_DEBUG_BLACKLIST,
    AE_CONFIG_DEBUG_NONE
} ae_config_debug_mode_t;

/* Log level mirror (logback names kept for serialization). */
typedef enum ae_config_log_level {
    AE_CONFIG_LOG_TRACE = 0,
    AE_CONFIG_LOG_DEBUG,
    AE_CONFIG_LOG_INFO,
    AE_CONFIG_LOG_WARN,
    AE_CONFIG_LOG_ERROR,
    AE_CONFIG_LOG_OFF
} ae_config_log_level_t;

/*
 * All containers below mirror the source model 1:1, including the
 * serialized JSON key names (camelCase).  String fields are inline
 * fixed buffers so the whole tree frees without a deep teardown.
 */

typedef struct ae_config_database_store {
    char connection_uri[AE_CONFIG_STR_LEN]; /* "connectionUri" */
    char collection[AE_CONFIG_STR_LEN];     /* "collection"     */
} ae_config_database_store_t;

typedef struct ae_config_database {
    ae_config_database_store_t server; /* "server" */
    ae_config_database_store_t game;   /* "game"   */
} ae_config_database_t;

typedef struct ae_config_structure {
    char resources[AE_CONFIG_STR_LEN];   /* "./resources/"  */
    char data[AE_CONFIG_STR_LEN];        /* "./data/"       */
    char packets[AE_CONFIG_STR_LEN];     /* "./packets/"    */
    char scripts[AE_CONFIG_STR_LEN];     /* "resources:Scripts/" */
    char plugins[AE_CONFIG_STR_LEN];     /* "./plugins/"    */
    char cache[AE_CONFIG_STR_LEN];       /* "./cache/"      */
} ae_config_structure_t;

typedef struct ae_config_language {
    char language[AE_CONFIG_NAME_LEN];   /* "language" ("en_US") */
    char fallback[AE_CONFIG_NAME_LEN];   /* "fallback"          */
    char document[AE_CONFIG_NAME_LEN];   /* "document"  ("EN")  */
} ae_config_language_t;

typedef struct ae_config_account {
    ae_bool auto_create;                  /* "autoCreate" */
    ae_bool experimental_real_password;   /* "EXPERIMENTAL_RealPassword" */
    char default_permissions[AE_CONFIG_MAX_PERMISSIONS][AE_CONFIG_NAME_LEN];
    ae_u32 default_permissions_count;
    ae_s32 max_player;                    /* "maxPlayer" (-1) */
} ae_config_account_t;

typedef struct ae_config_encryption {
    ae_bool use_encryption;    /* "useEncryption" */
    ae_bool use_in_routing;    /* "useInRouting"  */
    char keystore[AE_CONFIG_STR_LEN];
    char keystore_password[AE_CONFIG_STR_LEN];
} ae_config_encryption_t;

typedef struct ae_config_cors {
    ae_bool enabled;                                           /* "enabled" */
    char allowed_origins[AE_CONFIG_MAX_ORIGINS][AE_CONFIG_STR_LEN];
    ae_u32 allowed_origins_count;                              /* "allowedOrigins" */
} ae_config_cors_t;

typedef struct ae_config_policies {
    ae_config_cors_t cors; /* "cors" */
} ae_config_policies_t;

typedef struct ae_config_files {
    char index_file[AE_CONFIG_STR_LEN]; /* "indexFile" */
    char error_file[AE_CONFIG_STR_LEN]; /* "errorFile" */
} ae_config_files_t;

typedef struct ae_config_http {
    ae_bool start_immediately;  /* "startImmediately" */
    char bind_address[AE_CONFIG_STR_LEN];
    ae_s32 bind_port;
    char access_address[AE_CONFIG_STR_LEN];
    ae_s32 access_port;
    ae_config_encryption_t encryption;
    ae_config_policies_t policies;
    ae_config_files_t files;
} ae_config_http_t;

typedef struct ae_config_inventory_limits {
    ae_s32 weapons;
    ae_s32 relics;
    ae_s32 materials;
    ae_s32 furniture;
    ae_s32 all;
} ae_config_inventory_limits_t;

typedef struct ae_config_avatar_limits {
    ae_s32 single_player_team;
    ae_s32 multiplayer_team;
} ae_config_avatar_limits_t;

typedef struct ae_config_rates {
    float adventure_exp;
    float mora;
    float ley_lines;
} ae_config_rates_t;

typedef struct ae_config_resin_options {
    ae_bool resin_usage;
    ae_s32 cap;
    ae_s32 recharge_time;
} ae_config_resin_options_t;

typedef struct ae_config_questing {
    ae_bool enabled;
} ae_config_questing_t;

typedef struct ae_config_handbook_limits {
    ae_bool enabled;
    ae_s32 interval;
    ae_s32 max_requests;
    ae_s32 max_entities;
} ae_config_handbook_limits_t;

typedef struct ae_config_handbook_server {
    ae_bool enforced;
    char address[AE_CONFIG_STR_LEN];
    ae_s32 port;
    ae_bool can_change;
} ae_config_handbook_server_t;

typedef struct ae_config_handbook_options {
    ae_bool enable;
    ae_bool allow_commands;
    ae_config_handbook_limits_t limits;
    ae_config_handbook_server_t server;
} ae_config_handbook_options_t;

typedef struct ae_config_game_options {
    ae_config_inventory_limits_t inventory_limits;  /* "inventoryLimits" */
    ae_config_avatar_limits_t avatar_limits;        /* "avatarLimits" */
    ae_s32 scene_entity_limit;                      /* "sceneEntityLimit" */
    ae_bool watch_gacha_config;
    ae_bool enable_shop_items;
    ae_bool stamina_usage;
    ae_bool energy_usage;
    ae_bool fishhook_teleport;
    ae_bool trial_costumes;
    ae_bool force_legacy_drops;
    ae_config_questing_t questing;                  /* "questing" */
    ae_config_resin_options_t resin_options;        /* "resinOptions" */
    ae_config_rates_t rates;
    ae_config_handbook_options_t handbook;
} ae_config_game_options_t;

typedef struct ae_config_mail_item {
    ae_s32 item_id;
    ae_s32 item_count;
    ae_s32 item_level;
} ae_config_mail_item_t;

typedef struct ae_config_join_mail {
    char title[AE_CONFIG_STR_LEN];
    char content[1024];
    char sender[AE_CONFIG_STR_LEN];
    ae_config_mail_item_t items[AE_CONFIG_MAX_MAIL_ITEMS];
    ae_u32 items_count;
} ae_config_join_mail_t;

typedef struct ae_config_join_options {
    ae_s32 welcome_emotes[4];        /* "welcomeEmotes" */
    ae_u32 welcome_emotes_count;
    char welcome_message[AE_CONFIG_STR_LEN];
    ae_config_join_mail_t welcome_mail; /* "welcomeMail" */
} ae_config_join_options_t;

typedef struct ae_config_console_account {
    ae_s32 avatar_id;
    ae_s32 name_card_id;
    ae_s32 adventure_rank;
    ae_s32 world_level;
    char nick_name[AE_CONFIG_NAME_LEN];
    char signature[AE_CONFIG_STR_LEN];
} ae_config_console_account_t;

typedef struct ae_config_vision_options {
    char name[AE_CONFIG_NAME_LEN];
    ae_s32 vision_range;
    ae_s32 grid_width;
} ae_config_vision_options_t;

typedef struct ae_config_game {
    char bind_address[AE_CONFIG_STR_LEN];
    ae_s32 bind_port;
    char access_address[AE_CONFIG_STR_LEN];
    ae_s32 access_port;
    ae_bool use_unique_packet_key;
    ae_s32 load_entities_for_player_range;
    ae_bool enable_script_in_big_world;
    ae_bool enable_console;
    ae_s32 kcp_interval;
    ae_config_debug_mode_t log_packets;
    ae_bool is_show_packet_payload;
    ae_bool is_show_loop_packets;
    ae_bool cache_scene_entities_every_run;
    ae_config_game_options_t game_options;          /* "gameOptions" */
    ae_config_join_options_t join_options;          /* "joinOptions" */
    ae_config_console_account_t console_account;    /* "serverAccount" */
    ae_config_vision_options_t vision_options[AE_CONFIG_MAX_VISION];
    ae_u32 vision_options_count;                    /* "visionOptions" */
} ae_config_game_t;

typedef struct ae_config_region {
    char name[AE_CONFIG_NAME_LEN];   /* "Name" */
    char title[AE_CONFIG_STR_LEN];   /* "Title" */
    char ip[AE_CONFIG_STR_LEN];      /* "Ip"   */
    ae_s32 port;                     /* "Port" */
} ae_config_region_t;

typedef struct ae_config_dispatch {
    ae_config_region_t regions[AE_CONFIG_MAX_REGIONS]; /* "regions" */
    ae_u32 regions_count;
    char dispatch_url[AE_CONFIG_STR_LEN];
    char encryption_key[AE_CONFIG_STR_LEN]; /* base64 */
    char dispatch_key[AE_CONFIG_STR_LEN];   /* base64 */
    char default_name[AE_CONFIG_NAME_LEN];
    ae_config_debug_mode_t log_requests;
} ae_config_dispatch_t;

typedef struct ae_config_debug_options {
    ae_config_log_level_t server_logger_level;
    ae_config_log_level_t services_logger_level;
    ae_config_debug_mode_t log_packets;
    ae_bool is_show_packet_payload;
    ae_bool is_show_loop_packets;
    ae_config_debug_mode_t log_requests;
} ae_config_debug_options_t;

typedef struct ae_config_server {
    ae_s32 debug_whitelist[64];
    ae_u32 debug_whitelist_count;
    ae_s32 debug_blacklist[64];
    ae_u32 debug_blacklist_count;
    ae_config_run_mode_t run_mode;
    ae_bool log_commands;
    ae_bool fast_require;
    ae_config_http_t http;
    ae_config_game_t game;
    ae_config_dispatch_t dispatch;
    ae_config_debug_options_t debug_mode;
} ae_config_server_t;

typedef struct ae_config {
    ae_config_structure_t folder_structure; /* "folderStructure" */
    ae_config_database_t database_info;     /* "databaseInfo"    */
    ae_config_language_t language;          /* "language"        */
    ae_config_account_t account;            /* "account"         */
    ae_config_server_t server;              /* "server"          */
    ae_u32 version;                         /* "version"         */
} ae_config_t;

/* ============================================================ */

/**
 * ae_config_init - fill a config with the default values.
 * @config: config storage.
 *
 * Always succeeds and yields a schema-version 14 configuration.
 */
void ae_config_init(ae_config_t *config);

/**
 * ae_config_load_string - parse a JSON document into the config.
 * @config: config storage.
 * @json: null-terminated JSON text.
 *
 * Missing keys keep their current (usually default) values.
 * Returns AE_OK, AE_ERR_INVALID_ARG, or AE_ERR_BAD_DATA on malformed
 * JSON.  Unknown keys are ignored.
 */
ae_error_t ae_config_load_string(ae_config_t *config, const char *json);

/**
 * ae_config_load_file - read and parse a config file.
 * @config: config storage.
 * @path: file path.
 *
 * Returns AE_OK, AE_ERR_INVALID_ARG, AE_ERR_IO, or AE_ERR_BAD_DATA.
 */
ae_error_t ae_config_load_file(ae_config_t *config, const char *path);

/**
 * ae_config_save_string - serialize the config to a JSON string.
 * @config: config storage.
 * @out: receive a malloc'd string; caller frees with `free()`.
 * @pretty: true for indented output.
 *
 * Returns AE_OK or AE_ERR_INVALID_ARG.
 */
ae_error_t ae_config_save_string(const ae_config_t *config,
                                 char **out, ae_bool pretty);

/**
 * ae_config_save_file - write the config as indented JSON.
 * @config: config storage.
 * @path: file path.
 *
 * Returns AE_OK, AE_ERR_INVALID_ARG, or AE_ERR_IO.
 */
ae_error_t ae_config_save_file(const ae_config_t *config,
                               const char *path);

/**
 * ae_config_update - migrate an older config to the current schema.
 * @config: config storage.
 *
 * When the stored version is lower than AE_CONFIG_VERSION the field
 * is bumped to the current version.  Values are preserved; no field
 * is reset.  Returns true when a migration was applied.
 */
ae_bool ae_config_update(ae_config_t *config);

/* ============================================================ */

#endif /* AE_CONFIG_H */