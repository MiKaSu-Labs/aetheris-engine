/*
 * 
 *           ______ _______ _    _ ______ _____  _____  _____ 
 *     /\   |  ____|__   __| |  | |  ____|  __ \|_   _|/ ____|
 *    /  \  | |__     | |  | |__| | |__  | |__) | | | | (____ 
 *   / /\ \ |  __|    | |  |  __  |  __| |  _  /  | | |\___  \ 
 *  / ____ \| |____   | |  | |  | | |____| | \ \ _| |_ ____) |
 * /_/    \_\______|  |_|  |_|  |_|______|_|  \_\_____|_____/ 
 * 
 *                                                                                     v1.0
 * @name Aetheris
 *
 * @author dkitagawa
 *
 * @file test_ae_common.c
 *
 * @brief Tests for ae_common macros and overflow-checked arithmetic.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>

#include <aetheris/ae_common.h>

#include "ae_test.h"

/* ============================================================ */

static void test_array_size(void)
{
    ae_u32 values[8] = { 0 };

    AE_TEST_ASSERT(AE_ARRAY_SIZE(values) == 8);

    struct sample {
        ae_u32 a;
        ae_u64 b;
    } samples[3];

    AE_TEST_ASSERT(AE_ARRAY_SIZE(samples) == 3);
}

static void test_min_max(void)
{
    AE_TEST_ASSERT(AE_MIN(3, 7) == 3);
    AE_TEST_ASSERT(AE_MIN(7, 3) == 3);
    AE_TEST_ASSERT(AE_MAX(3, 7) == 7);
    AE_TEST_ASSERT(AE_MAX(7, 3) == 7);
}

static void test_overflow_checked_arith(void)
{
    ae_size product = 0;
    ae_size sum = 0;
    ae_bool ok;

    ok = ae_mul_overflow_uz((ae_size) 1000, (ae_size) 1000, &product);
    AE_TEST_ASSERT(ok);
    AE_TEST_ASSERT(product == 1000 * 1000);

    ok = ae_mul_overflow_uz((ae_size) 1024, SIZE_MAX, &product);
    AE_TEST_ASSERT(!ok);

    ok = ae_add_overflow_uz((ae_size) 5, (ae_size) 5, &sum);
    AE_TEST_ASSERT(ok);
    AE_TEST_ASSERT(sum == 10);

    ok = ae_add_overflow_uz(SIZE_MAX, (ae_size) 1, &sum);
    AE_TEST_ASSERT(!ok);

    ok = ae_add_overflow_uz(SIZE_MAX, (ae_size) 0, &sum);
    AE_TEST_ASSERT(ok);
}

/* ============================================================ */

static const ae_test_case_t test_cases[] = {
    {"AE_ARRAY_SIZE", test_array_size},
    {"AE_MIN / AE_MAX", test_min_max},
    {"Overflow-checked arithmetic", test_overflow_checked_arith},
};

int main(void)
{
    ae_u32 failed = AE_TEST_SUITE(test_cases);

    printf("ae_common: %u assertions, %u failed\n",
           ae_test_run_count(), ae_test_fail_count());

    return failed != 0 ? 1 : 0;
}

/* ============================================================ */