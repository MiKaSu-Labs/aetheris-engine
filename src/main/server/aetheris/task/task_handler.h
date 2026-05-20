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
 * @file task_handler.h
 *
 * @brief Base task handler interface for the Aetheris scheduler.
 *        Defines the ae_task_handler_t vtable that all scheduled tasks
 *        must implement. Mirrors the abstract TaskHandler class and the
 *        Quartz Job interface it extended.
 *
 *        Task state is persisted across executions by the task manager,
 *        equivalent to the @PersistJobDataAfterExecution behaviour.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef TASK_HANDLER_H
#define TASK_HANDLER_H

#include <stdbool.h>

#include "task.h"   /* ae_task_def_t */

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Forward declaration
 * ====================================================================== */
typedef struct ae_task_handler ae_task_handler_t;

/* =========================================================================
 * ae_task_handler_t , the task handler vtable
 *
 * Every scheduled task provides a populated instance of this struct.
 * The task manager calls these function pointers at the appropriate
 * points in the task lifecycle.
 *
 * All function pointers are required and must be non-NULL.
 * ====================================================================== */
struct ae_task_handler {

    /*
     * Executes the task. Called by the scheduler on each cron trigger.
     * Returns 0 on success, negative ae_error_t on failure.
     * 'ctx' carries per-execution context data supplied by the scheduler;
     * may be NULL when called via ae_task_restart_execute().
     */
    int (*execute)(
        ae_task_handler_t *self,
        void              *ctx);

    /*
     * Called when the task is enabled and registered with the scheduler.
     */
    void (*on_enable)(ae_task_handler_t *self);

    /*
     * Called when the task is disabled and removed from the scheduler.
     */
    void (*on_disable)(ae_task_handler_t *self);

    /* The task descriptor (cron expression, name, flags, etc.) */
    ae_task_def_t def;

    /* Implementation-specific data; opaque to the scheduler. */
    void *impl;
};

/* =========================================================================
 * ae_task_restart_execute()
 *
 * Re-runs the task outside its normal cron schedule by calling
 * execute(self, NULL). Equivalent to restartExecute() which called
 * execute(null) on the Quartz JobExecutionContext.
 *
 * Returns 0 on success, negative ae_error_t on failure.
 * ====================================================================== */
int ae_task_restart_execute(ae_task_handler_t *handler);

/* =========================================================================
 * Convenience dispatch macros
 * ====================================================================== */

#define ae_task_execute(h, ctx)  (h)->execute((h), (ctx))
#define ae_task_on_enable(h)     (h)->on_enable((h))
#define ae_task_on_disable(h)    (h)->on_disable((h))

#ifdef __cplusplus
}
#endif

#endif /* TASK_HANDLER_H */
