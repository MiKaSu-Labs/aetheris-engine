#ifndef DEBUG_CONSTANTS_H
#define DEBUG_CONSTANTS_H

/*
 * debug_constants.h
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * Mutable debug flags  (Java: public static boolean)
 * ---------------------------------------------------------------------- */
extern bool ae_log_abilities;
extern bool ae_log_lua_scripts;
extern bool ae_log_quest_start;
extern bool ae_log_missing_abilities;
extern bool ae_log_missing_lua_scripts;
extern bool ae_log_missing_ability_handlers;

/* -------------------------------------------------------------------------
 * Immutable flag  (Java: public static final boolean)
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
