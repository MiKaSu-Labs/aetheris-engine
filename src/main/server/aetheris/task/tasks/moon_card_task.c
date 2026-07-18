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
 * @file moon_card_task.c
 *
 * @brief Scheduled moon card daily reward task implementation.
 *        On each midnight trigger, iterates all online players and
 *        calls player_get_today_moon_card() for those with an active
 *        moon card subscription.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include "moon_card_task.h"

#include <pthread.h>
#include <stdlib.h>

#include "aetheris.h"
#include "game_server.h"
#include "player.h"

/* =========================================================================
 * vtable implementations
 * ====================================================================== */

static void _on_enable(ae_task_handler_t *self) {
    (void)self;
    ae_logger_debug(ae_logger, "[Task] MoonCard task enabled.");
}

static void _on_disable(ae_task_handler_t *self) {
    (void)self;
    ae_logger_debug(ae_logger, "[Task] MoonCard task disabled.");
}

/*
 * Fires at midnight every day (cron: "0 0 0 * * ?", 24-hour system).
 * Iterates every player slot, and for each player that is both online
 * and holds an active moon card subscription, grants the day's reward.
 */
static pthread_mutex_t _execute_mutex = PTHREAD_MUTEX_INITIALIZER;

static int _execute(ae_task_handler_t *self, void *ctx) {
    (void)self;
    (void)ctx;

    pthread_mutex_lock(&_execute_mutex);

    ae_player_iter_t *iter = game_server_player_iter(ae_game_server);
    ae_player_t      *player;

    while ((player = ae_player_iter_next(iter))) {
        if (ae_player_is_online(player) && ae_player_in_moon_card(player)) {
            ae_player_get_today_moon_card(player);
        }
    }

    ae_player_iter_free(iter);

    pthread_mutex_unlock(&_execute_mutex);
    return 0;
}

/* =========================================================================
 * moon_card_task_create()
 * ====================================================================== */
ae_task_handler_t *moon_card_task_create(void) {
    ae_task_handler_t *handler = calloc(1, sizeof(ae_task_handler_t));
    if (!handler) return NULL;

    handler->impl       = NULL;   /* no per-instance state needed */
    handler->execute    = _execute;
    handler->on_enable  = _on_enable;
    handler->on_disable = _on_disable;

    /* Task descriptor , mirrors the @Task annotation.
     * Fixed time period: 0:0:0 every day (24-hour system). */
    handler->def = (ae_task_def_t) AE_TASK_DEF(
        .task_name            = "MoonCard",
        .task_cron_expression = "0 0 0 * * ?",
        .trigger_name         = "MoonCardTrigger"
    );

    return handler;
}

/* =========================================================================
 * moon_card_task_free()
 * ====================================================================== */
void moon_card_task_free(ae_task_handler_t *handler) {
    if (!handler) return;
    /* No heap-allocated impl data to release. */
    free(handler);
}
