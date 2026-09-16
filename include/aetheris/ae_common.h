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
 * @file ae_common.h
 *
 * @brief Small shared macros for sizing and bounded arithmetic helpers.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_COMMON_H
#define AE_COMMON_H

#include <aetheris/ae_types.h>

/* ============================================================ */

/*
 * Element count of a fixed-size array. Evaluates the expression in the
 * same way a sizeof does without evaluating an lvalue's value.
 */
#define AE_ARRAY_SIZE(array) \
    (sizeof(array) / sizeof((array)[0]))

/*
 * Min and max. Arguments must be free of side effects; each is
 * evaluated at least once and the macros expand to a conditional
 * expression that evaluates the selected arm once more.
 */
#define AE_MIN(a, b) ((a) < (b) ? (a) : (b))
#define AE_MAX(a, b) ((a) > (b) ? (a) : (b))

/* ============================================================ */

/*
 * Overflow-checked arithmetic for allocation sizes. These are used
 * before a count * size (or sum) feeds into an allocation or index so
 * that wrapping is detected instead of silently producing a small
 * buffer. Both return false on overflow and leave *out untouched.
 */
static inline ae_bool ae_mul_overflow_uz(ae_size count, ae_size size, ae_size *out)
{
    if (count != 0 && size > SIZE_MAX / count)
        return false;

    *out = count * size;
    return true;
}

static inline ae_bool ae_add_overflow_uz(ae_size a, ae_size b, ae_size *out)
{
    if (a > SIZE_MAX - b)
        return false;

    *out = a + b;
    return true;
}

/* ============================================================ */

#endif /* AE_COMMON_H */