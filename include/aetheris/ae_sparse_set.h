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
 * @file ae_sparse_set.h
 *
 * @brief Parsed integer set built from compact CSV notation such as
 *        "1,3-5,8". Supports individual values and closed ranges.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_SPARSE_SET_H
#define AE_SPARSE_SET_H

#include <aetheris/ae_error.h>
#include <aetheris/ae_types.h>

/* ============================================================ */

/*
 * A single entry in the sparse set.  A range entry has
 * `is_range == true`; a single value entry has
 * `is_range == false`.
 */
typedef struct ae_sparse_entry {
    ae_s32 value;
    ae_s32 min;
    ae_s32 max;
    ae_bool is_range;
} ae_sparse_entry_t;

/*
 * A compact integer set parsed from CSV notation.
 *
 * Notation examples:
 *   "1,3-5,8"  -> single values 1, 8 and range [3,5]
 *   "10-20"    -> single range [10,20]
 *   "1"        -> single value 1
 *
 * Entries are stored in an inline buffer of at most @max_entries
 * slots.  Allocate via `ae_sparse_set_init` with a sufficiently
 * large buffer, or zero-initialise the struct and pass NULL to
 * init for a malloc-backed growable set.
 */
typedef struct ae_sparse_set {
    ae_sparse_entry_t *entries;
    ae_size count;
    ae_size capacity;

    /* When entries points to a caller-provided buffer this is
     * false; when malloc-backed it is true. */
    ae_bool owns_memory;
} ae_sparse_set_t;

/**
 * ae_sparse_set_init - parse a CSV string into a sparse set.
 * @set: set to initialise (must already be allocated by the caller).
 * @csv: CSV string such as "1,3-5,8".  NULL or empty yields an
 *       empty set.
 * @buf: optional caller-provided buffer for entries, or NULL to
 *       request malloc-backed storage.
 * @buf_cap: capacity of @buf, in entries.  Ignored when @buf is
 *           NULL.
 *
 * When @buf is NULL the entries array is allocated on the heap
 * and freed by `ae_sparse_set_destroy`.  When @buf is provided the
 * caller is responsible for its lifetime.
 *
 * Returns AE_OK on success, AE_ERR_INVALID_ARG when the string
 * contains unparseable tokens, and AE_ERR_OOM when the heap
 * buffer is too small.
 */
ae_error_t ae_sparse_set_init(ae_sparse_set_t *set,
                              const char *csv,
                              ae_sparse_entry_t *buf,
                              ae_size buf_cap);

/**
 * ae_sparse_set_destroy - release resources held by a sparse set.
 * @set: set to tear down.  Safe to call on a zero-initialised
 *       struct.
 *
 * If the set was heap-backed the entries array is freed.  Caller-
 * backed sets are left untouched.
 */
void ae_sparse_set_destroy(ae_sparse_set_t *set);

/**
 * ae_sparse_set_contains - membership test.
 * @set: parsed set.
 * @value: integer to look up.
 *
 * Returns true when @value falls inside at least one range or
 * equals a single-value entry.
 */
ae_bool ae_sparse_set_contains(const ae_sparse_set_t *set,
                               ae_s32 value);

/**
 * ae_sparse_set_count - return the number of entries.
 * @set: parsed set.
 */
ae_size ae_sparse_set_count(const ae_sparse_set_t *set);

/* ============================================================ */

#endif /* AE_SPARSE_SET_H */
