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
 * @file announcement_task.h
 *
 * @brief Scheduled announcement broadcast task.
 *        Fires every minute (cron: "0 * * * * ?"), checks all active
 *        announcement config items marked as tick-based, increments
 *        their interval counters, and broadcasts any that have reached
 *        their configured interval threshold to all online players.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef ANNOUNCEMENT_TASK_H
#define ANNOUNCEMENT_TASK_H

#include "task_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

/*
 * Allocates and returns the announcement task handler, pre-populated
 * with its cron descriptor and vtable.
 * Register with ae_task_map_register() to activate it.
 * Free with announcement_task_free() when no longer needed.
 */
ae_task_handler_t *announcement_task_create(void);

/*
 * Releases all resources owned by the announcement task handler.
 */
void announcement_task_free(ae_task_handler_t *handler);

#ifdef __cplusplus
}
#endif

#endif /* ANNOUNCEMENT_TASK_H */
