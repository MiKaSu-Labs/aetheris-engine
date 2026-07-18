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
 * @file moon_card_task.h
 *
 * @brief Scheduled moon card daily reward task.
 *        Fires once per day at midnight (cron: "0 0 0 * * ?").
 *        Iterates all online players and grants the daily moon card
 *        reward to those with an active moon card subscription.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef MOON_CARD_TASK_H
#define MOON_CARD_TASK_H

#include "task_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

/*
 * Allocates and returns the moon card task handler, pre-populated with
 * its cron descriptor and vtable.
 * Register with ae_task_map_register() to activate it.
 * Free with moon_card_task_free() when no longer needed.
 */
ae_task_handler_t *moon_card_task_create(void);

/*
 * Releases all resources owned by the moon card task handler.
 */
void moon_card_task_free(ae_task_handler_t *handler);

#ifdef __cplusplus
}
#endif

#endif /* MOON_CARD_TASK_H */
