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
 * @file task_map.c
 *
 * @brief Task registry and cron scheduler implementation.
 *        Manages the full lifecycle of scheduled tasks including
 *        registration, cron-driven execution, pause, resume,
 *        cancellation, and post-reset re-registration.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include "task_map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aetheris.h"
#include "cron.h"        /* ae_cron_t, ae_cron_parse(), ae_cron_next() */
#include "game_server.h" /* game_server_get_task_map()                 */

/* =========================================================================
 * Internal entry
 * ====================================================================== */
typedef struct {
    char               *name;     /* heap-allocated task name  */
    ae_task_handler_t  *handler;  /* non-owning pointer        */
    bool                paused;
} _task_entry_t;

/* =========================================================================
 * ae_task_map_t definition
 * ====================================================================== */
struct ae_task_map {
    _task_entry_t *tasks;        /* registered tasks           */
    size_t         tasks_len;
    size_t         tasks_cap;

    _task_entry_t *after_reset;  /* tasks flagged for post-reset re-execution */
    size_t         after_reset_len;
    size_t         after_reset_cap;
};

/* =========================================================================
 * Internal helpers
 * ====================================================================== */

static _task_entry_t *_find_entry(ae_task_map_t *map, const char *name) {
    for (size_t i = 0; i < map->tasks_len; i++) {
        if (strcmp(map->tasks[i].name, name) == 0)
            return &map->tasks[i];
    }
    return NULL;
}

static _task_entry_t *_find_entry_by_handler(ae_task_map_t     *map,
                                              ae_task_handler_t *handler) {
    for (size_t i = 0; i < map->tasks_len; i++) {
        if (map->tasks[i].handler == handler)
            return &map->tasks[i];
    }
    return NULL;
}

static void _remove_entry(ae_task_map_t *map, const char *name) {
    for (size_t i = 0; i < map->tasks_len; i++) {
        if (strcmp(map->tasks[i].name, name) == 0) {
            free(map->tasks[i].name);
            /* shift remaining entries left */
            memmove(&map->tasks[i], &map->tasks[i + 1],
                    (map->tasks_len - i - 1) * sizeof(_task_entry_t));
            map->tasks_len--;
            return;
        }
    }
}

static void _push_entry(ae_task_map_t     *map,
                        const char        *name,
                        ae_task_handler_t *handler) {
    if (map->tasks_len >= map->tasks_cap) {
        map->tasks_cap = map->tasks_cap ? map->tasks_cap * 2 : 8;
        map->tasks = realloc(map->tasks,
                             map->tasks_cap * sizeof(_task_entry_t));
    }
    map->tasks[map->tasks_len].name    = strdup(name);
    map->tasks[map->tasks_len].handler = handler;
    map->tasks[map->tasks_len].paused  = false;
    map->tasks_len++;
}

static void _push_after_reset(ae_task_map_t     *map,
                               ae_task_handler_t *handler) {
    for (size_t i = 0; i < map->after_reset_len; i++) {
        if (map->after_reset[i].handler == handler) return; /* already present */
    }
    if (map->after_reset_len >= map->after_reset_cap) {
        map->after_reset_cap = map->after_reset_cap ? map->after_reset_cap * 2 : 8;
        map->after_reset = realloc(map->after_reset,
                                   map->after_reset_cap * sizeof(_task_entry_t));
    }
    map->after_reset[map->after_reset_len].name    = NULL; /* not owned here */
    map->after_reset[map->after_reset_len].handler = handler;
    map->after_reset[map->after_reset_len].paused  = false;
    map->after_reset_len++;
}

/* =========================================================================
 * ae_task_map_create()
 * ====================================================================== */
ae_task_map_t *ae_task_map_create(bool scan) {
    ae_task_map_t *map = calloc(1, sizeof(ae_task_map_t));
    if (!map) return NULL;

    if (scan) ae_task_map_register_all(map);

    return map;
}

/* =========================================================================
 * ae_task_map_free()
 * ====================================================================== */
void ae_task_map_free(ae_task_map_t *map) {
    if (!map) return;

    /* Unregister and disable all active tasks */
    for (size_t i = 0; i < map->tasks_len; i++) {
        if (map->tasks[i].handler && map->tasks[i].handler->on_disable)
            map->tasks[i].handler->on_disable(map->tasks[i].handler);
        free(map->tasks[i].name);
    }

    free(map->tasks);
    free(map->after_reset);
    free(map);
}

/* =========================================================================
 * ae_task_map_get_instance()
 * ====================================================================== */
ae_task_map_t *ae_task_map_get_instance(void) {
    return game_server_get_task_map(ae_game_server);
}

/* =========================================================================
 * ae_task_map_register()
 * ====================================================================== */
ae_task_map_t *ae_task_map_register(ae_task_map_t     *map,
                                    const char        *task_name,
                                    ae_task_handler_t *handler) {
    if (!map || !task_name || !handler) return map;

    /* Parse and register the cron expression with the cron engine */
    ae_cron_t *cron = ae_cron_parse(handler->def.task_cron_expression);
    if (!cron) {
        ae_logger_error(ae_logger,
            "Failed to parse cron expression '%s' for task '%s'.",
            handler->def.task_cron_expression, task_name);
        return map;
    }
    ae_cron_register(cron, task_name,
                     handler->def.trigger_name,
                     handler);

    /* Store in the registry */
    _push_entry(map, task_name, handler);

    /* Execute immediately if flagged */
    if (handler->def.execute_immediately) {
        int rc = ae_task_execute(handler, NULL);
        if (rc != 0) {
            ae_logger_error(ae_logger,
                "Immediate execution of task '%s' failed: %d", task_name, rc);
        }
    }

    ae_task_on_enable(handler);
    return map;
}

/* =========================================================================
 * ae_task_map_unregister()
 * ====================================================================== */
ae_task_map_t *ae_task_map_unregister(ae_task_map_t     *map,
                                      ae_task_handler_t *handler) {
    if (!map || !handler) return map;

    _task_entry_t *entry = _find_entry_by_handler(map, handler);
    if (!entry) return map;

    const char *name = entry->name;

    /* Remove from cron engine */
    ae_cron_delete_job(name);

    /* Remove from registry */
    _remove_entry(map, name);

    ae_task_on_disable(handler);
    return map;
}

/* =========================================================================
 * ae_task_map_pause()
 * ====================================================================== */
bool ae_task_map_pause(ae_task_map_t *map, const char *task_name) {
    if (!map || !task_name) return false;

    _task_entry_t *entry = _find_entry(map, task_name);
    if (!entry) return false;

    if (ae_cron_pause_job(task_name) != 0) {
        ae_logger_error(ae_logger, "Failed to pause task '%s'.", task_name);
        return false;
    }

    entry->paused = true;
    return true;
}

/* =========================================================================
 * ae_task_map_resume()
 * ====================================================================== */
bool ae_task_map_resume(ae_task_map_t *map, const char *task_name) {
    if (!map || !task_name) return false;

    _task_entry_t *entry = _find_entry(map, task_name);
    if (!entry) return false;

    if (ae_cron_resume_job(task_name) != 0) {
        ae_logger_error(ae_logger, "Failed to resume task '%s'.", task_name);
        return false;
    }

    entry->paused = false;
    return true;
}

/* =========================================================================
 * ae_task_map_cancel()
 * ====================================================================== */
bool ae_task_map_cancel(ae_task_map_t *map, const char *task_name) {
    if (!map || !task_name) return false;

    _task_entry_t *entry = _find_entry(map, task_name);
    if (!entry) return false;

    ae_task_map_unregister(map, entry->handler);
    return true;
}

/* =========================================================================
 * ae_task_map_reset_now()
 * ====================================================================== */
void ae_task_map_reset_now(ae_task_map_t *map) {
    if (!map) return;

    /*
     * Snapshot the current handler list before unregistration so
     * we can re-register them after the reset cycle.
     */
    size_t snapshot_len = map->tasks_len;
    ae_task_handler_t **snapshot =
        malloc(snapshot_len * sizeof(ae_task_handler_t *));
    char **snapshot_names = malloc(snapshot_len * sizeof(char *));

    for (size_t i = 0; i < snapshot_len; i++) {
        snapshot[i]       = map->tasks[i].handler;
        snapshot_names[i] = strdup(map->tasks[i].name);
    }

    /* Unregister all tasks */
    for (size_t i = 0; i < snapshot_len; i++)
        ae_task_map_unregister(map, snapshot[i]);

    /* Run all after-reset handlers */
    for (size_t i = 0; i < map->after_reset_len; i++) {
        int rc = ae_task_restart_execute(map->after_reset[i].handler);
        if (rc != 0) {
            ae_logger_error(ae_logger,
                "After-reset execution failed for a task: %d", rc);
        }
    }
    map->after_reset_len = 0;

    /* Re-register all tasks */
    for (size_t i = 0; i < snapshot_len; i++) {
        ae_task_map_register(map, snapshot_names[i], snapshot[i]);
        free(snapshot_names[i]);
    }

    free(snapshot);
    free(snapshot_names);
}

/* =========================================================================
 * ae_task_map_get_handler()
 * ====================================================================== */
ae_task_handler_t *ae_task_map_get_handler(ae_task_map_t *map,
                                           const char    *task_name) {
    if (!map || !task_name) return NULL;
    _task_entry_t *entry = _find_entry(map, task_name);
    return entry ? entry->handler : NULL;
}

/* =========================================================================
 * ae_task_map_get_handlers()
 * ====================================================================== */
size_t ae_task_map_get_handlers(ae_task_map_t      *map,
                                ae_task_handler_t **out_handlers,
                                size_t              out_cap) {
    if (!map) return 0;
    if (!out_handlers || out_cap == 0) return map->tasks_len;

    size_t count = map->tasks_len < out_cap ? map->tasks_len : out_cap;
    for (size_t i = 0; i < count; i++)
        out_handlers[i] = map->tasks[i].handler;
    return count;
}

/* =========================================================================
 * ae_task_map_register_all()
 *
 * Replaces the Reflections-based class scan. Each translation unit that
 * defines a task handler calls ae_task_map_register_all() via a static
 * registration table populated at startup, equivalent to the reflector
 * scanning for @Task-annotated classes.
 *
 * The registration table is populated in task_registry.c (generated or
 * hand-maintained), which lists every ae_task_handler_t * factory.
 * ====================================================================== */
void ae_task_map_register_all(ae_task_map_t *map) {
    extern ae_task_handler_t **ae_task_registry;
    extern size_t              ae_task_registry_len;

    for (size_t i = 0; i < ae_task_registry_len; i++) {
        ae_task_handler_t *handler = ae_task_registry[i];
        ae_task_map_register(map, handler->def.task_name, handler);

        if (handler->def.execute_immediately_after_reset)
            _push_after_reset(map, handler);
    }

    /* Start the cron engine */
    if (ae_cron_start() != 0)
        ae_logger_error(ae_logger, "Failed to start the task scheduler.");
}