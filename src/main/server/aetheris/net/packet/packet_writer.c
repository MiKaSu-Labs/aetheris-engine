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
 * @file packet_writer.c
 *
 * @brief Growable little-endian byte buffer writer implementation.
 *        The internal buffer doubles in capacity as needed, mirroring
 *        the auto-growth behaviour of a ByteArrayOutputStream-backed
 *        binary writer.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include "packet_writer.h"

#include <stdlib.h>
#include <string.h>

/* =========================================================================
 * ae_packet_writer_t definition
 * ====================================================================== */
struct ae_packet_writer {
    uint8_t *buf;
    size_t   len;
    size_t   cap;
};

/* =========================================================================
 * Internal helpers
 * ====================================================================== */

/* Ensures at least 'extra' additional bytes of free capacity. */
static void _ensure_capacity(ae_packet_writer_t *w, size_t extra) {
    size_t need = w->len + extra;
    if (need <= w->cap) return;

    size_t new_cap = w->cap ? w->cap * 2 : 128;
    while (new_cap < need) new_cap *= 2;

    w->buf = realloc(w->buf, new_cap);
    w->cap = new_cap;
}

/* Appends a single raw byte. */
static void _put_byte(ae_packet_writer_t *w, uint8_t b) {
    _ensure_capacity(w, 1);
    w->buf[w->len++] = b;
}

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

ae_packet_writer_t *ae_packet_writer_create(void) {
    ae_packet_writer_t *w = calloc(1, sizeof(ae_packet_writer_t));
    if (!w) return NULL;

    w->cap = 128;
    w->buf = malloc(w->cap);
    if (!w->buf) {
        free(w);
        return NULL;
    }
    w->len = 0;
    return w;
}

void ae_packet_writer_free(ae_packet_writer_t *w) {
    if (!w) return;
    free(w->buf);
    free(w);
}

uint8_t *ae_packet_writer_build(const ae_packet_writer_t *w, size_t *out_len) {
    if (!w || !out_len) return NULL;

    uint8_t *copy = malloc(w->len);
    if (!copy && w->len > 0) return NULL;

    if (w->len > 0) memcpy(copy, w->buf, w->len);
    *out_len = w->len;
    return copy;
}

const uint8_t *ae_packet_writer_data(const ae_packet_writer_t *w) {
    return w ? w->buf : NULL;
}

size_t ae_packet_writer_len(const ae_packet_writer_t *w) {
    return w ? w->len : 0;
}

/* =========================================================================
 * Fill writers
 * ====================================================================== */

void ae_pw_write_empty(ae_packet_writer_t *w, int count) {
    while (count-- > 0) _put_byte(w, 0x00);
}

void ae_pw_write_max(ae_packet_writer_t *w, int count) {
    while (count-- > 0) _put_byte(w, 0xFF);
}

/* =========================================================================
 * Integer writers
 * ====================================================================== */

void ae_pw_write_int8(ae_packet_writer_t *w, int8_t v) {
    _put_byte(w, (uint8_t)v);
}

void ae_pw_write_bool(ae_packet_writer_t *w, bool b) {
    _put_byte(w, b ? 1 : 0);
}

void ae_pw_write_uint8(ae_packet_writer_t *w, uint8_t v) {
    _put_byte(w, v);
}

void ae_pw_write_uint16(ae_packet_writer_t *w, uint16_t v) {
    _put_byte(w, (uint8_t)( v       & 0xFFu));
    _put_byte(w, (uint8_t)((v >> 8) & 0xFFu));
}

void ae_pw_write_uint24(ae_packet_writer_t *w, uint32_t v) {
    _put_byte(w, (uint8_t)( v        & 0xFFu));
    _put_byte(w, (uint8_t)((v >>  8) & 0xFFu));
    _put_byte(w, (uint8_t)((v >> 16) & 0xFFu));
}

void ae_pw_write_int16(ae_packet_writer_t *w, int16_t v) {
    uint16_t u = (uint16_t)v;
    _put_byte(w, (uint8_t)( u       & 0xFFu));
    _put_byte(w, (uint8_t)((u >> 8) & 0xFFu));
}

void ae_pw_write_uint32(ae_packet_writer_t *w, uint32_t v) {
    _put_byte(w, (uint8_t)( v        & 0xFFu));
    _put_byte(w, (uint8_t)((v >>  8) & 0xFFu));
    _put_byte(w, (uint8_t)((v >> 16) & 0xFFu));
    _put_byte(w, (uint8_t)((v >> 24) & 0xFFu));
}

void ae_pw_write_int32(ae_packet_writer_t *w, int32_t v) {
    ae_pw_write_uint32(w, (uint32_t)v);
}

void ae_pw_write_uint64(ae_packet_writer_t *w, uint64_t v) {
    _put_byte(w, (uint8_t)( v        & 0xFFu));
    _put_byte(w, (uint8_t)((v >>  8) & 0xFFu));
    _put_byte(w, (uint8_t)((v >> 16) & 0xFFu));
    _put_byte(w, (uint8_t)((v >> 24) & 0xFFu));
    _put_byte(w, (uint8_t)((v >> 32) & 0xFFu));
    _put_byte(w, (uint8_t)((v >> 40) & 0xFFu));
    _put_byte(w, (uint8_t)((v >> 48) & 0xFFu));
    _put_byte(w, (uint8_t)((v >> 56) & 0xFFu));
}

/* =========================================================================
 * Floating-point writers
 * ====================================================================== */

void ae_pw_write_float(ae_packet_writer_t *w, float f) {
    uint32_t bits;
    memcpy(&bits, &f, sizeof(bits));
    ae_pw_write_uint32(w, bits);
}

void ae_pw_write_double(ae_packet_writer_t *w, double d) {
    uint64_t bits;
    memcpy(&bits, &d, sizeof(bits));
    ae_pw_write_uint64(w, bits);
}

/* =========================================================================
 * String writers
 * ====================================================================== */

void ae_pw_write_string16(ae_packet_writer_t *w,
                          const uint16_t     *str,
                          size_t              char_count) {
    if (!str) {
        ae_pw_write_uint16(w, 0);
        return;
    }

    ae_pw_write_uint16(w, (uint16_t)(char_count * 2));
    for (size_t i = 0; i < char_count; i++) {
        ae_pw_write_uint16(w, str[i]);
    }
}

void ae_pw_write_string8(ae_packet_writer_t *w,
                         const uint16_t     *str,
                         size_t              char_count) {
    if (!str) {
        ae_pw_write_uint16(w, 0);
        return;
    }

    ae_pw_write_uint16(w, (uint16_t)char_count);
    for (size_t i = 0; i < char_count; i++) {
        ae_pw_write_uint8(w, (uint8_t)str[i]);
    }
}

void ae_pw_write_direct_string8(ae_packet_writer_t *w,
                                const uint16_t     *str,
                                size_t              char_count,
                                int                 expected_size) {
    if (!str) return;

    for (int i = 0; i < expected_size; i++) {
        uint16_t c = ((size_t)i < char_count) ? str[i] : 0;
        ae_pw_write_uint8(w, (uint8_t)c);
    }
}

/* =========================================================================
 * Raw byte writers
 * ====================================================================== */

void ae_pw_write_bytes(ae_packet_writer_t *w,
                       const uint8_t      *bytes,
                       size_t              len) {
    if (!bytes || len == 0) return;
    _ensure_capacity(w, len);
    memcpy(w->buf + w->len, bytes, len);
    w->len += len;
}

void ae_pw_write_bytes_from_ints(ae_packet_writer_t *w,
                                 const int          *values,
                                 size_t              count) {
    if (!values || count == 0) return;
    _ensure_capacity(w, count);
    for (size_t i = 0; i < count; i++) {
        w->buf[w->len++] = (uint8_t)values[i];
    }
}
