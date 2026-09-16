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
 * @file ae_test.c
 *
 * @brief Implementation of the dependency-free Aetheris test harness.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>

#include <aetheris/ae_common.h>
#include <aetheris/ae_types.h>

#include "ae_test.h"

/* ============================================================ */

static ae_u32 run_count;
static ae_u32 fail_count;

/* ============================================================ */

ae_u32 ae_test_suite_run(const ae_test_case_t *suite, ae_size count)
{
    ae_size index;
    ae_u32 failed = 0;

    for (index = 0; index < count; index++) {
        ae_test_case_t test = suite[index];

        printf("test: %s\n", test.name);
        test.run();
        if (ae_test_fail_count() != 0)
            failed++;
    }

    return failed;
}

void ae_test_assert(bool ok, const char *expr, const char *file, ae_u32 line)
{
    run_count++;

    if (ok)
        return;

    fail_count++;
    printf("  assert failed at %s:%u: %s\n", file, line, expr);
}

ae_u32 ae_test_run_count(void)
{
    return run_count;
}

ae_u32 ae_test_fail_count(void)
{
    return fail_count;
}

/* ============================================================ */