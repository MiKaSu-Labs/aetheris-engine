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
 * @file ae_test.h
 *
 * @brief Minimal dependency-free unit test harness for Aetheris modules.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_TEST_H
#define AE_TEST_H

#include <aetheris/ae_common.h>
#include <aetheris/ae_types.h>

/* ============================================================ */

/*
 * The harness tracks assertions in the current test. A test registers a
 * name and a function via ae_test_run; the process exit status reports
 * whether any assertion failed across the whole run.
 */

typedef struct ae_test_case {
    const char *name;
    void (*run)(void);
} ae_test_case_t;

/**
 * ae_test_suite_run - execute a table of test cases.
 * @suite: table of test cases.
 * @count: number of entries in suite.
 *
 * Runs each case in order, reporting pass/fail per case.
 * Returns the number of failed cases.
 */
ae_u32 ae_test_suite_run(const ae_test_case_t *suite, ae_size count);

/**
 * ae_test_assert - record an assertion outcome.
 * @ok: nonzero when the assertion held.
 * @expr: textual form of the assertion for diagnostics.
 * @file: source file of the assertion.
 * @line: source line of the assertion.
 */
void ae_test_assert(bool ok, const char *expr, const char *file, ae_u32 line);

/**
 * ae_test_run_count - total number of assertions run.
 */
ae_u32 ae_test_run_count(void);

/**
 * ae_test_fail_count - total number of failed assertions.
 */
ae_u32 ae_test_fail_count(void);

#define AE_TEST_ASSERT(condition) \
    ae_test_assert((condition) != 0, #condition, __FILE__, __LINE__)

#define AE_TEST_SUITE(suite) \
    ae_test_suite_run((suite), AE_ARRAY_SIZE(suite))

/* ============================================================ */

#endif /* AE_TEST_H */