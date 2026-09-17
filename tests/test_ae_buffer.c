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
 * @file test_ae_buffer.c
 *
 * @brief Tests for the growable little-endian buffer writer.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>
#include <string.h>

#include <aetheris/ae_buffer.h>

#include "ae_test.h"

/* ============================================================ */

static void test_create_and_reset(void)
{
    ae_buffer_t *buf;

    AE_TEST_ASSERT(AE_OK == ae_buffer_create(128, &buf));
    AE_TEST_ASSERT(buf != NULL);
    AE_TEST_ASSERT(ae_buffer_len(buf) == 0);
    AE_TEST_ASSERT(ae_buffer_capacity(buf) >= 128);
    AE_TEST_ASSERT(ae_buffer_data(buf) != NULL);

    AE_TEST_ASSERT(AE_OK == ae_buffer_write_u8(buf, 0x11));
    AE_TEST_ASSERT(ae_buffer_len(buf) == 1);
    AE_TEST_ASSERT(AE_OK == ae_buffer_reset(buf));
    AE_TEST_ASSERT(ae_buffer_len(buf) == 0);
    AE_TEST_ASSERT(ae_buffer_capacity(buf) >= 128);

    ae_buffer_destroy(buf);
}

static void test_null_handling(void)
{
    ae_buffer_t *buf;

    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_buffer_create(16, NULL));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_buffer_reset(NULL));
    AE_TEST_ASSERT(ae_buffer_len(NULL) == 0);
    AE_TEST_ASSERT(ae_buffer_capacity(NULL) == 0);
    AE_TEST_ASSERT(ae_buffer_data(NULL) == NULL);
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_buffer_write_u8(NULL, 1));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_buffer_write_bytes(NULL, NULL, 0));
    ae_buffer_destroy(NULL);
    (void) buf;
}

static void test_scalar_endianness(void)
{
    ae_buffer_t *buf;
    const ae_u8 *d;
    ae_u8 expected_u16[2] = { 0x34, 0x12 };
    ae_u8 expected_u24[3] = { 0x78, 0x56, 0x34 };
    ae_u8 expected_u32[4] = { 0xBC, 0x9A, 0x78, 0x56 };
    ae_u8 expected_u64[8] = { 0xF0, 0xDE, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12 };
    ae_u8 expected_s16[2] = { 0xCD, 0xAB };
    ae_u8 expected_s32[4] = { 0xEF, 0xCD, 0xAB, 0x89 };

    AE_TEST_ASSERT(AE_OK == ae_buffer_create(0, &buf));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_u8(buf, 0x12));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_bool(buf, true));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_u16(buf, 0x1234));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_u24(buf, 0x345678));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_u32(buf, 0x56789ABC));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_u64(buf, 0x123456789ABCDEF0));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_s16(buf, (ae_s16) 0xABCD));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_s32(buf, (ae_s32) 0x89ABCDEF));

    AE_TEST_ASSERT(ae_buffer_len(buf) == 1 + 1 + 2 + 3 + 4 + 8 + 2 + 4);
    d = ae_buffer_data(buf);

    AE_TEST_ASSERT(d[0] == 0x12);
    AE_TEST_ASSERT(d[1] == 1);
    AE_TEST_ASSERT(memcmp(d + 2, expected_u16, 2) == 0);
    AE_TEST_ASSERT(memcmp(d + 4, expected_u24, 3) == 0);
    AE_TEST_ASSERT(memcmp(d + 7, expected_u32, 4) == 0);
    AE_TEST_ASSERT(memcmp(d + 11, expected_u64, 8) == 0);
    AE_TEST_ASSERT(memcmp(d + 19, expected_s16, 2) == 0);
    AE_TEST_ASSERT(memcmp(d + 21, expected_s32, 4) == 0);

    ae_buffer_destroy(buf);
}

static void test_float_writing(void)
{
    ae_buffer_t *buf;
    const ae_u8 *d;
    float f;
    double g;
    ae_u32 fbits;
    ae_u64 gbits;

    f = 1.0f;
    g = 1.0;
    memcpy(&fbits, &f, sizeof(fbits));
    memcpy(&gbits, &g, sizeof(gbits));

    AE_TEST_ASSERT(AE_OK == ae_buffer_create(0, &buf));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_f32(buf, f));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_f64(buf, g));
    AE_TEST_ASSERT(ae_buffer_len(buf) == 4 + 8);
    d = ae_buffer_data(buf);

    AE_TEST_ASSERT(((ae_u32) d[0]) == (fbits & 0xFF));
    AE_TEST_ASSERT(((ae_u32) d[1]) == ((fbits >> 8) & 0xFF));
    AE_TEST_ASSERT(((ae_u32) d[2]) == ((fbits >> 16) & 0xFF));
    AE_TEST_ASSERT(((ae_u32) d[3]) == ((fbits >> 24) & 0xFF));
    AE_TEST_ASSERT(((ae_u64) d[4]) == (gbits & 0xFF));
    AE_TEST_ASSERT(((ae_u64) d[11]) == ((gbits >> 56) & 0xFF));

    ae_buffer_destroy(buf);
}

static void test_empty_and_max(void)
{
    ae_buffer_t *buf;
    const ae_u8 *d;

    AE_TEST_ASSERT(AE_OK == ae_buffer_create(0, &buf));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_empty(buf, 0));
    AE_TEST_ASSERT(ae_buffer_len(buf) == 0);
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_empty(buf, 3));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_max(buf, 2));

    AE_TEST_ASSERT(ae_buffer_len(buf) == 5);
    d = ae_buffer_data(buf);
    AE_TEST_ASSERT(d[0] == 0 && d[1] == 0 && d[2] == 0);
    AE_TEST_ASSERT(d[3] == 0xFF && d[4] == 0xFF);

    ae_buffer_destroy(buf);
}

static void test_raw_bytes(void)
{
    ae_buffer_t *buf;
    const ae_u8 *d;
    ae_u8 data[3] = { 0xAA, 0xBB, 0xCC };

    AE_TEST_ASSERT(AE_OK == ae_buffer_create(0, &buf));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_bytes(buf, NULL, 0));
    AE_TEST_ASSERT(ae_buffer_len(buf) == 0);
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_buffer_write_bytes(buf, NULL, 3));
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_bytes(buf, data, sizeof(data)));

    d = ae_buffer_data(buf);
    AE_TEST_ASSERT(ae_buffer_len(buf) == 3);
    AE_TEST_ASSERT(memcmp(d, data, sizeof(data)) == 0);

    ae_buffer_destroy(buf);
}

static void test_strings(void)
{
    ae_buffer_t *buf;
    const ae_u8 *d;
    ae_u16 units[2] = { 0x0041, 0x0042 }; /* "AB" */
    ae_u8 s8[3] = { 'x', 'y', 'z' };

    AE_TEST_ASSERT(AE_OK == ae_buffer_create(0, &buf));

    /* String16: prefix is 2 * unit count, then units little-endian. */
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_string16(buf, units, 2));
    AE_TEST_ASSERT(ae_buffer_len(buf) == 2 + 4);
    d = ae_buffer_data(buf);
    AE_TEST_ASSERT(d[0] == 4 && d[1] == 0);
    AE_TEST_ASSERT(d[2] == 0x41 && d[3] == 0x00);
    AE_TEST_ASSERT(d[4] == 0x42 && d[5] == 0x00);

    /* Empty string: prefix 0 only. */
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_string16(buf, NULL, 0));
    AE_TEST_ASSERT(d[6] == 0 && d[7] == 0);

    /* String8: prefix is char count, then bytes. */
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_string8(buf, s8, 3));
    AE_TEST_ASSERT(ae_buffer_len(buf) == (2 + 4) + 2 + (2 + 3));
    d = ae_buffer_data(buf);
    AE_TEST_ASSERT(d[8] == 3 && d[9] == 0);
    AE_TEST_ASSERT(d[10] == 'x' && d[11] == 'y' && d[12] == 'z');

    /* Invalid pointer combinations. */
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_buffer_write_string16(buf, NULL, 1));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_buffer_write_string8(buf, NULL, 1));

    ae_buffer_destroy(buf);
}

static void test_direct_string8(void)
{
    ae_buffer_t *buf;
    const ae_u8 *d;
    ae_u8 s8[2] = { 'A', 'B' };

    AE_TEST_ASSERT(AE_OK == ae_buffer_create(0, &buf));

    /* Exact width with padding. */
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_direct_string8(buf, s8, 2, 5));
    AE_TEST_ASSERT(ae_buffer_len(buf) == 5);
    d = ae_buffer_data(buf);
    AE_TEST_ASSERT(d[0] == 'A' && d[1] == 'B' && d[2] == 0 && d[3] == 0 && d[4] == 0);

    /* Truncation when count exceeds width. */
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_direct_string8(buf, s8, 2, 1));
    AE_TEST_ASSERT(ae_buffer_len(buf) == 6);
    AE_TEST_ASSERT(d[5] == 'A');

    /* NULL string appends nothing. */
    AE_TEST_ASSERT(AE_OK == ae_buffer_write_direct_string8(buf, NULL, 0, 4));
    AE_TEST_ASSERT(ae_buffer_len(buf) == 6);

    ae_buffer_destroy(buf);
}

static void test_growth_preserves_contents(void)
{
    ae_buffer_t *buf;
    const ae_u8 *d;
    ae_size i;
    ae_size n = 1000;

    AE_TEST_ASSERT(AE_OK == ae_buffer_create(8, &buf));

    for (i = 0; i < n; i++)
        AE_TEST_ASSERT(AE_OK == ae_buffer_write_u8(buf, (ae_u32) (i & 0xFF)));

    AE_TEST_ASSERT(ae_buffer_len(buf) == n);
    AE_TEST_ASSERT(ae_buffer_capacity(buf) >= n);
    d = ae_buffer_data(buf);
    for (i = 0; i < n; i++)
        AE_TEST_ASSERT(d[i] == (ae_u8) (i & 0xFF));

    ae_buffer_destroy(buf);
}

static void test_overflow_length_guards(void)
{
    ae_buffer_t *buf;
    ae_u16 units[1] = { 0 };
    ae_u8 bytes[1] = { 0 };

    AE_TEST_ASSERT(AE_OK == ae_buffer_create(0, &buf));
    AE_TEST_ASSERT(AE_ERR_OVERFLOW ==
                   ae_buffer_write_string16(buf, units, UINT16_MAX / 2 + 1));
    AE_TEST_ASSERT(AE_ERR_OVERFLOW ==
                   ae_buffer_write_string8(buf, bytes, (ae_size) UINT16_MAX + 1));
    AE_TEST_ASSERT(ae_buffer_len(buf) == 0);

    ae_buffer_destroy(buf);
}

/* ============================================================ */

static const ae_test_case_t test_cases[] = {
    {"Create and reset", test_create_and_reset},
    {"NULL handling", test_null_handling},
    {"Scalar little-endian writes", test_scalar_endianness},
    {"Float writing", test_float_writing},
    {"Empty and max runs", test_empty_and_max},
    {"Raw byte runs", test_raw_bytes},
    {"Length-prefixed strings", test_strings},
    {"Direct string8", test_direct_string8},
    {"Growth preserves contents", test_growth_preserves_contents},
    {"Overflow length guards", test_overflow_length_guards},
};

int main(void)
{
    ae_u32 failed = AE_TEST_SUITE(test_cases);

    printf("buffer: %u assertions, %u failed\n",
           ae_test_run_count(), ae_test_fail_count());

    return failed != 0 ? 1 : 0;
}

/* ============================================================ */