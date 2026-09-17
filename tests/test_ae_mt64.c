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
 * @file test_ae_mt64.c
 *
 * @brief Tests for the MT19937-64 generator against reference values.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>

#include <aetheris/ae_mt64.h>

#include "ae_test.h"

/* ============================================================ */

static void test_seeded_sequence(void)
{
    ae_mt64_t mt;

    AE_TEST_ASSERT(AE_OK == ae_mt64_init(&mt, UINT64_C(5489)));

    AE_TEST_ASSERT(ae_mt64_next(&mt) == UINT64_C(14514284786278117030));
    AE_TEST_ASSERT(ae_mt64_next(&mt) == UINT64_C(4620546740167642908));
    AE_TEST_ASSERT(ae_mt64_next(&mt) == UINT64_C(13109570281517897720));
    AE_TEST_ASSERT(ae_mt64_next(&mt) == UINT64_C(17462938647148434322));
    AE_TEST_ASSERT(ae_mt64_next(&mt) == UINT64_C(355488278567739596));
}

static void test_reseed_changes_stream(void)
{
    ae_mt64_t mt;
    ae_u64 first, second;

    AE_TEST_ASSERT(AE_OK == ae_mt64_init(&mt, UINT64_C(1)));
    first = ae_mt64_next(&mt);
    AE_TEST_ASSERT(first != UINT64_C(0));

    ae_mt64_set_seed(&mt, UINT64_C(2));
    second = ae_mt64_next(&mt);
    AE_TEST_ASSERT(second != first);
}

static void test_long_stream_does_not_repeat_early(void)
{
    ae_mt64_t mt;
    ae_u64 seen[16];
    ae_size i, j;

    AE_TEST_ASSERT(AE_OK == ae_mt64_init(&mt, UINT64_C(42)));

    for (i = 0; i < AE_ARRAY_SIZE(seen); i++) {
        seen[i] = ae_mt64_next(&mt);
        for (j = 0; j < i; j++)
            AE_TEST_ASSERT(seen[j] != seen[i]);
    }
}

static void test_null_handling(void)
{
    ae_mt64_t mt;

    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_mt64_init(NULL, UINT64_C(1)));
    AE_TEST_ASSERT(ae_mt64_next(NULL) == UINT64_C(0));
    ae_mt64_set_seed(NULL, UINT64_C(1)); /* must not crash */
    (void) mt;
}

/* ============================================================ */

static const ae_test_case_t test_cases[] = {
    {"Seeded reference sequence", test_seeded_sequence},
    {"Reseed changes stream", test_reseed_changes_stream},
    {"Long stream has no early repeat", test_long_stream_does_not_repeat_early},
    {"NULL handling", test_null_handling},
};

int main(void)
{
    ae_u32 failed = AE_TEST_SUITE(test_cases);

    printf("mt64: %u assertions, %u failed\n",
           ae_test_run_count(), ae_test_fail_count());

    return failed != 0 ? 1 : 0;
}

/* ============================================================ */