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
 * @file tools.h
 *
 * @brief Server tooling utilities.
 *        Provides GM handbook generation, gacha mapping generation,
 *        language discovery, and resource info parsing.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef TOOLS_H
#define TOOLS_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * ae_scripts_type_t
 * ====================================================================== */
typedef enum {
    AE_SCRIPTS_TYPE_OFFICIAL,
    AE_SCRIPTS_TYPE_DUMPED,
    AE_SCRIPTS_TYPE_UNKNOWN,
} ae_scripts_type_t;

/* =========================================================================
 * ae_resource_info_t
 *
 * Parsed content of the 'resources.info' file.
 * All string fields are heap-allocated; free with ae_resource_info_free().
 * ====================================================================== */
typedef struct {
    char               *repository;
    char               *version;
    char               *patches;
    ae_scripts_type_t   scripts;
    bool                has_no_locals;
    bool                has_server_resources;
    bool                has_scene_script_data;
} ae_resource_info_t;

/* =========================================================================
 * GM Handbook generation
 * ====================================================================== */

/*
 * Generates the GM handbook files for each available language.
 * Prints a completion message to the logger on success.
 * Returns 0 on success, negative ae_error_t on failure.
 */
int tools_create_gm_handbooks(void);

/*
 * Generates the GM handbook files for each available language.
 * If 'message' is true, a completion message is logged on success.
 * Returns 0 on success, negative ae_error_t on failure.
 */
int tools_create_gm_handbooks_ex(bool message);

/* =========================================================================
 * Gacha mapping generation
 * ====================================================================== */

/*
 * Generates gacha mapping JSON strings for every supported language.
 * Writes results into 'out', an array of 'count' heap-allocated strings.
 * Caller must free each string and the array itself.
 * Returns the number of entries written, or -1 on failure.
 */
int tools_create_gacha_mapping_jsons(char ***out, size_t *count);

/*
 * Checks whether the gacha mappings file exists at its configured path.
 * If it does not exist, generates and writes it.
 */
void tools_generate_gacha_mappings(void);

/*
 * Writes gacha mappings to the file at 'path'.
 * Returns 0 on success, negative ae_error_t on failure.
 */
int tools_create_gacha_mappings(const char *path);

/* =========================================================================
 * Language discovery
 * ====================================================================== */

/*
 * Scans the TextMap resource directory and returns a NULL-terminated array
 * of lowercase language code strings (e.g. "en", "zh-cn").
 * Caller must free each string and the array itself.
 * Returns NULL on failure.
 */
char **tools_get_available_languages(void);

/*
 * Interactive console prompt that lists available languages and returns
 * the user's uppercased selection, or "EN" as the fallback.
 * Caller must free the returned string.
 *
 * @deprecated Use ae_config->language.language directly for new code.
 */
char *tools_get_language_option(void);

/* =========================================================================
 * Resource info
 * ====================================================================== */

/*
 * Parses the 'resources.info' file and returns a heap-allocated
 * ae_resource_info_t. On parse failure, returns a zeroed struct with
 * scripts set to AE_SCRIPTS_TYPE_UNKNOWN.
 * Caller must free with ae_resource_info_free().
 */
ae_resource_info_t *tools_resources_info(void);

/*
 * Serialises 'info' to a JSON string.
 * Caller must free the returned string.
 */
char *ae_resource_info_to_json(const ae_resource_info_t *info);

/*
 * Releases all memory owned by 'info', then frees the struct itself.
 */
void ae_resource_info_free(ae_resource_info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* TOOLS_H */
