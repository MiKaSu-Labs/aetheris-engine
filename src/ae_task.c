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
 * @file ae_task.c
 *
 * @brief Scheduled-task registry implementation.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdlib.h>
#include <string.h>

#include "aetheris/ae_task.h"

/* ============================================================ */

/* Locate a task slot by name.  Returns index or -1. */
static ae_s32 find_task(const ae_task_map_t *map, const char *name)
{
    ae_size i;

    if (!map || !name) {
        return -1;
    }

    for (i = 0; i < map->count; ++i) {
        if (strcmp(map->tasks[i].name, name) == 0) {
            return (ae_s32)i;
        }
    }

    return -1;
}

/* Ensure capacity for at least one more task when memory-backed. */
static ae_error_t grow(ae_task_map_t *map)
{
    ae_size new_cap;
    ae_task_t *new_tasks;
    ae_task_handler_t *new_handlers;

    if (map->count < map->capacity) {
        return AE_OK;
    }

    if (!map->owns_memory) {
        return AE_ERR_OUT_OF_MEMORY;
    }

    new_cap = map->capacity ? map->capacity * 2 : 8;
    new_tasks = realloc(map->tasks, new_cap * sizeof(ae_task_t));
    if (!new_tasks) {
        return AE_ERR_OUT_OF_MEMORY;
    }
    map->tasks = new_tasks;

    new_handlers = realloc(map->handlers,
                           new_cap * sizeof(ae_task_handler_t));
    if (!new_handlers) {
        return AE_ERR_OUT_OF_MEMORY;
    }
    map->handlers = new_handlers;

    map->capacity = new_cap;
    return AE_OK;
}

/* ============================================================ */

ae_error_t ae_task_map_init(ae_task_map_t *map,
                            ae_task_t *tasks,
                            ae_task_handler_t *handlers,
                            ae_size capacity)
{
    if (!map) {
        return AE_ERR_INVALID_ARG;
    }

    if (tasks && handlers) {
        map->tasks = tasks;
        map->handlers = handlers;
        map->capacity = capacity;
        map->owns_memory = false;
    } else {
        map->tasks = NULL;
        map->handlers = NULL;
        map->capacity = 0;
        map->owns_memory = true;
    }
    map->count = 0;

    return AE_OK;
}

void ae_task_map_destroy(ae_task_map_t *map)
{
    if (!map) {
        return;
    }

    if (map->owns_memory) {
        free(map->tasks);
        free(map->handlers);
    }

    map->tasks = NULL;
    map->handlers = NULL;
    map->count = 0;
    map->capacity = 0;
    map->owns_memory = false;
}

ae_error_t ae_task_map_register(ae_task_map_t *map,
                                const char *name,
                                const char *cron,
                                ae_s64 now,
                                const ae_task_handler_t *handler,
                                ae_bool exec_now,
                                ae_bool after_reset)
{
    ae_error_t err;
    ae_s32 idx;
    ae_s64 first_run;
    ae_cron_expr_t expr;

    if (!map || !name || !handler) {
        return AE_ERR_INVALID_ARG;
    }

    /* Validate the schedule before touching the map. */
    err = ae_cron_expr_parse(&expr, cron);
    if (err != AE_OK) {
        return err;
    }

    first_run = ae_cron_expr_next(&expr, now);
    if (first_run < 0) {
        return AE_ERR_INVALID_ARG;
    }

    idx = find_task(map, name);
    if (idx < 0) {
        err = grow(map);
        if (err != AE_OK) {
            return err;
        }
        idx = (ae_s32)map->count++;
    } else {
        /* Replacing an existing task re-enables it. */
        if (map->handlers[idx].on_disable) {
            map->handlers[idx].on_disable(map->handlers[idx].user_data);
        }
    }

    (void)strncpy(map->tasks[idx].name, name, AE_TASK_NAME_MAX - 1);
    map->tasks[idx].name[AE_TASK_NAME_MAX - 1] = '\0';
    map->tasks[idx].cron = expr;
    map->tasks[idx].next_run = first_run;
    map->tasks[idx].execute_now = exec_now;
    map->tasks[idx].after_reset = after_reset;
    map->tasks[idx].paused = false;
    map->tasks[idx].registered = true;

    map->handlers[idx] = *handler;

    if (exec_now && map->handlers[idx].on_execute) {
        map->handlers[idx].on_execute(map->handlers[idx].user_data, now);
    }
    if (map->handlers[idx].on_enable) {
        map->handlers[idx].on_enable(map->handlers[idx].user_data);
    }

    return AE_OK;
}

ae_error_t ae_task_map_unregister(ae_task_map_t *map, const char *name)
{
    ae_s32 idx = find_task(map, name);

    if (idx < 0) {
        return AE_ERR_NOT_FOUND;
    }

    if (map->handlers[idx].on_disable) {
        map->handlers[idx].on_disable(map->handlers[idx].user_data);
    }

    if ((ae_size)idx + 1 < map->count) {
        memmove(&map->tasks[idx], &map->tasks[idx + 1],
                (map->count - (ae_size)idx - 1) * sizeof(ae_task_t));
        memmove(&map->handlers[idx], &map->handlers[idx + 1],
                (map->count - (ae_size)idx - 1) *
                    sizeof(ae_task_handler_t));
    }
    map->count--;

    return AE_OK;
}

ae_error_t ae_task_map_pause(ae_task_map_t *map, const char *name)
{
    ae_s32 idx = find_task(map, name);

    if (idx < 0) {
        return AE_ERR_NOT_FOUND;
    }

    map->tasks[idx].paused = true;
    return AE_OK;
}

ae_error_t ae_task_map_resume(ae_task_map_t *map, const char *name)
{
    ae_s32 idx = find_task(map, name);

    if (idx < 0) {
        return AE_ERR_NOT_FOUND;
    }

    map->tasks[idx].paused = false;
    return AE_OK;
}

ae_error_t ae_task_map_cancel(ae_task_map_t *map, const char *name)
{
    return ae_task_map_unregister(map, name);
}

const ae_task_t *ae_task_map_get(const ae_task_map_t *map,
                                 const char *name)
{
    ae_s32 idx = find_task(map, name);

    if (idx < 0) {
        return NULL;
    }

    return &map->tasks[idx];
}

ae_task_handler_t *ae_task_map_get_handler(ae_task_map_t *map,
                                           const char *name)
{
    ae_s32 idx = find_task(map, name);

    if (idx < 0) {
        return NULL;
    }

    return &map->handlers[idx];
}

ae_size ae_task_map_count(const ae_task_map_t *map)
{
    if (!map) {
        return 0;
    }
    return map->count;
}

ae_size ae_task_map_tick(ae_task_map_t *map, ae_s64 now)
{
    ae_size i;
    ae_size fired = 0;

    if (!map) {
        return 0;
    }

    for (i = 0; i < map->count; ++i) {
        ae_task_t *task = &map->tasks[i];
        ae_task_handler_t *handler;

        if (!task->registered || task->paused || task->next_run < 0) {
            continue;
        }
        if (now < task->next_run) {
            continue;
        }

        /* Fire, then arm the following run. */
        handler = &map->handlers[i];
        if (handler->on_execute) {
            handler->on_execute(handler->user_data, now);
        }
        task->next_run = ae_cron_expr_next(&task->cron, now);
        fired++;
    }

    return fired;
}

ae_error_t ae_task_map_reset(ae_task_map_t *map, ae_s64 now)
{
    ae_size i;

    if (!map) {
        return AE_ERR_INVALID_ARG;
    }

    /* Disable everything first, mirroring a full reset. */
    for (i = 0; i < map->count; ++i) {
        if (map->handlers[i].on_disable) {
            map->handlers[i].on_disable(map->handlers[i].user_data);
        }
    }

    /* Run after-reset tasks once. */
    for (i = 0; i < map->count; ++i) {
        if (map->tasks[i].after_reset &&
            map->handlers[i].on_execute) {
            map->handlers[i].on_execute(map->handlers[i].user_data, now);
        }
    }

    /* Re-arm every surviving task. */
    for (i = 0; i < map->count; ++i) {
        ae_task_t *task = &map->tasks[i];
        ae_s64 first_run;

        task->after_reset = false;
        task->paused = false;
        first_run = ae_cron_expr_next(&task->cron, now);
        task->next_run = first_run;

        if (map->handlers[i].on_enable) {
            map->handlers[i].on_enable(map->handlers[i].user_data);
        }
    }

    return AE_OK;
}