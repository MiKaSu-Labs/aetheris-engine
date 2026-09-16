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
 * @file ae_packet_opcodes_utils.h
 *
 * @brief Opcode-to-name lookup and high-frequency-loop packet detection.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_PACKET_OPCODES_UTILS_H
#define AE_PACKET_OPCODES_UTILS_H

#include <aetheris/ae_types.h>

/**
 * ae_get_opcode_name - return the human-readable name for an opcode.
 * @opcode: the numeric opcode value.
 *
 * Returns "UNKNOWN" for @opcode <= 0 or for any opcode not present in
 * the protocol table.
 */
const char *ae_get_opcode_name(int opcode);

/**
 * ae_is_loop_packet - check whether @opcode is a high-frequency
 *                      "loop" packet.
 * @opcode: the numeric opcode value.
 *
 * Loop packets are sent every tick (e.g. ping, time sync, satiation
 * updates) and are typically excluded from verbose logging.
 */
ae_bool ae_is_loop_packet(int opcode);

#endif /* AE_PACKET_OPCODES_UTILS_H */
