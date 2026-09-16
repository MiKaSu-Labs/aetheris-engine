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
 * @file ae_base_packet.c
 *
 * @brief Outgoing frame builder for the protocol wire format.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdlib.h>
#include <string.h>

#include <aetheris/ae_base_packet.h>
#include <aetheris/ae_common.h>

/* ============================================================ */

static ae_error_t write_be_u16(ae_u8 *dst, ae_size *off, ae_u16 value)
{
    dst[(*off)++] = (ae_u8) ((value >> 8) & 0xFFu);
    dst[(*off)++] = (ae_u8) (value & 0xFFu);
    return AE_OK;
}

static ae_error_t write_be_u32(ae_u8 *dst, ae_size *off, ae_u32 value)
{
    dst[(*off)++] = (ae_u8) ((value >> 24) & 0xFFu);
    dst[(*off)++] = (ae_u8) ((value >> 16) & 0xFFu);
    dst[(*off)++] = (ae_u8) ((value >> 8) & 0xFFu);
    dst[(*off)++] = (ae_u8) (value & 0xFFu);
    return AE_OK;
}

/* ============================================================ */

ae_error_t ae_base_packet_build(const ae_base_packet_t *packet,
                                ae_u8 **out, ae_size *out_len)
{
    ae_size frame_len = 0;
    ae_size off = 0;
    ae_u8 *frame;
    ae_bool ok;

    if (packet == NULL || out == NULL || out_len == NULL)
        return AE_ERR_INVALID_ARG;

    if (packet->header_len > 0 && packet->header == NULL)
        return AE_ERR_INVALID_ARG;
    if (packet->data_len > 0 && packet->data == NULL)
        return AE_ERR_INVALID_ARG;

    /* Integer lengths are truncated to their wire widths. Reject values
     * that cannot be represented instead of producing a corrupt frame. */
    if (packet->opcode < 0 || packet->opcode > UINT16_MAX)
        return AE_ERR_OVERFLOW;
    if (packet->header_len > UINT16_MAX)
        return AE_ERR_OVERFLOW;
    if (packet->data_len > UINT32_MAX)
        return AE_ERR_OVERFLOW;

    /* u16 marker + u16 opcode + u16 header len + u32 data len
     * + header + data + u16 marker.  Fixed portion = 12 bytes. */
    frame_len = 0;
    ok = ae_add_overflow_uz(12, packet->header_len, &frame_len);
    if (ok)
        ok = ae_add_overflow_uz(frame_len, packet->data_len, &frame_len);
    if (!ok)
        return AE_ERR_OVERFLOW;

    frame = malloc(frame_len);
    if (frame == NULL)
        return AE_ERR_OUT_OF_MEMORY;

    write_be_u16(frame, &off, AE_BASE_PACKET_CONST1);
    write_be_u16(frame, &off, (ae_u16) packet->opcode);
    write_be_u16(frame, &off, (ae_u16) packet->header_len);
    write_be_u32(frame, &off, (ae_u32) packet->data_len);

    if (packet->header_len > 0)
        memcpy(frame + off, packet->header, packet->header_len);
    off += packet->header_len;

    if (packet->data_len > 0)
        memcpy(frame + off, packet->data, packet->data_len);
    off += packet->data_len;

    write_be_u16(frame, &off, AE_BASE_PACKET_CONST2);

    *out = frame;
    *out_len = frame_len;
    return AE_OK;
}

/* ============================================================ */