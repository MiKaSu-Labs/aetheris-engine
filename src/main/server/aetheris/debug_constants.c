/*
 * debug_constants.c
 */

#include "debug_constants.h"

/* -------------------------------------------------------------------------
 * Mutable debug flags — all default to false
 * ---------------------------------------------------------------------- */
bool ae_log_abilities                  = false;
bool ae_log_lua_scripts                = false;
bool ae_log_quest_start                = false;
bool ae_log_missing_abilities          = false;
bool ae_log_missing_lua_scripts        = false;
bool ae_log_missing_ability_handlers   = false;

/* -------------------------------------------------------------------------
 * Immutable flag — must remain false in production
 * ---------------------------------------------------------------------- */
const bool AE_ACCEPT_CLIENT_TOKEN = false;
