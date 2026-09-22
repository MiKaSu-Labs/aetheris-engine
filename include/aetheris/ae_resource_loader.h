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
 * @file ae_resource_loader.h
 *
 * @brief Static resource-definition registry and excel JSON loader.
 *
 * Resource files live under a resources directory in two roots:
 * `Server` (community overrides) and `ExcelBinOutput` (generated game
 * data). For a given filename the first root that contains any of the
 * supported extensions wins; within a root the extension priority is
 * TSJ, JSON, TSV. Each file is a JSON array; each element is handed to
 * a per-definition parse callback that populates the owning registry.
 *
 * This replaces the reflective class discovery of the reference
 * implementation with an explicit static table of resource
 * definitions, each carrying a load priority so that data a later
 * group depends on is present first.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_RESOURCE_LOADER_H
#define AE_RESOURCE_LOADER_H

#include <aetheris/ae_common.h>
#include <aetheris/ae_error.h>
#include <aetheris/ae_types.h>

/* ============================================================ */

/* Opaque cJSON tag forward declaration: the loader API only passes
 * element pointers around and never needs the struct definition. */
typedef struct cJSON cJSON;

/*
 * Resource files are grouped by load priority. Higher priorities are
 * loaded first so that consumers (curves, crafted tables, configs) are
 * in place before the resources that reference them. Within one
 * priority the order in which definitions were registered is kept.
 */
typedef enum ae_resource_load_priority {
    AE_RESOURCE_LOAD_PRIORITY_LOWEST = 0,
    AE_RESOURCE_LOAD_PRIORITY_LOW = 1,
    AE_RESOURCE_LOAD_PRIORITY_NORMAL = 2,
    AE_RESOURCE_LOAD_PRIORITY_HIGH = 3,
    AE_RESOURCE_LOAD_PRIORITY_HIGHEST = 4,
} ae_resource_load_priority_t;

 * ae_resource_load_priority_from_value - map a numeric priority.
 * @value: priority number.
 *
 * Returns the matching priority, or LOWEST for unknown values.
 */
ae_resource_load_priority_t ae_resource_load_priority_from_value(ae_s32 value);

/**
 * ae_resource_load_priority_name - stable name of a priority.
 * @priority: priority to describe.
 *
 * Returns a static name string ("HIGHEST".."LOWEST"), or NULL for an
 * out-of-range value.
 */
const char *ae_resource_load_priority_name(ae_resource_load_priority_t priority);

/*
 * Callback invoked once per element of a resource file's root array.
 * @object points to a single element of the parsed JSON document and
 * is only valid for the duration of the call.
 */
typedef ae_error_t (*ae_resource_parse_fn_t)(const cJSON *object, void *user);

/**
 * ae_resource_loader_resolve - locate a resource file on disk.
 * @resources_dir: base directory of the resource tree.
 * @filename: excel name such as "ItemExcelConfigData.json".
 * @out_path: buffer receiving the resolved path.
 * @out_cap: capacity of @out_path in bytes.
 *
 * Resolution order mirrors the reference: `Server` is preferred over
 * `ExcelBinOutput` and within a root the extension priority is TSJ,
 * JSON, TSV. When no candidate exists @out_path still receives the
 * canonical fallback path (Server/<stem>.tsj) and AE_ERR_NOT_FOUND is
 * returned so callers can distinguish "present" from "absent".
 */
ae_error_t ae_resource_loader_resolve(const char *resources_dir,
                                      const char *filename,
                                      char *out_path,
                                      ae_size out_cap);

/**
 * ae_resource_loader_load_json_array - parse one excel file.
 * @resources_dir: base directory of the resource tree.
 * @filename: excel name to load.
 * @parse_fn: per-element callback (required).
 * @user: value forwarded to every @parse_fn call.
 *
 * Resolves @filename, reads the file, parses the root JSON array and
 * invokes @parse_fn for each element. A missing file is not an error
 * (the reference skips absent resources); malformed JSON or a root
 * that is not an array yields AE_ERR_BAD_DATA. Per-element failures
 * are collected and the first one is returned, loading continues so a
 * single bad row does not discard the whole file.
 */
ae_error_t ae_resource_loader_load_json_array(const char *resources_dir,
                                              const char *filename,
                                              ae_resource_parse_fn_t parse_fn,
                                              void *user);

/*
 * A single loadable resource family, mirroring a resource class in the
 * reference implementation. @type_name must be unique across the whole
 * registry.
 */
typedef struct ae_resource_def {
    const char *type_name;
    ae_resource_load_priority_t priority;
    const char *const *filenames;
    ae_size filename_count;
    ae_error_t (*load)(const char *resources_dir,
                       const struct ae_resource_def *def);
} ae_resource_def_t;

/**
 * ae_resource_loader_register - add a definition to the registry.
 * @def: definition to add; a shallow copy is kept, the table itself
 *       must outlive the registry.
 *
 * Returns AE_ERR_ALREADY_EXISTS when @def->type_name is already
 * registered.
 */
ae_error_t ae_resource_loader_register(const ae_resource_def_t *def);

/**
 * ae_resource_loader_load_all - load every registered definition.
 * @resources_dir: base directory of the resource tree.
 * @reload: when false already-loaded definitions are skipped; when
 *          true every definition loads again.
 *
 * Definitions load in priority order (highest first). Loading
 * continues across failures and the first error is returned.
 */
ae_error_t ae_resource_loader_load_all(const char *resources_dir,
                                       ae_bool reload);

/**
 * ae_resource_loader_reset - drop all registrations and load state.
 *
 * Safe to call at any time. Registrations added afterwards start from
 * a clean slate.
 */
void ae_resource_loader_reset(void);

/* ============================================================ */

#endif /* AE_RESOURCE_LOADER_H */