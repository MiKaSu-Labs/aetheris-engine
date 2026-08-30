/*
 *
 *           ______ _______ _    _ ______ _____  _____  _____
 *     /\   |  ____|__   __| |  | |  ____|  __ \|_   _|/ ____|
 *    /  \  | |__     | |  | |__| | |__  | |__) | | | | (___
 *   / /\ \ |  __|    | |  |  __  |  __| |  _  /  | |  \___ \
 *  / ____ \| |____   | |  | |  | | |____| | \ \ _| |_ ____) |
 * /_/    \_\______|  |_|  |_|  |_|______|_|  \_\_____|_____/
 *
 *                                                             v1.0
 * @name Aetheris
 *
 * @author dkitagawa
 *
 * @file debug_constants.h
 *
 * @brief Global debug configuration flags for runtime diagnostics.
 *
 * Provides toggles for logging abilities, Lua scripts, quest events,
 * and missing resources. Also defines a critical security flag that
 * controls unsafe authentication behavior for development purposes.
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

/* =========================================================================
 * Mutable debug flags
 *
 * All flags default to false. They are intended to be set once at process
 * startup (e.g., from command-line arguments or a debug config block)
 * before any worker threads are started. Modifying them after worker
 * threads are running is not thread-safe; no synchronization is performed
 * on reads in hot-path logging checks.
 * ====================================================================== */

/** When true, log ability invocations as they are processed. */
extern bool ae_log_abilities;

/** When true, log each Lua script execution. */
extern bool ae_log_lua_scripts;

/** When true, log each quest start event. */
extern bool ae_log_quest_start;

/** When true, log a warning for every ability ID that has no handler. */
extern bool ae_log_missing_abilities;

/** When true, log a warning for every Lua script path that cannot be found. */
extern bool ae_log_missing_lua_scripts;

/** When true, log a warning for every ability whose handler function is
 *  not registered in the ability handler table. */
extern bool ae_log_missing_ability_handlers;

/* =========================================================================
 * Immutable security flag
 * ====================================================================== */

/**
 * @brief Controls whether client-supplied authentication tokens are accepted.
 *
 * When true, the server accepts any token and UID pair sent by the client
 * as valid, and overwrites the stored account token with the client value.
 * This bypasses all authentication checks and allows full account
 * impersonation and session hijacking.
 *
 * @warning MUST remain false in any production or publicly reachable
 * deployment. Enable only in an isolated development environment with no
 * real account data. There is no runtime guard preventing misuse once
 * this flag is true.
 *
 * Defined as a const bool, matching the extern-variable pattern used for
 * the other flags in this file, rather than a preprocessor macro: it has
 * a real type, is visible to a debugger, and can be inspected by address.
 * Because it has external linkage, the compiler can only fold it to a
 * compile-time constant and eliminate dependent branches within this
 * translation unit (or under whole-program/LTO optimization); other
 * translation units resolve its value at link time like any other extern
 * and do not get automatic dead-branch elimination from this alone.
 */
extern const bool AE_ACCEPT_CLIENT_TOKEN;

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_CONSTANTS_H */
