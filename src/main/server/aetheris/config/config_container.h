#ifndef CONFIG_CONTAINER_H
#define CONFIG_CONTAINER_H

/*
 * config_container.h
 */

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
 * Log level enum  (mirrors Logback Level)
 * ====================================================================== */
typedef enum {
    AE_LOG_LEVEL_ALL,
    AE_LOG_LEVEL_TRACE,
    AE_LOG_LEVEL_DEBUG,
    AE_LOG_LEVEL_INFO,
    AE_LOG_LEVEL_WARN,
    AE_LOG_LEVEL_ERROR,
    AE_LOG_LEVEL_OFF,
} ae_log_level_t;

/* =========================================================================
 * Leaf structs (innermost nested classes first)
 * ====================================================================== */

/* Database.DataStore */
typedef struct {
    char connection_uri[256];   /* "mongodb://localhost:27017" */
    char collection[64];        /* "aetheris"                  */
} ae_cfg_datastore_t;

/* Database */
typedef struct {
    ae_cfg_datastore_t server;
    ae_cfg_datastore_t game;
} ae_cfg_database_t;

/* Structure (folder paths) */
typedef struct {
    char resources[256];   /* "./resources/"       */
    char data[256];        /* "./data/"            */
    char packets[256];     /* "./packets/"         */
    char scripts[256];     /* "resources:Scripts/" */
    char plugins[256];     /* "./plugins/"         */
    char cache[256];       /* "./cache/"           */
} ae_cfg_structure_t;

/* Language */
typedef struct {
    char language[16];   /* BCP-47, e.g. "en-US" - Locale.getDefault() */
    char fallback[16];   /* "en-US"                                     */
    char document[8];    /* "EN"                                        */
} ae_cfg_language_t;

/* Account */
typedef struct {
    bool  auto_create;
    bool  experimental_real_password;
    char **default_permissions;      /* NULL-terminated array of strings */
    size_t default_permissions_len;
    int   max_player;                /* -1 = unlimited                   */
} ae_cfg_account_t;

/* Encryption */
typedef struct {
    bool use_encryption;    /* true  */
    bool use_in_routing;    /* true  */
    char keystore[256];     /* "./keystore.p12" */
    char keystore_password[128]; /* "123456"    */
} ae_cfg_encryption_t;

/* Policies.CORS */
typedef struct {
    bool   enabled;             /* true */
    char **allowed_origins;     /* {"*"} - NULL-terminated */
    size_t allowed_origins_len;
} ae_cfg_cors_t;

/* Policies */
typedef struct {
    ae_cfg_cors_t cors;
} ae_cfg_policies_t;

/* Files */
typedef struct {
    char index_file[256];   /* "./index.html" */
    char error_file[256];   /* "./404.html"   */
} ae_cfg_files_t;

/* HTTP */
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

/* GameOptions.InventoryLimits */
typedef struct {
    int weapons;    /* 2000  */
    int relics;     /* 2000  */
    int materials;  /* 2000  */
    int furniture;  /* 2000  */
    int all;        /* 30000 */
} ae_cfg_inventory_limits_t;

/* GameOptions.AvatarLimits */
typedef struct {
    int single_player_team;  /* 4 */
    int multiplayer_team;    /* 4 */
} ae_cfg_avatar_limits_t;

/* GameOptions.Rates */
typedef struct {
    float adventure_exp;  /* 1.0f */
    float mora;           /* 1.0f */
    float ley_lines;      /* 1.0f */
} ae_cfg_rates_t;

/* GameOptions.ResinOptions */
typedef struct {
    bool resin_usage;    /* false */
    int  cap;            /* 160   */
    int  recharge_time;  /* 480   */
} ae_cfg_resin_options_t;

/* GameOptions.Questing */
typedef struct {
    bool enabled;  /* true */
} ae_cfg_questing_t;

/* GameOptions.HandbookOptions.Limits */
typedef struct {
    bool enabled;       /* false */
    int  interval;      /* 3     */
    int  max_requests;  /* 10    */
    int  max_entities;  /* 25    */
} ae_cfg_handbook_limits_t;

/* GameOptions.HandbookOptions.Server */
typedef struct {
    bool enforced;        /* false       */
    char address[64];     /* "127.0.0.1" */
    int  port;            /* 443         */
    bool can_change;      /* true        */
} ae_cfg_handbook_server_t;

/* GameOptions.HandbookOptions */
typedef struct {
    bool enable;          /* false */
    bool allow_commands;  /* true  */
    ae_cfg_handbook_limits_t  limits;
    ae_cfg_handbook_server_t  server;
} ae_cfg_handbook_options_t;

/* GameOptions */
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

/* VisionOptions */
typedef struct {
    char name[64];    /* e.g. "VISION_LEVEL_NORMAL" */
    int  vision_range;
    int  grid_width;
} ae_cfg_vision_options_t;

/* JoinOptions.Mail.MailItem */
typedef struct {
    int item_id;
    int item_count;
    int item_level;
} ae_cfg_mail_item_t;

/* JoinOptions.Mail */
typedef struct {
    char title[256];
    char content[2048];
    char sender[64];
    ae_cfg_mail_item_t *items;      /* heap-allocated array */
    size_t              items_len;
} ae_cfg_join_mail_t;

/* JoinOptions */
typedef struct {
    int               welcome_emotes[3];   /* {2007, 1002, 4010} */
    char              welcome_message[512];
    ae_cfg_join_mail_t welcome_mail;
} ae_cfg_join_options_t;

/* ConsoleAccount */
typedef struct {
    int  avatar_id;         /* 10000007           */
    int  name_card_id;      /* 210001             */
    int  adventure_rank;    /* 1                  */
    int  world_level;       /* 0                  */
    char nick_name[64];     /* "Server"           */
    char signature[128];    /* "Welcome to Aetheris!" */
} ae_cfg_console_account_t;

/* Game */
#define AE_CFG_VISION_OPTIONS_COUNT 6
typedef struct {
    char bind_address[64];      /* "0.0.0.0"   */
    int  bind_port;             /* 22102       */
    char access_address[64];    /* "127.0.0.1" */
    int  access_port;           /* 0           */
    bool use_unique_packet_key; /* true        */
    int  load_entities_for_player_range; /* 300 */
    bool enable_script_in_big_world;     /* true */
    bool enable_console;                 /* true */
    int  kcp_interval;                   /* 20   */
    ae_server_debug_mode_t log_packets;  /* NONE */
    bool is_show_packet_payload;         /* false */
    bool is_show_loop_packets;           /* false */
    bool cache_scene_entities_every_run; /* false */
    ae_cfg_game_options_t     game_options;
    ae_cfg_join_options_t     join_options;
    ae_cfg_console_account_t  server_account;
    ae_cfg_vision_options_t   vision_options[AE_CFG_VISION_OPTIONS_COUNT];
} ae_cfg_game_t;

/* Region */
typedef struct {
    char name[64];     /* "os_usa"    */
    char title[64];    /* "Aetheris"  */
    char ip[64];       /* "127.0.0.1" */
    int  port;         /* 22102       */
} ae_cfg_region_t;

/* Dispatch */
typedef struct {
    ae_cfg_region_t       *regions;         /* heap-allocated array */
    size_t                 regions_len;
    char                   dispatch_url[256];  /* "ws://127.0.0.1:1111" */
    uint8_t               *encryption_key;     /* 32 random bytes       */
    size_t                 encryption_key_len;
    char                   dispatch_key[128];  /* base64(32 random bytes) */
    char                   default_name[64];   /* "Aetheris"             */
    ae_server_debug_mode_t log_requests;        /* NONE                  */
} ae_cfg_dispatch_t;

/* DebugMode */
typedef struct {
    ae_log_level_t          server_logger_level;    /* DEBUG */
    ae_log_level_t          services_loggers_level; /* INFO  */
    ae_server_debug_mode_t  log_packets;             /* ALL   */
    bool                    is_show_packet_payload;  /* false */
    bool                    is_show_loop_packets;    /* false */
    ae_server_debug_mode_t  log_requests;            /* ALL   */
} ae_cfg_debug_mode_t;

/* Server */
typedef struct {
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
 * Root config struct  (ConfigContainer itself)
 * ====================================================================== */
typedef struct {
    ae_cfg_structure_t  folder_structure;
    ae_cfg_database_t   database_info;
    ae_cfg_language_t   language;
    ae_cfg_account_t    account;
    ae_cfg_server_t     server;
    int                 version;   /* DO NOT TOUCH - must equal AE_CONFIG_CONTAINER_VERSION */
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
 * config_container_create_default()
 * Allocates and returns a fully populated default ae_config_t.
 * Caller owns the returned pointer; free with config_container_free().
 */
ae_config_t *config_container_create_default(void);

/**
 * config_container_free()
 * Releases all heap memory owned by the config (regions array,
 * encryption_key, default_permissions, CORS origins, mail items, etc.)
 * then frees the struct itself.
 */
void config_container_free(ae_config_t *config);

/**
 * config_container_update()
 * Mirrors ConfigContainer.updateConfig().
 * Checks the version field of the live ae_config; if outdated, copies
 * all fields into a fresh default instance, bumps the version, saves,
 * and reloads.
 */
void config_container_update(ae_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_CONTAINER_H */
