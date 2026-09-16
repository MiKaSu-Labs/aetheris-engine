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
 * @file ae_base_packet.h
 *
 * @brief Outgoing frame builder for the protocol wire format.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_BASE_PACKET_H
#define AE_BASE_PACKET_H

#include <aetheris/ae_buffer.h>

/* ============================================================ */

#define AE_BASE_PACKET_CONST1 0x4567
#define AE_BASE_PACKET_CONST2 0x89ab

/**
 * struct ae_base_packet - a packet awaiting wire framing.
 * @opcode: numeric protocol opcode.
 * @should_encrypt: true when the payload must be XOR-obfuscated.
 * @use_dispatch_key: true when the dispatch RSA key is required.
 * @should_build_header: true when a protocol header must be generated.
 * @header: raw header bytes (may be NULL when @header_len is zero).
 * @header_len: number of header bytes.
 * @data: raw payload bytes (may be NULL when @data_len is zero).
 * @data_len: number of payload bytes.
 */
typedef struct ae_base_packet {
    int opcode;
    ae_bool should_encrypt;
    ae_bool use_dispatch_key;
    ae_bool should_build_header;
    const ae_u8 *header;
    ae_size header_len;
    const ae_u8 *data;
    ae_size data_len;
} ae_base_packet_t;

/**
 * ae_base_packet_build - serialize a packet into its wire frame.
 * @packet: the packet to serialize.
 * @out: receives a freshly allocated big-endian frame; NULL on failure.
 * @out_len: receives the frame length.
 *
 * The frame layout is:
 *
 *     u16  0x4567 (start marker, big-endian)
 *     u16  opcode
 *     u16  header length
 *     u32  data length
 *     ...  header bytes
 *     ...  data bytes
 *     u16  0x89ab (end marker, big-endian)
 *
 * The caller must free @out with free(). On error @out is NULL.
 */
ae_error_t ae_base_packet_build(const ae_base_packet_t *packet,
                                ae_u8 **out, ae_size *out_len);

#endif /* AE_BASE_PACKET_H */