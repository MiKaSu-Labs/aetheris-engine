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
 * @file ae_sparse_set.c
 *
 * @brief Implementation of the CSV-based integer sparse set.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "aetheris/ae_sparse_set.h"

/* ============================================================ */

/* Internal helpers. */

/* Parse one token (either "N" or "N-M") and append an entry. */
static ae_error_t parse_token(ae_sparse_set_t *set, const char *start,
                              const char *end)
{
    const char *dash = NULL;
    const char *p;
    char tmp[32];

    for (p = start; p < end; ++p) {
        if (*p == '-') {
            dash = p;
            break;
        }
    }

    if (dash == NULL) {
        /* Single value. */
        long val;
        ae_size len = (ae_size)(end - start);
        if (len == 0 || len >= sizeof(tmp)) {
            return AE_ERR_INVALID_ARG;
        }
        memcpy(tmp, start, len);
        tmp[len] = '\0';
        val = strtol(tmp, NULL, 10);

        /* Grow if needed. */
        if (set->count == set->capacity) {
            ae_size new_cap = set->capacity ? set->capacity * 2 : 16;
            ae_sparse_entry_t *tmp_buf;
            if (set->owns_memory) {
                tmp_buf = realloc(set->entries,
                                  new_cap * sizeof(ae_sparse_entry_t));
                if (!tmp_buf) {
                    return AE_ERR_OUT_OF_MEMORY;
                }
            } else {
                tmp_buf = malloc(new_cap * sizeof(ae_sparse_entry_t));
                if (!tmp_buf) {
                    return AE_ERR_OUT_OF_MEMORY;
                }
                memcpy(tmp_buf, set->entries,
                       set->count * sizeof(ae_sparse_entry_t));
                set->owns_memory = true;
            }
            set->entries = tmp_buf;
            set->capacity = new_cap;
        }

        set->entries[set->count].value = (ae_s32)val;
        set->entries[set->count].min = 0;
        set->entries[set->count].max = 0;
        set->entries[set->count].is_range = false;
        set->count++;
    } else {
        /* Range "N-M". */
        long lo, hi;
        char lo_buf[32];
        char hi_buf[32];
        ae_size lo_len = (ae_size)(dash - start);
        ae_size hi_len = (ae_size)(end - dash - 1);

        if (lo_len == 0 || lo_len >= sizeof(lo_buf) ||
            hi_len == 0 || hi_len >= sizeof(hi_buf)) {
            return AE_ERR_INVALID_ARG;
        }

        memcpy(lo_buf, start, lo_len);
        lo_buf[lo_len] = '\0';
        lo = strtol(lo_buf, NULL, 10);

        memcpy(hi_buf, dash + 1, hi_len);
        hi_buf[hi_len] = '\0';
        hi = strtol(hi_buf, NULL, 10);

        if (lo > hi) {
            return AE_ERR_INVALID_ARG;
        }

        if (set->count == set->capacity) {
            ae_size new_cap = set->capacity ? set->capacity * 2 : 16;
            ae_sparse_entry_t *tmp_buf;
            if (set->owns_memory) {
                tmp_buf = realloc(set->entries,
                                  new_cap * sizeof(ae_sparse_entry_t));
                if (!tmp_buf) {
                    return AE_ERR_OUT_OF_MEMORY;
                }
            } else {
                tmp_buf = malloc(new_cap * sizeof(ae_sparse_entry_t));
                if (!tmp_buf) {
                    return AE_ERR_OUT_OF_MEMORY;
                }
                memcpy(tmp_buf, set->entries,
                       set->count * sizeof(ae_sparse_entry_t));
                set->owns_memory = true;
            }
            set->entries = tmp_buf;
            set->capacity = new_cap;
        }

        set->entries[set->count].value = 0;
        set->entries[set->count].min = (ae_s32)lo;
        set->entries[set->count].max = (ae_s32)hi;
        set->entries[set->count].is_range = true;
        set->count++;
    }

    return AE_OK;
}

/* ============================================================ */

ae_error_t ae_sparse_set_init(ae_sparse_set_t *set,
                              const char *csv,
                              ae_sparse_entry_t *buf,
                              ae_size buf_cap)
{
    const char *p;
    const char *token_start;
    ae_error_t err;

    if (!set) {
        return AE_ERR_INVALID_ARG;
    }

    set->entries = NULL;
    set->count = 0;
    set->capacity = 0;
    set->owns_memory = false;

    if (!csv || csv[0] == '\0') {
        return AE_OK;
    }

    if (buf && buf_cap > 0) {
        set->entries = buf;
        set->capacity = buf_cap;
        set->owns_memory = false;
    }

    p = csv;
    token_start = NULL;

    while (*p) {
        if (*p == ',') {
            if (token_start) {
                err = parse_token(set, token_start, p);
                if (err != AE_OK) {
                    return err;
                }
                token_start = NULL;
            }
        } else if (*p == ' ' || *p == '\n' || *p == '\t') {
            /* Skip whitespace, matching Java behaviour. */
        } else {
            if (!token_start) {
                token_start = p;
            }
        }
        ++p;
    }

    if (token_start) {
        err = parse_token(set, token_start, p);
        if (err != AE_OK) {
            return err;
        }
    }

    return AE_OK;
}

void ae_sparse_set_destroy(ae_sparse_set_t *set)
{
    if (!set) {
        return;
    }
    if (set->owns_memory && set->entries) {
        free(set->entries);
    }
    set->entries = NULL;
    set->count = 0;
    set->capacity = 0;
    set->owns_memory = false;
}

ae_bool ae_sparse_set_contains(const ae_sparse_set_t *set,
                               ae_s32 value)
{
    ae_size i;

    if (!set || !set->entries) {
        return false;
    }

    for (i = 0; i < set->count; ++i) {
        const ae_sparse_entry_t *e = &set->entries[i];
        if (e->is_range) {
            if (value >= e->min && value <= e->max) {
                return true;
            }
        } else {
            if (e->value == value) {
                return true;
            }
        }
    }

    return false;
}

ae_size ae_sparse_set_count(const ae_sparse_set_t *set)
{
    if (!set) {
        return 0;
    }
    return set->count;
}
