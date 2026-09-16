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
 * @file ae_crypto.c
 *
 * @brief XOR cipher, session-key generation, and MT64-based encrypt
 *        key derivation.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <string.h>

#include <aetheris/ae_crypto.h>
#include <aetheris/ae_mt64.h>

/* ============================================================ */

/* Platform CSPRNG ------------------------------------------------ */

#ifdef _WIN32
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#    include <bcrypt.h>
#else
#    include <sys/random.h>
#endif

ae_error_t ae_crypto_random_bytes(ae_u8 *out, ae_size len)
{
    if (out == NULL || len == 0)
        return AE_ERR_INVALID_ARG;

#ifdef _WIN32
    if (BCryptGenRandom(NULL, out, (ULONG) len,
                        BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0)
        return AE_ERR_IO;
#else
    {
        ae_size done = 0;

        while (done < len) {
            ssize_t n = getrandom(out + done, len - done, 0);
            if (n < 0)
                return AE_ERR_IO;
            done += (ae_size) n;
        }
    }
#endif

    return AE_OK;
}

/* ============================================================ */

ae_error_t ae_crypto_create_session_key(ae_u8 *out, ae_size len)
{
    if (out == NULL || len == 0)
        return AE_ERR_INVALID_ARG;

    return ae_crypto_random_bytes(out, len);
}

/* ============================================================ */

ae_error_t ae_crypto_xor(ae_u8 *data, ae_size data_len,
                         const ae_u8 *key, ae_size key_len)
{
    ae_size i;

    if (data == NULL || key == NULL || key_len == 0)
        return AE_ERR_INVALID_ARG;

    for (i = 0; i < data_len; i++)
        data[i] ^= key[i % key_len];

    return AE_OK;
}

/* ============================================================ */

ae_error_t ae_crypto_generate_encrypt_key(ae_u8 *encrypt_key,
                                          ae_u64 seed)
{
    ae_mt64_t mt;
    ae_u64 first;
    ae_u64 v;
    ae_u8 *p;
    ae_size i;

    if (encrypt_key == NULL)
        return AE_ERR_INVALID_ARG;

    /* 1. Seed with the caller-provided seed. */
    ae_mt64_init(&mt, seed);

    /* 2. Re-seed with the first output long. */
    first = ae_mt64_next(&mt);
    ae_mt64_set_seed(&mt, first);

    /* 3. Discard one output long. */
    (void) ae_mt64_next(&mt);

    /* 4. Write 512 longs in big-endian order (4096 bytes). */
    p = encrypt_key;
    for (i = 0; i < (AE_ENCRYPT_KEY_SIZE / 8); i++) {
        v = ae_mt64_next(&mt);
        p[0] = (ae_u8) (v >> 56);
        p[1] = (ae_u8) (v >> 48);
        p[2] = (ae_u8) (v >> 40);
        p[3] = (ae_u8) (v >> 32);
        p[4] = (ae_u8) (v >> 24);
        p[5] = (ae_u8) (v >> 16);
        p[6] = (ae_u8) (v >> 8);
        p[7] = (ae_u8) (v);
        p += 8;
    }

    return AE_OK;
}

/* ============================================================ */