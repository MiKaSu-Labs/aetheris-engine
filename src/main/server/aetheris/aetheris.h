/*
 * 
 *           ______ _______ _    _ ______ _____  _____  _____ 
 *     /\   |  ____|__   __| |  | |  ____|  __ \|_   _|/ ____|
 *    /  \  | |__     | |  | |__| | |__  | |__) | | | | (___  
 *   / /\ \ |  __|    | |  |  __  |  __| |  _  /  | |  \___ \ 
 *  / ____ \| |____   | |  | |  | | |____| | \ \ _| |_ ____) |
 * /_/    \_\______|  |_|  |_|  |_|______|_|  \_\_____|_____/ 
 * 
 *                                                                                     v1.0
 * @name Aetheris
 *
 * @author dkitagawa
 *
 * @file aetheris.h
 *
 * @brief Core Aetheris server framework API.
 *
 * Provides global state, lifecycle management (init, run, shutdown),
 * configuration and language handling, server mode selection,
 * thread pool utilities, and access to major subsystems such as
 * HTTP/game/dispatch servers, plugin system, and console interface.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 * 
 */

#ifndef AETHERIS_H
#define AETHERIS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/* -------------------------------------------------------------------------
 * Error codes
 * ---------------------------------------------------------------------- */
typedef enum {
    AE_OK            =  0,
    AE_ERR_GENERIC   = -1,
    AE_ERR_IO        = -2,
    AE_ERR_CONFIG    = -3,
    AE_ERR_INTERRUPT = -4,
} ae_error_t;

/* -------------------------------------------------------------------------
 * Enums
 * ---------------------------------------------------------------------- */
typedef enum {
    SERVER_RUN_MODE_HYBRID,
    SERVER_RUN_MODE_DISPATCH_ONLY,
    SERVER_RUN_MODE_GAME_ONLY,
} ae_server_run_mode_t;

typedef enum {
    SERVER_DEBUG_MODE_ALL,
    SERVER_DEBUG_MODE_MISSING,
    SERVER_DEBUG_MODE_WHITELIST,
    SERVER_DEBUG_MODE_BLACKLIST,
    SERVER_DEBUG_MODE_NONE,
} ae_server_debug_mode_t;

/* -------------------------------------------------------------------------
 * Opaque forward declarations
 * ---------------------------------------------------------------------- */
typedef struct ae_logger         ae_logger_t;
typedef struct ae_config         ae_config_t;
typedef struct ae_language       ae_language_t;
typedef struct ae_http_server    ae_http_server_t;
typedef struct ae_game_server    ae_game_server_t;
typedef struct ae_dispatch_server ae_dispatch_server_t;
typedef struct ae_plugin_manager ae_plugin_manager_t;
typedef struct ae_command_map    ae_command_map_t;
typedef struct ae_auth_system    ae_auth_system_t;
typedef struct ae_perm_handler   ae_perm_handler_t;
typedef struct ae_thread_pool    ae_thread_pool_t;
typedef struct ae_console_reader ae_console_reader_t;

/* -------------------------------------------------------------------------
 * Global state
 * All declared extern; defined in aetheris.c
 * ---------------------------------------------------------------------- */
extern const char          *AE_CONFIG_FILE;   /* "./config.json" */

extern ae_logger_t         *ae_logger;
extern ae_config_t         *ae_config;
extern ae_language_t       *ae_language;
extern char                *ae_preferred_language;
extern int                  ae_current_day_of_week;

extern ae_http_server_t    *ae_http_server;
extern ae_game_server_t    *ae_game_server;
extern ae_dispatch_server_t *ae_dispatch_server;
extern ae_plugin_manager_t *ae_plugin_manager;
extern ae_command_map_t    *ae_command_map;

extern ae_auth_system_t    *ae_auth_system;
extern ae_perm_handler_t   *ae_perm_handler;

extern ae_thread_pool_t    *ae_thread_pool;
extern ae_console_reader_t *ae_console_reader;

/* Run-mode override; set to -1 to use config value */
extern ae_server_run_mode_t ae_run_mode_override;
extern bool                 ae_no_console;

/* -------------------------------------------------------------------------
 * Lifecycle
 * ---------------------------------------------------------------------- */

/**
 * ae_init()
 * Must be called before ae_main().
 */
ae_error_t ae_init(void);

/**
 * ae_main() - replaces public static void main(String[] args).
 * argc/argv are forwarded from the process entry point.
 */
ae_error_t ae_main(int argc, char **argv);

/**
 * ae_shutdown() - registered as a shutdown hook; safe to call manually.
 */
void ae_shutdown(void);

/* -------------------------------------------------------------------------
 * Dispatch / server start helpers
 * ---------------------------------------------------------------------- */
ae_error_t ae_start_dispatch(void);

/* -------------------------------------------------------------------------
 * Configuration subsystem
 * ---------------------------------------------------------------------- */
ae_error_t ae_load_config(void);
ae_error_t ae_save_config(const ae_config_t *config); /* pass NULL for defaults */

/* -------------------------------------------------------------------------
 * Language subsystem
 * ---------------------------------------------------------------------- */
ae_error_t      ae_load_language(void);
ae_language_t  *ae_get_language_by_code(const char *lang_code);

/* -------------------------------------------------------------------------
 * Run-mode helpers
 * ---------------------------------------------------------------------- */
ae_server_run_mode_t ae_get_run_mode(void);

/* -------------------------------------------------------------------------
 * Console
 * ---------------------------------------------------------------------- */
ae_console_reader_t *ae_get_console(void);   /* lazy-init, like getConsole() */
void                 ae_start_console(void);

/* -------------------------------------------------------------------------
 * Utilities
 * ---------------------------------------------------------------------- */
void ae_update_day_of_week(void);
void ae_force_reload(void);

/* -------------------------------------------------------------------------
 * Thread-pool helpers
 * ---------------------------------------------------------------------- */
ae_thread_pool_t *ae_get_thread_pool(void);
ae_error_t        ae_thread_pool_submit(ae_thread_pool_t *pool,
                                        void (*fn)(void *), void *arg);
void              ae_thread_pool_shutdown(ae_thread_pool_t *pool,
                                          int timeout_seconds);

#endif /* AETHERIS_H */
