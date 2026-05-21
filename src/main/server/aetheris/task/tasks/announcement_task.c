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
 * @file announcement_task.c
 *
 * @brief Scheduled announcement broadcast task implementation.
 *        Tracks per-template interval counters in a thread-safe map,
 *        selects announcements that have reached their threshold, and
 *        broadcasts them to all online players via the announcement system.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include "announcement_task.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "aetheris.h"
#include "announcement_system.h"
#include "game_server.h"

/* =========================================================================
 * ae_announcement_task_impl_t
 *
 * Private implementation data embedded in handler->impl.
 *
 * interval_map : tracks how many ticks have elapsed per template id
 *                since the last broadcast for that template.
 * mutex        : guards interval_map to match the 'synchronized' keyword
 *                on the original execute() method.
 * ====================================================================== */
typedef struct {
    int           *template_ids;   /* parallel arrays , heap-allocated */
    int           *tick_counts;
    size_t         map_len;
    size_t         map_cap;
    pthread_mutex_t mutex;
} ae_announcement_task_impl_t;

/* =========================================================================
 * Internal helpers
 * ====================================================================== */

/* Returns the current tick count for template_id, or -1 if not found. */
static int _get_tick(ae_announcement_task_impl_t *impl, int template_id) {
    for (size_t i = 0; i < impl->map_len; i++) {
        if (impl->template_ids[i] == template_id)
            return impl->tick_counts[i];
    }
    return -1;
}

/* Sets the tick count for template_id, inserting if not present. */
static void _set_tick(ae_announcement_task_impl_t *impl,
                      int template_id, int value) {
    for (size_t i = 0; i < impl->map_len; i++) {
        if (impl->template_ids[i] == template_id) {
            impl->tick_counts[i] = value;
            return;
        }
    }
    /* Insert new entry */
    if (impl->map_len >= impl->map_cap) {
        impl->map_cap = impl->map_cap ? impl->map_cap * 2 : 16;
        impl->template_ids = realloc(impl->template_ids,
                                     impl->map_cap * sizeof(int));
        impl->tick_counts  = realloc(impl->tick_counts,
                                     impl->map_cap * sizeof(int));
    }
    impl->template_ids[impl->map_len] = template_id;
    impl->tick_counts[impl->map_len]  = value;
    impl->map_len++;
}

/* =========================================================================
 * vtable implementations
 * ====================================================================== */

static void _on_enable(ae_task_handler_t *self) {
    (void)self;
    ae_logger_debug(ae_logger, "[Task] Announcement task enabled.");
}

static void _on_disable(ae_task_handler_t *self) {
    (void)self;
    ae_logger_debug(ae_logger, "[Task] Announcement task disabled.");
}

static int _execute(ae_task_handler_t *self, void *ctx) {
    (void)ctx;
    ae_announcement_task_impl_t *impl =
        (ae_announcement_task_impl_t *)self->impl;

    pthread_mutex_lock(&impl->mutex);

    time_t now = time(NULL);

    /* Fetch all tick-based announcement config items that are currently
     * active (begin_time <= now < end_time). */
    ae_announcement_system_t *ann_sys =
        game_server_get_announcement_system(ae_game_server);

    ae_announce_config_item_t **active = NULL;
    size_t active_len = 0;
    announcement_system_get_tick_items(ann_sys, now, &active, &active_len);

    /* Increment tick counters for every active template */
    for (size_t i = 0; i < active_len; i++) {
        int tid   = active[i]->template_id;
        int ticks = _get_tick(impl, tid);
        _set_tick(impl, tid, ticks < 0 ? 1 : ticks + 1);
    }

    /* Collect items whose tick count has reached their interval */
    ae_announce_config_item_t **to_send =
        malloc(active_len * sizeof(ae_announce_config_item_t *));
    size_t to_send_len = 0;

    for (size_t i = 0; i < active_len; i++) {
        int tid      = active[i]->template_id;
        int ticks    = _get_tick(impl, tid);
        int interval = active[i]->interval;
        if (ticks >= interval) {
            to_send[to_send_len++] = active[i];
        }
    }

    /* Broadcast and reset interval counters for sent items */
    announcement_system_broadcast(ann_sys, to_send, to_send_len);
    ae_logger_trace(ae_logger,
        "Broadcast %zu announcement(s) to all online players.", to_send_len);

    for (size_t i = 0; i < to_send_len; i++)
        _set_tick(impl, to_send[i]->template_id, 0);

    free(to_send);
    free(active);

    pthread_mutex_unlock(&impl->mutex);
    return 0;
}

/* =========================================================================
 * announcement_task_create()
 * ====================================================================== */
ae_task_handler_t *announcement_task_create(void) {
    ae_announcement_task_impl_t *impl =
        calloc(1, sizeof(ae_announcement_task_impl_t));
    if (!impl) return NULL;

    pthread_mutex_init(&impl->mutex, NULL);

    ae_task_handler_t *handler = calloc(1, sizeof(ae_task_handler_t));
    if (!handler) {
        free(impl);
        return NULL;
    }

    handler->impl       = impl;
    handler->execute    = _execute;
    handler->on_enable  = _on_enable;
    handler->on_disable = _on_disable;

    /* Task descriptor , mirrors the @Task annotation */
    handler->def = (ae_task_def_t) AE_TASK_DEF(
        .task_name              = "Announcement",
        .task_cron_expression   = "0 * * * * ?",
        .trigger_name           = "AnnouncementTrigger"
    );

    return handler;
}

/* =========================================================================
 * announcement_task_free()
 * ====================================================================== */
void announcement_task_free(ae_task_handler_t *handler) {
    if (!handler) return;

    ae_announcement_task_impl_t *impl =
        (ae_announcement_task_impl_t *)handler->impl;

    if (impl) {
        pthread_mutex_destroy(&impl->mutex);
        free(impl->template_ids);
        free(impl->tick_counts);
        free(impl);
    }

    free(handler);
}
