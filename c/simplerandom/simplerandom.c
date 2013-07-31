/*
 * simplerandom.c
 *
 * Simple Pseudo-random Number Generators
 */


/*****************************************************************************
 * Includes
 ****************************************************************************/

#include "simplerandom.h"


/*****************************************************************************
 * Functions
 ****************************************************************************/

/*********
 * Cong
 ********/

void simplerandom_cong_seed(SimpleRandomCong_t * p_cong, uint32_t seed)
{
    p_cong->cong = seed;
    /* Byte interface */
    p_cong->byte_buffer = 0;
    p_cong->byte_index = 0;
}

uint32_t simplerandom_cong_next(SimpleRandomCong_t * p_cong)
{
    uint32_t    cong;

    cong = UINT32_C(69069) * p_cong->cong + 12345u;
    p_cong->cong = cong;

    return cong;
}

/*********
 * SHR3
 ********/

void simplerandom_shr3_seed(SimpleRandomSHR3_t * p_shr3, uint32_t seed)
{
    if (seed == 0)
    {
        seed = UINT32_C(0xFFFFFFFF);
    }

    p_shr3->shr3 = seed;

    /* Byte interface */
    p_shr3->byte_buffer = 0;
    p_shr3->byte_index = 0;
}

uint32_t simplerandom_shr3_next(SimpleRandomSHR3_t * p_shr3)
{
    uint32_t    shr3;


    shr3 = p_shr3->shr3;
    shr3 ^= (shr3 << 13);
    shr3 ^= (shr3 >> 17);
    shr3 ^= (shr3 << 5);
    p_shr3->shr3 = shr3;

    return shr3;
}

/*********
 * MWC1
 ********/

/*
 * See notes for simplerandom_mwc2_seed().
 */
void simplerandom_mwc1_seed(SimpleRandomMWC1_t * p_mwc, uint32_t seed_upper, uint32_t seed_lower)
{
    simplerandom_mwc2_seed(p_mwc, seed_upper, seed_lower);
}

uint32_t simplerandom_mwc1_next(SimpleRandomMWC1_t * p_mwc)
{
    p_mwc->mwc_upper = 36969u * (p_mwc->mwc_upper & 0xFFFFu) + (p_mwc->mwc_upper >> 16u);
    p_mwc->mwc_lower = 18000u * (p_mwc->mwc_lower & 0xFFFFu) + (p_mwc->mwc_lower >> 16u);
    
    return (p_mwc->mwc_upper << 16u) + p_mwc->mwc_lower;
}

/*********
 * MWC2
 ********/

/* There are some bad seed values. See:
 *     http://eprint.iacr.org/2011/007.pdf
 *
 * Of course 0 is bad for either part.
 *
 * For upper part, seed value 0x9068FFFF is bad. That
 * is, 36969 * 0x10000 - 1.
 *
 * For lower part, seed value 0x464FFFFF, or any multiple,
 * is bad. That is, 18000 * 0x10000 - 1.
 */
void simplerandom_mwc2_seed(SimpleRandomMWC2_t * p_mwc, uint32_t seed_upper, uint32_t seed_lower)
{
    if ((seed_upper == 0) || (seed_upper == UINT32_C(0x9068FFFF)))
    {
        seed_upper = UINT32_C(0xFFFFFFFF);
    }
    if ((seed_lower == 0) || (seed_lower == UINT32_C(0x464FFFFF) * 1u) ||
                             (seed_lower == UINT32_C(0x464FFFFF) * 2u) ||
                             (seed_lower == UINT32_C(0x464FFFFF) * 3u))
    {
        seed_lower = UINT32_C(0xFFFFFFFF);
    }

    p_mwc->mwc_upper = seed_upper;
    p_mwc->mwc_lower = seed_lower;

    /* Byte interface */
    p_mwc->byte_buffer = 0;
    p_mwc->byte_index = 0;
}

/*
 * This is almost identical to simplerandom_mwc1_next(), except that when
 * combining the upper and lower values in the last step, the upper 16 bits of
 * mwc_upper are added in too, instead of just being discarded.
 */
uint32_t simplerandom_mwc2_next(SimpleRandomMWC2_t * p_mwc)
{
    p_mwc->mwc_upper = 36969u * (p_mwc->mwc_upper & 0xFFFFu) + (p_mwc->mwc_upper >> 16u);
    p_mwc->mwc_lower = 18000u * (p_mwc->mwc_lower & 0xFFFFu) + (p_mwc->mwc_lower >> 16u);
    
    return (p_mwc->mwc_upper << 16u) + (p_mwc->mwc_upper >> 16u) + p_mwc->mwc_lower;
}

/*********
 * KISS
 ********/

void simplerandom_kiss_seed(SimpleRandomKISS_t * p_kiss, uint32_t seed_mwc_upper, uint32_t seed_mwc_lower, uint32_t seed_cong, uint32_t seed_shr3)
{
    /* Initialise MWC2 RNG */
    if ((seed_mwc_upper == 0) || (seed_mwc_upper == UINT32_C(0x9068FFFF)))
    {
        seed_mwc_upper = UINT32_C(0xFFFFFFFF);
    }
    if ((seed_mwc_lower == 0) || (seed_mwc_lower == UINT32_C(0x464FFFFF) * 1u) ||
                                 (seed_mwc_lower == UINT32_C(0x464FFFFF) * 2u) ||
                                 (seed_mwc_lower == UINT32_C(0x464FFFFF) * 3u))
    {
        seed_mwc_lower = UINT32_C(0xFFFFFFFF);
    }
    p_kiss->mwc_upper = seed_mwc_upper;
    p_kiss->mwc_lower = seed_mwc_lower;

    /* Initialise Cong RNG */
    p_kiss->cong = seed_cong;

    /* Initialise SHR3 RNG */
    if (seed_shr3 == 0)
    {
        seed_shr3 = UINT32_C(0xFFFFFFFF);
    }
    p_kiss->shr3 = seed_shr3;

    /* Byte interface */
    p_kiss->byte_buffer = 0;
    p_kiss->byte_index = 0;
}

uint32_t simplerandom_kiss_next(SimpleRandomKISS_t * p_kiss)
{
    uint32_t    cong;
    uint32_t    shr3;
    uint32_t    mwc2;


    /* Calculate next MWC2 RNG */
    p_kiss->mwc_upper = 36969u * (p_kiss->mwc_upper & 0xFFFFu) + (p_kiss->mwc_upper >> 16u);
    p_kiss->mwc_lower = 18000u * (p_kiss->mwc_lower & 0xFFFFu) + (p_kiss->mwc_lower >> 16u);
    mwc2 = (p_kiss->mwc_upper << 16u) + (p_kiss->mwc_upper >> 16u) + p_kiss->mwc_lower;

    /* Calculate next Cong RNG */
    cong = UINT32_C(69069) * p_kiss->cong + 12345u;
    p_kiss->cong = cong;

    /* Calculate next SHR3 RNG */
    shr3 = p_kiss->shr3;
    shr3 ^= (shr3 << 13);
    shr3 ^= (shr3 >> 17);
    shr3 ^= (shr3 << 5);
    p_kiss->shr3 = shr3;

    return ((mwc2 ^ cong) + shr3);
}

uint8_t simplerandom_kiss_next_byte(SimpleRandomKISS_t * p_kiss)
{
    if (p_kiss->byte_index == 0)
    {
        p_kiss->byte_buffer = simplerandom_kiss_next(p_kiss);
        p_kiss->byte_index = 3;
    }
    else
    {
        p_kiss->byte_buffer >>= 8u;
        p_kiss->byte_index--;
    }
    return (p_kiss->byte_buffer & 0xFFu);
}

#ifdef UINT64_C

/*********
 * MWC64
 ********/

/* There are some bad seed values. See notes for MWC.
 *
 * For MWC64, a seed that is any multiple of
 * 0x29A65EACFFFFFFFF is bad.
 * That is, 698769069 * 0x100000000 - 1.
 */
void simplerandom_mwc64_seed(SimpleRandomMWC64_t * p_mwc, uint32_t seed_upper, uint32_t seed_lower)
{
    uint64_t    seed64;

    seed64 = ((uint64_t)seed_upper << 32u) + seed_lower;
    if ((seed64 % UINT64_C(0x29A65EACFFFFFFFF)) == 0)
    {
        seed_upper = UINT32_C(0xFFFFFFFF);
        seed_lower = UINT32_C(0xFFFFFFFF);
    }

    p_mwc->mwc_upper = seed_upper;
    p_mwc->mwc_lower = seed_lower;

    /* Byte interface */
    p_mwc->byte_buffer = 0;
    p_mwc->byte_index = 0;
}

uint32_t simplerandom_mwc64_next(SimpleRandomMWC64_t * p_mwc)
{
    uint64_t    mwc64;


    mwc64 = UINT64_C(698769069) * p_mwc->mwc_lower + p_mwc->mwc_upper;
    p_mwc->mwc_upper = (mwc64 >> 32u);
    p_mwc->mwc_lower = (uint32_t)mwc64;

    return (uint32_t)mwc64;
}

/*********
 * KISS2
 ********/

void simplerandom_kiss2_seed(SimpleRandomKISS2_t * p_kiss2, uint32_t seed_mwc_upper, uint32_t seed_mwc_lower, uint32_t seed_cong, uint32_t seed_shr3)
{
    uint64_t    seed_mwc64;

    /* Initialise MWC64 RNG */
    seed_mwc64 = ((uint64_t)seed_mwc_upper << 32u) + seed_mwc_lower;
    if ((seed_mwc64 % UINT64_C(0x29A65EACFFFFFFFF)) == 0)
    {
        seed_mwc_upper = UINT32_C(0xFFFFFFFF);
        seed_mwc_lower = UINT32_C(0xFFFFFFFF);
    }
    p_kiss2->mwc_upper = seed_mwc_upper;
    p_kiss2->mwc_lower = seed_mwc_lower;

    /* Initialise Cong RNG */
    p_kiss2->cong = seed_cong;

    /* Initialise SHR3 RNG */
    if (seed_shr3 == 0)
    {
        seed_shr3 = UINT32_C(0xFFFFFFFF);
    }
    p_kiss2->shr3 = seed_shr3;

    /* Byte interface */
    p_kiss2->byte_buffer = 0;
    p_kiss2->byte_index = 0;
}

uint32_t simplerandom_kiss2_next(SimpleRandomKISS2_t * p_kiss2)
{
    uint64_t    mwc64;
    uint32_t    shr3;


    /* Calculate next MWC64 RNG */
    mwc64 = UINT64_C(698769069) * p_kiss2->mwc_lower + p_kiss2->mwc_upper;
    p_kiss2->mwc_upper = (mwc64 >> 32u);
    p_kiss2->mwc_lower = (uint32_t)mwc64;

    /* Calculate next Cong RNG */
    p_kiss2->cong = UINT32_C(69069) * p_kiss2->cong + 12345u;

    /* Calculate next SHR3 RNG */
    shr3 = p_kiss2->shr3;
    shr3 ^= (shr3 << 13);
    shr3 ^= (shr3 >> 17);
    shr3 ^= (shr3 << 5);
    p_kiss2->shr3 = shr3;

    return ((uint32_t)mwc64 + p_kiss2->cong + shr3);
}

#endif /* defined(UINT64_C) */


/*********
 * LFSR113
 ********/

#define LFSR_SEED_SHIFT         16u
#define LFSR_SEED_ALT_SHIFT     24u

#define LFSR_SEED_Z1_MIN_VALUE  2u
#define LFSR_SEED_Z2_MIN_VALUE  8u
#define LFSR_SEED_Z3_MIN_VALUE  16u
#define LFSR_SEED_Z4_MIN_VALUE  128u

void simplerandom_lfsr113_seed(SimpleRandomLFSR113_t * p_lfsr113, uint32_t seed_z1, uint32_t seed_z2, uint32_t seed_z3, uint32_t seed_z4)
{
    uint32_t    working_seed;

    /* Seed z1 */
    working_seed = seed_z1 ^ (seed_z1 << LFSR_SEED_SHIFT);
    if (working_seed < LFSR_SEED_Z1_MIN_VALUE)
    {
        working_seed = seed_z1 << LFSR_SEED_ALT_SHIFT;
        if (working_seed < LFSR_SEED_Z1_MIN_VALUE)
        {
            working_seed = ~working_seed;
        }
    }
    p_lfsr113->z1 = working_seed;

    /* Seed z2 */
    working_seed = seed_z2 ^ (seed_z2 << LFSR_SEED_SHIFT);
    if (working_seed < LFSR_SEED_Z2_MIN_VALUE)
    {
        working_seed = seed_z2 << LFSR_SEED_ALT_SHIFT;
        if (working_seed < LFSR_SEED_Z2_MIN_VALUE)
        {
            working_seed = ~working_seed;
        }
    }
    p_lfsr113->z2 = working_seed;

    /* Seed z3 */
    working_seed = seed_z3 ^ (seed_z3 << LFSR_SEED_SHIFT);
    if (working_seed < LFSR_SEED_Z3_MIN_VALUE)
    {
        working_seed = seed_z3 << LFSR_SEED_ALT_SHIFT;
        if (working_seed < LFSR_SEED_Z3_MIN_VALUE)
        {
            working_seed = ~working_seed;
        }
    }
    p_lfsr113->z3 = working_seed;

    /* Seed z4 */
    working_seed = seed_z4 ^ (seed_z4 << LFSR_SEED_SHIFT);
    if (working_seed < LFSR_SEED_Z4_MIN_VALUE)
    {
        working_seed = seed_z4 << LFSR_SEED_ALT_SHIFT;
        if (working_seed < LFSR_SEED_Z4_MIN_VALUE)
        {
            working_seed = ~working_seed;
        }
    }
    p_lfsr113->z4 = working_seed;

    /* Byte interface */
    p_lfsr113->byte_buffer = 0;
    p_lfsr113->byte_index = 0;
}

uint32_t simplerandom_lfsr113_next(SimpleRandomLFSR113_t * p_lfsr113)
{
    uint32_t    b;
    uint32_t    z1;
    uint32_t    z2;
    uint32_t    z3;
    uint32_t    z4;


    z1 = p_lfsr113->z1;
    z2 = p_lfsr113->z2;
    z3 = p_lfsr113->z3;
    z4 = p_lfsr113->z4;

    b  = ((z1 << 6) ^ z1) >> 13;
    z1 = ((z1 & UINT32_C(0xFFFFFFFE)) << 18) ^ b;

    b  = ((z2 << 2) ^ z2) >> 27;
    z2 = ((z2 & UINT32_C(0xFFFFFFF8)) << 2) ^ b;

    b  = ((z3 << 13) ^ z3) >> 21;
    z3 = ((z3 & UINT32_C(0xFFFFFFF0)) << 7) ^ b;

    b  = ((z4 << 3) ^ z4) >> 12;
    z4 = ((z4 & UINT32_C(0xFFFFFF80)) << 13) ^ b;

    p_lfsr113->z1 = z1;
    p_lfsr113->z2 = z2;
    p_lfsr113->z3 = z3;
    p_lfsr113->z4 = z4;

    return (z1 ^ z2 ^ z3 ^ z4);
}

/*********
 * LFSR88
 ********/

void simplerandom_lfsr88_seed(SimpleRandomLFSR88_t * p_lfsr88, uint32_t seed_z1, uint32_t seed_z2, uint32_t seed_z3)
{
    uint32_t    working_seed;

    /* Seed z1 */
    working_seed = seed_z1 ^ (seed_z1 << LFSR_SEED_SHIFT);
    if (working_seed < LFSR_SEED_Z1_MIN_VALUE)
    {
        working_seed = seed_z1 << LFSR_SEED_ALT_SHIFT;
        if (working_seed < LFSR_SEED_Z1_MIN_VALUE)
        {
            working_seed = ~working_seed;
        }
    }
    p_lfsr88->z1 = working_seed;

    /* Seed z2 */
    working_seed = seed_z2 ^ (seed_z2 << LFSR_SEED_SHIFT);
    if (working_seed < LFSR_SEED_Z2_MIN_VALUE)
    {
        working_seed = seed_z2 << LFSR_SEED_ALT_SHIFT;
        if (working_seed < LFSR_SEED_Z2_MIN_VALUE)
        {
            working_seed = ~working_seed;
        }
    }
    p_lfsr88->z2 = working_seed;

    /* Seed z3 */
    working_seed = seed_z3 ^ (seed_z3 << LFSR_SEED_SHIFT);
    if (working_seed < LFSR_SEED_Z3_MIN_VALUE)
    {
        working_seed = seed_z3 << LFSR_SEED_ALT_SHIFT;
        if (working_seed < LFSR_SEED_Z3_MIN_VALUE)
        {
            working_seed = ~working_seed;
        }
    }
    p_lfsr88->z3 = working_seed;

    /* Byte interface */
    p_lfsr88->byte_buffer = 0;
    p_lfsr88->byte_index = 0;
}

uint32_t simplerandom_lfsr88_next(SimpleRandomLFSR88_t * p_lfsr88)
{
    uint32_t    b;
    uint32_t    z1;
    uint32_t    z2;
    uint32_t    z3;


    z1 = p_lfsr88->z1;
    z2 = p_lfsr88->z2;
    z3 = p_lfsr88->z3;

    b  = ((z1 << 13) ^ z1) >> 19;
    z1 = ((z1 & UINT32_C(0xFFFFFFFE)) << 12) ^ b;

    b  = ((z2 << 2) ^ z2) >> 25;
    z2 = ((z2 & UINT32_C(0xFFFFFFF8)) << 4) ^ b;

    b  = ((z3 << 3) ^ z3) >> 11;
    z3 = ((z3 & UINT32_C(0xFFFFFFF0)) << 17) ^ b;

    p_lfsr88->z1 = z1;
    p_lfsr88->z2 = z2;
    p_lfsr88->z3 = z3;

    return (z1 ^ z2 ^ z3);
}

