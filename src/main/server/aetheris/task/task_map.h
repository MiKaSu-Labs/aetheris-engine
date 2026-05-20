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
 * @file task_map.h
 *
 * @brief Task registry and cron scheduler for the Aetheris server.
 *        Manages registration, unregistration, pause, resume, and
 *        cancellation of scheduled tasks. Drives the cron execution
 *        loop and handles post-reset task re-registration.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef TASK_MAP_H
#define TASK_MAP_H

#include <stdbool.h>
#include <stddef.h>

#include "task_handler.h"  /* ae_task_handler_t, ae_task_def_t */

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * ae_task_map_t , opaque task registry handle
 * ====================================================================== */
typedef struct ae_task_map ae_task_map_t;

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

/*
 * Allocates and returns a new task map.
 * If 'scan' is true, the registry is populated immediately by scanning
 * all statically registered task descriptors (see ae_task_map_register_all).
 * Caller must free with ae_task_map_free().
 */
ae_task_map_t *ae_task_map_create(bool scan);

/*
 * Frees all resources owned by the task map, unregistering and disabling
 * every active task before release.
 */
void ae_task_map_free(ae_task_map_t *map);

/*
 * Returns the global task map instance from the active game server.
 * Equivalent to GameServer.getTaskMap().
 */
ae_task_map_t *ae_task_map_get_instance(void);

/* =========================================================================
 * Registration
 * ====================================================================== */

/*
 * Registers a task handler under 'task_name'.
 * Schedules it with the cron expression in handler->def, calls
 * on_enable(), and immediately calls execute(NULL) if
 * handler->def.execute_immediately is true.
 * Returns the map for chaining.
 */
ae_task_map_t *ae_task_map_register(
    ae_task_map_t     *map,
    const char        *task_name,
    ae_task_handler_t *handler);

/*
 * Unregisters and disables a task handler.
 * Removes it from the scheduler, calls on_disable(), and removes it
 * from the internal registry.
 * Returns the map for chaining.
 */
ae_task_map_t *ae_task_map_unregister(
    ae_task_map_t     *map,
    ae_task_handler_t *handler);

/* =========================================================================
 * Scheduler control
 * ====================================================================== */

/*
 * Pauses the scheduler job identified by 'task_name'.
 * Returns true on success, false on failure.
 */
bool ae_task_map_pause(ae_task_map_t *map, const char *task_name);

/*
 * Resumes a previously paused scheduler job identified by 'task_name'.
 * Returns true on success, false on failure.
 */
bool ae_task_map_resume(ae_task_map_t *map, const char *task_name);

/*
 * Cancels and fully unregisters the task identified by 'task_name'.
 * Returns true on success, false if the task was not found or removal
 * failed.
 */
bool ae_task_map_cancel(ae_task_map_t *map, const char *task_name);

/* =========================================================================
 * Reset
 *
 * Unregisters all tasks, runs every task flagged
 * execute_immediately_after_reset via ae_task_restart_execute(), then
 * re-registers all tasks under their original names.
 * ====================================================================== */
void ae_task_map_reset_now(ae_task_map_t *map);

/* =========================================================================
 * Introspection
 * ====================================================================== */

/*
 * Returns the handler registered under 'task_name', or NULL if not found.
 */
ae_task_handler_t *ae_task_map_get_handler(
    ae_task_map_t *map,
    const char    *task_name);

/*
 * Writes pointers to all registered handlers into 'out_handlers'
 * (caller-supplied array of at least 'out_cap' pointers).
 * Returns the number of handlers written.
 * Pass out_handlers=NULL and out_cap=0 to query the count only.
 */
size_t ae_task_map_get_handlers(
    ae_task_map_t      *map,
    ae_task_handler_t **out_handlers,
    size_t              out_cap);

#ifdef __cplusplus
}
#endif

#endif /* TASK_MAP_H */