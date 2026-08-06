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
 * @file packet_handler.c
 *
 * @brief Base packet handler shared data.
 *        Defines the AE_EMPTY_BYTE_ARRAY sentinel used by all handlers
 *        when a zero-length header or payload must be passed without
 *        allocating a buffer.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include "packet_handler.h"

/* =========================================================================
 * AE_EMPTY_BYTE_ARRAY
 *
 * A zero-length read-only byte array used as a non-NULL sentinel in
 * place of a heap allocation when header or payload data is absent.
 * ====================================================================== */
const uint8_t AE_EMPTY_BYTE_ARRAY[0] = {};
