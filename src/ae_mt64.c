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
 * @file ae_mt64.c
 *
 * @brief MT19937-64 pseudorandom generator implementation.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <aetheris/ae_mt64.h>

/* ============================================================ */

/* Period parameters for the 64-bit Twister. */
#define MT_MATRIX_A  UINT64_C(0xB5026F5AA96619E9) /* constant vector a */
#define MT_UPPER_MASK UINT64_C(0xFFFFFFFF80000000) /* most significant w-r bits */
#define MT_LOWER_MASK UINT64_C(0x7FFFFFFF)         /* least significant r bits */

/* Initializer seed, matching the reference implementation. */
#define MT_DEFAULT_SEED UINT64_C(5489)

/* ============================================================ */

void ae_mt64_set_seed(ae_mt64_t *mt, ae_u64 seed)
{
    ae_s32 i;

    if (mt == NULL)
        return;

    mt->mt[0] = seed;
    for (i = 1; i <= AE_MT64_N - 1; i++) {
        mt->mt[i] = (UINT64_C(0x5851F42D4C957F2D) *
                     (mt->mt[i - 1] ^ (mt->mt[i - 1] >> 62)) + (ae_u64) i);
    }
    mt->mti = AE_MT64_N;
}

ae_error_t ae_mt64_init(ae_mt64_t *mt, ae_u64 seed)
{
    if (mt == NULL)
        return AE_ERR_INVALID_ARG;

    ae_mt64_set_seed(mt, seed);
    return AE_OK;
}

ae_u64 ae_mt64_next(ae_mt64_t *mt)
{
    ae_u64 mag01[2] = { UINT64_C(0), MT_MATRIX_A };
    ae_u64 y;
    ae_s32 i;

    if (mt == NULL)
        return UINT64_C(0);

    /* mti == N + 1 marks a fresh structure that was never seeded. */
    if (mt->mti == AE_MT64_N + 1)
        ae_mt64_set_seed(mt, MT_DEFAULT_SEED);

    if (mt->mti >= AE_MT64_N) {
        for (i = 0; i < AE_MT64_N - AE_MT64_M; i++) {
            y = ((mt->mt[i] & MT_UPPER_MASK) |
                 (mt->mt[i + 1] & MT_LOWER_MASK));
            mt->mt[i] = mt->mt[i + AE_MT64_M] ^
                        (y >> 1) ^ mag01[(ae_size) (y & UINT64_C(1))];
        }
        for (; i < AE_MT64_N - 1; i++) {
            y = ((mt->mt[i] & MT_UPPER_MASK) |
                 (mt->mt[i + 1] & MT_LOWER_MASK));
            mt->mt[i] = mt->mt[i + (AE_MT64_M - AE_MT64_N)] ^
                        (y >> 1) ^ mag01[(ae_size) (y & UINT64_C(1))];
        }
        y = ((mt->mt[AE_MT64_N - 1] & MT_UPPER_MASK) |
             (mt->mt[0] & MT_LOWER_MASK));
        mt->mt[AE_MT64_N - 1] = mt->mt[AE_MT64_M - 1] ^
                                (y >> 1) ^ mag01[(ae_size) (y & UINT64_C(1))];
        mt->mti = 0;
    }

    y = mt->mt[mt->mti++];
    y ^= (y >> 29) & UINT64_C(0x5555555555555555);
    y ^= (y << 17) & UINT64_C(0x71D67FFFEDA60000);
    y ^= (y << 37) & UINT64_C(0xFFF7EEE000000000);
    y ^= (y >> 43);

    return y;
}

/* ============================================================ */