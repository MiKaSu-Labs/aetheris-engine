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
 * @file ae_types.h
 *
 * @brief Fixed-width scalar aliases shared across the Aetheris public API.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_TYPES_H
#define AE_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ============================================================ */

/*
 * Aetheris uses explicit-width scalar types wherever width matters for a
 * contract. The ae_uN and ae_sN families map to the C99 <stdint.h> types;
 * ae_size and ae_bool mirror size_t and bool respectively so the public
 * API reads consistently under the ae_ prefix.
 */

typedef uint8_t   ae_u8;
typedef uint16_t  ae_u16;
typedef uint32_t  ae_u32;
typedef uint64_t  ae_u64;

typedef int8_t    ae_s8;
typedef int16_t   ae_s16;
typedef int32_t   ae_s32;
typedef int64_t   ae_s64;

typedef size_t    ae_size;
typedef bool      ae_bool;

/* ============================================================ */

#endif /* AE_TYPES_H */