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
 * @file aetheris.h
 *
 * @brief Core Aetheris server framework API.
 *
 * Declares global state, lifecycle entry points (init, run, shutdown),
 * configuration and language subsystems, run-mode selection, thread-pool
 * utilities, and opaque handles for every major server subsystem.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AETHERIS_H
#define AETHERIS_H

#include <stdbool.h>

/* =========================================================================
 * Error codes
 * ====================================================================== */

typedef enum {
    AE_OK            =  0,  /**< Success. */
    AE_ERR_GENERIC   = -1,  /**< Unspecified error. */
    AE_ERR_IO        = -2,  /**< I/O failure (file open, read, write). */
    AE_ERR_CONFIG    = -3,  /**< Configuration parse or validation failure. */
    AE_ERR_INTERRUPT = -4,  /**< Operation interrupted by signal. */
} ae_error_t;

/* =========================================================================
 * Enumerations
 * ====================================================================== */

/** Controls which server subsystems are started at runtime. */
typedef enum {
    SERVER_RUN_MODE_HYBRID,         /**< Start both game and HTTP servers. */
    SERVER_RUN_MODE_DISPATCH_ONLY,  /**< Start HTTP + dispatch server only. */
    SERVER_RUN_MODE_GAME_ONLY,      /**< Start game server only. */
} ae_server_run_mode_t;

/** Controls which packet opcodes are logged during a session. */
typedef enum {
    SERVER_DEBUG_MODE_ALL,          /**< Log every opcode. */
    SERVER_DEBUG_MODE_MISSING,      /**< Log only unhandled opcodes. */
    SERVER_DEBUG_MODE_WHITELIST,    /**< Log only explicitly listed opcodes. */
    SERVER_DEBUG_MODE_BLACKLIST,    /**< Log all except listed opcodes. */
    SERVER_DEBUG_MODE_NONE,         /**< Suppress all opcode logging. */
} ae_server_debug_mode_t;

/* =========================================================================
 * Opaque type forward declarations
 * ====================================================================== */

typedef struct ae_logger          ae_logger_t;
typedef struct ae_config          ae_config_t;
typedef struct ae_language        ae_language_t;
typedef struct ae_http_server     ae_http_server_t;
typedef struct ae_game_server     ae_game_server_t;
typedef struct ae_dispatch_server ae_dispatch_server_t;
typedef struct ae_plugin_manager  ae_plugin_manager_t;
typedef struct ae_command_map     ae_command_map_t;
typedef struct ae_auth_system     ae_auth_system_t;
typedef struct ae_perm_handler    ae_perm_handler_t;
typedef struct ae_thread_pool     ae_thread_pool_t;
typedef struct ae_console_reader  ae_console_reader_t;

/* =========================================================================
 * Global state
 *
 * All pointers are NULL until ae_init() or the relevant subsystem
 * initializer has run. Callers must not free these pointers directly;
 * ae_shutdown() owns their lifetimes.
 * ====================================================================== */

/** Path to the JSON configuration file. Never NULL; set at process start. */
extern const char *AE_CONFIG_FILE;

extern ae_logger_t          *ae_logger;           /**< Primary server logger. */
extern ae_config_t          *ae_config;            /**< Active server config. */
extern ae_language_t        *ae_language;          /**< Active language pack. */
extern char                 *ae_preferred_language; /**< BCP-47 locale tag. */
extern int                   ae_current_day_of_week; /**< 1=Sunday ... 7=Saturday. */

extern ae_http_server_t     *ae_http_server;       /**< HTTP/dispatch front-end. */
extern ae_game_server_t     *ae_game_server;       /**< Game session handler. */
extern ae_dispatch_server_t *ae_dispatch_server;   /**< Dispatch-only TCP listener. */
extern ae_plugin_manager_t  *ae_plugin_manager;    /**< Loaded plugin registry. */
extern ae_command_map_t     *ae_command_map;       /**< Console command registry. */

extern ae_auth_system_t     *ae_auth_system;       /**< Active authentication impl. */
extern ae_perm_handler_t    *ae_perm_handler;      /**< Active permission impl. */

extern ae_thread_pool_t     *ae_thread_pool;       /**< Shared background thread pool. */
extern ae_console_reader_t  *ae_console_reader;    /**< Interactive console state. */

/**
 * Run-mode override set by command-line arguments.
 * When cast to int this equals -1, the value from ae_config is used.
 * Set before calling ae_main().
 */
extern ae_server_run_mode_t ae_run_mode_override;

/** When true, the interactive console is suppressed in DISPATCH_ONLY mode. */
extern bool ae_no_console;

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

/**
 * @brief Initialize the Aetheris runtime.
 *
 * Must be called once before ae_main(). Sets up the logger, loads and
 * optionally migrates the server configuration, loads the language pack,
 * runs structural startup checks, and creates the global thread pool.
 *
 * On any non-AE_OK return, global state may be left partially initialized
 * (e.g., the logger or config set while a later step failed). The caller
 * must treat this as fatal: do not call ae_main(); terminate the process
 * instead of retrying.
 *
 * Not thread-safe. Call from the main thread only, exactly once.
 *
 * @return AE_OK on success.
 * @return AE_ERR_GENERIC if the logger or thread pool cannot be created.
 * @return AE_ERR_CONFIG if the configuration file exists but is malformed.
 * @return AE_ERR_IO if the configuration file cannot be written during
 *         first-run default generation.
 */
ae_error_t ae_init(void);

/**
 * @brief Run the Aetheris server.
 *
 * Parses startup arguments, creates server instances for the active run
 * mode, loads game resources, starts servers and plugins, registers
 * signal-based shutdown, and then blocks in the interactive console loop
 * until the process exits.
 *
 * On any non-AE_OK return, one or more subsystems may already have been
 * created or started. The caller must treat this as fatal and terminate
 * the process; calling ae_shutdown() first is safe (it NULL-checks each
 * subsystem before use) but is not required before exiting.
 *
 * ae_init() must have returned AE_OK before this is called.
 * Not thread-safe. Call from the main thread only.
 *
 * @param argc  Argument count forwarded from main().
 * @param argv  Argument vector forwarded from main(). May be NULL when
 *              argc is 0, but passing a valid argv is strongly preferred.
 * @return AE_OK on clean exit (including early exit from argument parsing).
 * @return AE_ERR_GENERIC if a required subsystem fails to start.
 * @return AE_ERR_IO      if a required I/O operation fails during startup.
 */
ae_error_t ae_main(int argc, char **argv);

/**
 * @brief Shut down the Aetheris runtime.
 *
 * Disables all plugins, signals the game server to stop, and drains both
 * the main thread pool and the database event executor, each with a
 * 5-second timeout. Safe to call from a signal handler (internally it
 * only calls async-signal-safe primitives before delegating to non-safe
 * teardown from the main thread).
 *
 * Idempotent: calling it more than once is safe; subsystem pointers are
 * checked for NULL before use. Registered automatically for SIGINT and
 * SIGTERM by ae_main().
 */
void ae_shutdown(void);

/* =========================================================================
 * Dispatch / server start helpers
 * ====================================================================== */

/**
 * @brief Start the HTTP server and, in DISPATCH_ONLY mode, the dispatch
 *        TCP listener.
 *
 * Idempotent with respect to the HTTP server (http_server_start() is
 * responsible for guarding against double-start). In DISPATCH_ONLY mode
 * a dispatch_server is created bound to 0.0.0.0:1111.
 *
 * Not thread-safe. Must be called from the main thread during startup.
 *
 * @return AE_OK on success.
 * @return AE_ERR_GENERIC if the dispatch server cannot be created or
 *         either server fails to bind/listen.
 */
ae_error_t ae_start_dispatch(void);

/* =========================================================================
 * Configuration subsystem
 * ====================================================================== */

/**
 * @brief Load the server configuration from AE_CONFIG_FILE.
 *
 * If the file does not exist, a default configuration is generated,
 * written to disk via ae_save_config(), and assigned to ae_config.
 * If the file exists but cannot be parsed, an error is logged and
 * AE_ERR_CONFIG is returned. On any failure, ae_config retains its
 * previous value (NULL if this is the first call); a previous
 * configuration is only released after a new one has been successfully
 * loaded.
 *
 * Not thread-safe. ae_config must not be accessed concurrently.
 *
 * @return AE_OK on success.
 * @return AE_ERR_CONFIG if the file exists but is malformed.
 * @return AE_ERR_GENERIC if no config file exists and a default
 *         configuration cannot be allocated.
 * @return AE_ERR_IO if the default configuration cannot be written.
 */
ae_error_t ae_load_config(void);

/**
 * @brief Serialize and write a configuration to AE_CONFIG_FILE.
 *
 * Writes to a temporary file alongside AE_CONFIG_FILE and renames it
 * into place on success, so a failed encode or a failed write never
 * truncates or corrupts an existing, valid configuration file.
 *
 * @param config  Configuration to write. If NULL, a freshly allocated
 *                default configuration is used and freed before return;
 *                the caller need not manage that temporary object.
 *                If non-NULL, the pointer is borrowed for the duration
 *                of the call only; the caller retains ownership.
 *
 * Not thread-safe. ae_config must not be accessed concurrently.
 *
 * @return AE_OK on success.
 * @return AE_ERR_IO if the temporary file cannot be opened, written,
 *         flushed, or renamed into place.
 * @return AE_ERR_GENERIC if config is NULL and a default configuration
 *         cannot be allocated, or if the configuration cannot be
 *         encoded to JSON.
 */
ae_error_t ae_save_config(const ae_config_t *config);

/* =========================================================================
 * Language subsystem
 * ====================================================================== */

/**
 * @brief Load the language pack selected by ae_config->language.language.
 *
 * Resolves the locale string to a BCP-47 code and looks up the matching
 * ae_language_t. On success, ae_language is set to the returned pack; on
 * failure, ae_language retains its previous value (NULL if this is the
 * first call). The returned pointer is borrowed from the language
 * registry; do not free.
 *
 * Not thread-safe. Call from the main thread during startup or reload.
 *
 * @return AE_OK if the language pack was found and set.
 * @return AE_ERR_GENERIC if no pack matches the configured locale.
 */
ae_error_t ae_load_language(void);

/**
 * @brief Look up a language pack by BCP-47 code.
 *
 * @param lang_code  BCP-47 language code (e.g. "en", "zh-CN"). Must not
 *                   be NULL.
 * @return Borrowed pointer to the matching ae_language_t, or NULL if none
 *         is registered for that code. Do not free the returned pointer.
 */
ae_language_t *ae_get_language_by_code(const char *lang_code);

/* =========================================================================
 * Run-mode helpers
 * ====================================================================== */

/**
 * @brief Return the effective server run mode.
 *
 * Returns ae_run_mode_override when it has been set (i.e., cast to int
 * it is not -1), otherwise returns ae_config->server.run_mode.
 *
 * @return The active ae_server_run_mode_t value.
 */
ae_server_run_mode_t ae_get_run_mode(void);

/* =========================================================================
 * Console
 * ====================================================================== */

/**
 * @brief Return the global console reader, creating it on first call.
 *
 * Lazy-initializes ae_console_reader via console_reader_create() if it
 * is NULL. The returned pointer is owned by the global state; do not free.
 *
 * Not thread-safe with respect to the first call. Subsequent calls after
 * initialization are safe to make from any thread.
 *
 * @return Pointer to the ae_console_reader_t. Never NULL on success;
 *         aborts on allocation failure.
 */
ae_console_reader_t *ae_get_console(void);

/**
 * @brief Start the interactive console and block until the process exits.
 *
 * In DISPATCH_ONLY + ae_no_console mode, logs a message and returns
 * immediately without blocking. Otherwise blocks in a read loop, invoking
 * ae_command_map for each input line. A double Ctrl-C exits the process.
 *
 * Not thread-safe. Call from the main thread only.
 */
void ae_start_console(void);

/* =========================================================================
 * Utilities
 * ====================================================================== */

/**
 * @brief Refresh ae_current_day_of_week from the system clock.
 *
 * Sets ae_current_day_of_week to a 1-based weekday index (1=Sunday,
 * 7=Saturday) derived from localtime(). Call once at startup and again
 * whenever a day boundary is crossed.
 *
 * Not thread-safe with respect to ae_current_day_of_week.
 */
void ae_update_day_of_week(void);

/**
 * @brief Reload config, language, and game subsystem data in place.
 *
 * Calls ae_load_config(), ae_load_language(), and the gacha and shop
 * system loaders on ae_game_server. Intended for live-reload scenarios
 * triggered from the console or a signal handler.
 *
 * Not thread-safe. Subsystem state is replaced without locking; ensure
 * no other threads are actively reading the affected globals.
 */
void ae_force_reload(void);

/* =========================================================================
 * Thread-pool helpers
 * ====================================================================== */

/**
 * @brief Return the global shared thread pool.
 *
 * @return ae_thread_pool, which may be NULL before ae_init() succeeds.
 *         The returned pointer is owned by the global state; do not free.
 */
ae_thread_pool_t *ae_get_thread_pool(void);

/**
 * @brief Submit a work item to the given thread pool.
 *
 * @param pool  Thread pool to submit to. Must not be NULL.
 * @param fn    Function to execute. Must not be NULL.
 * @param arg   Opaque argument forwarded to fn. May be NULL.
 *
 * @return AE_OK on success.
 * @return AE_ERR_GENERIC if the pool is saturated or the item cannot be
 *         queued.
 */
ae_error_t ae_thread_pool_submit(ae_thread_pool_t *pool,
                                 void (*fn)(void *), void *arg);

/**
 * @brief Signal the pool to stop accepting new work and wait for
 *        in-flight items to complete.
 *
 * Blocks until all running tasks finish or until timeout_seconds elapses,
 * whichever comes first. Tasks still queued after the timeout are
 * abandoned.
 *
 * @param pool             Thread pool to drain. Must not be NULL.
 * @param timeout_seconds  Maximum number of seconds to wait. Pass 0 to
 *                         return immediately after signalling stop.
 */
void ae_thread_pool_shutdown(ae_thread_pool_t *pool, int timeout_seconds);

#endif /* AETHERIS_H */
