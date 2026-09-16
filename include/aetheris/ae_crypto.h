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
 * @file ae_crypto.h
 *
 * @brief XOR cipher, session-key generation, and MT64-based encrypt
 *        key derivation.  RSA region-encrypt/sign is deferred to
 *        Phase 4 (see ae_crypto_region).
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_CRYPTO_H
#define AE_CRYPTO_H

#include <aetheris/ae_error.h>
#include <aetheris/ae_types.h>

/* ============================================================ */

/**
 * ae_crypto_xor - XOR a buffer in place with a repeating key.
 * @data: buffer to transform.
 * @data_len: length of @data.
 * @key: key bytes.
 * @key_len: length of @key (must be > 0).
 *
 * Each byte of @data is XORed with key[i % key_len].  The
 * transformation is self-inverse: applying it twice restores the
 * original data.
 */
ae_error_t ae_crypto_xor(ae_u8 *data, ae_size data_len,
                         const ae_u8 *key, ae_size key_len);

/**
 * ae_crypto_random_bytes - fill a buffer with OS CSPRNG bytes.
 * @out: destination buffer.
 * @len: number of bytes to generate.
 *
 * Uses getrandom() on POSIX and BCryptGenRandom() on Windows.
 */
ae_error_t ae_crypto_random_bytes(ae_u8 *out, ae_size len);

/**
 * ae_crypto_create_session_key - generate a random session key.
 * @out: destination buffer.
 * @len: number of bytes to generate.
 */
ae_error_t ae_crypto_create_session_key(ae_u8 *out, ae_size len);

/**
 * AE_ENCRYPT_KEY_SIZE - fixed encrypt-key length in bytes.
 *
 * Matches the 4096-byte key produced by the reference implementation.
 */
#define AE_ENCRYPT_KEY_SIZE ((ae_size) 4096)

/**
 * ae_crypto_generate_encrypt_key - derive a 4096-byte encrypt key
 * using MT19937-64.
 * @encrypt_key: output buffer, must be at least AE_ENCRYPT_KEY_SIZE
 *               bytes.
 * @seed: the random seed fed into the MT64; returned by the caller
 *        for later use.
 *
 * The algorithm mirrors the Java reference:
 *
 * 1. Create an MT64 seeded with @seed.
 * 2. Re-seed with the first output long.
 * 3. Discard one output long.
 * 4. Write 512 output longs in big-endian byte order, producing
 *    exactly 4096 bytes.
 */
ae_error_t ae_crypto_generate_encrypt_key(ae_u8 *encrypt_key,
                                          ae_u64 seed);

#endif /* AE_CRYPTO_H */