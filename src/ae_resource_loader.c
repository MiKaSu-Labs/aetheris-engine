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
 * @file ae_resource_loader.c
 *
 * @brief Static resource-definition registry and excel JSON loader.
 *
 * Definitions are registered at startup in dependency order, then
 * `ae_resource_loader_load_all` drives them by descending load
 * priority. Each definition loads its named excel files; per-element
 * JSON callbacks populate the owning gamedata registries.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#include "cJSON.h"

#include <aetheris/ae_common.h>
#include <aetheris/ae_error.h>
#include <aetheris/ae_resource_loader.h>

/* ============================================================ */

/* Resource tree roots, in lookup order. The Server root holds
 * community overrides and shadows the generated excel files. */
static const char *const k_roots[] = {
    "Server",
    "ExcelBinOutput",
};

/* Extension priority, mirrors the reference TSJ > JSON > TSV rule. */
static const char *const k_extensions[] = {
    "tsj",
    "json",
    "tsv",
};

/* Upper bound on a single resource file. Scene and spawn tables are
 * the largest excel files shipped and stay far below this. */
#define AE_RESOURCE_MAX_FILE_SIZE (256u * 1024u * 1024u)

/* A registered definition plus its registration index used to keep
 * load order stable within one priority. */
typedef struct ae_def_registration {
    const ae_resource_def_t *def;
    ae_size reg_index;
} ae_def_registration_t;

static ae_def_registration_t *g_registrations;
static ae_size g_registration_count;
static ae_size g_registration_capacity;

/* Names of definitions that have already been loaded. */
static char **g_loaded_names;
static ae_size g_loaded_count;
static ae_size g_loaded_capacity;

/* ============================================================ */

ae_resource_load_priority_t ae_resource_load_priority_from_value(ae_s32 value)
{
    if (value < (ae_s32)AE_RESOURCE_LOAD_PRIORITY_LOWEST ||
        value > (ae_s32)AE_RESOURCE_LOAD_PRIORITY_HIGHEST) {
        return AE_RESOURCE_LOAD_PRIORITY_LOWEST;
    }
    return (ae_resource_load_priority_t)value;
}

const char *ae_resource_load_priority_name(
    ae_resource_load_priority_t priority)
{
    static const char *const names[] = {
        "LOWEST", "LOW", "NORMAL", "HIGH", "HIGHEST",
    };
    ae_s32 value = (ae_s32)priority;
    if (value < 0 ||
        value > (ae_s32)(AE_ARRAY_SIZE(names) - 1u)) {
        return NULL;
    }
    return names[(ae_size)value];
}

/* ============================================================ */

/*
 * Splits `filename` into stem and extension. "A.json" yields
 * stem="A", ext="json"; a name without a dot keeps the whole name in
 * stem and an empty extension.
 */
static void split_filename(const char *filename,
                           char *stem, ae_size stem_cap,
                           char *ext, ae_size ext_cap)
{
    const char *dot = strrchr(filename, '.');
    const char *stem_end = dot ? dot : filename + strlen(filename);
    ae_size stem_len = (ae_size)(stem_end - filename);
    ae_size ext_len = dot ? strlen(dot + 1) : 0;

    if (stem_len >= stem_cap) {
        stem_len = stem_cap - 1u;
    }
    if (ext_len >= ext_cap) {
        ext_len = ext_cap - 1u;
    }
    memcpy(stem, filename, stem_len);
    stem[stem_len] = '\0';
    if (dot) {
        memcpy(ext, dot + 1, ext_len);
    }
    ext[ext_len] = '\0';
}

/*
 * True when a file exists at `path`. Uses stat so directories do not
 * count as files.
 */
static ae_bool file_exists(const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    return S_ISREG(st.st_mode) != 0;
}

/* ============================================================ */

ae_error_t ae_resource_loader_resolve(const char *resources_dir,
                                      const char *filename,
                                      char *out_path,
                                      ae_size out_cap)
{
    char stem[128];
    char ext[16];
    char candidate[512];
    ae_size k;

    if (!resources_dir || !filename || !out_path) {
        return AE_ERR_INVALID_ARG;
    }
    if (strlen(resources_dir) > sizeof(candidate) - 2u) {
        return AE_ERR_INVALID_ARG;
    }
    memset(stem, 0, sizeof(stem));
    memset(ext, 0, sizeof(ext));
    split_filename(filename, stem, sizeof(stem), ext, sizeof(ext));

    for (k = 0; k < AE_ARRAY_SIZE(k_roots); k++) {
        ae_size e;
        for (e = 0; e < AE_ARRAY_SIZE(k_extensions); e++) {
            int written = snprintf(candidate, sizeof(candidate), "%s/%s/%s.%s",
                                   resources_dir, k_roots[k], stem,
                                   k_extensions[e]);
            if (written < 0 ||
                (ae_size)written >= sizeof(candidate)) {
                return AE_ERR_OVERFLOW;
            }
            if (file_exists(candidate)) {
                if (strlen(candidate) >= out_cap) {
                    return AE_ERR_BUFFER_TOO_SMALL;
                }
                memcpy(out_path, candidate, strlen(candidate) + 1u);
                return AE_OK;
            }
        }
    }

    /* No candidate exists: return the canonical fallback location so
     * callers know where a missing resource would be written. */
    {
        int written = snprintf(candidate, sizeof(candidate),
                               "%s/%s/%s.%s",
                               resources_dir, k_roots[0], stem,
                               k_extensions[0]);
        if (written < 0 ||
            (ae_size)written >= sizeof(candidate)) {
            return AE_ERR_OVERFLOW;
        }
        if (strlen(candidate) >= out_cap) {
            return AE_ERR_BUFFER_TOO_SMALL;
        }
        memcpy(out_path, candidate, strlen(candidate) + 1u);
        return AE_ERR_NOT_FOUND;
    }
}

/* ============================================================ */

/*
 * Reads a whole file into a NUL-terminated heap buffer. Returns AE_OK
 * and sets *out_text, or a negative error code leaving *out_text NULL.
 */
static ae_error_t read_file_text(const char *path, char **out_text)
{
    FILE *fp;
    long len;
    size_t got;
    char *text;

    fp = fopen(path, "rb");
    if (!fp) {
        return AE_ERR_IO;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return AE_ERR_IO;
    }
    len = ftell(fp);
    if (len < 0) {
        fclose(fp);
        return AE_ERR_IO;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return AE_ERR_IO;
    }
    if ((unsigned long)len > AE_RESOURCE_MAX_FILE_SIZE) {
        fclose(fp);
        return AE_ERR_OVERFLOW;
    }
    text = (char *)malloc((size_t)len + 1u);
    if (!text) {
        fclose(fp);
        return AE_ERR_OUT_OF_MEMORY;
    }
    got = fread(text, 1u, (size_t)len, fp);
    fclose(fp);
    if (got != (size_t)len) {
        free(text);
        return AE_ERR_IO;
    }
    text[got] = '\0';
    *out_text = text;
    return AE_OK;
}

ae_error_t ae_resource_loader_load_json_array(const char *resources_dir,
                                              const char *filename,
                                              ae_resource_parse_fn_t parse_fn,
                                              void *user)
{
    char path[512];
    char *text;
    cJSON *root;
    ae_size count;
    ae_error_t first_err;
    ae_error_t err;
    ae_size i;

    if (!parse_fn) {
        return AE_ERR_INVALID_ARG;
    }
    err = ae_resource_loader_resolve(resources_dir, filename,
                                     path, sizeof(path));
    if (err == AE_ERR_NOT_FOUND) {
        return AE_OK;
    }
    if (err != AE_OK) {
        return err;
    }
    err = read_file_text(path, &text);
    if (err != AE_OK) {
        return err;
    }
    root = cJSON_Parse(text);
    free(text);
    if (!root) {
        return AE_ERR_BAD_DATA;
    }
    if (!cJSON_IsArray(root)) {
        cJSON_Delete(root);
        return AE_ERR_BAD_DATA;
    }

    first_err = AE_OK;
    count = (ae_size)cJSON_GetArraySize(root);
    for (i = 0; i < count; i++) {
        cJSON *item = cJSON_GetArrayItem(root, (int)i);
        err = parse_fn(item, user);
        if (err != AE_OK && first_err == AE_OK) {
            first_err = err;
        }
    }
    cJSON_Delete(root);
    return first_err;
}

/* ============================================================ */

ae_error_t ae_resource_loader_register(const ae_resource_def_t *def)
{
    ae_def_registration_t *grown;
    ae_size new_cap;
    ae_size bytes;
    ae_size i;

    if (!def || !def->type_name || !def->load ||
        (def->filename_count != 0 && !def->filenames)) {
        return AE_ERR_INVALID_ARG;
    }
    for (i = 0; i < g_registration_count; i++) {
        if (strcmp(g_registrations[i].def->type_name, def->type_name) == 0) {
            return AE_ERR_ALREADY_EXISTS;
        }
    }
    if (g_registration_count == g_registration_capacity) {
        new_cap = g_registration_capacity ? g_registration_capacity * 2u : 8u;
        if (!ae_mul_overflow_uz(new_cap, sizeof(ae_def_registration_t),
                                &bytes)) {
            return AE_ERR_OVERFLOW;
        }
        grown = (ae_def_registration_t *)realloc(g_registrations, bytes);
        if (!grown) {
            return AE_ERR_OUT_OF_MEMORY;
        }
        g_registrations = grown;
        g_registration_capacity = new_cap;
    }
    g_registrations[g_registration_count].def = def;
    g_registrations[g_registration_count].reg_index = g_registration_count;
    g_registration_count++;
    return AE_OK;
}

/* Compares registration indices by descending priority then ascending
 * registration order to keep intra-priority ordering deterministic. */
static int compare_registrations(const void *a, const void *b)
{
    ae_size ia = *(const ae_size *)a;
    ae_size ib = *(const ae_size *)b;
    ae_s32 pa = (ae_s32)g_registrations[ia].def->priority;
    ae_s32 pb = (ae_s32)g_registrations[ib].def->priority;

    if (pa != pb) {
        return pa > pb ? -1 : 1;
    }
    if (g_registrations[ia].reg_index < g_registrations[ib].reg_index) {
        return -1;
    }
    if (g_registrations[ia].reg_index > g_registrations[ib].reg_index) {
        return 1;
    }
    return 0;
}

static ae_bool is_loaded(const char *type_name)
{
    ae_size i;
    for (i = 0; i < g_loaded_count; i++) {
        if (strcmp(g_loaded_names[i], type_name) == 0) {
            return true;
        }
    }
    return false;
}

/*
 * Records a successfully-performed load. Best effort: tracking is
 * dropped when the bookkeeping allocation fails.
 */
static void mark_loaded(const char *type_name)
{
    char **grown;
    size_t bytes;

    for (ae_size i = 0; i < g_loaded_count; i++) {
        if (strcmp(g_loaded_names[i], type_name) == 0) {
            return;
        }
    }
    if (g_loaded_count == g_loaded_capacity) {
        ae_size new_cap = g_loaded_capacity ? g_loaded_capacity * 2u : 8u;
        if (!ae_mul_overflow_uz(new_cap, sizeof(char *), &bytes)) {
            return;
        }
        grown = (char **)realloc(g_loaded_names, bytes);
        if (!grown) {
            return;
        }
        g_loaded_names = grown;
        g_loaded_capacity = new_cap;
    }
    g_loaded_names[g_loaded_count] = strdup(type_name);
    if (g_loaded_names[g_loaded_count]) {
        g_loaded_count++;
    }
}

ae_error_t ae_resource_loader_load_all(const char *resources_dir,
                                       ae_bool reload)
{
    ae_error_t first_err;
    ae_size *order;
    ae_size k;

    if (!resources_dir) {
        return AE_ERR_INVALID_ARG;
    }
    if (reload) {
        for (ae_size i = 0; i < g_loaded_count; i++) {
            free(g_loaded_names[i]);
        }
        g_loaded_count = 0;
    }
    if (g_registration_count == 0) {
        return AE_OK;
    }

    order = (ae_size *)malloc(g_registration_count * sizeof *order);
    if (!order) {
        return AE_ERR_OUT_OF_MEMORY;
    }
    for (k = 0; k < g_registration_count; k++) {
        order[k] = k;
    }
    qsort(order, g_registration_count, sizeof(order[0]),
          compare_registrations);

    first_err = AE_OK;
    for (k = 0; k < g_registration_count; k++) {
        ae_def_registration_t *reg = &g_registrations[order[k]];
        ae_error_t err;

        if (!reload && is_loaded(reg->def->type_name)) {
            continue;
        }
        err = reg->def->load(resources_dir, reg->def);
        if (err != AE_OK && first_err == AE_OK) {
            first_err = err;
        }
        mark_loaded(reg->def->type_name);
    }
    free(order);
    return first_err;
}

void ae_resource_loader_reset(void)
{
    for (ae_size i = 0; i < g_loaded_count; i++) {
        free(g_loaded_names[i]);
    }
    free(g_loaded_names);
    g_loaded_names = NULL;
    g_loaded_count = 0;
    g_loaded_capacity = 0;

    free(g_registrations);
    g_registrations = NULL;
    g_registration_count = 0;
    g_registration_capacity = 0;
}

/* ============================================================ */