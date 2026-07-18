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
 * @file base_packet.c
 *
 * @brief Base network packet implementation.
 *        Manages header and payload buffers, serialises PacketHead via
 *        nanopb, and assembles the complete big-endian wire frame.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include "base_packet.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

/* nanopb generated headers for PacketHead */
#include "pb_encode.h"
#include "packet_head.pb.h"  /* PacketHead fields + struct */

/* =========================================================================
 * Internal helpers
 * ====================================================================== */

/* Returns the current time in milliseconds since the Unix epoch. */
static int64_t _now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (int64_t)ts.tv_sec * 1000LL + (int64_t)(ts.tv_nsec / 1000000LL);
}

/* Replaces *dst / *dst_len with a copy of src / src_len.
 * Frees the old buffer. src may be NULL (resulting in a NULL buffer). */
static void _replace_buf(uint8_t **dst, size_t *dst_len,
                         const uint8_t *src, size_t src_len) {
    free(*dst);
    if (!src || src_len == 0) {
        *dst     = NULL;
        *dst_len = 0;
        return;
    }
    *dst = malloc(src_len);
    if (*dst) {
        memcpy(*dst, src, src_len);
        *dst_len = src_len;
    } else {
        *dst_len = 0;
    }
}

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

ae_base_packet_t *ae_packet_create(int opcode) {
    ae_base_packet_t *pkt = calloc(1, sizeof(ae_base_packet_t));
    if (!pkt) return NULL;
    pkt->opcode          = opcode;
    pkt->should_encrypt  = true;
    return pkt;
}

ae_base_packet_t *ae_packet_create_with_seq(int opcode, int client_sequence) {
    ae_base_packet_t *pkt = ae_packet_create(opcode);
    if (!pkt) return NULL;
    ae_packet_build_header(pkt, client_sequence);
    return pkt;
}

ae_base_packet_t *ae_packet_create_ex(int opcode, bool build_header) {
    ae_base_packet_t *pkt = ae_packet_create(opcode);
    if (!pkt) return NULL;
    pkt->should_build_header = build_header;
    return pkt;
}

void ae_packet_free(ae_base_packet_t *pkt) {
    if (!pkt) return;
    free(pkt->header);
    free(pkt->data);
    free(pkt);
}

/* =========================================================================
 * Header
 * ====================================================================== */

ae_base_packet_t *ae_packet_build_header(ae_base_packet_t *pkt,
                                         int               client_sequence) {
    if (!pkt) return NULL;

    /* Keep the existing header when client_sequence == 0 and one exists. */
    if (pkt->header != NULL && client_sequence == 0)
        return pkt;

    /* Populate the nanopb PacketHead message. */
    PacketHead head = PacketHead_init_zero;
    head.client_sequence_id = (uint32_t)client_sequence;
    head.sent_ms            = (uint64_t)_now_ms();

    /* Determine serialised size, then encode. */
    size_t encoded_size = 0;
    if (!pb_get_encoded_size(&encoded_size, PacketHead_fields, &head))
        return pkt;

    uint8_t *buf = malloc(encoded_size);
    if (!buf) return pkt;

    pb_ostream_t stream = pb_ostream_from_buffer(buf, encoded_size);
    if (!pb_encode(&stream, PacketHead_fields, &head)) {
        free(buf);
        return pkt;
    }

    free(pkt->header);
    pkt->header     = buf;
    pkt->header_len = stream.bytes_written;
    return pkt;
}

void ae_packet_set_header(ae_base_packet_t *pkt,
                          const uint8_t    *header,
                          size_t            len) {
    if (!pkt) return;
    _replace_buf(&pkt->header, &pkt->header_len, header, len);
}

/* =========================================================================
 * Payload
 * ====================================================================== */

void ae_packet_set_data(ae_base_packet_t *pkt,
                        const uint8_t    *data,
                        size_t            len) {
    if (!pkt) return;
    _replace_buf(&pkt->data, &pkt->data_len, data, len);
}

bool ae_packet_set_proto(ae_base_packet_t *pkt,
                         const void       *fields,
                         const void       *proto_msg) {
    if (!pkt || !fields || !proto_msg) return false;

    size_t encoded_size = 0;
    if (!pb_get_encoded_size(&encoded_size,
                             (const pb_field_t *)fields,
                             proto_msg))
        return false;

    uint8_t *buf = malloc(encoded_size);
    if (!buf) return false;

    pb_ostream_t stream = pb_ostream_from_buffer(buf, encoded_size);
    if (!pb_encode(&stream, (const pb_field_t *)fields, proto_msg)) {
        free(buf);
        return false;
    }

    free(pkt->data);
    pkt->data     = buf;
    pkt->data_len = stream.bytes_written;
    return true;
}

/* =========================================================================
 * Wire serialisation
 *
 * Frame layout (all big-endian):
 *   [2]  0x4567
 *   [2]  opcode
 *   [2]  header_len
 *   [4]  data_len
 *   [N]  header
 *   [M]  data
 *   [2]  0x89AB
 * ====================================================================== */
bool ae_packet_build(const ae_base_packet_t *pkt,
                     uint8_t               **out,
                     size_t                 *out_len) {
    if (!pkt || !out || !out_len) return false;

    /* Treat NULL buffers as zero-length. */
    size_t hlen = pkt->header ? pkt->header_len : 0;
    size_t dlen = pkt->data   ? pkt->data_len   : 0;

    /* Total frame size: 2+2+2+4 + hlen + dlen + 2 */
    size_t total = 2u + 2u + 2u + 4u + hlen + dlen + 2u;

    uint8_t *frame = malloc(total);
    if (!frame) return false;

    uint8_t *p = frame;

    ae_write_uint16(p, AE_PACKET_MAGIC_HEAD);    p += 2;
    ae_write_uint16(p, (uint16_t)pkt->opcode);   p += 2;
    ae_write_uint16(p, (uint16_t)hlen);          p += 2;
    ae_write_uint32(p, (uint32_t)dlen);          p += 4;

    if (hlen) { memcpy(p, pkt->header, hlen); p += hlen; }
    if (dlen) { memcpy(p, pkt->data,   dlen); p += dlen; }

    ae_write_uint16(p, AE_PACKET_MAGIC_TAIL);

    *out     = frame;
    *out_len = total;
    return true;
}
