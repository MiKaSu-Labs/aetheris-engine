/*
 * 
 *           ______ _______ _    _ ______ _____  _____  _____ 
 *     /\   |  ____|__   __| |  | |  ____|  __ \|_   _|/ ____|
 *    /  \  | |__     | |  | |__| | |__  | |__) | | | | (___  
 *   / /\ \ |  __|    | |  |  __  |  __| |  _  /  | | |  \___ \ 
 *  / ____ \| |____   | |  |  |  | | |____| | \ \ _| |_ ____) |
 * /_/    \_\______|  |_|  |_|  |_|______|_|  \_\_____|_____/ 
 * 
 *                                                                                     v1.0
 * @name Aetheris
 *
 * @author dkitagawa
 *
 * @file ae_task.h
 *
 * @brief Scheduled-task registry with a cron-driven dispatch loop.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_TASK_H
#define AE_TASK_H

#include <aetheris/ae_cron.h>
#include <aetheris/ae_error.h>
#include <aetheris/ae_types.h>

/* ============================================================ */

#define AE_TASK_NAME_MAX 64

/*
 * A task handler is the C equivalent of a single Java "-Task"
 * implementation: three lifecycle callbacks.  `user_data` is passed
 * verbatim to every callback and is owned by the application.
 */
typedef struct ae_task_handler {
    void *user_data;

    /* Invoked when the task is (re)registered. May be NULL. */
    void (*on_enable)(void *user_data);

    /* Invoked when the task is unregistered. May be NULL. */
    void (*on_disable)(void *user_data);

    /* Invoked each time the schedule fires. May be NULL. */
    void (*on_execute)(void *user_data, ae_s64 due_time);
} ae_task_handler_t;

/*
 * A registered scheduled task.  Handlers/vtable details are kept in
 * the owning registry; this struct is descriptive and is returned by
 * `ae_task_map_get()` to preserve task metadata.
 */
typedef struct ae_task {
    char name[AE_TASK_NAME_MAX];
    ae_cron_expr_t cron;
    ae_s64 next_run;      /* epoch seconds (UTC); -1 when not armed. */
    ae_bool execute_now;  /* run immediately on registration.       */
    ae_bool after_reset;  /* run once after a full reset.           */
    ae_bool paused;       /* paused tasks do not fire.              */
    ae_bool registered;
} ae_task_t;

/*
 * The registry: a growable array of task slots.  Allocate with
 * `ae_task_map_init` (malloc-backed) or a fixed caller buffer, and
 * release with `ae_task_map_destroy`.
 *
 * Lookups are by name over a plain array, matching the small task
 * counts of a game server.
 */
typedef struct ae_task_map {
    ae_task_t *tasks;
    ae_task_handler_t *handlers;
    ae_size count;
    ae_size capacity;
    ae_bool owns_memory;
} ae_task_map_t;

/**
 * ae_task_map_init - initialise a task registry.
 * @map: registry to initialise.
 * @tasks: optional caller buffer for task metadata (may be NULL).
 * @handlers: optional caller buffer for handlers (may be NULL).
 * @capacity: capacity of the caller buffers, in entries (used only
 *            when both buffers are non-NULL).
 *
 * When caller buffers are supplied the map never allocates.  When
 * either buffer is NULL the map grows itself as needed.
 *
 * Returns AE_OK on success.
 */
ae_error_t ae_task_map_init(ae_task_map_t *map,
                            ae_task_t *tasks,
                            ae_task_handler_t *handlers,
                            ae_size capacity);

/**
 * ae_task_map_destroy - release a task registry.
 * @map: registry to tear down.  Safe on a zero-initialised struct.
 */
void ae_task_map_destroy(ae_task_map_t *map);

/**
 * ae_task_map_register - add or replace a scheduled task.
 * @map: registry.
 * @name: unique task name (copied).
 * @cron: Quartz cron expression, e.g. "0 0 0 * * ?".
 * @now: current epoch seconds (UTC), to arm the first run.
 * @handler: callbacks and user data (copied by value).
 * @exec_now: run the handler immediately during registration.
 * @after_reset: run the handler once when the registry is reset.
 *
 * Re-registering an existing name replaces its schedule and handler.
 *
 * Returns AE_OK, AE_ERR_INVALID_ARG for a bad expression or name, or
 * AE_ERR_OUT_OF_MEMORY when the map must grow and cannot.
 */
ae_error_t ae_task_map_register(ae_task_map_t *map,
                                const char *name,
                                const char *cron,
                                ae_s64 now,
                                const ae_task_handler_t *handler,
                                ae_bool exec_now,
                                ae_bool after_reset);

/**
 * ae_task_map_unregister - remove a task and disable its handler.
 * @map: registry.
 * @name: task name.
 *
 * Returns AE_OK or AE_ERR_NOT_FOUND.
 */
ae_error_t ae_task_map_unregister(ae_task_map_t *map, const char *name);

/**
 * ae_task_map_pause - pause a task without unregistering it.
 * @map: registry.
 * @name: task name.
 *
 * Paused tasks never fire until resumed.  Returns AE_OK or
 * AE_ERR_NOT_FOUND.
 */
ae_error_t ae_task_map_pause(ae_task_map_t *map, const char *name);

/**
 * ae_task_map_resume - resume a paused task.
 * @map: registry.
 * @name: task name.
 *
 * Returns AE_OK or AE_ERR_NOT_FOUND.
 */
ae_error_t ae_task_map_resume(ae_task_map_t *map, const char *name);

/**
 * ae_task_map_cancel - unregister a task by name.
 * @map: registry.
 * @name: task name.
 *
 * Equivalent to unregister and returns AE_OK or AE_ERR_NOT_FOUND.
 */
ae_error_t ae_task_map_cancel(ae_task_map_t *map, const char *name);

/**
 * ae_task_map_get - fetch a registered task by name.
 * @map: registry.
 * @name: task name.
 *
 * Returns a pointer to the task metadata, or NULL if not found.  The
 * pointer stays valid until the map is destroyed or the task is
 * unregistered/resized.
 */
const ae_task_t *ae_task_map_get(const ae_task_map_t *map,
                                 const char *name);

/**
 * ae_task_map_get_handler - fetch the handler of a registered task.
 * @map: registry.
 * @name: task name.
 *
 * Returns NULL when not found.
 */
ae_task_handler_t *ae_task_map_get_handler(ae_task_map_t *map,
                                           const char *name);

/**
 * ae_task_map_count - number of registered tasks.
 * @map: registry.
 */
ae_size ae_task_map_count(const ae_task_map_t *map);

/**
 * ae_task_map_tick - fire every task whose schedule is due.
 * @map: registry.
 * @now: current epoch seconds (UTC).
 *
 * Iterates registered, non-paused tasks and invokes their handler
 * whenever `now >= next_run`, then arms the following run.  Returns
 * the number of tasks that fired.
 */
ae_size ae_task_map_tick(ae_task_map_t *map, ae_s64 now);

/**
 * ae_task_map_reset - unregister everything, run after-reset tasks,
 * then re-register the survivors.
 * @map: registry.
 * @now: current epoch seconds (UTC).
 *
 * Runs handlers flagged `after_reset` exactly once each, then arms
 * every surviving task fresh from `now`.  Returns AE_OK.
 */
ae_error_t ae_task_map_reset(ae_task_map_t *map, ae_s64 now);

/* ============================================================ */

#endif /* AE_TASK_H */