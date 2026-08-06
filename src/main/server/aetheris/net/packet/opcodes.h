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
 * @file opcodes.h
 *
 * @brief Opcode descriptor for packet handlers.
 *        Defines ae_opcode_def_t, a compile-time descriptor that
 *        associates a handler with its opcode value and an optional
 *        disabled flag that prevents the handler from being registered
 *        with the packet router.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef OPCODES_H
#define OPCODES_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * ae_opcode_def_t
 *
 * Packet/handler opcode descriptor. Embed one of these in every packet
 * handler struct to declare which opcode it handles and whether it
 * should be skipped during handler registration.
 * ====================================================================== */
typedef struct {
    int  value;     /* Opcode for the packet/handler.                    */
    bool disabled;  /* HANDLER ONLY , set true to skip registration.     */
} ae_opcode_def_t;

/* =========================================================================
 * AE_OPCODE_DEF , convenience macro for declaring an opcode descriptor
 * with defaults matching the annotation:
 *   value    = 0        (must be supplied)
 *   disabled = false
 *
 * Usage:
 *   static const ae_opcode_def_t my_handler_opcode =
 *       AE_OPCODE_DEF(.value = OpCode_GetPlayerTokenReq);
 *
 *   static const ae_opcode_def_t disabled_handler_opcode =
 *       AE_OPCODE_DEF(.value = OpCode_Foo, .disabled = true);
 * ====================================================================== */
#define AE_OPCODE_DEF(...)           \
    {                                \
        .value    = 0,               \
        .disabled = false,           \
        __VA_ARGS__                  \
    }

#ifdef __cplusplus
}
#endif

#endif /* OPCODES_H */
