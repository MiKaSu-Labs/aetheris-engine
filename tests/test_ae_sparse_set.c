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
 * @file test_ae_sparse_set.c
 *
 * @brief Tests for the CSV integer sparse set.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>
#include <string.h>

#include <aetheris/ae_sparse_set.h>

#include "ae_test.h"

/* ============================================================ */

static void test_single_values(void)
{
    ae_sparse_set_t set;
    AE_TEST_ASSERT(AE_OK == ae_sparse_set_init(&set, "1,5,9", NULL, 0));
    AE_TEST_ASSERT(3 == ae_sparse_set_count(&set));
    AE_TEST_ASSERT(ae_sparse_set_contains(&set, 1));
    AE_TEST_ASSERT(ae_sparse_set_contains(&set, 5));
    AE_TEST_ASSERT(ae_sparse_set_contains(&set, 9));
    AE_TEST_ASSERT(!ae_sparse_set_contains(&set, 2));
    AE_TEST_ASSERT(!ae_sparse_set_contains(&set, 8));
    AE_TEST_ASSERT(!ae_sparse_set_contains(&set, 10));
    ae_sparse_set_destroy(&set);
}

static void test_ranges(void)
{
    ae_sparse_set_t set;
    AE_TEST_ASSERT(AE_OK == ae_sparse_set_init(&set, "3-7", NULL, 0));
    AE_TEST_ASSERT(1 == ae_sparse_set_count(&set));
    AE_TEST_ASSERT(ae_sparse_set_contains(&set, 3));
    AE_TEST_ASSERT(ae_sparse_set_contains(&set, 5));
    AE_TEST_ASSERT(ae_sparse_set_contains(&set, 7));
    AE_TEST_ASSERT(!ae_sparse_set_contains(&set, 2));
    AE_TEST_ASSERT(!ae_sparse_set_contains(&set, 8));
    ae_sparse_set_destroy(&set);
}

static void test_mixed(void)
{
    ae_sparse_set_t set;
    AE_TEST_ASSERT(AE_OK == ae_sparse_set_init(&set, "1,3-5,8,11-13", NULL, 0));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 1));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 3));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 4));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 5));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 8));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 11));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 12));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 13));
    AE_TEST_ASSERT(!ae_sparse_set_contains(&set, 2));
    AE_TEST_ASSERT(!ae_sparse_set_contains(&set, 6));
    AE_TEST_ASSERT(!ae_sparse_set_contains(&set, 9));
    AE_TEST_ASSERT(!ae_sparse_set_contains(&set, 14));
    ae_sparse_set_destroy(&set);
}

static void test_whitespace_and_empty(void)
{
    ae_sparse_set_t set;

    AE_TEST_ASSERT(AE_OK == ae_sparse_set_init(&set, "1, 2-4 ,5", NULL, 0));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 1));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 4));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 5));
    AE_TEST_ASSERT(!ae_sparse_set_contains(&set, 6));
    ae_sparse_set_destroy(&set);

    AE_TEST_ASSERT(AE_OK == ae_sparse_set_init(&set, "", NULL, 0));
    AE_TEST_ASSERT(0 == ae_sparse_set_count(&set));
    AE_TEST_ASSERT(!ae_sparse_set_contains(&set, 1));
    ae_sparse_set_destroy(&set);

    AE_TEST_ASSERT(AE_OK == ae_sparse_set_init(&set, NULL, NULL, 0));
    AE_TEST_ASSERT(0 == ae_sparse_set_count(&set));
    ae_sparse_set_destroy(&set);
}

static void test_inverted_range_rejected(void)
{
    ae_sparse_set_t set;
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_sparse_set_init(&set, "5-3", NULL, 0));
}

static void test_caller_buffer(void)
{
    ae_sparse_entry_t buf[16];
    ae_sparse_set_t set;

    AE_TEST_ASSERT(AE_OK == ae_sparse_set_init(&set, "2,10-12", buf, 16));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 2));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 11));
    AE_TEST_ASSERT(!ae_sparse_set_contains(&set, 3));
    AE_TEST_ASSERT(buf == set.entries);
    ae_sparse_set_destroy(&set);
}

static void test_caller_buffer_overflow_grows(void)
{
    ae_sparse_entry_t buf[2];
    ae_sparse_set_t set;

    AE_TEST_ASSERT(AE_OK == ae_sparse_set_init(&set, "1,2,3,4,5", buf, 2));
    AE_TEST_ASSERT(5 == ae_sparse_set_count(&set));
    AE_TEST_ASSERT(true == ae_sparse_set_contains(&set, 5));
    AE_TEST_ASSERT(set.entries != buf);
    AE_TEST_ASSERT(set.owns_memory == true);
    ae_sparse_set_destroy(&set);
}

/* ============================================================ */

static const ae_test_case_t test_cases[] = {
    {"Single value entries", test_single_values},
    {"Range entries", test_ranges},
    {"Mixed singles and ranges", test_mixed},
    {"Empty and whitespace handling", test_whitespace_and_empty},
    {"Inverted ranges rejected", test_inverted_range_rejected},
    {"Caller-provided buffer", test_caller_buffer},
    {"Caller buffer overflow grows", test_caller_buffer_overflow_grows},
};

int main(void)
{
    ae_u32 failed = AE_TEST_SUITE(test_cases);

    printf("sparse_set: %u assertions, %u failed\n",
           ae_test_run_count(), ae_test_fail_count());

    return failed != 0 ? 1 : 0;
}

/* ============================================================ */