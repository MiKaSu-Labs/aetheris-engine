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
 * @file ae_buffer.h
 *
 * @brief Growable little-endian byte buffer with fixed-width writers.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_BUFFER_H
#define AE_BUFFER_H

#include <aetheris/ae_error.h>
#include <aetheris/ae_types.h>

/* ============================================================ */

/*
 * The buffer is an append-only, little-endian byte container. Writers
 * append scalar values and raw byte runs; capacity grows by doubling
 * while preserving existing contents. Every growing writer fails with
 * AE_ERR_OUT_OF_MEMORY and leaves the buffer unchanged when
 * allocation fails.
 *
 * The buffer is owned by its caller, is not thread-safe, and must not
 * be concurrently written while it is being read.
 */

typedef struct ae_buffer ae_buffer_t;

/**
 * ae_buffer_create - allocate an empty buffer.
 * @initial_capacity: initial capacity in bytes (0 selects a default).
 * @out: receives the new buffer.
 *
 * The buffer starts empty with at least @initial_capacity bytes of
 * storage. Returns AE_OK and sets *out on success, a negative error
 * code and leaves *out untouched on failure. @out must not be NULL.
 */
ae_error_t ae_buffer_create(ae_size initial_capacity, ae_buffer_t **out);

/**
 * ae_buffer_destroy - free a buffer.
 * @buf: buffer to release.
 *
 * All storage owned by @buf is freed and @buf becomes invalid. Passing
 * NULL is a no-op.
 */
void ae_buffer_destroy(ae_buffer_t *buf);

/**
 * ae_buffer_reset - rewind a buffer to empty.
 * @buf: buffer to clear.
 *
 * The write position is reset to zero; allocated capacity is retained.
 * Returns AE_ERR_INVALID_ARG for NULL input.
 */
ae_error_t ae_buffer_reset(ae_buffer_t *buf);

/**
 * ae_buffer_len - number of bytes currently written.
 * @buf: buffer to query.
 *
 * Returns zero for NULL input.
 */
ae_size ae_buffer_len(const ae_buffer_t *buf);

/**
 * ae_buffer_capacity - allocated capacity in bytes.
 * @buf: buffer to query.
 *
 * Returns zero for NULL input.
 */
ae_size ae_buffer_capacity(const ae_buffer_t *buf);

/**
 * ae_buffer_data - read-only view of the written bytes.
 * @buf: buffer to query.
 *
 * Returns the base pointer of the written data; valid until the next
 * grow. The pointer may be NULL when nothing is written. Returns NULL
 * for NULL input. The returned view is not a NUL-terminated string.
 */
const ae_u8 *ae_buffer_data(const ae_buffer_t *buf);

/* ============================================================ */

/**
 * ae_buffer_write_u8 - append one byte.
 * @buf: destination.
 * @value: byte value (low 8 bits are stored).
 *
 * Returns AE_OK on success.
 */
ae_error_t ae_buffer_write_u8(ae_buffer_t *buf, ae_u32 value);

/**
 * ae_buffer_write_bool - append a byte boolean.
 * @buf: destination.
 * @value: true appends 1, false appends 0.
 *
 * Returns AE_OK on success.
 */
ae_error_t ae_buffer_write_bool(ae_buffer_t *buf, ae_bool value);

/**
 * ae_buffer_write_empty - append a run of zero bytes.
 * @buf: destination.
 * @count: number of zeros to append.
 *
 * A count of zero is a no-op. Returns AE_OK on success.
 */
ae_error_t ae_buffer_write_empty(ae_buffer_t *buf, ae_size count);

/**
 * ae_buffer_write_max - append a run of 0xFF bytes.
 * @buf: destination.
 * @count: number of 0xFF bytes to append.
 *
 * A count of zero is a no-op. Returns AE_OK on success.
 */
ae_error_t ae_buffer_write_max(ae_buffer_t *buf, ae_size count);

/**
 * ae_buffer_write_u16 - append a little-endian 16-bit value.
 * @buf: destination.
 * @value: value to append.
 *
 * Returns AE_OK on success.
 */
ae_error_t ae_buffer_write_u16(ae_buffer_t *buf, ae_u16 value);

/**
 * ae_buffer_write_u24 - append a little-endian 24-bit value.
 * @buf: destination.
 * @value: value to append (low 24 bits).
 *
 * Returns AE_OK on success.
 */
ae_error_t ae_buffer_write_u24(ae_buffer_t *buf, ae_u32 value);

/**
 * ae_buffer_write_u32 - append a little-endian 32-bit value.
 * @buf: destination.
 * @value: value to append.
 *
 * Returns AE_OK on success.
 */
ae_error_t ae_buffer_write_u32(ae_buffer_t *buf, ae_u32 value);

/**
 * ae_buffer_write_u64 - append a little-endian 64-bit value.
 * @buf: destination.
 * @value: value to append.
 *
 * Returns AE_OK on success.
 */
ae_error_t ae_buffer_write_u64(ae_buffer_t *buf, ae_u64 value);

/**
 * ae_buffer_write_s16 - append a little-endian signed 16-bit value.
 * @buf: destination.
 * @value: value to append.
 *
 * Returns AE_OK on success.
 */
ae_error_t ae_buffer_write_s16(ae_buffer_t *buf, ae_s16 value);

/**
 * ae_buffer_write_s32 - append a little-endian signed 32-bit value.
 * @buf: destination.
 * @value: value to append.
 *
 * Returns AE_OK on success.
 */
ae_error_t ae_buffer_write_s32(ae_buffer_t *buf, ae_s32 value);

/**
 * ae_buffer_write_f32 - append an IEEE-754 single as bytes.
 * @buf: destination.
 * @value: value to append.
 *
 * The raw bit pattern is stored little-endian. Returns AE_OK.
 */
ae_error_t ae_buffer_write_f32(ae_buffer_t *buf, float value);

/**
 * ae_buffer_write_f64 - append an IEEE-754 double as bytes.
 * @buf: destination.
 * @value: value to append.
 *
 * The raw bit pattern is stored little-endian. Returns AE_OK.
 */
ae_error_t ae_buffer_write_f64(ae_buffer_t *buf, double value);

/* ============================================================ */

/**
 * ae_buffer_write_bytes - append a raw byte run.
 * @buf: destination.
 * @data: bytes to append.
 * @count: number of bytes.
 *
 * @data may be NULL only when @count is zero. Returns AE_OK, or
 * AE_ERR_INVALID_ARG for an invalid combination.
 */
ae_error_t ae_buffer_write_bytes(ae_buffer_t *buf, const ae_u8 *data, ae_size count);

/**
 * ae_buffer_write_string16 - append a length-prefixed UTF-16 string.
 * @buf: destination.
 * @units: UTF-16 code units to append.
 * @count: number of code units.
 *
 * Writes a 16-bit length equal to the UTF-16 byte size (count * 2),
 * followed by each code unit in little-endian order. A NULL @units
 * with @count zero writes an empty string. @units must be non-NULL
 * when @count is non-zero.
 */
ae_error_t ae_buffer_write_string16(ae_buffer_t *buf, const ae_u16 *units,
                                    ae_size count);

/**
 * ae_buffer_write_string8 - append a length-prefixed byte string.
 * @buf: destination.
 * @bytes: characters to append.
 * @count: number of characters.
 *
 * Writes a 16-bit length (@count) followed by the low byte of each
 * character. A NULL @bytes with @count zero writes an empty string.
 * @bytes must be non-NULL when @count is non-zero.
 */
ae_error_t ae_buffer_write_string8(ae_buffer_t *buf, const ae_u8 *bytes,
                                   ae_size count);

/**
 * ae_buffer_write_direct_string8 - append a fixed-width byte string.
 * @buf: destination.
 * @bytes: characters to append.
 * @count: number of characters.
 * @width: exact number of bytes to write.
 *
 * Writes exactly @width bytes; characters beyond @count become zero
 * padding, and characters beyond @width are dropped. No length prefix
 * is written. A NULL @bytes with @count zero appends nothing.
 */
ae_error_t ae_buffer_write_direct_string8(ae_buffer_t *buf, const ae_u8 *bytes,
                                          ae_size count, ae_size width);

/* ============================================================ */

#endif /* AE_BUFFER_H */