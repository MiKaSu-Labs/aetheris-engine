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
 * @file configuration.c
 *
 * @brief Configuration alias initialization and path utility implementation.
 *
 * Initializes global runtime aliases pointing into the active configuration
 * structure and provides helper functions for resolving common resource
 * directory paths such as data, plugins, scripts, and packets.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include "configuration.h"

#include <stdio.h>
#include <string.h>

#include "aetheris.h"      /* ae_config                        */
#include "file_utils.h"    /* ae_file_utils_get_resource_path  */

/* =========================================================================
 * Alias globals -- all populated by ae_configuration_init()
 * ====================================================================== */

ae_config_t            *AE_C                = NULL;
ae_language_config_t   *AE_LANGUAGE         = NULL;
const char             *AE_FALLBACK_LANGUAGE = NULL;
const char             *AE_DOCUMENT_LANGUAGE = NULL;
ae_server_config_t     *AE_SERVER           = NULL;
ae_database_config_t   *AE_DATABASE         = NULL;
ae_account_config_t    *AE_ACCOUNT          = NULL;

ae_http_config_t       *AE_HTTP_INFO        = NULL;
ae_game_config_t       *AE_GAME_INFO        = NULL;
ae_dispatch_config_t   *AE_DISPATCH_INFO    = NULL;
ae_debug_mode_config_t *AE_DEBUG_MODE_INFO  = NULL;
ae_encryption_config_t *AE_HTTP_ENCRYPTION  = NULL;
ae_policies_config_t   *AE_HTTP_POLICIES    = NULL;
ae_files_config_t      *AE_HTTP_STATIC_FILES = NULL;
ae_game_options_t      *AE_GAME_OPTIONS     = NULL;
ae_inventory_limits_t  *AE_INVENTORY_LIMITS  = NULL;
ae_handbook_options_t  *AE_HANDBOOK         = NULL;
bool                    AE_FAST_REQUIRE      = false;

/* Folder paths; see the "module-private" note in configuration.h */
const char *ae_cfg_data_folder    = NULL;
const char *ae_cfg_plugins_folder = NULL;
const char *ae_cfg_scripts_folder = NULL;
const char *ae_cfg_packets_folder = NULL;

/* =========================================================================
 * ae_configuration_init()
 * ====================================================================== */
void ae_configuration_init(void) {
    ae_config_t *c = ae_config; /* convenience local alias */

    AE_C                 = c;
    AE_LANGUAGE          = &c->language;
    AE_FALLBACK_LANGUAGE = c->language.fallback;
    AE_DOCUMENT_LANGUAGE = c->language.document;
    AE_SERVER            = &c->server;
    AE_DATABASE          = &c->database_info;
    AE_ACCOUNT           = &c->account;

    AE_HTTP_INFO         = &c->server.http;
    AE_GAME_INFO         = &c->server.game;
    AE_DISPATCH_INFO     = &c->server.dispatch;
    AE_DEBUG_MODE_INFO   = &c->server.debug_mode;
    AE_HTTP_ENCRYPTION   = &c->server.http.encryption;
    AE_HTTP_POLICIES     = &c->server.http.policies;
    AE_HTTP_STATIC_FILES = &c->server.http.files;
    AE_GAME_OPTIONS      = &c->server.game.game_options;
    AE_INVENTORY_LIMITS  = &c->server.game.game_options.inventory_limits;
    AE_HANDBOOK          = &c->server.game.game_options.handbook;
    AE_FAST_REQUIRE      = c->server.fast_require;

    ae_cfg_data_folder    = c->folder_structure.data;
    ae_cfg_plugins_folder = c->folder_structure.plugins;
    ae_cfg_scripts_folder = c->folder_structure.scripts;
    ae_cfg_packets_folder = c->folder_structure.packets;
}

/* =========================================================================
 * Path helpers
 * ====================================================================== */

const char *ae_cfg_data(char *out, size_t out_size) {
    snprintf(out, out_size, "%s", ae_cfg_data_folder);
    return out;
}

const char *ae_cfg_data_path(const char *path, char *out, size_t out_size) {
    int n = snprintf(out, out_size, "%s/%s", ae_cfg_data_folder, path);
    if (n < 0 || (size_t)n >= out_size)
        return NULL;
    return out;
}

const char *ae_cfg_resource(const char *path, char *out, size_t out_size) {
    return ae_file_utils_get_resource_path(path, out, out_size);
}

const char *ae_cfg_plugin(char *out, size_t out_size) {
    snprintf(out, out_size, "%s", ae_cfg_plugins_folder);
    return out;
}

const char *ae_cfg_plugin_path(const char *path, char *out, size_t out_size) {
    int n = snprintf(out, out_size, "%s/%s", ae_cfg_plugins_folder, path);
    if (n < 0 || (size_t)n >= out_size)
        return NULL;
    return out;
}

const char *ae_cfg_script(const char *path, char *out, size_t out_size) {
    int n = snprintf(out, out_size, "%s/%s", ae_cfg_scripts_folder, path);
    if (n < 0 || (size_t)n >= out_size)
        return NULL;
    return out;
}

const char *ae_cfg_packet(const char *path, char *out, size_t out_size) {
    int n = snprintf(out, out_size, "%s/%s", ae_cfg_packets_folder, path);
    if (n < 0 || (size_t)n >= out_size)
        return NULL;
    return out;
}
