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
 * @file debug_constants.h
 *
 * @brief Global debug configuration flags for runtime diagnostics.
 *
 * Provides toggles for logging abilities, Lua scripts, quest events,
 * and missing resources. Also defines critical debug-only security flags
 * that control unsafe server behavior for development and testing purposes.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef DEBUG_CONSTANTS_H
#define DEBUG_CONSTANTS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * Mutable debug flags
 * ---------------------------------------------------------------------- */
extern bool ae_log_abilities;
extern bool ae_log_lua_scripts;
extern bool ae_log_quest_start;
extern bool ae_log_missing_abilities;
extern bool ae_log_missing_lua_scripts;
extern bool ae_log_missing_ability_handlers;

/* -------------------------------------------------------------------------
 * Immutable flag
 *
 * WARNING: DANGEROUS SETTING. DO NOT ENABLE UNLESS YOU KNOW WHAT YOU ARE
 * DOING. This allows the *client* to send *ANY* token and UID pair to the
 * server. The server will then accept the token and UID pair as valid, and
 * set the account's token to the client-specified one. This can allow for
 * IMPERSONATION and HIJACKING of accounts/servers.
 * ---------------------------------------------------------------------- */
extern const bool AE_ACCEPT_CLIENT_TOKEN;

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_CONSTANTS_H */
