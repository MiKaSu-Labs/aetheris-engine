#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/*
 * configuration.h
 */

#include <stdbool.h>
#include <stddef.h>

#include "aetheris.h"          /* ae_config                          */
#include "config_container.h"  /* ae_config_t and all nested structs */
#include "file_utils.h"        /* ae_file_utils_get_resource_path()  */

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Aliases into the live config
 *
 * These are pointers into ae_config, so they always reflect the current
 * runtime configuration even after a ae_force_reload().
 * Populated by ae_configuration_init(); call after ae_load_config().
 * ====================================================================== */

/* Top-level aliases */
extern ae_config_t            *AE_C;              /* 'c' - short for config   */
extern ae_language_config_t   *AE_LANGUAGE;
extern ae_language_config_t   *AE_FALLBACK_LANGUAGE;
extern const char             *AE_DOCUMENT_LANGUAGE;
extern ae_server_config_t     *AE_SERVER;
extern ae_database_config_t   *AE_DATABASE;
extern ae_account_config_t    *AE_ACCOUNT;

/* server.http */
extern ae_http_config_t       *AE_HTTP_INFO;

/* server.game */
extern ae_game_config_t       *AE_GAME_INFO;

/* server.dispatch */
extern ae_dispatch_config_t   *AE_DISPATCH_INFO;

/* server.debugMode */
extern ae_debug_mode_config_t *AE_DEBUG_MODE_INFO;

/* server.http.encryption */
extern ae_encryption_config_t *AE_HTTP_ENCRYPTION;

/* server.http.policies */
extern ae_policies_config_t   *AE_HTTP_POLICIES;

/* server.http.files */
extern ae_files_config_t      *AE_HTTP_STATIC_FILES;

/* server.game.gameOptions */
extern ae_game_options_t      *AE_GAME_OPTIONS;

/* server.game.gameOptions.inventoryLimits */
extern ae_inventory_limits_t  *AE_INVENTORY_LIMITS;

/* server.game.gameOptions.handbook */
extern ae_handbook_options_t  *AE_HANDBOOK;

/* server.fastRequire */
extern bool                    AE_FAST_REQUIRE;

/*
 * module-private globals; accessed only through the path helpers below. */
extern const char *ae_cfg_data_folder;
extern const char *ae_cfg_plugins_folder;
extern const char *ae_cfg_scripts_folder;
extern const char *ae_cfg_packets_folder;

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

/**
 * ae_configuration_init()
 * Populates all alias pointers from the live ae_config.
 * Must be called after ae_load_config() (and again after ae_force_reload()).
 */
void ae_configuration_init(void);

/* =========================================================================
 * Path helpers
 *
 * Each function writes the result into `out` (size `out_size`) and returns
 * `out` for convenience.
 *
 * The @Deprecated originals are preserved so existing call-sites compile;
 * prefer ae_file_utils_get_resource_path() for new code.
 * ====================================================================== */

/** @deprecated - use ae_config directly. Returns data folder root. */
const char *ae_cfg_data(char *out, size_t out_size);

/** @deprecated - joins data folder with `path`. */
const char *ae_cfg_data_path(const char *path, char *out, size_t out_size);

/** @deprecated - resolves a resource path via file_utils. */
const char *ae_cfg_resource(const char *path, char *out, size_t out_size);

/** Returns plugins folder root. */
const char *ae_cfg_plugin(char *out, size_t out_size);

/** Joins plugins folder with `path`. */
const char *ae_cfg_plugin_path(const char *path, char *out, size_t out_size);

/** @deprecated - joins scripts folder with `path`. */
const char *ae_cfg_script(const char *path, char *out, size_t out_size);

/** @deprecated - joins packets folder with `path`. */
const char *ae_cfg_packet(const char *path, char *out, size_t out_size);

/* =========================================================================
 * lr() fallback helpers
 *
 * Returns `left` if it is non-null / non-empty / non-zero, else `right`.
 * ====================================================================== */

/** Generic pointer fallback: returns left if non-NULL, else right. */
static inline const void *ae_lr_ptr(const void *left, const void *right) {
    return left != NULL ? left : right;
}

/** String fallback: returns left if non-empty, else right. */
static inline const char *ae_lr_str(const char *left, const char *right) {
    return (left != NULL && left[0] != '\0') ? left : right;
}

/** Integer fallback: returns left if non-zero, else right. */
static inline int ae_lr_int(int left, int right) {
    return left != 0 ? left : right;
}

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURATION_H */
