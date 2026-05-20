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
 * @file task_handler.c
 *
 * @brief Base task handler shared logic.
 *        Provides ae_task_restart_execute(), the only non-abstract
 *        behaviour shared by all task handler implementations.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include "task_handler.h"

/* =========================================================================
 * ae_task_restart_execute()
 *
 * Invokes the handler's execute function with a NULL context, re-running
 * the task outside its normal cron schedule.
 * ====================================================================== */
int ae_task_restart_execute(ae_task_handler_t *handler) {
    if (!handler || !handler->execute) return -1;
    return handler->execute(handler, NULL);
}
