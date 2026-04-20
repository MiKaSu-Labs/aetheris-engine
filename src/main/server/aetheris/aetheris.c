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
 * @file aetheris.c
 *
 * @brief Initializes and runs the Aetheris server runtime, setting up configuration, subsystems, servers, plugins, and the interactive console, and managing the full application lifecycle including startup and shutdown.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 * Compile (example):
 * 
 *   cc -std=c11 -Wall -Wextra -o aetheris aetheris.c \
 *       -lpthread -lreadline \
 *       $(pkg-config --cflags --libs jansson)   # for JSON config
 */

#include "aetheris.h"

#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Pull in the subsystem headers (stubs until each module is translated) */
#include "auth_default.h"
#include "command_map.h"
#include "crypto.h"
#include "database_manager.h"
#include "dispatch_server.h"
#include "game_server.h"
#include "http_server.h"
#include "json_utils.h"
#include "lang.h"
#include "perm_default.h"
#include "plugin_manager.h"
#include "resource_loader.h"
#include "server_helper.h"
#include "startup_args.h"
#include "tools.h"
#include "utils.h"

/* =========================================================================
 * Global state definitions
 * ====================================================================== */

const char *AE_CONFIG_FILE = "./config.json";

ae_logger_t *ae_logger = NULL;
ae_config_t *ae_config = NULL;
ae_language_t *ae_language = NULL;
char *ae_preferred_language = NULL;
int ae_current_day_of_week = 0;

ae_http_server_t *ae_http_server = NULL;
ae_game_server_t *ae_game_server = NULL;
ae_dispatch_server_t *ae_dispatch_server = NULL;
ae_plugin_manager_t *ae_plugin_manager = NULL;
ae_command_map_t *ae_command_map = NULL;

ae_auth_system_t *ae_auth_system = NULL;
ae_perm_handler_t *ae_perm_handler = NULL;

ae_thread_pool_t *ae_thread_pool = NULL;
ae_console_reader_t *ae_console_reader = NULL;

/* -1 sentinel means "use config value" */
ae_server_run_mode_t ae_run_mode_override = (ae_server_run_mode_t)-1;
bool ae_no_console = false;

/* =========================================================================
 * Internal helpers
 * ====================================================================== */

/* Replaces Runtime.getRuntime().addShutdownHook(...) */
static void _signal_handler(int sig) {
  (void)sig;
  ae_shutdown();
  exit(0);
}

/* =========================================================================
 * ae_init()  -  Static initialiser block
 * ====================================================================== */
ae_error_t ae_init(void) {
  ae_error_t err;

  /* Set up logger (replaces LoggerFactory.getLogger + logback config) */
  ae_logger = ae_logger_create("Aetheris");
  if (!ae_logger)
    return AE_ERR_GENERIC;

  /* Silence the MongoDB sub-logger (no direct equivalent in C;
     handled by the MongoDB C driver's log level instead) */
  /* mongodb_driver_set_log_level(MONGODB_LOG_OFF); */

  /* Load server configuration */
  err = ae_load_config();
  if (err != AE_OK)
    return err;

  /* Attempt to update / migrate configuration */
  config_container_update(ae_config);

  ae_logger_info(ae_logger, "Loading Aetheris...");

  /* Load translation files */
  err = ae_load_language();
  if (err != AE_OK)
    return err;

  /* Structural startup checks */
  utils_startup_check();

  /* Create the global thread pool
   * Fixed pool of 6 threads */
  ae_thread_pool = ae_thread_pool_create(6);
  if (!ae_thread_pool)
    return AE_ERR_GENERIC;

  return AE_OK;
}

/* =========================================================================
 * ae_main()  -  public static void main(String[] args)
 * ====================================================================== */
ae_error_t ae_main(int argc, char **argv) {
  ae_error_t err;

  /* Load cryptographic keys */
  crypto_load_keys();

  /* Parse start-up arguments; returns true if we should exit early */
  if (startup_args_parse(argc, argv)) {
    return AE_OK;
  }

  ae_server_run_mode_t run_mode = ae_get_run_mode();

  /* Create command map */
  ae_command_map = command_map_create(/*register_defaults=*/true);

  /* Log startup messages */
  ae_logger_info(ae_logger, translate("messages.status.starting"));
  ae_logger_info(ae_logger, translate("messages.status.game_version"),
                 GAME_CONSTANTS_VERSION);
  ae_logger_info(ae_logger, translate("messages.status.version"),
                 BUILD_CONFIG_VERSION, BUILD_CONFIG_GIT_HASH);

  /* Initialise database */
  err = database_manager_initialize();
  if (err != AE_OK)
    return err;

  /* Initialise the default auth + permission systems */
  ae_auth_system = default_authentication_create();
  ae_perm_handler = default_permission_handler_create();

  /* Create server instances based on run mode */
  if (run_mode == SERVER_RUN_MODE_HYBRID ||
      run_mode == SERVER_RUN_MODE_GAME_ONLY) {
    ae_game_server = game_server_create();
  }
  if (run_mode == SERVER_RUN_MODE_HYBRID ||
      run_mode == SERVER_RUN_MODE_DISPATCH_ONLY) {
    ae_http_server = http_server_create();
  }

  /* Create the server helper (holds refs to both servers) */
  server_helper_create(ae_game_server, ae_http_server);

  /* Create the plugin manager */
  ae_plugin_manager = plugin_manager_create();

  /* Register HTTP routes (only when not GAME_ONLY) */
  if (run_mode != SERVER_RUN_MODE_GAME_ONLY) {
    http_server_add_router(ae_http_server, ROUTER_UNHANDLED_REQUEST);
    http_server_add_router(ae_http_server, ROUTER_DEFAULT_REQUEST);
    http_server_add_router(ae_http_server, ROUTER_REGION_HANDLER);
    http_server_add_router(ae_http_server, ROUTER_LOG_HANDLER);
    http_server_add_router(ae_http_server, ROUTER_GENERIC_HANDLER);
    http_server_add_router(ae_http_server, ROUTER_ANNOUNCEMENTS_HANDLER);
    http_server_add_router(ae_http_server, ROUTER_AUTHENTICATION_HANDLER);
    http_server_add_router(ae_http_server, ROUTER_GACHA_HANDLER);
    http_server_add_router(ae_http_server, ROUTER_DOCUMENTATION_HANDLER);
    http_server_add_router(ae_http_server, ROUTER_HANDBOOK_HANDLER);
  }

  /* Start HTTP server immediately if configured to do so */
  bool started = ae_config->server.http.start_immediately;
  if (started) {
    ae_logger_info(ae_logger, "HTTP server is starting...");
    err = ae_start_dispatch();
    if (err != AE_OK)
      return err;

    ae_logger_info(ae_logger, "Game server is starting...");
  }

  /* Load game resources (not needed for dispatch-only) */
  if (run_mode != SERVER_RUN_MODE_DISPATCH_ONLY) {
    ae_update_day_of_week();
    err = resource_loader_load_all();
    if (err != AE_OK)
      return err;

    tools_create_gm_handbooks(false);
    tools_generate_gacha_mappings();
  }

  /* Start the appropriate servers */
  switch (run_mode) {
  case SERVER_RUN_MODE_HYBRID:
    if (!started) {
      err = ae_start_dispatch();
      if (err != AE_OK)
        return err;
    }
    err = game_server_start(ae_game_server);
    if (err != AE_OK)
      return err;
    break;

  case SERVER_RUN_MODE_DISPATCH_ONLY:
    if (!started) {
      err = ae_start_dispatch();
      if (err != AE_OK)
        return err;
    }
    break;

  case SERVER_RUN_MODE_GAME_ONLY:
    err = game_server_start(ae_game_server);
    if (err != AE_OK)
      return err;
    break;

  default:
    ae_logger_error(ae_logger, translate("messages.status.run_mode_error"),
                    run_mode);
    ae_logger_error(ae_logger, translate("messages.status.run_mode_help"));
    ae_logger_error(ae_logger, translate("messages.status.shutdown"));
    return AE_ERR_GENERIC;
  }

  /* Enable all plugins */
  plugin_manager_enable_plugins(ae_plugin_manager);

  /* Register signal-based shutdown hook (replaces addShutdownHook) */
  signal(SIGINT, _signal_handler);
  signal(SIGTERM, _signal_handler);

  /* Open the interactive console (blocks until exit) */
  ae_start_console();

  return AE_OK;
}

/* =========================================================================
 * ae_shutdown()  -  private static void onShutdown()
 * ====================================================================== */
void ae_shutdown(void) {
  if (ae_plugin_manager)
    plugin_manager_disable_plugins(ae_plugin_manager);

  if (ae_game_server)
    game_server_on_shutdown(ae_game_server);

  /* Drain the main thread pool (5 s timeout) */
  if (ae_thread_pool)
    ae_thread_pool_shutdown(ae_thread_pool, 5);

  /* Drain the database event executor (5 s timeout) */
  ae_thread_pool_t *db_executor = database_helper_get_event_executor();
  if (db_executor)
    ae_thread_pool_shutdown(db_executor, 5);
}

/* =========================================================================
 * ae_start_dispatch()  -  public static void startDispatch()
 * ====================================================================== */
ae_error_t ae_start_dispatch(void) {
  ae_error_t err = http_server_start(ae_http_server);
  if (err != AE_OK)
    return err;

  if (ae_get_run_mode() == SERVER_RUN_MODE_DISPATCH_ONLY) {
    ae_dispatch_server = dispatch_server_create("0.0.0.0", 1111);
    if (!ae_dispatch_server)
      return AE_ERR_GENERIC;

    err = dispatch_server_start(ae_dispatch_server);
    if (err != AE_OK)
      return err;
  }

  return AE_OK;
}

/* =========================================================================
 * Language helpers
 * ====================================================================== */
ae_error_t ae_load_language(void) {
  const char *locale = ae_config->language.language;
  const char *lang_code = utils_get_language_code(locale);
  ae_language = language_get_by_code(lang_code);
  return ae_language ? AE_OK : AE_ERR_GENERIC;
}

ae_language_t *ae_get_language_by_code(const char *lang_code) {
  return language_get_by_code(lang_code);
}

/* =========================================================================
 * Configuration helpers
 * ====================================================================== */
ae_error_t ae_load_config(void) {
  FILE *f = fopen(AE_CONFIG_FILE, "r");
  if (!f) {
    ae_logger_info(ae_logger, "config.json could not be found. "
                              "Generating a default configuration ...");
    ae_config = config_container_create_default();
    return ae_save_config(ae_config);
  }
  fclose(f);

  ae_config = json_utils_load_config(AE_CONFIG_FILE);
  if (!ae_config) {
    ae_logger_error(
        ae_logger,
        "There was an error while trying to load the configuration "
        "from config.json. Please make sure that there are no syntax "
        "errors. If you want to start with a default configuration, "
        "delete your existing config.json.");
    return AE_ERR_CONFIG;
  }
  return AE_OK;
}

ae_error_t ae_save_config(const ae_config_t *config) {
  const ae_config_t *to_save =
      config ? config : config_container_create_default();

  FILE *f = fopen(AE_CONFIG_FILE, "w");
  if (!f) {
    ae_logger_error(ae_logger, "Unable to write to config file.");
    return AE_ERR_IO;
  }

  char *json = json_utils_encode_config(to_save);
  if (!json) {
    fclose(f);
    ae_logger_error(ae_logger, "Unable to encode config.");
    return AE_ERR_GENERIC;
  }

  fputs(json, f);
  free(json);
  fclose(f);
  return AE_OK;
}

/* =========================================================================
 * Run-mode helper
 * ====================================================================== */
ae_server_run_mode_t ae_get_run_mode(void) {
  if ((int)ae_run_mode_override != -1)
    return ae_run_mode_override;
  return ae_config->server.run_mode;
}

/* =========================================================================
 * Console helpers
 * ====================================================================== */
ae_console_reader_t *ae_get_console(void) {
  if (!ae_console_reader)
    ae_console_reader = console_reader_create();
  return ae_console_reader;
}

void ae_start_console(void) {
  /* Suppress console in dispatch-only + no-console mode */
  if (ae_get_run_mode() == SERVER_RUN_MODE_DISPATCH_ONLY && ae_no_console) {
    ae_logger_info(ae_logger, translate("messages.dispatch.no_commands_error"));
    return;
  }

  ae_force_reload();
  ae_logger_info(ae_logger, translate("messages.status.done"));

  char input[4096];
  bool is_last_interrupted = false;

  while (ae_config->server.game.enable_console) {
    ae_console_reader_t *reader = ae_get_console();
    int rc = console_reader_read_line(reader, "> ", input, sizeof(input));

    if (rc == CONSOLE_INTERRUPTED) {
      if (!is_last_interrupted) {
        is_last_interrupted = true;
        ae_logger_info(ae_logger, "Press Ctrl-C again to shutdown.");
        continue;
      } else {
        exit(0);
      }
    } else if (rc == CONSOLE_EOF) {
      ae_logger_info(ae_logger, "EOF detected.");
      continue;
    } else if (rc == CONSOLE_IO_ERROR) {
      ae_logger_error(
          ae_logger, "An IO error occurred while trying to read from console.");
      return;
    }

    is_last_interrupted = false;

    if (command_map_invoke(ae_command_map, NULL, NULL, input) != AE_OK) {
      ae_logger_error(ae_logger, translate("messages.game.command_error"));
    }
  }
}

/* =========================================================================
 * Utility helpers
 * ====================================================================== */
void ae_update_day_of_week(void) {
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  /* tm_wday: 0=Sunday ... 6=Saturday.
   * Preserve the same 1-based convention. */
  ae_current_day_of_week = tm->tm_wday + 1;
  ae_logger_debug(ae_logger, "Set day of week to %d", ae_current_day_of_week);
}

void ae_force_reload(void) {
  ae_load_config();
  ae_load_language();
  game_server_gacha_system_load(ae_game_server);
  game_server_shop_system_load(ae_game_server);
}

/* =========================================================================
 * Thread-pool thin wrappers
 * ====================================================================== */
ae_thread_pool_t *ae_get_thread_pool(void) { return ae_thread_pool; }

/* =========================================================================
 * Process entry point
 * ====================================================================== */
int main(int argc, char **argv) {
  ae_error_t err = ae_init();
  if (err != AE_OK) {
    fprintf(stderr, "ae_init() failed: %d\n", err);
    return 1;
  }

  err = ae_main(argc, argv);
  if (err != AE_OK) {
    fprintf(stderr, "ae_main() failed: %d\n", err);
    return 1;
  }

  return 0;
}
