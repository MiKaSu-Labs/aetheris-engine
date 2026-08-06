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
 * @file packet_handler.h
 *
 * @brief Base packet handler interface.
 *        Defines ae_packet_handler_t, the vtable that all inbound packet
 *        handlers must implement. Each handler is associated with an
 *        opcode via an embedded ae_opcode_def_t and is registered with
 *        the packet router at startup unless its descriptor marks it
 *        as disabled.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H

#include <stddef.h>
#include <stdint.h>

#include "game_session.h"  /* ae_game_session_t */
#include "opcodes.h"       /* ae_opcode_def_t   */

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Forward declaration
 * ====================================================================== */
typedef struct ae_packet_handler ae_packet_handler_t;

/* =========================================================================
 * Shared empty-byte-array sentinel
 *
 * Used in place of NULL when a zero-length header or payload must be
 * passed to a handler without allocating. Never written to.
 * ====================================================================== */
extern const uint8_t AE_EMPTY_BYTE_ARRAY[0];

/* =========================================================================
 * ae_packet_handler_t , the packet handler vtable
 *
 * Embed an instance of this struct (populated with a handle function
 * pointer and an opcode descriptor) in each concrete handler.
 * Register with the packet router; the router checks opcode_def.disabled
 * before adding the handler to its dispatch table.
 * ====================================================================== */
struct ae_packet_handler {

    /*
     * Processes an inbound packet.
     *
     * 'session'      , the game session that received the packet.
     * 'header'       , raw serialised PacketHead bytes (may be
     *                  AE_EMPTY_BYTE_ARRAY when no header is present).
     * 'header_len'   , length of 'header' in bytes.
     * 'payload'      , raw serialised protobuf payload bytes (may be
     *                  AE_EMPTY_BYTE_ARRAY when no payload is present).
     * 'payload_len'  , length of 'payload' in bytes.
     *
     * Returns 0 on success, negative ae_error_t on failure.
     */
    int (*handle)(
        ae_packet_handler_t  *self,
        ae_game_session_t    *session,
        const uint8_t        *header,
        size_t                header_len,
        const uint8_t        *payload,
        size_t                payload_len);

    /* Opcode this handler is bound to, and its disabled flag. */
    ae_opcode_def_t opcode_def;

    /* Implementation-specific data; opaque to the packet router. */
    void *impl;
};

/* =========================================================================
 * Convenience dispatch macro
 * ====================================================================== */
#define ae_packet_handle(h, session, hdr, hdr_len, pay, pay_len) \
    (h)->handle((h), (session), (hdr), (hdr_len), (pay), (pay_len))

#ifdef __cplusplus
}
#endif

#endif /* PACKET_HANDLER_H */
