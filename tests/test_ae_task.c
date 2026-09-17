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
 * @file test_ae_task.c
 *
 * @brief Tests for the scheduled-task registry.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>
#include <string.h>

#include <aetheris/ae_task.h>

#include "ae_test.h"

/* ============================================================ */

/* Shared counters for the test handlers. */
static ae_u32 g_enabled;
static ae_u32 g_disabled;
static ae_u32 g_fired;
static ae_s64 g_last_due;
static ae_bool g_extern_ctx_ok;

static void handler_enable(void *user_data)
{
    (void)user_data;
    g_enabled++;
}

static void handler_disable(void *user_data)
{
    (void)user_data;
    g_disabled++;
}

static void handler_execute(void *user_data, ae_s64 due_time)
{
    const ae_u32 *tag = user_data;
    g_fired++;
    g_last_due = due_time;
    if (tag && *tag == 77) {
        g_extern_ctx_ok = true;
    }
}

static void reset_counters(void)
{
    g_enabled = 0;
    g_disabled = 0;
    g_fired = 0;
    g_last_due = -1;
    g_extern_ctx_ok = false;
}

/* T_SEP16_0000 = 2026-09-16 00:00:00 UTC. */
#define T0 1789516800LL

/* ============================================================ */

static void test_register_and_immediate(void)
{
    ae_task_map_t map;
    ae_task_handler_t h;
    const ae_task_t *task;

    reset_counters();
    AE_TEST_ASSERT(AE_OK == ae_task_map_init(&map, NULL, NULL, 0));

    h.user_data = NULL;
    h.on_enable = handler_enable;
    h.on_disable = handler_disable;
    h.on_execute = handler_execute;

    AE_TEST_ASSERT(AE_OK ==
        ae_task_map_register(&map, "daily", "0 0 0 * * ?", T0,
                             &h, true, false));

    AE_TEST_ASSERT(g_fired == 1);   /* immediate */
    AE_TEST_ASSERT(g_enabled == 1);
    AE_TEST_ASSERT(ae_task_map_count(&map) == 1);

    task = ae_task_map_get(&map, "daily");
    AE_TEST_ASSERT(task != NULL);
    AE_TEST_ASSERT(task->execute_now);
    AE_TEST_ASSERT(task->next_run > T0);

    ae_task_map_destroy(&map);
}

static void test_tick_fires_due(void)
{
    ae_task_map_t map;
    ae_task_handler_t h;

    reset_counters();
    AE_TEST_ASSERT(AE_OK == ae_task_map_init(&map, NULL, NULL, 0));

    h.user_data = NULL;
    h.on_enable = handler_enable;
    h.on_disable = handler_disable;
    h.on_execute = handler_execute;

    /* Every minute at second 0. */
    AE_TEST_ASSERT(AE_OK ==
        ae_task_map_register(&map, "minute", "0 * * * * ?", T0,
                             &h, false, false));

    /* One minute later is the first fire. */
    AE_TEST_ASSERT(ae_task_map_tick(&map, T0) == 0);
    AE_TEST_ASSERT(ae_task_map_tick(&map, T0 + 59) == 0);
    AE_TEST_ASSERT(ae_task_map_tick(&map, T0 + 60) == 1);
    AE_TEST_ASSERT(g_fired == 1);
    AE_TEST_ASSERT(g_last_due == T0 + 60);

    /* Next fire is one further minute away. */
    AE_TEST_ASSERT(ae_task_map_tick(&map, T0 + 60 + 59) == 0);
    AE_TEST_ASSERT(ae_task_map_tick(&map, T0 + 120) == 1);
    AE_TEST_ASSERT(g_fired == 2);

    ae_task_map_destroy(&map);
}

static void test_pause_resume(void)
{
    ae_task_map_t map;
    ae_task_handler_t h;

    reset_counters();
    AE_TEST_ASSERT(AE_OK == ae_task_map_init(&map, NULL, NULL, 0));

    h.user_data = NULL;
    h.on_enable = handler_enable;
    h.on_disable = handler_disable;
    h.on_execute = handler_execute;

    AE_TEST_ASSERT(AE_OK ==
        ae_task_map_register(&map, "minute", "0 * * * * ?", T0,
                             &h, false, false));

    AE_TEST_ASSERT(AE_OK == ae_task_map_pause(&map, "minute"));
    AE_TEST_ASSERT(ae_task_map_tick(&map, T0 + 3600) == 0);
    AE_TEST_ASSERT(g_fired == 0);

    AE_TEST_ASSERT(AE_OK == ae_task_map_resume(&map, "minute"));
    AE_TEST_ASSERT(ae_task_map_tick(&map, T0 + 3600 + 60) == 1);
    AE_TEST_ASSERT(g_fired == 1);

    AE_TEST_ASSERT(AE_ERR_NOT_FOUND == ae_task_map_pause(&map, "nope"));
    AE_TEST_ASSERT(AE_ERR_NOT_FOUND == ae_task_map_resume(&map, "nope"));

    ae_task_map_destroy(&map);
}

static void test_unregister_cancel(void)
{
    ae_task_map_t map;
    ae_task_handler_t h;

    reset_counters();
    AE_TEST_ASSERT(AE_OK == ae_task_map_init(&map, NULL, NULL, 0));

    h.user_data = NULL;
    h.on_enable = handler_enable;
    h.on_disable = handler_disable;
    h.on_execute = handler_execute;

    AE_TEST_ASSERT(AE_OK ==
        ae_task_map_register(&map, "a", "0 * * * * ?", T0, &h, false, false));
    AE_TEST_ASSERT(AE_OK ==
        ae_task_map_register(&map, "b", "0 * * * * ?", T0, &h, false, false));

    AE_TEST_ASSERT(AE_OK == ae_task_map_unregister(&map, "a"));
    AE_TEST_ASSERT(g_disabled == 1);
    AE_TEST_ASSERT(ae_task_map_count(&map) == 1);

    AE_TEST_ASSERT(AE_OK == ae_task_map_cancel(&map, "b"));
    AE_TEST_ASSERT(ae_task_map_count(&map) == 0);

    AE_TEST_ASSERT(AE_ERR_NOT_FOUND ==
                   ae_task_map_unregister(&map, "a"));

    ae_task_map_destroy(&map);
}

static void test_user_data_passthrough(void)
{
    ae_task_map_t map;
    ae_task_handler_t h;
    ae_u32 tag = 77;

    reset_counters();
    AE_TEST_ASSERT(AE_OK == ae_task_map_init(&map, NULL, NULL, 0));

    h.user_data = &tag;
    h.on_enable = NULL;
    h.on_disable = NULL;
    h.on_execute = handler_execute;

    AE_TEST_ASSERT(AE_OK ==
        ae_task_map_register(&map, "ctx", "0 * * * * ?", T0,
                             &h, true, false));
    AE_TEST_ASSERT(g_extern_ctx_ok);

    ae_task_map_destroy(&map);
}

static void test_reset_executes_afterreset(void)
{
    ae_task_map_t map;
    ae_task_handler_t h;

    reset_counters();
    AE_TEST_ASSERT(AE_OK == ae_task_map_init(&map, NULL, NULL, 0));

    h.user_data = NULL;
    h.on_enable = handler_enable;
    h.on_disable = handler_disable;
    h.on_execute = handler_execute;

    AE_TEST_ASSERT(AE_OK ==
        ae_task_map_register(&map, "resetme", "0 * * * * ?", T0,
                             &h, false, true));

    ae_task_map_tick(&map, T0 + 60);   /* one normal fire */
    AE_TEST_ASSERT(g_fired == 1);

    AE_TEST_ASSERT(AE_OK == ae_task_map_reset(&map, T0 + 120));

    /* The after-reset task ran once, then the task re-enabled. */
    AE_TEST_ASSERT(g_fired == 2);
    AE_TEST_ASSERT(g_enabled == 2);

    const ae_task_t *task = ae_task_map_get(&map, "resetme");
    AE_TEST_ASSERT(task != NULL);
    AE_TEST_ASSERT(!task->after_reset);

    ae_task_map_destroy(&map);
}

static void test_caller_buffer(void)
{
    ae_task_map_t map;
    ae_task_t tasks[4];
    ae_task_handler_t handlers[4];
    ae_task_handler_t h;

    reset_counters();
    AE_TEST_ASSERT(AE_OK ==
        ae_task_map_init(&map, tasks, handlers, 4));

    h.user_data = NULL;
    h.on_enable = handler_enable;
    h.on_disable = handler_disable;
    h.on_execute = handler_execute;

    AE_TEST_ASSERT(AE_OK ==
        ae_task_map_register(&map, "slot1", "0 * * * * ?", T0,
                             &h, false, false));
    AE_TEST_ASSERT(ae_task_map_count(&map) == 1);

    AE_TEST_ASSERT(map.tasks == tasks);

    ae_task_map_destroy(&map);
}

static void test_null_and_invalid(void)
{
    ae_task_map_t map;
    ae_task_handler_t h;

    reset_counters();
    AE_TEST_ASSERT(AE_OK == ae_task_map_init(&map, NULL, NULL, 0));

    AE_TEST_ASSERT(AE_ERR_INVALID_ARG ==
                   ae_task_map_register(NULL, "x", "0 * * * * ?", T0,
                                        &h, false, false));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG ==
                   ae_task_map_register(&map, NULL, "0 * * * * ?", T0,
                                        &h, false, false));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG ==
                   ae_task_map_register(&map, "x", "0 * * * * ?", T0,
                                        NULL, false, false));
    AE_TEST_ASSERT(AE_ERR_BAD_DATA ==
                   ae_task_map_register(&map, "x", "garbage", T0,
                                        &h, false, false));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG ==
                   ae_task_map_register(&map, "x", NULL, T0,
                                        &h, false, false));

    AE_TEST_ASSERT(ae_task_map_get(NULL, "x") == NULL);
    AE_TEST_ASSERT(ae_task_map_get(&map, "x") == NULL);
    AE_TEST_ASSERT(ae_task_map_get_handler(&map, "x") == NULL);
    AE_TEST_ASSERT(ae_task_map_count(NULL) == 0);
    AE_TEST_ASSERT(ae_task_map_tick(NULL, T0) == 0);

    ae_task_map_destroy(&map);
}

/* ============================================================ */

static const ae_test_case_t test_cases[] = {
    {"Register and execute immediately", test_register_and_immediate},
    {"Tick fires due tasks", test_tick_fires_due},
    {"Pause and resume", test_pause_resume},
    {"Unregister and cancel", test_unregister_cancel},
    {"User data passthrough", test_user_data_passthrough},
    {"Reset executes after-reset", test_reset_executes_afterreset},
    {"Caller-provided buffers", test_caller_buffer},
    {"NULL and invalid arguments", test_null_and_invalid},
};

int main(void)
{
    ae_u32 failed = AE_TEST_SUITE(test_cases);

    printf("task: %u assertions, %u failed\n",
           ae_test_run_count(), ae_test_fail_count());

    return failed != 0 ? 1 : 0;
}

/* ============================================================ */