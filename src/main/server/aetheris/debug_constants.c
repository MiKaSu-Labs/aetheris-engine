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
 * @file debug_constants.c
 *
 * @brief Defines runtime debug flags and security-sensitive debug options.
 *
 * Implements global debug toggles used for logging abilities, Lua scripts,
 * quests, and missing resource diagnostics. Also defines a critical security
 * flag controlling whether client-provided authentication tokens are accepted.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include "debug_constants.h"

/* -------------------------------------------------------------------------
 * Mutable debug flags - all default to false
 * ---------------------------------------------------------------------- */
bool ae_log_abilities                  = false;
bool ae_log_lua_scripts                = false;
bool ae_log_quest_start                = false;
bool ae_log_missing_abilities          = false;
bool ae_log_missing_lua_scripts        = false;
bool ae_log_missing_ability_handlers   = false;

/* -------------------------------------------------------------------------
 * Immutable flag - must remain false in production
 * ---------------------------------------------------------------------- */
const bool AE_ACCEPT_CLIENT_TOKEN = false;
