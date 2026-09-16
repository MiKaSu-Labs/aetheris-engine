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
 * @file ae_error.h
 *
 * @brief Core error model: ae_error_t codes and negative-return convention.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_ERROR_H
#define AE_ERROR_H

#include <aetheris/ae_types.h>

/* ============================================================ */

/*
 * Aetheris uses an explicit error code (ae_error_t) carried by return
 * values. AE_OK is zero; every failure is a distinct negative code.
 * Functions that can fail return ae_error_t and never leave a caller
 * to inspect hidden global state.
 */

typedef int32_t ae_error_t;

#define AE_OK               ((ae_error_t) 0)

#define AE_ERR_INVALID_ARG  ((ae_error_t) -1)
#define AE_ERR_INVALID_STATE ((ae_error_t) -2)
#define AE_ERR_OUT_OF_MEMORY ((ae_error_t) -3)
#define AE_ERR_NOT_FOUND    ((ae_error_t) -4)
#define AE_ERR_ALREADY_EXISTS ((ae_error_t) -5)
#define AE_ERR_BUFFER_TOO_SMALL ((ae_error_t) -6)
#define AE_ERR_BAD_DATA     ((ae_error_t) -7)
#define AE_ERR_BAD_LENGTH   ((ae_error_t) -8)
#define AE_ERR_OVERFLOW     ((ae_error_t) -9)
#define AE_ERR_IO           ((ae_error_t) -10)
#define AE_ERR_TIMEOUT      ((ae_error_t) -11)
#define AE_ERR_UNSUPPORTED  ((ae_error_t) -12)
#define AE_ERR_PERMISSION   ((ae_error_t) -13)
#define AE_ERR_ABORTED      ((ae_error_t) -14)

/* ============================================================ */

/**
 * ae_error_ok - predicate for success.
 * @err: error code to test.
 *
 * Returns true when err equals AE_OK.
 */
ae_bool ae_error_ok(ae_error_t err);

/**
 * ae_error_failed - predicate for failure.
 * @err: error code to test.
 *
 * Returns true when err is not AE_OK.
 */
ae_bool ae_error_failed(ae_error_t err);

/**
 * ae_error_str - return a stable name for an error code.
 * @err: error code to describe.
 *
 * Returns a static, null-terminated name for known codes and a generic
 * name for unknown values. The result is never NULL and need not be
 * freed.
 */
const char *ae_error_str(ae_error_t err);

/* ============================================================ */

#endif /* AE_ERROR_H */