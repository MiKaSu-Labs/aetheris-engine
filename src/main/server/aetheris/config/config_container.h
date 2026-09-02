/*
 *
 *           ______ _______ _    _ ______ _____  _____  _____
 *     /\   |  ____|__   __| |  | |  ____|  __ \|_   _|/ ____|
 *    /  \  | |__     | |  | |__| | |__  | |__) | | | | (___
 *   / /\ \ |  __|    | |  |  __  |  __| |  _  /  | |  \___ \
 *  / ____ \| |____   | |  | |  | | |____| | \ \ _| |_ ____) |
 * /_/    \_\______|  |_|  |_|  |_|______|_|  \_\_____|_____/
 *
 *                                                             v1.0
 * @name Aetheris
 *
 * @author dkitagawa
 *
 * @file config_container.h
 *
 * @brief Complete hierarchical configuration schema for the Aetheris server.
 *
 * Defines the full runtime configuration structure including server, game,
 * HTTP, dispatch, database, account, localization, debug, and policy settings.
 * Also provides lifecycle functions for creating, updating, and freeing
 * configuration containers with versioning support and default initialization.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef CONFIG_CONTAINER_H
#define CONFIG_CONTAINER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "aetheris.h"   /* ae_server_run_mode_t, ae_server_debug_mode_t */
#include "int_set.h"    /* ae_int_set_t                                 */

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Config schema version
 * DO NOT CHANGE
 * ====================================================================== */
#define AE_CONFIG_CONTAINER_VERSION 14

/* =========================================================================
 * Log level enum
 *
 * Ordered from most to least verbose.
 * ====================================================================== */
typedef enum {
    AE_LOG_LEVEL_ALL,    /**< Log everything. */
    AE_LOG_LEVEL_TRACE,
    AE_LOG_LEVEL_DEBUG,
    AE_LOG_LEVEL_INFO,
    AE_LOG_LEVEL_WARN,
    AE_LOG_LEVEL_ERROR,
    AE_LOG_LEVEL_OFF,    /**< Suppress all logging. */
} ae_log_level_t;

/* =========================================================================
 * Leaf structs (innermost nested structs first)
 * ====================================================================== */

/** One MongoDB connection's URI and target collection. Used twice, in
 *  ae_cfg_database_t::server and ::game. */
typedef struct {
    char connection_uri[256];   /* "mongodb://localhost:27017" */
    char collection[64];        /* "aetheris"                  */
} ae_cfg_datastore_t;

/** Database connections. Nested in ae_config_t::database_info. */
typedef struct {
    ae_cfg_datastore_t server;
    ae_cfg_datastore_t game;
} ae_cfg_database_t;

/** Filesystem paths the server reads from and writes to. Nested in
 *  ae_config_t::folder_structure. */
typedef struct {
    char resources[256];   /* "./resources/"       */
    char data[256];        /* "./data/"            */
    char packets[256];     /* "./packets/"         */
    char scripts[256];     /* "resources:Scripts/" */
    char plugins[256];     /* "./plugins/"         */
    char cache[256];       /* "./cache/"           */
} ae_cfg_structure_t;

/** Locale selection for translated server messages. Nested in
 *  ae_config_t::language. */
typedef struct {
    char language[16];   /* BCP-47, e.g. "en-US" */
    char fallback[16];   /* "en-US"              */
    char document[8];    /* Documentation language code, e.g. "EN" */
} ae_cfg_language_t;

/** Account creation and permission defaults. Nested in
 *  ae_config_t::account. */
typedef struct {
    bool  auto_create;
    bool  experimental_real_password;

    /** NULL-terminated array of permission-name strings. Ownership: each
     *  string and the array itself are heap-allocated and owned by the
     *  ae_config_t that holds this struct; both are freed by
     *  config_container_free(). NULL (with len 0) means no defaults. */
    char **default_permissions;
    size_t default_permissions_len;

    int   max_player;                /* -1 = unlimited */
} ae_cfg_account_t;

/** TLS keystore settings for the HTTP server. Nested in
 *  ae_cfg_http_t::encryption. */
typedef struct {
    bool use_encryption;    /* true  */
    bool use_in_routing;    /* true  */
    char keystore[256];     /* "./keystore.p12" */
    char keystore_password[128]; /* "123456"    */
} ae_cfg_encryption_t;

/** Cross-origin resource sharing policy for the HTTP server. Nested in
 *  ae_cfg_policies_t::cors. */
typedef struct {
    bool enabled;             /* true */

    /** NULL-terminated array of allowed-origin strings, e.g. {"*"}.
     *  Ownership: each string and the array itself are heap-allocated
     *  and owned by the ae_config_t that holds this struct; both are
     *  freed by config_container_free(). */
    char **allowed_origins;
    size_t allowed_origins_len;
} ae_cfg_cors_t;

/** HTTP-level access policies. Currently just CORS. Nested in
 *  ae_cfg_http_t::policies. */
typedef struct {
    ae_cfg_cors_t cors;
} ae_cfg_policies_t;

/** Static file paths served by the HTTP server. Nested in
 *  ae_cfg_http_t::files. */
typedef struct {
    char index_file[256];   /* "./index.html" */
    char error_file[256];   /* "./404.html"   */
} ae_cfg_files_t;

/** HTTP/dispatch front-end configuration. Nested in
 *  ae_cfg_server_t::http. */
typedef struct {
    bool start_immediately;     /* false       */
    char bind_address[64];      /* "0.0.0.0"   */
    int  bind_port;             /* 443         */
    char access_address[64];    /* "127.0.0.1" */
    int  access_port;           /* 0           */
    ae_cfg_encryption_t encryption;
    ae_cfg_policies_t   policies;
    ae_cfg_files_t      files;
} ae_cfg_http_t;

/** Per-category inventory size caps. Nested in
 *  ae_cfg_game_options_t::inventory_limits. */
typedef struct {
    int weapons;    /* 2000  */
    int relics;     /* 2000  */
    int materials;  /* 2000  */
    int furniture;  /* 2000  */
    int all;        /* 30000 */
} ae_cfg_inventory_limits_t;

/** Team size caps. Nested in ae_cfg_game_options_t::avatar_limits. */
typedef struct {
    int single_player_team;  /* 4 */
    int multiplayer_team;    /* 4 */
} ae_cfg_avatar_limits_t;

/** Gameplay reward rate multipliers. Nested in
 *  ae_cfg_game_options_t::rates. */
typedef struct {
    float adventure_exp;  /* 1.0f */
    float mora;           /* 1.0f */
    float ley_lines;      /* 1.0f */
} ae_cfg_rates_t;

/** Resin (stamina-like energy resource) system settings. Nested in
 *  ae_cfg_game_options_t::resin_options. */
typedef struct {
    bool resin_usage;    /* false */
    int  cap;            /* 160   */
    int  recharge_time;  /* 480, in seconds per point */
} ae_cfg_resin_options_t;

/** Quest system toggle. Nested in ae_cfg_game_options_t::questing. */
typedef struct {
    bool enabled;  /* true */
} ae_cfg_questing_t;

/** Rate limits for the GM handbook. Nested in
 *  ae_cfg_handbook_options_t::limits. */
typedef struct {
    bool enabled;       /* false */
    int  interval;      /* 3, in seconds  */
    int  max_requests;  /* 10, per interval */
    int  max_entities;  /* 25, per request  */
} ae_cfg_handbook_limits_t;

/** Network settings for the GM handbook's own HTTP server. Nested in
 *  ae_cfg_handbook_options_t::server. */
typedef struct {
    bool enforced;        /* false       */
    char address[64];     /* "127.0.0.1" */
    int  port;            /* 443         */
    bool can_change;      /* true; whether clients may override address/port */
} ae_cfg_handbook_server_t;

/** GM handbook feature toggle plus its rate limits and server settings.
 *  Nested in ae_cfg_game_options_t::handbook. */
typedef struct {
    bool enable;          /* false */
    bool allow_commands;  /* true  */
    ae_cfg_handbook_limits_t  limits;
    ae_cfg_handbook_server_t  server;
} ae_cfg_handbook_options_t;

/** Gameplay tuning: inventory/team limits, rates, and feature toggles.
 *  Nested in ae_cfg_game_t::game_options. */
typedef struct {
    ae_cfg_inventory_limits_t inventory_limits;
    ae_cfg_avatar_limits_t    avatar_limits;
    int   scene_entity_limit;       /* 1000  */
    bool  watch_gacha_config;       /* false */
    bool  enable_shop_items;        /* true  */
    bool  stamina_usage;            /* true  */
    bool  energy_usage;             /* true  */
    bool  fishhook_teleport;        /* true  */
    bool  trial_costumes;           /* false */
    bool  force_legacy_drops;       /* true  */
    ae_cfg_questing_t       questing;       /* JSON key: "questing" / "questOptions" */
    ae_cfg_resin_options_t  resin_options;
    ae_cfg_rates_t          rates;
    ae_cfg_handbook_options_t handbook;
} ae_cfg_game_options_t;

/** One avatar vision-range tier. Used as an array element in
 *  ae_cfg_game_t::vision_options. */
typedef struct {
    char name[64];    /* e.g. "VISION_LEVEL_NORMAL" */
    int  vision_range;
    int  grid_width;
} ae_cfg_vision_options_t;

/** One item attached to the new-player welcome mail. Used as an array
 *  element in ae_cfg_join_mail_t::items. */
typedef struct {
    int item_id;
    int item_count;
    int item_level;
} ae_cfg_mail_item_t;

/** The in-game mail new players receive on first login. Nested in
 *  ae_cfg_join_options_t::welcome_mail. */
typedef struct {
    char title[256];
    char content[2048];
    char sender[64];

    /** Heap-allocated array of attached items; NULL with items_len 0 if
     *  none. Ownership: owned by the ae_config_t that holds this struct;
     *  freed by config_container_free(). */
    ae_cfg_mail_item_t *items;
    size_t              items_len;
} ae_cfg_join_mail_t;

/** First-login experience: starting emotes, a welcome message, and
 *  welcome mail. Nested in ae_cfg_game_t::join_options. */
typedef struct {
    int               welcome_emotes[3];   /* {2007, 1002, 4010} */
    char              welcome_message[512];
    ae_cfg_join_mail_t welcome_mail;
} ae_cfg_join_options_t;

/** Identity the server console presents when it acts as a player (e.g.
 *  for GM commands). Nested in ae_cfg_game_t::server_account. */
typedef struct {
    int  avatar_id;         /* 10000007           */
    int  name_card_id;      /* 210001             */
    int  adventure_rank;    /* 1                  */
    int  world_level;       /* 0                  */
    char nick_name[64];     /* "Server"           */
    char signature[128];    /* "Welcome to Aetheris!" */
} ae_cfg_console_account_t;

/** Number of entries in ae_cfg_game_t::vision_options. */
#define AE_CFG_VISION_OPTIONS_COUNT 6

/** Game server network and gameplay configuration. Nested in
 *  ae_cfg_server_t::game. */
typedef struct {
    char bind_address[64];      /* "0.0.0.0"   */
    int  bind_port;             /* 22102       */
    char access_address[64];    /* "127.0.0.1" */
    int  access_port;           /* 0           */
    bool use_unique_packet_key; /* true        */
    int  load_entities_for_player_range; /* 300 */
    bool enable_script_in_big_world;     /* true */
    bool enable_console;                 /* true */
    int  kcp_interval;                   /* 20, in milliseconds */
    ae_server_debug_mode_t log_packets;  /* NONE */
    bool is_show_packet_payload;         /* false */
    bool is_show_loop_packets;           /* false */
    bool cache_scene_entities_every_run; /* false */
    ae_cfg_game_options_t     game_options;
    ae_cfg_join_options_t     join_options;
    ae_cfg_console_account_t  server_account;
    ae_cfg_vision_options_t   vision_options[AE_CFG_VISION_OPTIONS_COUNT];
} ae_cfg_game_t;

/** One selectable region entry shown to clients at login. Used as an
 *  array element in ae_cfg_dispatch_t::regions. */
typedef struct {
    char name[64];     /* "os_usa"    */
    char title[64];    /* "Aetheris"  */
    char ip[64];       /* "127.0.0.1" */
    int  port;         /* 22102       */
} ae_cfg_region_t;

/** Dispatch server settings: the region list clients choose from, and
 *  the session encryption key used to secure client/server traffic.
 *  Nested in ae_cfg_server_t::dispatch. */
typedef struct {
    /** Heap-allocated array of selectable regions; NULL with regions_len
     *  0 if none configured. Ownership: owned by the ae_config_t that
     *  holds this struct; freed by config_container_free(). */
    ae_cfg_region_t *regions;
    size_t            regions_len;

    char dispatch_url[256];  /* "ws://127.0.0.1:1111" */

    /** 32 random bytes generated at first config creation. Ownership:
     *  heap-allocated, owned by the ae_config_t that holds this struct;
     *  freed by config_container_free(). NULL if generation failed. */
    uint8_t *encryption_key;
    size_t   encryption_key_len;

    char dispatch_key[128];  /* base64(encryption_key) */
    char default_name[64];   /* "Aetheris"             */
    ae_server_debug_mode_t log_requests;  /* NONE */
} ae_cfg_dispatch_t;

/** Logging verbosity and packet/request tracing toggles. Nested in
 *  ae_cfg_server_t::debug_mode. */
typedef struct {
    ae_log_level_t          server_logger_level;    /* DEBUG */
    ae_log_level_t          services_loggers_level; /* INFO  */
    ae_server_debug_mode_t  log_packets;             /* ALL   */
    bool                    is_show_packet_payload;  /* false */
    bool                    is_show_loop_packets;    /* false */
    ae_server_debug_mode_t  log_requests;            /* ALL   */
} ae_cfg_debug_mode_t;

/** Top-level server configuration: run mode, debug ID filters, and the
 *  http/game/dispatch subsystems. Nested in ae_config_t::server. */
typedef struct {
    /** Opcode/entity ID filters for debug logging. Ownership: both are
     *  heap-allocated ae_int_set_t instances owned by the ae_config_t
     *  that holds this struct; freed by config_container_free(). */
    ae_int_set_t          *debug_whitelist;  /* empty set */
    ae_int_set_t          *debug_blacklist;  /* empty set */

    ae_server_run_mode_t   run_mode;         /* HYBRID    */
    bool                   log_commands;     /* false     */
    bool                   fast_require;     /* true      */
    ae_cfg_http_t          http;
    ae_cfg_game_t          game;
    ae_cfg_dispatch_t      dispatch;
    ae_cfg_debug_mode_t    debug_mode;
} ae_cfg_server_t;

/* =========================================================================
 * Root config struct
 *
 * The complete Aetheris server configuration. One instance is held at a
 * time in the global ae_config (declared in aetheris.h); see
 * config_container_create_default(), config_container_update(), and
 * config_container_free() below for its lifecycle.
 * ====================================================================== */
typedef struct {
    ae_cfg_structure_t  folder_structure;
    ae_cfg_database_t   database_info;
    ae_cfg_language_t   language;
    ae_cfg_account_t    account;
    ae_cfg_server_t     server;

    /** Schema version this instance was built against. Set by
     *  config_container_create_default() and by config_container_update()
     *  after a successful migration. DO NOT set this field by hand --
     *  it must always equal AE_CONFIG_CONTAINER_VERSION for a config
     *  that config_container_update() will treat as current. */
    int version;
} ae_config_t;

/* =========================================================================
 * Convenience typedefs so configuration.h aliases stay readable
 * ====================================================================== */
typedef ae_cfg_database_t        ae_database_config_t;
typedef ae_cfg_language_t        ae_language_config_t;
typedef ae_cfg_account_t         ae_account_config_t;
typedef ae_cfg_server_t          ae_server_config_t;
typedef ae_cfg_http_t            ae_http_config_t;
typedef ae_cfg_game_t            ae_game_config_t;
typedef ae_cfg_dispatch_t        ae_dispatch_config_t;
typedef ae_cfg_debug_mode_t      ae_debug_mode_config_t;
typedef ae_cfg_encryption_t      ae_encryption_config_t;
typedef ae_cfg_policies_t        ae_policies_config_t;
typedef ae_cfg_files_t           ae_files_config_t;
typedef ae_cfg_game_options_t    ae_game_options_t;
typedef ae_cfg_inventory_limits_t ae_inventory_limits_t;
typedef ae_cfg_handbook_options_t ae_handbook_options_t;

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

/**
 * @brief Allocate and return a fully populated default ae_config_t.
 *
 * Not thread-safe. Call from the main thread only.
 *
 * @return A fully populated default configuration. The caller owns it
 *         and must release it with config_container_free().
 * @return NULL if the top-level allocation fails. Some individual
 *         sub-allocations inside a successfully-returned config (the
 *         dispatch encryption key, CORS origins, welcome mail items)
 *         are best-effort: on their own failure they are left NULL
 *         with their matching _len field at 0, which every consumer of
 *         those fields must treat as "absent" rather than "error."
 */
ae_config_t *config_container_create_default(void);

/**
 * @brief Release all heap memory owned by a config, then the struct
 *        itself.
 *
 * Frees the regions array, encryption_key, default_permissions (array
 * and each string), CORS allowed_origins (array and each string), and
 * welcome mail items, then frees config itself.
 *
 * @param config Configuration to release. May be NULL, in which case
 *               this is a no-op. After this call, config and every
 *               pointer it owned are invalid and must not be used.
 */
void config_container_free(ae_config_t *config);

/**
 * @brief Migrate a configuration to the current schema version in place.
 *
 * If config->version already equals AE_CONFIG_CONTAINER_VERSION, this
 * is a no-op. Otherwise: builds a fresh default configuration, copies
 * config's plain (non-heap-owned) field values onto it, saves it to
 * disk, and reloads. Heap-owned fields (regions, encryption_key, CORS
 * origins, mail items, default_permissions) are intentionally left as
 * the fresh defaults rather than carried over, since a schema version
 * bump may have changed their shape or meaning; any customization in
 * those specific fields is lost across a migration.
 *
 * @warning As a side effect of the reload above, the ae_config_t that
 * config points to may be freed before this function returns (via the
 * global ae_config reload path in aetheris.c), making config a dangling
 * pointer. Callers must not use config after calling this function;
 * re-read the current configuration through the global ae_config
 * instead of holding onto the pointer passed in here.
 *
 * @param config Configuration to check and, if needed, migrate. Must
 *               not be NULL.
 *
 * Not thread-safe. Call from the main thread only.
 */
void config_container_update(ae_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_CONTAINER_H */
