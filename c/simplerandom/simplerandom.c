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

static inline void mwc2_sanitize_upper(SimpleRandomMWC2_t * p_mwc);
static inline void mwc2_sanitize_lower(SimpleRandomMWC2_t * p_mwc);


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

    if (mix_extras && p_seeds != NULL)
    {
        simplerandom_cong_mix(p_cong, p_seeds + num_seeds_used, num_seeds - num_seeds_used);
        num_seeds_used = num_seeds;
    }
    return num_seeds_used;
}

void simplerandom_cong_seed(SimpleRandomCong_t * p_cong, uint32_t seed)
{
    p_cong->cong = seed;
#if 0
    /* Not needed because for Cong, all state values are valid. */
    simplerandom_cong_sanitize(p_cong);
#endif
}

void simplerandom_cong_sanitize(SimpleRandomCong_t * p_cong)
{
    /* All state values are valid for Cong. No sanitizing needed. */
    (const void *) p_cong;
}

void simplerandom_cong_mix(SimpleRandomCong_t * p_cong, const uint32_t * p_data, size_t num_data)
{
    if (p_data != NULL)
    {
        while (num_data)
        {
            --num_data;
            p_cong->cong ^= *p_data++;
            simplerandom_cong_sanitize(p_cong);
            simplerandom_cong_next(p_cong);
        }
    }
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

    if (mix_extras && p_seeds != NULL)
    {
        simplerandom_shr3_mix(p_shr3, p_seeds + num_seeds_used, num_seeds - num_seeds_used);
        num_seeds_used = num_seeds;
    }
    return num_seeds_used;
}

void simplerandom_shr3_seed(SimpleRandomSHR3_t * p_shr3, uint32_t seed)
{
    p_shr3->shr3 = seed;
    simplerandom_shr3_sanitize(p_shr3);
}

void simplerandom_shr3_sanitize(SimpleRandomSHR3_t * p_shr3)
{
    /* Zero is a bad state value for SHR3. */
    if (p_shr3->shr3 == 0)
    {
        p_shr3->shr3 = UINT32_C(0xFFFFFFFF);
    }
}

void simplerandom_shr3_mix(SimpleRandomSHR3_t * p_shr3, const uint32_t * p_data, size_t num_data)
{
    if (p_data != NULL)
    {
        while (num_data)
        {
            --num_data;
            p_shr3->shr3 ^= *p_data++;
            simplerandom_shr3_sanitize(p_shr3);
            simplerandom_shr3_next(p_shr3);
        }
    }
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

    if (mix_extras && p_seeds != NULL)
    {
        simplerandom_mwc1_mix(p_mwc, p_seeds + num_seeds_used, num_seeds - num_seeds_used);
        num_seeds_used = num_seeds;
    }
    return num_seeds_used;
}

/*
 * See notes for simplerandom_mwc2_seed().
 */
void simplerandom_mwc1_seed(SimpleRandomMWC1_t * p_mwc, uint32_t seed_upper, uint32_t seed_lower)
{
    simplerandom_mwc2_seed(p_mwc, seed_upper, seed_lower);
}

void simplerandom_mwc1_sanitize(SimpleRandomMWC1_t * p_mwc)
{
    simplerandom_mwc2_sanitize(p_mwc);
}

static inline uint32_t mwc1_current(SimpleRandomMWC1_t * p_mwc)
{
    return (p_mwc->mwc_upper << 16u) + p_mwc->mwc_lower;
}

void simplerandom_mwc1_mix(SimpleRandomMWC1_t * p_mwc, const uint32_t * p_data, size_t num_data)
{
    uint32_t    current;

    if (p_data != NULL)
    {
        current = mwc1_current(p_mwc);
        while (num_data)
        {
            --num_data;
            switch ((current >> 31u) & 0x1)     /* Switch on 1 highest bit */
            {
                case 0:
                    p_mwc->mwc_upper ^= *p_data;
                    mwc2_sanitize_upper(p_mwc);
                    break;
                case 1:
                    p_mwc->mwc_lower ^= *p_data;
                    mwc2_sanitize_lower(p_mwc);
                    break;
            }
            ++p_data;
            current = simplerandom_mwc1_next(p_mwc);
        }
    }
}

uint32_t simplerandom_mwc1_next(SimpleRandomMWC1_t * p_mwc)
{
    p_mwc->mwc_upper = 36969u * (p_mwc->mwc_upper & 0xFFFFu) + (p_mwc->mwc_upper >> 16u);
    p_mwc->mwc_lower = 18000u * (p_mwc->mwc_lower & 0xFFFFu) + (p_mwc->mwc_lower >> 16u);
    
    return mwc1_current(p_mwc);
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
    simplerandom_mwc2_seed(p_mwc, seed_upper, seed_lower);

    if (mix_extras && p_seeds != NULL)
    {
        simplerandom_mwc2_mix(p_mwc, p_seeds + num_seeds_used, num_seeds - num_seeds_used);
        num_seeds_used = num_seeds;
    }
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
    p_mwc->mwc_upper = seed_upper;
    p_mwc->mwc_lower = seed_lower;
    simplerandom_mwc2_sanitize(p_mwc);
}

static inline void mwc2_sanitize_upper(SimpleRandomMWC2_t * p_mwc)
{
    uint32_t    temp;

    temp = p_mwc->mwc_upper;
    if ((temp == 0) || (temp == UINT32_C(0x9068FFFF)))
    {
        p_mwc->mwc_upper = temp ^ UINT32_C(0xFFFFFFFF);
    }
}

static inline void mwc2_sanitize_lower(SimpleRandomMWC2_t * p_mwc)
{
    uint32_t    temp;

    temp = p_mwc->mwc_lower;
    if ((temp == 0) ||  (temp == UINT32_C(0x464FFFFF) * 1u) ||
                        (temp == UINT32_C(0x464FFFFF) * 2u) ||
                        (temp == UINT32_C(0x464FFFFF) * 3u))
    {
        p_mwc->mwc_lower = temp ^ UINT32_C(0xFFFFFFFF);
    }
}

void simplerandom_mwc2_sanitize(SimpleRandomMWC2_t * p_mwc)
{
    mwc2_sanitize_upper(p_mwc);
    mwc2_sanitize_lower(p_mwc);
}

static inline uint32_t mwc2_current(SimpleRandomMWC2_t * p_mwc)
{
    return (p_mwc->mwc_upper << 16u) + (p_mwc->mwc_upper >> 16u) + p_mwc->mwc_lower;
}

void simplerandom_mwc2_mix(SimpleRandomMWC2_t * p_mwc, const uint32_t * p_data, size_t num_data)
{
    uint32_t    current;

    if (p_data != NULL)
    {
        current = mwc2_current(p_mwc);
        while (num_data)
        {
            --num_data;
            switch ((current >> 31u) & 0x1)     /* Switch on 1 highest bit */
            {
                case 0:
                    p_mwc->mwc_upper ^= *p_data;
                    mwc2_sanitize_upper(p_mwc);
                    break;
                case 1:
                    p_mwc->mwc_lower ^= *p_data;
                    mwc2_sanitize_lower(p_mwc);
                    break;
            }
            ++p_data;
            current = simplerandom_mwc2_next(p_mwc);
        }
    }
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

    return mwc2_current(p_mwc);
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

    if (mix_extras && p_seeds != NULL)
    {
        simplerandom_kiss_mix(p_kiss, p_seeds + num_seeds_used, num_seeds - num_seeds_used);
        num_seeds_used = num_seeds;
    }
    return num_seeds_used;
}

void simplerandom_kiss_seed(SimpleRandomKISS_t * p_kiss, uint32_t seed_mwc_upper, uint32_t seed_mwc_lower, uint32_t seed_cong, uint32_t seed_shr3)
{
    p_kiss->mwc_upper = seed_mwc_upper;
    p_kiss->mwc_lower = seed_mwc_lower;
    p_kiss->cong = seed_cong;
    p_kiss->shr3 = seed_shr3;
    simplerandom_kiss_sanitize(p_kiss);
}

static inline void kiss_sanitize_mwc_upper(SimpleRandomKISS_t * p_kiss)
{
    uint32_t    temp;

    temp = p_kiss->mwc_upper;
    if ((temp == 0) || (temp == UINT32_C(0x9068FFFF)))
    {
        p_kiss->mwc_upper = temp ^ UINT32_C(0xFFFFFFFF);
    }
}

static inline void kiss_sanitize_mwc_lower(SimpleRandomKISS_t * p_kiss)
{
    uint32_t    temp;

    temp = p_kiss->mwc_lower;
    if ((temp == 0) ||  (temp == UINT32_C(0x464FFFFF) * 1u) ||
                        (temp == UINT32_C(0x464FFFFF) * 2u) ||
                        (temp == UINT32_C(0x464FFFFF) * 3u))
    {
        p_kiss->mwc_lower = temp ^ UINT32_C(0xFFFFFFFF);
    }
}

static inline void kiss_sanitize_shr3(SimpleRandomKISS_t * p_kiss)
{
    /* Zero is a bad state value for SHR3. */
    if (p_kiss->shr3 == 0)
    {
        p_kiss->shr3 = UINT32_C(0xFFFFFFFF);
    }
}

void simplerandom_kiss_sanitize(SimpleRandomKISS_t * p_kiss)
{
    kiss_sanitize_mwc_upper(p_kiss);
    kiss_sanitize_mwc_lower(p_kiss);
    /* No sanitize needed for Cong, because all state values are valid. */
    kiss_sanitize_shr3(p_kiss);
}

static inline uint32_t kiss_current(SimpleRandomKISS_t * p_kiss)
{
    uint32_t    mwc2;
    uint32_t    cong;
    uint32_t    shr3;


    mwc2 = (p_kiss->mwc_upper << 16u) + (p_kiss->mwc_upper >> 16u) + p_kiss->mwc_lower;
    cong = p_kiss->cong;
    shr3 = p_kiss->shr3;
    return ((mwc2 ^ cong) + shr3);
}

void simplerandom_kiss_mix(SimpleRandomKISS_t * p_kiss, const uint32_t * p_data, size_t num_data)
{
    uint32_t    current;

    if (p_data != NULL)
    {
        current = kiss_current(p_kiss);
        while (num_data)
        {
            --num_data;
            switch ((current >> 30) & 0x3)  /* Switch on 2 highest bits */
            {
                case 0:
                    p_kiss->mwc_upper ^= *p_data;
                    kiss_sanitize_mwc_upper(p_kiss);
                    break;
                case 1:
                    p_kiss->mwc_lower ^= *p_data;
                    kiss_sanitize_mwc_lower(p_kiss);
                    break;
                case 2:
                    p_kiss->cong ^= *p_data;
                    break;
                case 3:
                    p_kiss->shr3 ^= *p_data;
                    kiss_sanitize_shr3(p_kiss);
                    break;
            }
            ++p_data;
            current = simplerandom_kiss_next(p_kiss);
        }
    }
}

uint32_t simplerandom_kiss_next(SimpleRandomKISS_t * p_kiss)
{
    uint32_t    cong;
    uint32_t    shr3;


    /* Calculate next MWC2 RNG */
    p_kiss->mwc_upper = 36969u * (p_kiss->mwc_upper & 0xFFFFu) + (p_kiss->mwc_upper >> 16u);
    p_kiss->mwc_lower = 18000u * (p_kiss->mwc_lower & 0xFFFFu) + (p_kiss->mwc_lower >> 16u);

    /* Calculate next Cong RNG */
    cong = UINT32_C(69069) * p_kiss->cong + 12345u;
    p_kiss->cong = cong;

    /* Calculate next SHR3 RNG */
    shr3 = p_kiss->shr3;
    shr3 ^= (shr3 << 13);
    shr3 ^= (shr3 >> 17);
    shr3 ^= (shr3 << 5);
    p_kiss->shr3 = shr3;

    return kiss_current(p_kiss);
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

    if (mix_extras && p_seeds != NULL)
    {
        simplerandom_mwc64_mix(p_mwc, p_seeds + num_seeds_used, num_seeds - num_seeds_used);
        num_seeds_used = num_seeds;
    }
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
    p_mwc->mwc_upper = seed_upper;
    p_mwc->mwc_lower = seed_lower;
    simplerandom_mwc64_sanitize(p_mwc);
}

void simplerandom_mwc64_sanitize(SimpleRandomMWC64_t * p_mwc)
{
    uint64_t    seed64;

    seed64 = ((uint64_t)p_mwc->mwc_upper << 32u) + p_mwc->mwc_lower;
    if ((seed64 % UINT64_C(0x29A65EACFFFFFFFF)) == 0)
    {
        /* Invert both upper and lower to get a good seed. */
        p_mwc->mwc_upper ^= UINT32_C(0xFFFFFFFF);
        p_mwc->mwc_lower ^= UINT32_C(0xFFFFFFFF);
    }
}

static inline uint32_t mwc64_current(SimpleRandomMWC64_t * p_mwc)
{
    return p_mwc->mwc_lower;
}

void simplerandom_mwc64_mix(SimpleRandomMWC64_t * p_mwc, const uint32_t * p_data, size_t num_data)
{
    uint32_t    current;

    if (p_data != NULL)
    {
        current = mwc64_current(p_mwc);
        while (num_data)
        {
            --num_data;
            switch ((current >> 31u) & 0x1)     /* Switch on 1 highest bit */
            {
                case 0:
                    p_mwc->mwc_upper ^= *p_data;
                    break;
                case 1:
                    p_mwc->mwc_lower ^= *p_data;
                    break;
            }
            simplerandom_mwc64_sanitize(p_mwc);
            ++p_data;
            current = simplerandom_mwc64_next(p_mwc);
        }
    }
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

    if (mix_extras && p_seeds != NULL)
    {
        simplerandom_kiss2_mix(p_kiss2, p_seeds + num_seeds_used, num_seeds - num_seeds_used);
        num_seeds_used = num_seeds;
    }
    return num_seeds_used;
}

void simplerandom_kiss2_seed(SimpleRandomKISS2_t * p_kiss2, uint32_t seed_mwc_upper, uint32_t seed_mwc_lower, uint32_t seed_cong, uint32_t seed_shr3)
{
    p_kiss2->mwc_upper = seed_mwc_upper;
    p_kiss2->mwc_lower = seed_mwc_lower;
    p_kiss2->cong = seed_cong;
    p_kiss2->shr3 = seed_shr3;
    simplerandom_kiss2_sanitize(p_kiss2);
}

static inline void kiss2_sanitize_mwc64(SimpleRandomKISS2_t * p_kiss2)
{
    uint64_t    seed64;

    seed64 = ((uint64_t)p_kiss2->mwc_upper << 32u) + p_kiss2->mwc_lower;
    if ((seed64 % UINT64_C(0x29A65EACFFFFFFFF)) == 0)
    {
        /* Invert both upper and lower to get a good seed. */
        p_kiss2->mwc_upper ^= UINT32_C(0xFFFFFFFF);
        p_kiss2->mwc_lower ^= UINT32_C(0xFFFFFFFF);
    }
}

static inline void kiss2_sanitize_shr3(SimpleRandomKISS2_t * p_kiss2)
{
    /* Zero is a bad state value for SHR3. */
    if (p_kiss2->shr3 == 0)
    {
        p_kiss2->shr3 = UINT32_C(0xFFFFFFFF);
    }
}

void simplerandom_kiss2_sanitize(SimpleRandomKISS2_t * p_kiss2)
{
    kiss2_sanitize_mwc64(p_kiss2);
    /* No sanitize needed for Cong, because all state values are valid. */
    kiss2_sanitize_shr3(p_kiss2);
}

static inline uint32_t kiss2_current(SimpleRandomKISS2_t * p_kiss2)
{
    return (p_kiss2->mwc_lower + p_kiss2->cong + p_kiss2->shr3);
}

void simplerandom_kiss2_mix(SimpleRandomKISS2_t * p_kiss2, const uint32_t * p_data, size_t num_data)
{
    uint32_t    current;

    if (p_data != NULL)
    {
        current = kiss2_current(p_kiss2);
        while (num_data)
        {
            --num_data;
            switch ((current >> 30) & 0x3)  /* Switch on 2 highest bits */
            {
                case 0:
                    p_kiss2->mwc_upper ^= *p_data;
                    kiss2_sanitize_mwc64(p_kiss2);
                    break;
                case 1:
                    p_kiss2->mwc_lower ^= *p_data;
                    kiss2_sanitize_mwc64(p_kiss2);
                    break;
                case 2:
                    p_kiss2->cong ^= *p_data;
                    break;
                case 3:
                    p_kiss2->shr3 ^= *p_data;
                    kiss2_sanitize_shr3(p_kiss2);
                    break;
            }
            ++p_data;
            current = simplerandom_kiss2_next(p_kiss2);
        }
    }
}

uint32_t simplerandom_kiss2_next(SimpleRandomKISS2_t * p_kiss2)
{
    uint64_t    mwc64;
    uint32_t    cong;
    uint32_t    shr3;


    /* Calculate next MWC64 RNG */
    mwc64 = UINT64_C(698769069) * p_kiss2->mwc_lower + p_kiss2->mwc_upper;
    p_kiss2->mwc_upper = (mwc64 >> 32u);
    p_kiss2->mwc_lower = (uint32_t)mwc64;

    /* Calculate next Cong RNG */
    cong = UINT32_C(69069) * p_kiss2->cong + 12345u;
    p_kiss2->cong = cong;

    /* Calculate next SHR3 RNG */
    shr3 = p_kiss2->shr3;
    shr3 ^= (shr3 << 13);
    shr3 ^= (shr3 >> 17);
    shr3 ^= (shr3 << 5);
    p_kiss2->shr3 = shr3;

    return ((uint32_t)mwc64 + cong + shr3);
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

    if (mix_extras && p_seeds != NULL)
    {
        simplerandom_lfsr113_mix(p_lfsr113, p_seeds + num_seeds_used, num_seeds - num_seeds_used);
        num_seeds_used = num_seeds;
    }
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

static inline void lfsr113_sanitize_z1(SimpleRandomLFSR113_t * p_lfsr113)
{
    uint32_t    working_seed;

    working_seed = p_lfsr113->z1;
    if (working_seed < LFSR_SEED_Z1_MIN_VALUE)
    {
        p_lfsr113->z1 = ~working_seed;
    }
}

static inline void lfsr113_sanitize_z2(SimpleRandomLFSR113_t * p_lfsr113)
{
    uint32_t    working_seed;

    working_seed = p_lfsr113->z2;
    if (working_seed < LFSR_SEED_Z2_MIN_VALUE)
    {
        p_lfsr113->z2 = ~working_seed;
    }
}

static inline void lfsr113_sanitize_z3(SimpleRandomLFSR113_t * p_lfsr113)
{
    uint32_t    working_seed;

    working_seed = p_lfsr113->z3;
    if (working_seed < LFSR_SEED_Z3_MIN_VALUE)
    {
        p_lfsr113->z3 = ~working_seed;
    }
}

static inline void lfsr113_sanitize_z4(SimpleRandomLFSR113_t * p_lfsr113)
{
    uint32_t    working_seed;

    working_seed = p_lfsr113->z4;
    if (working_seed < LFSR_SEED_Z4_MIN_VALUE)
    {
        p_lfsr113->z4 = ~working_seed;
    }
}

void simplerandom_lfsr113_sanitize(SimpleRandomLFSR113_t * p_lfsr113)
{
    lfsr113_sanitize_z1(p_lfsr113);
    lfsr113_sanitize_z2(p_lfsr113);
    lfsr113_sanitize_z3(p_lfsr113);
    lfsr113_sanitize_z4(p_lfsr113);
}

static inline uint32_t lfsr113_current(SimpleRandomLFSR113_t * p_lfsr113)
{
    return (p_lfsr113->z1 ^ p_lfsr113->z2 ^ p_lfsr113->z3 ^ p_lfsr113->z4);
}

void simplerandom_lfsr113_mix(SimpleRandomLFSR113_t * p_lfsr113, const uint32_t * p_data, size_t num_data)
{
    uint32_t    current;

    if (p_data != NULL)
    {
        current = lfsr113_current(p_lfsr113);
        while (num_data)
        {
            --num_data;
            switch ((current >> 30) & 0x3)  /* Switch on 2 highest bits */
            {
                case 0:
                    p_lfsr113->z1 ^= *p_data;
                    lfsr113_sanitize_z1(p_lfsr113);
                    break;
                case 1:
                    p_lfsr113->z2 ^= *p_data;
                    lfsr113_sanitize_z2(p_lfsr113);
                    break;
                case 2:
                    p_lfsr113->z3 ^= *p_data;
                    lfsr113_sanitize_z3(p_lfsr113);
                    break;
                case 3:
                    p_lfsr113->z4 ^= *p_data;
                    lfsr113_sanitize_z4(p_lfsr113);
                    break;
            }
            ++p_data;
            current = simplerandom_lfsr113_next(p_lfsr113);
        }
    }
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

    if (mix_extras && p_seeds != NULL)
    {
        simplerandom_lfsr88_mix(p_lfsr88, p_seeds + num_seeds_used, num_seeds - num_seeds_used);
        num_seeds_used = num_seeds;
    }
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

static inline void lfsr88_sanitize_z1(SimpleRandomLFSR88_t * p_lfsr88)
{
    uint32_t    working_seed;

    working_seed = p_lfsr88->z1;
    if (working_seed < LFSR_SEED_Z1_MIN_VALUE)
    {
        p_lfsr88->z1 = ~working_seed;
    }
}

static inline void lfsr88_sanitize_z2(SimpleRandomLFSR88_t * p_lfsr88)
{
    uint32_t    working_seed;

    working_seed = p_lfsr88->z2;
    if (working_seed < LFSR_SEED_Z2_MIN_VALUE)
    {
        p_lfsr88->z2 = ~working_seed;
    }
}

static inline void lfsr88_sanitize_z3(SimpleRandomLFSR88_t * p_lfsr88)
{
    uint32_t    working_seed;

    working_seed = p_lfsr88->z3;
    if (working_seed < LFSR_SEED_Z3_MIN_VALUE)
    {
        p_lfsr88->z3 = ~working_seed;
    }
}

void simplerandom_lfsr88_sanitize(SimpleRandomLFSR88_t * p_lfsr88)
{
    lfsr88_sanitize_z1(p_lfsr88);
    lfsr88_sanitize_z2(p_lfsr88);
    lfsr88_sanitize_z3(p_lfsr88);
}

static inline uint32_t lfsr88_current(SimpleRandomLFSR88_t * p_lfsr88)
{
    return (p_lfsr88->z1 ^ p_lfsr88->z2 ^ p_lfsr88->z3);
}

void simplerandom_lfsr88_mix(SimpleRandomLFSR88_t * p_lfsr88, const uint32_t * p_data, size_t num_data)
{
    uint32_t    current;

    if (p_data != NULL)
    {
        current = lfsr88_current(p_lfsr88);
        while (num_data)
        {
            --num_data;
            if (current < UINT32_C(1431655765))         /* constant is 2^32 / 3 */
            {
                p_lfsr88->z1 ^= *p_data;
                lfsr88_sanitize_z1(p_lfsr88);
            }
            else if (current < UINT32_C(2863311531))    /* constant is 2^32 * 2 / 3 */
            {
                p_lfsr88->z2 ^= *p_data;
                lfsr88_sanitize_z2(p_lfsr88);
            }
            else
            {
                p_lfsr88->z3 ^= *p_data;
                lfsr88_sanitize_z3(p_lfsr88);
            }
            ++p_data;
            current = simplerandom_lfsr88_next(p_lfsr88);
        }
    }
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

