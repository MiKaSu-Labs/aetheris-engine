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
 * @file test_ae_crypto.c
 *
 * @brief Tests for the XOR cipher, session keys, and encrypt-key
 *        derivation.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>
#include <string.h>

#include <aetheris/ae_crypto.h>

#include "ae_test.h"

/* ============================================================ */

static void test_xor_self_inverse(void)
{
    ae_u8 data[4] = { 0x11, 0x22, 0x33, 0x44 };
    ae_u8 key[3] = { 0x01, 0x02, 0x03 };
    ae_u8 copy[4];

    memcpy(copy, data, sizeof(data));

    AE_TEST_ASSERT(AE_OK == ae_crypto_xor(data, sizeof(data), key, sizeof(key)));
    AE_TEST_ASSERT(memcmp(data, copy, sizeof(data)) != 0);

    /* Applying twice restores the original. */
    AE_TEST_ASSERT(AE_OK == ae_crypto_xor(data, sizeof(data), key, sizeof(key)));
    AE_TEST_ASSERT(memcmp(data, copy, sizeof(data)) == 0);
}

static void test_xor_key_longer_than_data(void)
{
    ae_u8 data[2] = { 0xFF, 0xFF };
    ae_u8 key[4] = { 0xFF, 0x00, 0xFF, 0x00 };

    AE_TEST_ASSERT(AE_OK == ae_crypto_xor(data, sizeof(data), key, sizeof(key)));
    /* byte[i] ^= key[i % 4]: 0xFF^0xFF=0x00, 0xFF^0x00=0xFF */
    AE_TEST_ASSERT(data[0] == 0x00);
    AE_TEST_ASSERT(data[1] == 0xFF);
}

static void test_xor_null_and_empty(void)
{
    ae_u8 data[1] = { 0x01 };
    ae_u8 key[1] = { 0x01 };

    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_crypto_xor(NULL, 1, key, 1));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_crypto_xor(data, 1, NULL, 1));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_crypto_xor(data, 1, key, 0));
    /* Zero-length data is a no-op. */
    AE_TEST_ASSERT(AE_OK == ae_crypto_xor(data, 0, key, 1));
    AE_TEST_ASSERT(data[0] == 0x01);
}

static void test_session_key(void)
{
    ae_u8 key[16];

    AE_TEST_ASSERT(AE_OK == ae_crypto_create_session_key(key, sizeof(key)));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_crypto_create_session_key(NULL, 16));
    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_crypto_create_session_key(key, 0));
}

static void test_random_bytes_unique(void)
{
    ae_u8 a[32];
    ae_u8 b[32];

    AE_TEST_ASSERT(AE_OK == ae_crypto_random_bytes(a, sizeof(a)));
    AE_TEST_ASSERT(AE_OK == ae_crypto_random_bytes(b, sizeof(b)));
    AE_TEST_ASSERT(memcmp(a, b, sizeof(a)) != 0);
}

static void test_encrypt_key_golden(void)
{
    ae_u8 key[AE_ENCRYPT_KEY_SIZE];
    /* Reference values derived from the Java/C reference MT64 when
     * seeded with each value and following the derivation rules. */
    const ae_u8 expected_seed0_head[16] = {
        0x00, 0xd5, 0x53, 0xeb, 0x7a, 0x1b, 0x83, 0x0f,
        0x3c, 0x9b, 0x32, 0x39, 0xe4, 0xb4, 0x09, 0x77
    };
    const ae_u8 expected_seed0_mid[16] = {
        0x14, 0xfd, 0x1d, 0x5f, 0x8f, 0x05, 0xb4, 0x68,
        0x23, 0xba, 0x6a, 0xba, 0x26, 0x51, 0xa9, 0xc8
    };
    const ae_u8 expected_seed0_tail[16] = {
        0xd8, 0x5b, 0x70, 0x7a, 0x3d, 0x5c, 0x18, 0x55,
        0xa7, 0xd5, 0x1a, 0x75, 0x8f, 0x0b, 0xa4, 0xd5
    };

    AE_TEST_ASSERT(AE_OK == ae_crypto_generate_encrypt_key(key, 0));
    AE_TEST_ASSERT(memcmp(key, expected_seed0_head, 16) == 0);
    AE_TEST_ASSERT(memcmp(key + 2040, expected_seed0_mid, 16) == 0);
    AE_TEST_ASSERT(memcmp(key + AE_ENCRYPT_KEY_SIZE - 16, expected_seed0_tail, 16) == 0);

    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_crypto_generate_encrypt_key(NULL, 0));
}

static void test_encrypt_key_deterministic_and_distinct(void)
{
    ae_u8 k0_a[AE_ENCRYPT_KEY_SIZE];
    ae_u8 k0_b[AE_ENCRYPT_KEY_SIZE];
    ae_u8 k1[AE_ENCRYPT_KEY_SIZE];

    AE_TEST_ASSERT(AE_OK == ae_crypto_generate_encrypt_key(k0_a, 42));
    AE_TEST_ASSERT(AE_OK == ae_crypto_generate_encrypt_key(k0_b, 42));
    AE_TEST_ASSERT(memcmp(k0_a, k0_b, AE_ENCRYPT_KEY_SIZE) == 0);

    AE_TEST_ASSERT(AE_OK == ae_crypto_generate_encrypt_key(k1, 43));
    AE_TEST_ASSERT(memcmp(k0_a, k1, AE_ENCRYPT_KEY_SIZE) != 0);
}

/* ============================================================ */

static const ae_test_case_t test_cases[] = {
    {"XOR is self-inverse", test_xor_self_inverse},
    {"XOR key longer than data", test_xor_key_longer_than_data},
    {"XOR NULL and empty handling", test_xor_null_and_empty},
    {"Session key generation", test_session_key},
    {"Random bytes are unique", test_random_bytes_unique},
    {"Encrypt key golden verifier", test_encrypt_key_golden},
    {"Encrypt key deterministic and distinct", test_encrypt_key_deterministic_and_distinct},
};

int main(void)
{
    ae_u32 failed = AE_TEST_SUITE(test_cases);

    printf("crypto: %u assertions, %u failed\n",
           ae_test_run_count(), ae_test_fail_count());

    return failed != 0 ? 1 : 0;
}

/* ============================================================ */