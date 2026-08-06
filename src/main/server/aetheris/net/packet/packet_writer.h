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
 * @file packet_writer.h
 *
 * @brief Growable little-endian byte buffer writer for packet payloads.
 *        Provides primitive, string, and raw byte-array writers that
 *        append to an internal auto-growing buffer, mirroring a
 *        ByteArrayOutputStream-backed binary writer.
 *
 *        All multi-byte primitives are written least-significant-byte
 *        first (little-endian). This is the packet body encoding used
 *        beneath the big-endian frame produced by base_packet.h.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef PACKET_WRITER_H
#define PACKET_WRITER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * ae_packet_writer_t , opaque growable buffer handle
 * ====================================================================== */
typedef struct ae_packet_writer ae_packet_writer_t;

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

/*
 * Allocates a new writer with an initial internal capacity of 128 bytes.
 * Caller must free with ae_packet_writer_free().
 */
ae_packet_writer_t *ae_packet_writer_create(void);

/*
 * Releases all resources owned by the writer.
 */
void ae_packet_writer_free(ae_packet_writer_t *w);

/*
 * Returns a heap-allocated copy of everything written so far, and
 * writes its length to '*out_len'. The writer's internal buffer is
 * left untouched, so writing may continue and build() may be called
 * again later.
 * Caller must free the returned buffer.
 * Returns NULL on allocation failure.
 */
uint8_t *ae_packet_writer_build(const ae_packet_writer_t *w, size_t *out_len);

/*
 * Returns a direct, non-owning pointer to the writer's internal buffer.
 * Valid until the next write call (which may reallocate the buffer).
 * Do not free the returned pointer.
 */
const uint8_t *ae_packet_writer_data(const ae_packet_writer_t *w);

/*
 * Returns the number of bytes currently written to the buffer.
 */
size_t ae_packet_writer_len(const ae_packet_writer_t *w);

/* =========================================================================
 * Fill writers
 * ====================================================================== */

/* Appends 'count' zero bytes (0x00). */
void ae_pw_write_empty(ae_packet_writer_t *w, int count);

/* Appends 'count' max bytes (0xFF). */
void ae_pw_write_max(ae_packet_writer_t *w, int count);

/* =========================================================================
 * Integer writers
 * ====================================================================== */

/* Writes a single signed byte. */
void ae_pw_write_int8(ae_packet_writer_t *w, int8_t v);

/* Writes a boolean as a single byte: 1 for true, 0 for false. */
void ae_pw_write_bool(ae_packet_writer_t *w, bool b);

/* Writes a single unsigned byte. */
void ae_pw_write_uint8(ae_packet_writer_t *w, uint8_t v);

/* Writes an unsigned 16-bit value, little-endian. */
void ae_pw_write_uint16(ae_packet_writer_t *w, uint16_t v);

/* Writes the low 24 bits of 'v', little-endian. */
void ae_pw_write_uint24(ae_packet_writer_t *w, uint32_t v);

/* Writes a signed 16-bit value, little-endian. */
void ae_pw_write_int16(ae_packet_writer_t *w, int16_t v);

/* Writes an unsigned 32-bit value, little-endian. */
void ae_pw_write_uint32(ae_packet_writer_t *w, uint32_t v);

/* Writes a signed 32-bit value, little-endian. */
void ae_pw_write_int32(ae_packet_writer_t *w, int32_t v);

/* Writes an unsigned 64-bit value, little-endian. */
void ae_pw_write_uint64(ae_packet_writer_t *w, uint64_t v);

/* =========================================================================
 * Floating-point writers
 * ====================================================================== */

/* Writes a 32-bit float as its raw IEEE-754 bit pattern, little-endian. */
void ae_pw_write_float(ae_packet_writer_t *w, float f);

/* Writes a 64-bit double as its raw IEEE-754 bit pattern, little-endian. */
void ae_pw_write_double(ae_packet_writer_t *w, double d);

/* =========================================================================
 * String writers
 *
 * Strings are represented as UTF-16 code unit arrays (uint16_t), the
 * same underlying representation used internally by the original
 * string type. Pass NULL for 'str' to write a null/empty string.
 * ====================================================================== */

/*
 * Writes a uint16 byte-length prefix (char_count * 2) followed by each
 * code unit in 'str' as a little-endian uint16.
 * If 'str' is NULL, writes only a zero length prefix.
 */
void ae_pw_write_string16(ae_packet_writer_t *w,
                          const uint16_t     *str,
                          size_t              char_count);

/*
 * Writes a uint16 character-count prefix followed by each code unit in
 * 'str' truncated to a single unsigned byte.
 * If 'str' is NULL, writes only a zero length prefix.
 */
void ae_pw_write_string8(ae_packet_writer_t *w,
                         const uint16_t     *str,
                         size_t              char_count);

/*
 * Writes exactly 'expected_size' bytes: each code unit from 'str'
 * (truncated to a byte) up to 'char_count', then zero-padded for the
 * remainder. Writes nothing at all if 'str' is NULL.
 */
void ae_pw_write_direct_string8(ae_packet_writer_t *w,
                                const uint16_t     *str,
                                size_t              char_count,
                                int                 expected_size);

/* =========================================================================
 * Raw byte writers
 * ====================================================================== */

/* Appends 'len' raw bytes from 'bytes'. */
void ae_pw_write_bytes(ae_packet_writer_t *w,
                       const uint8_t      *bytes,
                       size_t              len);

/*
 * Appends 'count' values from 'values', each truncated to a single
 * byte (the low 8 bits of each int).
 */
void ae_pw_write_bytes_from_ints(ae_packet_writer_t *w,
                                 const int          *values,
                                 size_t              count);

#ifdef __cplusplus
}
#endif

#endif /* PACKET_WRITER_H */
