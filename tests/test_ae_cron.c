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
 * @file test_ae_cron.c
 *
 * @brief Tests for the cron expression parser and next-fire time.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>
#include <string.h>

#include <aetheris/ae_cron.h>

#include "ae_test.h"

/* ============================================================ */

/* Verified reference values (epoch seconds, UTC). */
#define T_SEP16_0000 1789516800ULL /* 2026-09-16 00:00:00 */
#define T_SEP16_1000 1789552800ULL /* 2026-09-16 10:00:00 */
#define T_SEP16_1002 1789552920ULL /* 2026-09-16 10:02:00 */
#define T_SEP16_1005 1789553100ULL /* 2026-09-16 10:05:00 */
#define T_SEP16_1028 1789554480ULL /* 2026-09-16 10:28:00 */
#define T_SEP16_1030 1789554600ULL /* 2026-09-16 10:30:00 */
#define T_SEP16_1055 1789556100ULL /* 2026-09-16 10:55:00 */
#define T_SEP16_1100 1789556400ULL /* 2026-09-16 11:00:00 */
#define T_SEP16_1200 1789560000ULL /* 2026-09-16 12:00:00 */
#define T_SEP16_1759 1789581540ULL /* 2026-09-16 17:59:00 */
#define T_SEP16_2300 1789599600ULL /* 2026-09-16 23:00:00 */
#define T_SEP17_0000 1789603200ULL /* 2026-09-17 00:00:00 */
#define T_SEP17_1200 1789646400ULL /* 2026-09-17 12:00:00 */
#define T_SEP18_FRI  1789689600ULL /* 2026-09-18 00:00:00 (Fri) */
#define T_SEP19_SAT  1789776000ULL /* 2026-09-19 00:00:00 (Sat) */
#define T_SEP20_SUN  1789862400ULL /* 2026-09-20 00:00:00 (Sun) */
#define T_SEP02_1200 1788350400ULL /* 2026-09-02 12:00:00 */
#define T_SEP15_1200 1789473600ULL /* 2026-09-15 12:00:00 */
#define T_SEP16_0800 1789545600ULL /* 2026-09-16 08:00:00 */
#define T_SEP16_0900 1789549200ULL /* 2026-09-16 09:00:00 */

static void test_daily_midnight(void)
{
    ae_cron_expr_t expr;
    ae_s64 next;

    AE_TEST_ASSERT(AE_OK == ae_cron_expr_parse(&expr, "0 0 0 * * ?"));

    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_1000);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP17_0000);

    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_2300);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP17_0000);

    /* Same time on the day itself: strictly greater, so the next
     * fire is tomorrow. */
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_0000);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP17_0000);

    AE_TEST_ASSERT(ae_cron_expr_valid(&expr));
}

static void test_every_minute(void)
{
    ae_cron_expr_t expr;
    ae_s64 next;

    AE_TEST_ASSERT(AE_OK == ae_cron_expr_parse(&expr, "0 * * * * ?"));

    /* 10:00:45 -> 10:01:00 */
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_1000 + 45);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP16_1000 + 60);

    /* Exactly on the minute boundary -> one minute later. */
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_1000);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP16_1000 + 60);
}

static void test_daily_noon(void)
{
    ae_cron_expr_t expr;
    ae_s64 next;

    AE_TEST_ASSERT(AE_OK == ae_cron_expr_parse(&expr, "0 0 12 * * ?"));

    /* Before noon -> noon today. */
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_1000);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP16_1200);

    /* After noon -> noon tomorrow. */
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_1200);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP17_1200);
}

static void test_weekday_only(void)
{
    ae_cron_expr_t expr; /* Sunday only. */
    ae_s64 next;

    AE_TEST_ASSERT(AE_OK == ae_cron_expr_parse(&expr, "0 0 0 ? * 1"));

    /* Wed 2026-09-16 -> Sun 2026-09-20. */
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_1000);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP20_SUN);

    /* Sunday itself, before midnight -> next Sunday 00:00 is one day
     * later since the fire time is strictly greater than @from. */
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP20_SUN - 1);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP20_SUN);

    AE_TEST_ASSERT(AE_OK == ae_cron_expr_parse(&expr, "0 0 0 ? * 7")); /* Sat */
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_1000);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP19_SAT);
}

static void test_day_of_month(void)
{
    ae_cron_expr_t expr; /* Noon on the 15th. */
    ae_s64 next;

    AE_TEST_ASSERT(AE_OK == ae_cron_expr_parse(&expr, "0 0 12 15 * ?"));

    /* 2026-09-02 -> 2026-09-15. */
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP02_1200);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP15_1200);
}

static void test_step_and_range(void)
{
    ae_cron_expr_t expr;
    ae_s64 next;

    /* Every 5 minutes (0,5,...,55). */
    AE_TEST_ASSERT(AE_OK == ae_cron_expr_parse(&expr, "0 */5 * * * ?"));
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_1002);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP16_1005);
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_1028);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP16_1030);
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_1055);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP16_1100);

    /* Working hours 9-17. */
    AE_TEST_ASSERT(AE_OK == ae_cron_expr_parse(&expr, "0 0 9-17 * * ?"));
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_1759);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP17_0000 + 9 * 3600);
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_0800);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP16_0900);
}

static void test_both_day_fields_or(void)
{
    ae_cron_expr_t expr;
    ae_s64 next;

    /* 15th OR Fri/Sat. */
    AE_TEST_ASSERT(AE_OK == ae_cron_expr_parse(&expr, "0 0 0 15 * 6-7"));
    next = ae_cron_expr_next(&expr, (ae_s64)T_SEP16_1000);
    AE_TEST_ASSERT(next == (ae_s64)T_SEP18_FRI);
}

static void test_null_and_invalid(void)
{
    ae_cron_expr_t expr;

    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_cron_expr_parse(NULL, "0 0 0 * * ?"));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_cron_expr_parse(&expr, NULL));

    /* Missing fields. */
    AE_TEST_ASSERT(AE_ERR_BAD_DATA == ae_cron_expr_parse(&expr, "0 0 0 * *"));
    /* Out of range. */
    AE_TEST_ASSERT(AE_ERR_BAD_DATA == ae_cron_expr_parse(&expr, "60 * * * * ?"));
    AE_TEST_ASSERT(AE_ERR_BAD_DATA == ae_cron_expr_parse(&expr, "* 60 * * * ?"));
    AE_TEST_ASSERT(AE_ERR_BAD_DATA == ae_cron_expr_parse(&expr, "* * 24 * * ?"));
    AE_TEST_ASSERT(AE_ERR_BAD_DATA == ae_cron_expr_parse(&expr, "* * * 0 * ?"));
    AE_TEST_ASSERT(AE_ERR_BAD_DATA == ae_cron_expr_parse(&expr, "* * * 32 * ?"));
    AE_TEST_ASSERT(AE_ERR_BAD_DATA == ae_cron_expr_parse(&expr, "* * * * 0 ?"));
    AE_TEST_ASSERT(AE_ERR_BAD_DATA == ae_cron_expr_parse(&expr, "* * * * 13 ?"));
    AE_TEST_ASSERT(AE_ERR_BAD_DATA == ae_cron_expr_parse(&expr, "* * * * * 0"));
    AE_TEST_ASSERT(AE_ERR_BAD_DATA == ae_cron_expr_parse(&expr, "* * * * * 8"));

    /* Garbage. */
    AE_TEST_ASSERT(AE_ERR_BAD_DATA == ae_cron_expr_parse(&expr, "a * * * * ?"));

    AE_TEST_ASSERT(!ae_cron_expr_valid(&expr));
}

static void test_next_invalid(void)
{
    ae_cron_expr_t expr;

    AE_TEST_ASSERT(-1 == ae_cron_expr_next(NULL, 0));

    memset(&expr, 0, sizeof(expr));
    AE_TEST_ASSERT(-1 == ae_cron_expr_next(&expr, 0));
}

/* ============================================================ */

static const ae_test_case_t test_cases[] = {
    {"Daily midnight", test_daily_midnight},
    {"Every minute", test_every_minute},
    {"Daily noon", test_daily_noon},
    {"Weekday only", test_weekday_only},
    {"Day of month", test_day_of_month},
    {"Step and range", test_step_and_range},
    {"Both day fields", test_both_day_fields_or},
    {"NULL and invalid expressions", test_null_and_invalid},
    {"Next on invalid expression", test_next_invalid},
};

int main(void)
{
    ae_u32 failed = AE_TEST_SUITE(test_cases);

    printf("cron: %u assertions, %u failed\n",
           ae_test_run_count(), ae_test_fail_count());

    return failed != 0 ? 1 : 0;
}

/* ============================================================ */