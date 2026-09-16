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
 * @file ae_buffer.c
 *
 * @brief Growable little-endian buffer writer implementation.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdlib.h>
#include <string.h>

#include <aetheris/ae_buffer.h>
#include <aetheris/ae_common.h>

/* ============================================================ */

#define AE_BUFFER_DEFAULT_CAPACITY 128

struct ae_buffer {
    ae_u8 *data;
    ae_size len;
    ae_size capacity;
};

/* ============================================================ */

/*
 * Reserve enough room to append @need bytes to a buffer. Grows by
 * doubling and preserves contents on every growth; returns
 * AE_ERR_OVERFLOW when the arithmetic would wrap and
 * AE_ERR_OUT_OF_MEMORY when allocation fails. On any failure the
 * buffer contents and capacity are left unchanged.
 */
static ae_error_t reserve(ae_buffer_t *buf, ae_size need)
{
    ae_size new_capacity;
    ae_size grown;
    ae_u8 *new_data;
    ae_bool ok;

    if (buf->capacity >= need)
        return AE_OK;

    ok = ae_mul_overflow_uz(buf->capacity, 2, &grown);
    if (!ok || grown < need)
        grown = need;

    new_capacity = grown;
    new_data = (ae_u8 *) realloc(buf->data, new_capacity);
    if (new_data == NULL)
        return AE_ERR_OUT_OF_MEMORY;

    buf->data = new_data;
    buf->capacity = new_capacity;
    return AE_OK;
}

static ae_error_t append_bytes(ae_buffer_t *buf, const ae_u8 *data, ae_size count)
{
    ae_size new_len;
    ae_bool ok;

    if (buf == NULL)
        return AE_ERR_INVALID_ARG;
    if (count > 0 && data == NULL)
        return AE_ERR_INVALID_ARG;

    ok = ae_add_overflow_uz(buf->len, count, &new_len);
    if (!ok)
        return AE_ERR_OVERFLOW;

    if (new_len > buf->capacity) {
        ae_error_t err = reserve(buf, new_len);
        if (err != AE_OK)
            return err;
    }

    if (count > 0)
        memcpy(buf->data + buf->len, data, count);
    buf->len = new_len;
    return AE_OK;
}

/* ============================================================ */

ae_error_t ae_buffer_create(ae_size initial_capacity, ae_buffer_t **out)
{
    ae_buffer_t *buf;

    if (out == NULL)
        return AE_ERR_INVALID_ARG;

    buf = (ae_buffer_t *) malloc(sizeof(*buf));
    if (buf == NULL)
        return AE_ERR_OUT_OF_MEMORY;

    if (initial_capacity == 0)
        initial_capacity = AE_BUFFER_DEFAULT_CAPACITY;

    buf->data = (ae_u8 *) malloc(initial_capacity);
    if (buf->data == NULL) {
        free(buf);
        return AE_ERR_OUT_OF_MEMORY;
    }

    buf->len = 0;
    buf->capacity = initial_capacity;
    *out = buf;
    return AE_OK;
}

void ae_buffer_destroy(ae_buffer_t *buf)
{
    if (buf == NULL)
        return;

    free(buf->data);
    free(buf);
}

ae_error_t ae_buffer_reset(ae_buffer_t *buf)
{
    if (buf == NULL)
        return AE_ERR_INVALID_ARG;

    buf->len = 0;
    return AE_OK;
}

ae_size ae_buffer_len(const ae_buffer_t *buf)
{
    return buf == NULL ? 0 : buf->len;
}

ae_size ae_buffer_capacity(const ae_buffer_t *buf)
{
    return buf == NULL ? 0 : buf->capacity;
}

const ae_u8 *ae_buffer_data(const ae_buffer_t *buf)
{
    return buf == NULL ? NULL : buf->data;
}

/* ============================================================ */

ae_error_t ae_buffer_write_u8(ae_buffer_t *buf, ae_u32 value)
{
    ae_u8 byte = (ae_u8) value;
    return append_bytes(buf, &byte, 1);
}

ae_error_t ae_buffer_write_bool(ae_buffer_t *buf, ae_bool value)
{
    ae_u8 byte = value ? 1 : 0;
    return append_bytes(buf, &byte, 1);
}

ae_error_t ae_buffer_write_empty(ae_buffer_t *buf, ae_size count)
{
    ae_size new_len;
    ae_bool ok;

    if (buf == NULL)
        return AE_ERR_INVALID_ARG;
    if (count == 0)
        return AE_OK;

    ok = ae_add_overflow_uz(buf->len, count, &new_len);
    if (!ok)
        return AE_ERR_OVERFLOW;

    if (new_len > buf->capacity) {
        ae_error_t err = reserve(buf, new_len);
        if (err != AE_OK)
            return err;
    }

    memset(buf->data + buf->len, 0, count);
    buf->len = new_len;
    return AE_OK;
}

ae_error_t ae_buffer_write_max(ae_buffer_t *buf, ae_size count)
{
    ae_size new_len;
    ae_bool ok;

    if (buf == NULL)
        return AE_ERR_INVALID_ARG;
    if (count == 0)
        return AE_OK;

    ok = ae_add_overflow_uz(buf->len, count, &new_len);
    if (!ok)
        return AE_ERR_OVERFLOW;

    if (new_len > buf->capacity) {
        ae_error_t err = reserve(buf, new_len);
        if (err != AE_OK)
            return err;
    }

    memset(buf->data + buf->len, 0xFF, count);
    buf->len = new_len;
    return AE_OK;
}

ae_error_t ae_buffer_write_u16(ae_buffer_t *buf, ae_u16 value)
{
    ae_u8 bytes[2] = {
        (ae_u8) (value & 0xFF),
        (ae_u8) ((value >> 8) & 0xFF),
    };
    return append_bytes(buf, bytes, sizeof(bytes));
}

ae_error_t ae_buffer_write_u24(ae_buffer_t *buf, ae_u32 value)
{
    ae_u8 bytes[3] = {
        (ae_u8) (value & 0xFF),
        (ae_u8) ((value >> 8) & 0xFF),
        (ae_u8) ((value >> 16) & 0xFF),
    };
    return append_bytes(buf, bytes, sizeof(bytes));
}

ae_error_t ae_buffer_write_u32(ae_buffer_t *buf, ae_u32 value)
{
    ae_u8 bytes[4] = {
        (ae_u8) (value & 0xFF),
        (ae_u8) ((value >> 8) & 0xFF),
        (ae_u8) ((value >> 16) & 0xFF),
        (ae_u8) ((value >> 24) & 0xFF),
    };
    return append_bytes(buf, bytes, sizeof(bytes));
}

ae_error_t ae_buffer_write_u64(ae_buffer_t *buf, ae_u64 value)
{
    ae_u8 bytes[8];
    ae_size i;

    for (i = 0; i < 8; i++)
        bytes[i] = (ae_u8) ((value >> (i * 8)) & 0xFF);

    return append_bytes(buf, bytes, sizeof(bytes));
}

ae_error_t ae_buffer_write_s16(ae_buffer_t *buf, ae_s16 value)
{
    return ae_buffer_write_u16(buf, (ae_u16) value);
}

ae_error_t ae_buffer_write_s32(ae_buffer_t *buf, ae_s32 value)
{
    return ae_buffer_write_u32(buf, (ae_u32) value);
}

ae_error_t ae_buffer_write_f32(ae_buffer_t *buf, float value)
{
    ae_u32 bits;

    memcpy(&bits, &value, sizeof(bits));
    return ae_buffer_write_u32(buf, bits);
}

ae_error_t ae_buffer_write_f64(ae_buffer_t *buf, double value)
{
    ae_u64 bits;

    memcpy(&bits, &value, sizeof(bits));
    return ae_buffer_write_u64(buf, bits);
}

/* ============================================================ */

ae_error_t ae_buffer_write_bytes(ae_buffer_t *buf, const ae_u8 *data, ae_size count)
{
    if (buf == NULL)
        return AE_ERR_INVALID_ARG;
    if (count > 0 && data == NULL)
        return AE_ERR_INVALID_ARG;
    if (count == 0)
        return AE_OK;

    return append_bytes(buf, data, count);
}

ae_error_t ae_buffer_write_string16(ae_buffer_t *buf, const ae_u16 *units,
                                    ae_size count)
{
    ae_size byte_count;
    ae_error_t err;
    ae_size new_len;
    ae_size i;
    ae_bool ok;

    if (buf == NULL)
        return AE_ERR_INVALID_ARG;
    if (count > 0 && units == NULL)
        return AE_ERR_INVALID_ARG;

    if (count > UINT16_MAX / 2)
        return AE_ERR_OVERFLOW;
    byte_count = count * 2;

    ok = ae_add_overflow_uz(buf->len, (ae_size) 2 + byte_count, &new_len);
    if (!ok)
        return AE_ERR_OVERFLOW;

    err = reserve(buf, new_len);
    if (err != AE_OK)
        return err;

    {
        ae_u16 len_prefix = (ae_u16) byte_count;
        ae_u8 *dst = buf->data + buf->len;

        dst[0] = (ae_u8) (len_prefix & 0xFF);
        dst[1] = (ae_u8) ((len_prefix >> 8) & 0xFF);
        dst += 2;

        for (i = 0; i < count; i++) {
            dst[i * 2 + 0] = (ae_u8) (units[i] & 0xFF);
            dst[i * 2 + 1] = (ae_u8) ((units[i] >> 8) & 0xFF);
        }
    }

    buf->len += (ae_size) 2 + byte_count;
    return AE_OK;
}

ae_error_t ae_buffer_write_string8(ae_buffer_t *buf, const ae_u8 *bytes,
                                   ae_size count)
{
    ae_error_t err;
    ae_size new_len;
    ae_size i;
    ae_bool ok;

    if (buf == NULL)
        return AE_ERR_INVALID_ARG;
    if (count > 0 && bytes == NULL)
        return AE_ERR_INVALID_ARG;

    if (count > UINT16_MAX)
        return AE_ERR_OVERFLOW;

    ok = ae_add_overflow_uz(buf->len, (ae_size) 2 + count, &new_len);
    if (!ok)
        return AE_ERR_OVERFLOW;

    err = reserve(buf, new_len);
    if (err != AE_OK)
        return err;

    {
        ae_u16 len_prefix = (ae_u16) count;
        ae_u8 *dst = buf->data + buf->len;

        dst[0] = (ae_u8) (len_prefix & 0xFF);
        dst[1] = (ae_u8) ((len_prefix >> 8) & 0xFF);
        dst += 2;

        for (i = 0; i < count; i++)
            dst[i] = bytes[i];
    }

    buf->len += (ae_size) 2 + count;
    return AE_OK;
}

ae_error_t ae_buffer_write_direct_string8(ae_buffer_t *buf, const ae_u8 *bytes,
                                          ae_size count, ae_size width)
{
    ae_error_t err;
    ae_size new_len;
    ae_size i;
    ae_bool ok;

    if (buf == NULL)
        return AE_ERR_INVALID_ARG;
    if (count > 0 && bytes == NULL)
        return AE_ERR_INVALID_ARG;
    if (bytes == NULL && count == 0)
        return AE_OK;

    ok = ae_add_overflow_uz(buf->len, width, &new_len);
    if (!ok)
        return AE_ERR_OVERFLOW;

    err = reserve(buf, new_len);
    if (err != AE_OK)
        return err;

    {
        ae_u8 *dst = buf->data + buf->len;
        ae_size limit = count < width ? count : width;

        for (i = 0; i < limit; i++)
            dst[i] = bytes[i];
        for (; i < width; i++)
            dst[i] = 0;
    }

    buf->len += width;
    return AE_OK;
}

/* ============================================================ */