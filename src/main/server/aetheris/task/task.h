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
 * @file task.h
 *
 * @brief Task scheduler annotation descriptor.
 *        Defines the ae_task_def_t struct used to declare scheduled tasks
 *        with cron expressions, trigger names, and execution flags.
 *
 *        Cron expression field order: Second Minute Hour Day Month Week Year
 *          Seconds  : 0-59
 *          Minute   : 0-59
 *          Hour     : 0-23
 *          Day      : 1-31
 *          Month    : 1-12
 *          Week     : 1-7
 *          Year     : optional
 *
 *        Use '*' in a field to mean 'every unit'.
 *        Use 'a,b' to specify multiple values (e.g. '15,30' for every 15
 *        and every 30 minutes). Use '?' for unspecified day/week fields.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef TASK_H
#define TASK_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * ae_task_def_t
 *
 * Describes a scheduled task. Mirrors the @Task annotation fields.
 * Populate statically with AE_TASK_DEF() and register via the task
 * manager (task_manager.h).
 * ====================================================================== */
typedef struct {
    const char *task_name;                    /* human-readable task name         */
    const char *task_cron_expression;         /* cron expression string           */
    const char *trigger_name;                 /* scheduler trigger identifier     */
    bool        execute_immediately_after_reset; /* run once right after reset    */
    bool        execute_immediately;          /* run once at server startup       */
} ae_task_def_t;

/* =========================================================================
 * AE_TASK_DEF , convenience macro for declaring a task descriptor with
 * the same defaults as the original annotation:
 *   task_name                       = "NO_NAME"
 *   task_cron_expression            = "0 0 0 0 0 ?"
 *   trigger_name                    = "NO_NAME"
 *   execute_immediately_after_reset = false
 *   execute_immediately             = false
 *
 * Usage:
 *   static ae_task_def_t my_task = AE_TASK_DEF(
 *       .task_name             = "DailyReset",
 *       .task_cron_expression  = "0 0 0 * * ?",
 *       .trigger_name          = "DailyResetTrigger",
 *       .execute_immediately   = true
 *   );
 * ====================================================================== */
#define AE_TASK_DEF(...)                          \
    {                                             \
        .task_name                       = "NO_NAME", \
        .task_cron_expression            = "0 0 0 0 0 ?", \
        .trigger_name                    = "NO_NAME", \
        .execute_immediately_after_reset = false,  \
        .execute_immediately             = false,  \
        __VA_ARGS__                               \
    }

#ifdef __cplusplus
}
#endif

#endif /* TASK_H */
