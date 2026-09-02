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
 * @file configuration.h
 *
 * @brief Runtime configuration access layer and convenience API.
 *
 * Provides global aliases into the active configuration, lifecycle initialization
 * for binding those aliases to the loaded config, path helper utilities for
 * resource lookup, and lightweight fallback helpers for safe configuration access.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

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
 * Each of these is a pointer set to point somewhere inside ae_config, so
 * reading through them reflects the current runtime configuration --
 * but only as of the last call to ae_configuration_init(). ALL_CAPS
 * names here are this codebase's convention for these config aliases
 * specifically; unlike a #define or a const, every one of them is a
 * plain mutable extern pointer that gets re-pointed on each call.
 *
 * @warning Every alias below becomes a dangling pointer if ae_config is
 * ever replaced (by ae_load_config() or config_container_update()
 * succeeding) without a following call to ae_configuration_init(). Both
 * existing call sites that can replace ae_config (ae_init() and
 * ae_force_reload()) already call ae_configuration_init() immediately
 * afterward; any new code path that reloads the configuration must do
 * the same.
 * ====================================================================== */

/* Top-level aliases */
extern ae_config_t            *AE_C;              /* 'c' - short for config   */
extern ae_language_config_t   *AE_LANGUAGE;
extern const char             *AE_FALLBACK_LANGUAGE;
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

/* server.debug_mode */
extern ae_debug_mode_config_t *AE_DEBUG_MODE_INFO;

/* server.http.encryption */
extern ae_encryption_config_t *AE_HTTP_ENCRYPTION;

/* server.http.policies */
extern ae_policies_config_t   *AE_HTTP_POLICIES;

/* server.http.files */
extern ae_files_config_t      *AE_HTTP_STATIC_FILES;

/* server.game.game_options */
extern ae_game_options_t      *AE_GAME_OPTIONS;

/* server.game.game_options.inventory_limits */
extern ae_inventory_limits_t  *AE_INVENTORY_LIMITS;

/* server.game.game_options.handbook */
extern ae_handbook_options_t  *AE_HANDBOOK;

/* server.fast_require */
extern bool                    AE_FAST_REQUIRE;

/*
 * Folder-path globals. Not compiler-enforced private (they're extern,
 * so anything including this header can see them) -- "module-private"
 * here means a convention: read the current folder paths through the
 * ae_cfg_* path helpers below rather than these directly.
 */
extern const char *ae_cfg_data_folder;
extern const char *ae_cfg_plugins_folder;
extern const char *ae_cfg_scripts_folder;
extern const char *ae_cfg_packets_folder;

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

/**
 * @brief Populate every alias above from the live ae_config.
 *
 * Must be called once after ae_load_config() first succeeds, and again
 * after anything that may replace the global ae_config pointer (see the
 * warning on the aliases above) -- currently ae_init() and
 * ae_force_reload().
 *
 * Reads the global ae_config, which must already be non-NULL (i.e.
 * ae_load_config() must have already succeeded at least once); calling
 * this before that is a precondition violation, not a checked error.
 *
 * Not thread-safe. Call from the main thread only.
 */
void ae_configuration_init(void);

/* =========================================================================
 * Path helpers
 *
 * Each function writes the result into `out` (size `out_size`) and
 * returns `out` on success. `out` must not be NULL. `out_size` of 0 is
 * a well-defined no-op (nothing is written; inherited directly from
 * snprintf's own standard-mandated behavior for a 0 size).
 *
 * The functions that combine a folder with `path` (ae_cfg_data_path,
 * ae_cfg_plugin_path, ae_cfg_script, ae_cfg_packet) return NULL instead
 * of `out` if the combined result would not fit in `out_size` -- check
 * the return value; a truncated path can point at the wrong file.
 *
 * The lowercase @deprecated tags below are Doxygen's own deprecation
 * marker, not a claim about any other language. These deprecated
 * originals are kept only so existing call sites keep compiling;
 * prefer ae_file_utils_get_resource_path() for new code.
 * ====================================================================== */

/** @deprecated - use ae_config directly. Returns data folder root. */
const char *ae_cfg_data(char *out, size_t out_size);

/** @deprecated - joins data folder with `path`. NULL on truncation. */
const char *ae_cfg_data_path(const char *path, char *out, size_t out_size);

/** @deprecated - resolves a resource path via file_utils. */
const char *ae_cfg_resource(const char *path, char *out, size_t out_size);

/** Returns plugins folder root. */
const char *ae_cfg_plugin(char *out, size_t out_size);

/** Joins plugins folder with `path`. NULL on truncation. */
const char *ae_cfg_plugin_path(const char *path, char *out, size_t out_size);

/** @deprecated - joins scripts folder with `path`. NULL on truncation. */
const char *ae_cfg_script(const char *path, char *out, size_t out_size);

/** @deprecated - joins packets folder with `path`. NULL on truncation. */
const char *ae_cfg_packet(const char *path, char *out, size_t out_size);

/* =========================================================================
 * lr() fallback helpers
 *
 * Each returns `left` if it is "present" (non-NULL / non-empty /
 * non-zero, per type), else `right` -- with no guarantee `right` itself
 * is present. Because 0 means "absent" for ae_lr_int, it is not a safe
 * fallback pattern for a field where 0 is itself a meaningful value
 * (e.g. a count that may legitimately be zero).
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
