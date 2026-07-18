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
 * @file base_packet.h
 *
 * @brief Base network packet structure and wire-format serialiser.
 *        Encapsulates an opcode, a serialised PacketHead header, and a
 *        serialised protobuf payload. Assembles the on-wire frame:
 *          [0x4567][opcode][header_len][data_len][header][data][0x89AB]
 *        All multi-byte fields are big-endian.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef BASE_PACKET_H
#define BASE_PACKET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Wire-format magic constants
 * ====================================================================== */
#define AE_PACKET_MAGIC_HEAD  ((uint16_t)0x4567u)  /* 17767  */
#define AE_PACKET_MAGIC_TAIL  ((uint16_t)0x89ABu)  /* unsigned view of -30293 */

/* =========================================================================
 * ae_base_packet_t
 * ====================================================================== */
typedef struct {
    int      opcode;
    bool     should_encrypt;       /* default true                         */
    bool     should_build_header;  /* build header automatically on send   */
    bool     use_dispatch_key;     /* select dispatch vs. session key      */

    uint8_t *header;               /* heap-allocated; NULL means empty     */
    size_t   header_len;

    uint8_t *data;                 /* heap-allocated; NULL means empty     */
    size_t   data_len;
} ae_base_packet_t;

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

/* Allocates a packet with the given opcode. */
ae_base_packet_t *ae_packet_create(int opcode);

/*
 * Allocates a packet with the given opcode and immediately builds a
 * PacketHead header stamped with 'client_sequence'.
 */
ae_base_packet_t *ae_packet_create_with_seq(int opcode, int client_sequence);

/*
 * Allocates a packet with the given opcode.
 * If 'build_header' is true, a header will be built before the packet
 * is sent (caller must call ae_packet_build_header() manually or rely
 * on the send path to do so).
 */
ae_base_packet_t *ae_packet_create_ex(int opcode, bool build_header);

/*
 * Releases all heap memory owned by the packet.
 */
void ae_packet_free(ae_base_packet_t *pkt);

/* =========================================================================
 * Header
 * ====================================================================== */

/*
 * Serialises a PacketHead with the given client_sequence_id and the
 * current wall-clock millisecond timestamp, then stores it as the
 * packet header.
 *
 * If the packet already has a header and client_sequence == 0 the
 * existing header is kept unchanged.
 *
 * Returns 'pkt' for chaining.
 */
ae_base_packet_t *ae_packet_build_header(ae_base_packet_t *pkt,
                                         int               client_sequence);

/*
 * Replaces the packet header with a copy of 'header' (length 'len').
 * Frees any previously stored header.
 */
void ae_packet_set_header(ae_base_packet_t *pkt,
                          const uint8_t    *header,
                          size_t            len);

/* =========================================================================
 * Payload
 * ====================================================================== */

/*
 * Replaces the packet data with a copy of 'data' (length 'len').
 * Frees any previously stored data.
 */
void ae_packet_set_data(ae_base_packet_t *pkt,
                        const uint8_t    *data,
                        size_t            len);

/*
 * Serialises 'proto_msg' (a nanopb message) into bytes and stores the
 * result as the packet data.
 *
 * 'encode_fn' is the nanopb-generated encode function for the message:
 *   bool (*encode_fn)(pb_ostream_t *, const pb_field_t *, const void *)
 * 'fields' is the nanopb field descriptor array for the message type.
 *
 * Returns true on success, false on serialisation failure.
 */
bool ae_packet_set_proto(ae_base_packet_t *pkt,
                         const void       *fields,
                         const void       *proto_msg);

/* =========================================================================
 * Wire serialisation
 *
 * Assembles the complete on-wire frame into a heap-allocated buffer:
 *
 *   [uint16 0x4567]
 *   [uint16 opcode]
 *   [uint16 header_len]
 *   [uint32 data_len]
 *   [header_len bytes: header]
 *   [data_len   bytes: data]
 *   [uint16 0x89AB]
 *
 * All multi-byte fields are big-endian.
 *
 * On success, writes the buffer pointer to '*out' and its length to
 * '*out_len', then returns true. Caller must free '*out'.
 * Returns false on allocation failure.
 * ====================================================================== */
bool ae_packet_build(const ae_base_packet_t *pkt,
                     uint8_t               **out,
                     size_t                 *out_len);

/* =========================================================================
 * Low-level write helpers (big-endian, exposed for reuse)
 * ====================================================================== */

/* Writes a big-endian uint16 to buf[0..1]. buf must have >= 2 bytes. */
static inline void ae_write_uint16(uint8_t *buf, uint16_t v) {
    buf[0] = (uint8_t)((v >> 8) & 0xFFu);
    buf[1] = (uint8_t)( v       & 0xFFu);
}

/* Writes a big-endian uint32 to buf[0..3]. buf must have >= 4 bytes. */
static inline void ae_write_uint32(uint8_t *buf, uint32_t v) {
    buf[0] = (uint8_t)((v >> 24) & 0xFFu);
    buf[1] = (uint8_t)((v >> 16) & 0xFFu);
    buf[2] = (uint8_t)((v >>  8) & 0xFFu);
    buf[3] = (uint8_t)( v        & 0xFFu);
}

#ifdef __cplusplus
}
#endif

#endif /* BASE_PACKET_H */
