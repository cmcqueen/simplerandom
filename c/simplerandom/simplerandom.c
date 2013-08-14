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

size_t simplerandom_cong_num_seeds(const SimpleRandomCong_t * p_cong)
{
    (const void *)p_cong;   /* We only use this parameter for type checking. */

    return 1u;
}

size_t simplerandom_cong_seed_array(SimpleRandomCong_t * p_cong, const uint32_t * p_seeds, size_t num_seeds, bool mix_extras)
{
    uint32_t    seed = 0;
    size_t      num_seeds_used = 0;

    if (num_seeds >= 1u && p_seeds != NULL)
    {
        seed = p_seeds[0];
        num_seeds_used = 1u;
    }
    simplerandom_cong_seed(p_cong, seed);
    return num_seeds_used;
}

void simplerandom_cong_seed(SimpleRandomCong_t * p_cong, uint32_t seed)
{
    p_cong->cong = seed;
}

uint32_t simplerandom_cong_next(SimpleRandomCong_t * p_cong)
{
    uint32_t    cong;

    cong = UINT32_C(69069) * p_cong->cong + 12345u;
    p_cong->cong = cong;

    return cong;
}

uint8_t simplerandom_cong_next_uint8(SimpleRandomCong_t * p_cong)
{
    /* Return most-significant 8 bits. */
    return (simplerandom_cong_next(p_cong) >> 24u);
}

uint16_t simplerandom_cong_next_uint16(SimpleRandomCong_t * p_cong)
{
    /* Return most-significant 16 bits. */
    return (simplerandom_cong_next(p_cong) >> 16u);
}

/*********
 * SHR3
 ********/

size_t simplerandom_shr3_num_seeds(const SimpleRandomSHR3_t * p_shr3)
{
    (const void *)p_shr3;   /* We only use this parameter for type checking. */

    return 1u;
}

size_t simplerandom_shr3_seed_array(SimpleRandomSHR3_t * p_shr3, const uint32_t * p_seeds, size_t num_seeds, bool mix_extras)
{
    uint32_t    seed = 0;
    size_t      num_seeds_used = 0;

    if (num_seeds >= 1u && p_seeds != NULL)
    {
        seed = p_seeds[0];
        num_seeds_used = 1u;
    }
    simplerandom_shr3_seed(p_shr3, seed);
    return num_seeds_used;
}

void simplerandom_shr3_seed(SimpleRandomSHR3_t * p_shr3, uint32_t seed)
{
    if (seed == 0)
    {
        seed = UINT32_C(0xFFFFFFFF);
    }

    p_shr3->shr3 = seed;
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

uint8_t simplerandom_shr3_next_uint8(SimpleRandomSHR3_t * p_shr3)
{
    /* Return most-significant 8 bits. */
    return (simplerandom_shr3_next(p_shr3) >> 24u);
}

uint16_t simplerandom_shr3_next_uint16(SimpleRandomSHR3_t * p_shr3)
{
    /* Return most-significant 16 bits. */
    return (simplerandom_shr3_next(p_shr3) >> 16u);
}

/*********
 * MWC1
 ********/

size_t simplerandom_mwc1_num_seeds(const SimpleRandomMWC1_t * p_mwc)
{
    (const void *)p_mwc;    /* We only use this parameter for type checking. */

    return 2u;
}

size_t simplerandom_mwc1_seed_array(SimpleRandomMWC1_t * p_mwc, const uint32_t * p_seeds, size_t num_seeds, bool mix_extras)
{
    return simplerandom_mwc2_seed_array(p_mwc, p_seeds, num_seeds, mix_extras);
}

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

uint8_t simplerandom_mwc1_next_uint8(SimpleRandomMWC1_t * p_mwc)
{
    /* Return most-significant 8 bits. */
    return (simplerandom_mwc1_next(p_mwc) >> 24u);
}

uint16_t simplerandom_mwc1_next_uint16(SimpleRandomMWC1_t * p_mwc)
{
    /* Return most-significant 16 bits. */
    return (simplerandom_mwc1_next(p_mwc) >> 16u);
}

/*********
 * MWC2
 ********/

size_t simplerandom_mwc2_num_seeds(const SimpleRandomMWC2_t * p_mwc)
{
    (const void *)p_mwc;    /* We only use this parameter for type checking. */

    return 2u;
}

size_t simplerandom_mwc2_seed_array(SimpleRandomMWC2_t * p_mwc, const uint32_t * p_seeds, size_t num_seeds, bool mix_extras)
{
    uint32_t    seed_upper = 0;
    uint32_t    seed_lower = 0;
    size_t      num_seeds_used = 0;

    if (p_seeds != NULL)
    {
        if (num_seeds > 2u)
            num_seeds_used = 2u;
        else
            num_seeds_used = num_seeds;
        switch (num_seeds_used)
        {
            case 2:     seed_lower = p_seeds[1];    /* drop through to next case */
            case 1:     seed_upper = p_seeds[0];
        }
    }
    simplerandom_mwc1_seed(p_mwc, seed_upper, seed_lower);
    return num_seeds_used;
}

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

uint8_t simplerandom_mwc2_next_uint8(SimpleRandomMWC2_t * p_mwc)
{
    /* Return most-significant 8 bits. */
    return (simplerandom_mwc2_next(p_mwc) >> 24u);
}

uint16_t simplerandom_mwc2_next_uint16(SimpleRandomMWC2_t * p_mwc)
{
    /* Return most-significant 16 bits. */
    return (simplerandom_mwc2_next(p_mwc) >> 16u);
}

/*********
 * KISS
 ********/

size_t simplerandom_kiss_num_seeds(const SimpleRandomKISS_t * p_kiss)
{
    (const void *)p_kiss;   /* We only use this parameter for type checking. */

    return 4u;
}

size_t simplerandom_kiss_seed_array(SimpleRandomKISS_t * p_kiss, const uint32_t * p_seeds, size_t num_seeds, bool mix_extras)
{
    uint32_t    seed_mwc_upper = 0;
    uint32_t    seed_mwc_lower = 0;
    uint32_t    seed_cong = 0;
    uint32_t    seed_shr3 = 0;
    size_t      num_seeds_used = 0;

    if (p_seeds != NULL)
    {
        if (num_seeds > 4u)
            num_seeds_used = 4u;
        else
            num_seeds_used = num_seeds;
        switch (num_seeds_used)
        {
            case 4:     seed_shr3       = p_seeds[3];   /* drop through to next case */
            case 3:     seed_cong       = p_seeds[2];   /* drop through to next case */
            case 2:     seed_mwc_lower  = p_seeds[1];   /* drop through to next case */
            case 1:     seed_mwc_upper  = p_seeds[0];
        }
    }
    simplerandom_kiss_seed(p_kiss, seed_mwc_upper, seed_mwc_lower, seed_cong, seed_shr3);
    return num_seeds_used;
}

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

uint8_t simplerandom_kiss_next_uint8(SimpleRandomKISS_t * p_kiss)
{
    /* Return most-significant 8 bits. */
    return (simplerandom_kiss_next(p_kiss) >> 24u);
}

uint16_t simplerandom_kiss_next_uint16(SimpleRandomKISS_t * p_kiss)
{
    /* Return most-significant 16 bits. */
    return (simplerandom_kiss_next(p_kiss) >> 16u);
}

#ifdef UINT64_C

/*********
 * MWC64
 ********/

size_t simplerandom_mwc64_num_seeds(const SimpleRandomMWC64_t * p_mwc)
{
    (const void *)p_mwc;    /* We only use this parameter for type checking. */

    return 2u;
}

size_t simplerandom_mwc64_seed_array(SimpleRandomMWC64_t * p_mwc, const uint32_t * p_seeds, size_t num_seeds, bool mix_extras)
{
    uint32_t    seed_upper = 0;
    uint32_t    seed_lower = 0;
    size_t      num_seeds_used = 0;


    if (p_seeds != NULL)
    {
        if (num_seeds > 2u)
            num_seeds_used = 2u;
        else
            num_seeds_used = num_seeds;
        switch (num_seeds_used)
        {
            case 2:     seed_lower = p_seeds[1];    /* drop through to next case */
            case 1:     seed_upper = p_seeds[0];
        }
    }
    simplerandom_mwc64_seed(p_mwc, seed_upper, seed_lower);
    return num_seeds_used;
}

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
}

uint32_t simplerandom_mwc64_next(SimpleRandomMWC64_t * p_mwc)
{
    uint64_t    mwc64;


    mwc64 = UINT64_C(698769069) * p_mwc->mwc_lower + p_mwc->mwc_upper;
    p_mwc->mwc_upper = (mwc64 >> 32u);
    p_mwc->mwc_lower = (uint32_t)mwc64;

    return (uint32_t)mwc64;
}

uint8_t simplerandom_mwc64_next_uint8(SimpleRandomMWC64_t * p_mwc)
{
    /* Return most-significant 8 bits. */
    return (simplerandom_mwc64_next(p_mwc) >> 24u);
}

uint16_t simplerandom_mwc64_next_uint16(SimpleRandomMWC64_t * p_mwc)
{
    /* Return most-significant 16 bits. */
    return (simplerandom_mwc64_next(p_mwc) >> 16u);
}

/*********
 * KISS2
 ********/

size_t simplerandom_kiss2_num_seeds(const SimpleRandomKISS2_t * p_kiss2)
{
    (const void *)p_kiss2;  /* We only use this parameter for type checking. */

    return 4u;
}

size_t simplerandom_kiss2_seed_array(SimpleRandomKISS2_t * p_kiss2, const uint32_t * p_seeds, size_t num_seeds, bool mix_extras)
{
    uint32_t    seed_mwc_upper = 0;
    uint32_t    seed_mwc_lower = 0;
    uint32_t    seed_cong = 0;
    uint32_t    seed_shr3 = 0;
    size_t      num_seeds_used = 0;

    if (p_seeds != NULL)
    {
        if (num_seeds > 4u)
            num_seeds_used = 4u;
        else
            num_seeds_used = num_seeds;
        switch (num_seeds_used)
        {
            case 4:     seed_shr3       = p_seeds[3];   /* drop through to next case */
            case 3:     seed_cong       = p_seeds[2];   /* drop through to next case */
            case 2:     seed_mwc_lower  = p_seeds[1];   /* drop through to next case */
            case 1:     seed_mwc_upper  = p_seeds[0];
        }
    }
    simplerandom_kiss2_seed(p_kiss2, seed_mwc_upper, seed_mwc_lower, seed_cong, seed_shr3);
    return num_seeds_used;
}

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

uint8_t simplerandom_kiss2_next_uint8(SimpleRandomKISS2_t * p_kiss2)
{
    /* Return most-significant 8 bits. */
    return (simplerandom_kiss2_next(p_kiss2) >> 24u);
}

uint16_t simplerandom_kiss2_next_uint16(SimpleRandomKISS2_t * p_kiss2)
{
    /* Return most-significant 16 bits. */
    return (simplerandom_kiss2_next(p_kiss2) >> 16u);
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

size_t simplerandom_lfsr113_num_seeds(const SimpleRandomLFSR113_t * p_lfsr113)
{
    (const void *)p_lfsr113;    /* We only use this parameter for type checking. */

    return 4u;
}

size_t simplerandom_lfsr113_seed_array(SimpleRandomLFSR113_t * p_lfsr113, const uint32_t * p_seeds, size_t num_seeds, bool mix_extras)
{
    uint32_t    seed_z1 = 0;
    uint32_t    seed_z2 = 0;
    uint32_t    seed_z3 = 0;
    uint32_t    seed_z4 = 0;
    size_t      num_seeds_used = 0;

    if (p_seeds != NULL)
    {
        if (num_seeds > 4u)
            num_seeds_used = 4u;
        else
            num_seeds_used = num_seeds;
        switch (num_seeds_used)
        {
            case 4:     seed_z4 = p_seeds[3];   /* drop through to next case */
            case 3:     seed_z3 = p_seeds[2];   /* drop through to next case */
            case 2:     seed_z2 = p_seeds[1];   /* drop through to next case */
            case 1:     seed_z1 = p_seeds[0];
        }
    }
    simplerandom_lfsr113_seed(p_lfsr113, seed_z1, seed_z2, seed_z3, seed_z4);
    return num_seeds_used;
}

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

uint8_t simplerandom_lfsr113_next_uint8(SimpleRandomLFSR113_t * p_lfsr113)
{
    /* Return most-significant 8 bits. */
    return (simplerandom_lfsr113_next(p_lfsr113) >> 24u);
}

uint16_t simplerandom_lfsr113_next_uint16(SimpleRandomLFSR113_t * p_lfsr113)
{
    /* Return most-significant 16 bits. */
    return (simplerandom_lfsr113_next(p_lfsr113) >> 16u);
}

/*********
 * LFSR88
 ********/

size_t simplerandom_lfsr88_num_seeds(const SimpleRandomLFSR88_t * p_lfsr88)
{
    (const void *)p_lfsr88; /* We only use this parameter for type checking. */

    return 3u;
}

size_t simplerandom_lfsr88_seed_array(SimpleRandomLFSR88_t * p_lfsr88, const uint32_t * p_seeds, size_t num_seeds, bool mix_extras)
{
    uint32_t    seed_z1 = 0;
    uint32_t    seed_z2 = 0;
    uint32_t    seed_z3 = 0;
    size_t      num_seeds_used = 0;

    if (p_seeds != NULL)
    {
        if (num_seeds > 3u)
            num_seeds_used = 3u;
        else
            num_seeds_used = num_seeds;
        switch (num_seeds_used)
        {
            case 3:     seed_z3 = p_seeds[2];   /* drop through to next case */
            case 2:     seed_z2 = p_seeds[1];   /* drop through to next case */
            case 1:     seed_z1 = p_seeds[0];
        }
    }
    simplerandom_lfsr88_seed(p_lfsr88, seed_z1, seed_z2, seed_z3);
    return num_seeds_used;
}

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

uint8_t simplerandom_lfsr88_next_uint8(SimpleRandomLFSR88_t * p_lfsr88)
{
    /* Return most-significant 8 bits. */
    return (simplerandom_lfsr88_next(p_lfsr88) >> 24u);
}

uint16_t simplerandom_lfsr88_next_uint16(SimpleRandomLFSR88_t * p_lfsr88)
{
    /* Return most-significant 16 bits. */
    return (simplerandom_lfsr88_next(p_lfsr88) >> 16u);
}

