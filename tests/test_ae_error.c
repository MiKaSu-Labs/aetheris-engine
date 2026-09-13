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
 * @file test_ae_error.c
 *
 * @brief Tests for the ae_error_t code model and predicates.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>

#include <aetheris/ae_error.h>

#include "ae_test.h"

/* ============================================================ */

static void test_ok_is_zero(void)
{
    AE_TEST_ASSERT(AE_OK == 0);
}

static void test_codes_are_negative(void)
{
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG < 0);
    AE_TEST_ASSERT(AE_ERR_INVALID_STATE < 0);
    AE_TEST_ASSERT(AE_ERR_OUT_OF_MEMORY < 0);
    AE_TEST_ASSERT(AE_ERR_NOT_FOUND < 0);
    AE_TEST_ASSERT(AE_ERR_ALREADY_EXISTS < 0);
    AE_TEST_ASSERT(AE_ERR_BUFFER_TOO_SMALL < 0);
    AE_TEST_ASSERT(AE_ERR_BAD_DATA < 0);
    AE_TEST_ASSERT(AE_ERR_BAD_LENGTH < 0);
    AE_TEST_ASSERT(AE_ERR_OVERFLOW < 0);
    AE_TEST_ASSERT(AE_ERR_IO < 0);
    AE_TEST_ASSERT(AE_ERR_TIMEOUT < 0);
    AE_TEST_ASSERT(AE_ERR_UNSUPPORTED < 0);
    AE_TEST_ASSERT(AE_ERR_PERMISSION < 0);
    AE_TEST_ASSERT(AE_ERR_ABORTED < 0);
}

static void test_predicates(void)
{
    AE_TEST_ASSERT(ae_error_ok(AE_OK));
    AE_TEST_ASSERT(!ae_error_ok(AE_ERR_IO));
    AE_TEST_ASSERT(ae_error_failed(AE_ERR_IO));
    AE_TEST_ASSERT(!ae_error_failed(AE_OK));
}

static void test_error_str(void)
{
    AE_TEST_ASSERT(ae_error_str(AE_OK) != NULL);
    AE_TEST_ASSERT(ae_error_str(AE_ERR_NOT_FOUND) != NULL);
    AE_TEST_ASSERT(ae_error_str((ae_error_t) -999) != NULL);
}

/* ============================================================ */

static const ae_test_case_t test_cases[] = {
    {"AE_OK is zero", test_ok_is_zero},
    {"Failure codes are negative", test_codes_are_negative},
    {"Predicates", test_predicates},
    {"Error name strings", test_error_str},
};

int main(void)
{
    ae_u32 failed = AE_TEST_SUITE(test_cases);

    printf("ae_error: %u assertions, %u failed\n",
           ae_test_run_count(), ae_test_fail_count());

    return failed != 0 ? 1 : 0;
}

/* ============================================================ */