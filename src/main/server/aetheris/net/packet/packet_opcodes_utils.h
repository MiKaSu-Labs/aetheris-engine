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
 * @file packet_opcodes_utils.h
 *
 * @brief Packet opcode lookup and diagnostic utilities.
 *        Provides opcode-to-name resolution (a compile-time table
 *        replacing reflection-based field scanning), loop-packet
 *        classification for log filtering, and a packet ID dump
 *        utility for external tooling.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef PACKET_OPCODES_UTILS_H
#define PACKET_OPCODES_UTILS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Loop packet classification
 *
 * Loop packets are high-frequency, low-significance packets (heartbeats,
 * per-tick satiation updates, path queries) that are excluded from
 * verbose packet logging to avoid flooding the log output.
 * ====================================================================== */

/*
 * Returns true if 'opcode' is classified as a loop packet.
 */
bool ae_opcode_is_loop_packet(int opcode);

/* =========================================================================
 * Opcode name resolution
 *
 * Replaces the reflection-based Int2ObjectMap<String> built by scanning
 * PacketOpcodes fields at class-load time. The equivalent table here is
 * generated at compile time directly from the opcode definitions and
 * sorted by value for binary-search lookup.
 * ====================================================================== */

/*
 * Returns the constant name matching 'opcode' (e.g. "PingReq"),
 * or "UNKNOWN" if 'opcode' is not a recognised value or is <= 0.
 * The returned string is statically allocated and must not be freed.
 */
const char *ae_opcode_get_name(int opcode);

/* =========================================================================
 * Diagnostics
 * ====================================================================== */

/*
 * Writes every known opcode (value > 0) to './PacketIds_<version>.json'
 * as a JSON object sorted by numeric value ascending, in the form:
 *   { "<opcode>": "<name>", ... }
 * 'version' is taken from ae_version.
 * Returns 0 on success, negative ae_error_t on failure.
 */
int ae_opcode_dump_packet_ids(void);

#ifdef __cplusplus
}
#endif

#endif /* PACKET_OPCODES_UTILS_H */
