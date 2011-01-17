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
 * Local function prototypes
 ****************************************************************************/

static uint32_t fib_adjust_seed(uint32_t seed);


/*****************************************************************************
 * Functions
 ****************************************************************************/

/*********
 * Cong
 ********/

void simplerandom_cong_seed(SimpleRandomCong_t * p_cong, uint32_t seed)
{
    *p_cong = seed;
}

uint32_t simplerandom_cong_next(SimpleRandomCong_t * p_cong)
{
    *p_cong = UINT32_C(69069) * *p_cong + UINT32_C(1234567);

    return *p_cong;
}

/*********
 * SHR3
 ********/

void simplerandom_shr3_seed(SimpleRandomSHR3_t * p_shr3, uint32_t seed)
{
    if (seed == 0)
    {
        seed = UINT32_C(123456789);
    }

    *p_shr3 = seed;
}

uint32_t simplerandom_shr3_next(SimpleRandomSHR3_t * p_shr3)
{
    uint32_t    shr3;


    shr3 = *p_shr3;
    shr3 ^= (shr3 << 17);
    shr3 ^= (shr3 >> 13);
    shr3 ^= (shr3 << 5);
    *p_shr3 = shr3;

    return shr3;
}

/*********
 * MWC
 ********/

/* There are some bad seed values. See:
 *     http://eprint.iacr.org/2011/007.pdf
 *
 * Of course 0 is bad for either part.
 * For upper part, seed value 0x9068FFFF is bad.
 * For lower part, seed value 0x464FFFFF is bad.
 */
void simplerandom_mwc_seed(SimpleRandomMWC_t * p_mwc, uint32_t seed_upper, uint32_t seed_lower)
{
    if ((seed_upper == 0) || (seed_upper == UINT32_C(0x9068FFFF)))
    {
        seed_upper = UINT32_C(362436069);
    }
    if ((seed_lower == 0) || (seed_upper == UINT32_C(0x464FFFFF)))
    {
        seed_lower = UINT32_C(521288629);
    }

    p_mwc->mwc_upper = seed_upper;
    p_mwc->mwc_lower = seed_lower;
}

uint32_t simplerandom_mwc_next(SimpleRandomMWC_t * p_mwc)
{
    p_mwc->mwc_upper = 36969u * (p_mwc->mwc_upper & 0xFFFFu) + (p_mwc->mwc_upper >> 16u);
    p_mwc->mwc_lower = 18000u * (p_mwc->mwc_lower & 0xFFFFu) + (p_mwc->mwc_lower >> 16u);
    
    return (p_mwc->mwc_upper << 16u) + p_mwc->mwc_lower;
}

/*********
 * KISS
 ********/

void simplerandom_kiss_seed(SimpleRandomKISS_t * p_kiss, uint32_t seed_mwc_upper, uint32_t seed_mwc_lower, uint32_t seed_cong, uint32_t seed_shr3)
{
    if ((seed_mwc_upper == 0) || (seed_mwc_upper == UINT32_C(0x9068FFFF)))
    {
        seed_mwc_upper = UINT32_C(362436069);
    }
    if ((seed_mwc_lower == 0) || (seed_mwc_lower == UINT32_C(0x464FFFFF)))
    {
        seed_mwc_lower = UINT32_C(521288629);
    }
    if (seed_shr3 == 0)
    {
        seed_shr3 = UINT32_C(123456789);
    }

    p_kiss->mwc_upper = seed_mwc_upper;
    p_kiss->mwc_lower = seed_mwc_lower;
    p_kiss->cong = seed_cong;
    p_kiss->shr3 = seed_shr3;
}

uint32_t simplerandom_kiss_next(SimpleRandomKISS_t * p_kiss)
{
    uint32_t    shr3;
    uint32_t    mwc;


    p_kiss->mwc_upper = 36969u * (p_kiss->mwc_upper & 0xFFFFu) + (p_kiss->mwc_upper >> 16u);
    p_kiss->mwc_lower = 18000u * (p_kiss->mwc_lower & 0xFFFFu) + (p_kiss->mwc_lower >> 16u);
    mwc = (p_kiss->mwc_upper << 16u) + p_kiss->mwc_lower;

    p_kiss->cong = UINT32_C(69069) * p_kiss->cong + UINT32_C(1234567);

    shr3 = p_kiss->shr3;
    shr3 ^= (shr3 << 17);
    shr3 ^= (shr3 >> 13);
    shr3 ^= (shr3 << 5);
    p_kiss->shr3 = shr3;

    return ((mwc ^ p_kiss->cong) + shr3);
}

/*********
 * LFIB4
 ********/

/* LFIB4 seed
 *
 * It is not practical to pass the 256 seed values to this
 * function as parameters. Before calling this function,
 * initialise p_lfib4->t[] with 256 good-quality
 * [pseudo]random values.
 */
void simplerandom_lfib4_seed(SimpleRandomLFIB4_t * p_lfib4)
{
    p_lfib4->c = 0;
}

void simplerandom_lfib4_seed_from_kiss(SimpleRandomLFIB4_t * p_lfib4, uint32_t seed_mwc_upper, uint32_t seed_mwc_lower, uint32_t seed_cong, uint32_t seed_shr3)
{
    SimpleRandomKISS_t  kiss;
    unsigned int        i;

    simplerandom_kiss_seed(&kiss, seed_mwc_upper, seed_mwc_lower, seed_cong, seed_shr3);
    for (i = 0; i < 256; i++)
    {
        p_lfib4->t[i] = simplerandom_kiss_next(&kiss);
    }
    p_lfib4->c = 0;
}

uint32_t simplerandom_lfib4_next(SimpleRandomLFIB4_t * p_lfib4)
{
    uint32_t    new_val;
    uint8_t     c;

    c = ++(p_lfib4->c);
    new_val =  (p_lfib4->t[c] +
                p_lfib4->t[(uint8_t)(c + 58)] +
                p_lfib4->t[(uint8_t)(c + 119)] +
                p_lfib4->t[(uint8_t)(c + 178)]);
    p_lfib4->t[c] = new_val;

    return new_val;
}

/*********
 * SWB
 ********/

/* SWB seed
 *
 * It is not practical to pass the 256 seed values to this
 * function as parameters. Before calling this function,
 * initialise p_lfib4->t[] with 256 good-quality
 * [pseudo]random values.
 */
void simplerandom_swb_seed(SimpleRandomSWB_t * p_swb)
{
    p_swb->c = 0;
    p_swb->borrow = 0;
}

void simplerandom_swb_seed_from_kiss(SimpleRandomSWB_t * p_swb, uint32_t seed_mwc_upper, uint32_t seed_mwc_lower, uint32_t seed_cong, uint32_t seed_shr3)
{
    SimpleRandomKISS_t  kiss;
    unsigned int        i;

    simplerandom_kiss_seed(&kiss, seed_mwc_upper, seed_mwc_lower, seed_cong, seed_shr3);
    for (i = 0; i < 256; i++)
    {
        p_swb->t[i] = simplerandom_kiss_next(&kiss);
    }
    p_swb->c = 0;
    p_swb->borrow = 0;
}

uint32_t simplerandom_swb_next(SimpleRandomSWB_t * p_swb)
{
    uint32_t    x;
    uint32_t    y;
    uint32_t    new_val;
    uint8_t     c;

    c = ++(p_swb->c);

    x = p_swb->t[(uint8_t)(c + 34)];
    y = p_swb->t[(uint8_t)(c + 19)] + p_swb->borrow;
    new_val = x - y;
    p_swb->t[c] = new_val;
    if (x < y)
    {
        p_swb->borrow = 1;
    }
    else
    {
        p_swb->borrow = 0;
    }

    return new_val;
}

/*********
 * Fib
 ********/

static uint32_t fib_adjust_seed(uint32_t seed)
{
    if ((seed % 2) == 0)
    {
        seed++;
    }
    if ((seed % 8) == 1)
    {
        seed += 2;
    }

    return seed;
}

void simplerandom_fib_seed(SimpleRandomFib_t * p_fib, uint32_t seed_a, uint32_t seed_b)
{
    if (seed_a == 0)
    {
        seed_a = UINT32_C(1468761293);
    }
    if (seed_b == 0)
    {
        seed_b = UINT32_C(3460192787);
    }
    if (fib_adjust_seed(seed_a) != seed_a)
    {
        seed_b = fib_adjust_seed(seed_b);
    }

    p_fib->fib_a = seed_a;
    p_fib->fib_b = seed_b;
}

uint32_t simplerandom_fib_next(SimpleRandomFib_t * p_fib)
{
    uint32_t    old_b;
    

    old_b = p_fib->fib_b;
    p_fib->fib_b = p_fib->fib_a + old_b;
    p_fib->fib_a = old_b;

    return old_b;
}

/*********
 * Cong2
 ********/

void simplerandom_cong2_seed(SimpleRandomCong2_t * p_cong2, uint32_t seed)
{
    *p_cong2 = seed;
}

uint32_t simplerandom_cong2_next(SimpleRandomCong2_t * p_cong2)
{
    *p_cong2 = UINT32_C(69069) * *p_cong2 + 12345u;

    return *p_cong2;
}

/*********
 * SHR3_2
 ********/

void simplerandom_shr3_2_seed(SimpleRandomSHR3_2_t * p_shr3_2, uint32_t seed)
{
    if (seed == 0)
    {
        seed = UINT32_C(362436000);
    }

    *p_shr3_2 = seed;
}

uint32_t simplerandom_shr3_2_next(SimpleRandomSHR3_2_t * p_shr3_2)
{
    uint32_t    shr3_2;


    shr3_2 = *p_shr3_2;
    shr3_2 ^= (shr3_2 << 13);
    shr3_2 ^= (shr3_2 >> 17);
    shr3_2 ^= (shr3_2 << 5);
    *p_shr3_2 = shr3_2;

    return shr3_2;
}

#ifdef UINT64_C

/*********
 * MWC64
 ********/

void simplerandom_mwc64_seed(SimpleRandomMWC64_t * p_mwc64, uint32_t seed_upper, uint32_t seed_lower)
{
    if ((seed_upper == 0) && (seed_lower == 0))
    {
        seed_upper = UINT32_C(7654321);
        seed_lower = UINT32_C(521288629);
    }

    p_mwc64->mwc64_upper = seed_upper;
    p_mwc64->mwc64_lower = seed_lower;
}

uint32_t simplerandom_mwc64_next(SimpleRandomMWC64_t * p_mwc64)
{
    uint64_t    mwc64;


    mwc64 = UINT64_C(698769069) * p_mwc64->mwc64_lower + p_mwc64->mwc64_upper;
    p_mwc64->mwc64_upper = (mwc64 >> 32u);
    p_mwc64->mwc64_lower = (uint32_t)mwc64;

    return (uint32_t)mwc64;
}

/*********
 * KISS2
 ********/

void simplerandom_kiss2_seed(SimpleRandomKISS2_t * p_kiss2, uint32_t seed_mwc64_upper, uint32_t seed_mwc64_lower, uint32_t seed_cong2, uint32_t seed_shr3_2)
{
    if ((seed_mwc64_upper == 0) && (seed_mwc64_lower == 0))
    {
        seed_mwc64_upper = UINT32_C(7654321);
        seed_mwc64_lower = UINT32_C(521288629);
    }
    if (seed_shr3_2 == 0)
    {
        seed_shr3_2 = UINT32_C(362436000);
    }

    p_kiss2->mwc64_upper = seed_mwc64_upper;
    p_kiss2->mwc64_lower = seed_mwc64_lower;
    p_kiss2->cong2 = seed_cong2;
    p_kiss2->shr3_2 = seed_shr3_2;
}

uint32_t simplerandom_kiss2_next(SimpleRandomKISS2_t * p_kiss2)
{
    uint64_t    mwc64;
    uint32_t    shr3_2;


    mwc64 = UINT64_C(698769069) * p_kiss2->mwc64_lower + p_kiss2->mwc64_upper;
    p_kiss2->mwc64_upper = (mwc64 >> 32u);
    p_kiss2->mwc64_lower = (uint32_t)mwc64;

    p_kiss2->cong2 = UINT32_C(69069) * p_kiss2->cong2 + 12345u;

    shr3_2 = p_kiss2->shr3_2;
    shr3_2 ^= (shr3_2 << 13);
    shr3_2 ^= (shr3_2 >> 17);
    shr3_2 ^= (shr3_2 << 5);
    p_kiss2->shr3_2 = shr3_2;

    return ((uint32_t)mwc64 + p_kiss2->cong2 + shr3_2);
}

#endif /* defined(UINT64_C) */

