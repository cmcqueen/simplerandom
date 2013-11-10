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
    /* No sanitize is needed because for Cong, all state values are valid. */
}

void simplerandom_cong_sanitize(SimpleRandomCong_t * p_cong)
{
    /* All state values are valid for Cong. No sanitizing needed. */
    (const void *) p_cong;
}

uint32_t simplerandom_cong_next(SimpleRandomCong_t * p_cong)
{
    uint32_t    cong;

    cong = UINT32_C(69069) * p_cong->cong + 12345u;
    p_cong->cong = cong;

    return cong;
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

/*********
 * MWC2
 *
 * MWC1 and MWC2 are very similar, apart from deriving the final random value
 * from the state. So they can share code. MWC2 is preferred, so we put MWC2
 * first, and then MWC1 can call some MWC2 functions.
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
        seed_upper = (num_seeds >= 1) ? p_seeds[0] : 0;
        seed_lower = (num_seeds >= 2) ? p_seeds[1] : seed_upper;
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

/* The good state values are all modulo 0x9068FFFF. Values above that would
 * jump to a corresponding good state value on the first "next" operation.
 * Any integer multiple of 0x9068FFFF, including 0, is a bad state.
 */
static inline void mwc2_sanitize_upper(SimpleRandomMWC2_t * p_mwc)
{
    uint32_t    state_orig;
    uint32_t    temp;

    state_orig = p_mwc->mwc_upper;
    temp = state_orig;
    /* The following is equivalent to % 0x9068FFFF, without using modulo
     * operation which may be expensive on embedded targets. For
     * uint32_t and this divisor, we only need 'if' rather than 'while'. */
    if (temp >= UINT32_C(0x9068FFFF))
        temp -= UINT32_C(0x9068FFFF);
    if (temp == 0)
    {
        /* Any integer multiple of 0x9068FFFF, including 0, is a bad state.
         * Use an alternate state value by inverting the original value. */
        temp = state_orig ^ UINT32_C(0xFFFFFFFF);
        if (temp >= UINT32_C(0x9068FFFF))
            temp -= UINT32_C(0x9068FFFF);
    }
    p_mwc->mwc_upper = temp;
}

/* The good state values are all modulo 0x9068FFFF. Values above that would
 * jump to a corresponding good state value on the first "next" operation.
 * Any integer multiple of 0x464FFFFF, including 0, is a bad state.
 */
static inline void mwc2_sanitize_lower(SimpleRandomMWC2_t * p_mwc)
{
    uint32_t    state_orig;
    uint32_t    temp;

    state_orig = p_mwc->mwc_lower;
    temp = state_orig;
    /* The following is equivalent to % 0x464FFFFF, without using modulo
     * operation which may be expensive on embedded targets. For
     * uint32_t and this divisor, it may loop up to 3 times. */
    while (temp >= UINT32_C(0x464FFFFF))
        temp -= UINT32_C(0x464FFFFF);
    if (temp == 0)
    {
        /* Any integer multiple of 0x464FFFFF, including 0, is a bad state.
         * Use an alternate state value by inverting the original value. */
        temp = state_orig ^ UINT32_C(0xFFFFFFFF);
        while (temp >= UINT32_C(0x464FFFFF))
            temp -= UINT32_C(0x464FFFFF);
    }

    p_mwc->mwc_lower = temp;
}

void simplerandom_mwc2_sanitize(SimpleRandomMWC2_t * p_mwc)
{
    mwc2_sanitize_upper(p_mwc);
    mwc2_sanitize_lower(p_mwc);
}

static inline void mwc2_next_upper(SimpleRandomMWC2_t * p_mwc)
{
    p_mwc->mwc_upper = 36969u * (p_mwc->mwc_upper & 0xFFFFu) + (p_mwc->mwc_upper >> 16u);
}

static inline void mwc2_next_lower(SimpleRandomMWC2_t * p_mwc)
{
    p_mwc->mwc_lower = 18000u * (p_mwc->mwc_lower & 0xFFFFu) + (p_mwc->mwc_lower >> 16u);
}

/*
 * This is almost identical to simplerandom_mwc1_next(), except that when
 * combining the upper and lower values in the last step, the upper 16 bits of
 * mwc_upper are added in too, instead of just being discarded.
 */
uint32_t simplerandom_mwc2_next(SimpleRandomMWC2_t * p_mwc)
{
    mwc2_next_upper(p_mwc);
    mwc2_next_lower(p_mwc);
    return mwc2_current(p_mwc);
}

void simplerandom_mwc2_mix(SimpleRandomMWC2_t * p_mwc, const uint32_t * p_data, size_t num_data)
{
    uint32_t    current;

    if (p_data != NULL)
    {
        while (num_data)
        {
            --num_data;
            current = mwc2_current(p_mwc);
            switch ((current >> 24u) & 0x1u)    /* Switch on 1 high bit */
            {
                case 0:
                    p_mwc->mwc_upper ^= *p_data;
                    mwc2_sanitize_upper(p_mwc);
                    mwc2_next_upper(p_mwc);
                    break;
                case 1:
                    p_mwc->mwc_lower ^= *p_data;
                    mwc2_sanitize_lower(p_mwc);
                    mwc2_next_lower(p_mwc);
                    break;
            }
            ++p_data;
        }
    }
}

/*********
 * MWC1
 *
 * MWC1 is very similar to MWC2, so many functions can be shared.
 ********/

size_t simplerandom_mwc1_num_seeds(const SimpleRandomMWC1_t * p_mwc)
{
    (const void *)p_mwc;    /* We only use this parameter for type checking. */

    return 2u;
}

/* This is almost identical to simplerandom_mwc2_seed_array(), except the mix
 * function is slightly different because it depends on random values to decide
 * which state value to mix into.
 */
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
        seed_upper = (num_seeds >= 1) ? p_seeds[0] : 0;
        seed_lower = (num_seeds >= 2) ? p_seeds[1] : seed_upper;
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

uint32_t simplerandom_mwc1_next(SimpleRandomMWC1_t * p_mwc)
{
    mwc2_next_upper(p_mwc);
    mwc2_next_lower(p_mwc);
    return mwc1_current(p_mwc);
}

/* This is nearly identical to the MWC2 mix function, except for the call to
 * mwc1_current() which is the essence of the difference between MWC1 and MWC2.
 */
void simplerandom_mwc1_mix(SimpleRandomMWC1_t * p_mwc, const uint32_t * p_data, size_t num_data)
{
    uint32_t    current;

    if (p_data != NULL)
    {
        while (num_data)
        {
            --num_data;
            current = mwc1_current(p_mwc);
            switch ((current >> 24u) & 0x1u)    /* Switch on 1 high bit */
            {
                case 0:
                    p_mwc->mwc_upper ^= *p_data;
                    mwc2_sanitize_upper(p_mwc);
                    mwc2_next_upper(p_mwc);
                    break;
                case 1:
                    p_mwc->mwc_lower ^= *p_data;
                    mwc2_sanitize_lower(p_mwc);
                    mwc2_next_lower(p_mwc);
                    break;
            }
            ++p_data;
        }
    }
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
        seed_mwc_upper  = (num_seeds >= 1) ? p_seeds[0] : 0;
        seed_mwc_lower  = (num_seeds >= 2) ? p_seeds[1] : seed_mwc_upper;
        seed_cong       = (num_seeds >= 3) ? p_seeds[2] : seed_mwc_lower;
        seed_shr3       = (num_seeds >= 4) ? p_seeds[3] : seed_cong;
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

/* The good state values are all modulo 0x9068FFFF. Values above that would
 * jump to a corresponding good state value on the first "next" operation.
 * Any integer multiple of 0x9068FFFF, including 0, is a bad state.
 */
static inline void kiss_sanitize_mwc_upper(SimpleRandomKISS_t * p_kiss)
{
    uint32_t    state_orig;
    uint32_t    temp;

    state_orig = p_kiss->mwc_upper;
    temp = state_orig;
    /* The following is equivalent to % 0x9068FFFF, without using modulo
     * operation which may be expensive on embedded targets. For
     * uint32_t and this divisor, we only need 'if' rather than 'while'. */
    if (temp >= UINT32_C(0x9068FFFF))
        temp -= UINT32_C(0x9068FFFF);
    if (temp == 0)
    {
        /* Any integer multiple of 0x9068FFFF, including 0, is a bad state.
         * Use an alternate state value by inverting the original value. */
        temp = state_orig ^ UINT32_C(0xFFFFFFFF);
        if (temp >= UINT32_C(0x9068FFFF))
            temp -= UINT32_C(0x9068FFFF);
    }
    p_kiss->mwc_upper = temp;
}

/* The good state values are all modulo 0x9068FFFF. Values above that would
 * jump to a corresponding good state value on the first "next" operation.
 * Any integer multiple of 0x464FFFFF, including 0, is a bad state.
 */
static inline void kiss_sanitize_mwc_lower(SimpleRandomKISS_t * p_kiss)
{
    uint32_t    state_orig;
    uint32_t    temp;

    state_orig = p_kiss->mwc_lower;
    temp = state_orig;
    /* The following is equivalent to % 0x464FFFFF, without using modulo
     * operation which may be expensive on embedded targets. For
     * uint32_t and this divisor, it may loop up to 3 times. */
    while (temp >= UINT32_C(0x464FFFFF))
        temp -= UINT32_C(0x464FFFFF);
    if (temp == 0)
    {
        /* Any integer multiple of 0x464FFFFF, including 0, is a bad state.
         * Use an alternate state value by inverting the original value. */
        temp = state_orig ^ UINT32_C(0xFFFFFFFF);
        while (temp >= UINT32_C(0x464FFFFF))
            temp -= UINT32_C(0x464FFFFF);
    }

    p_kiss->mwc_lower = temp;
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

static inline void kiss_next_mwc_upper(SimpleRandomKISS_t * p_kiss)
{
    p_kiss->mwc_upper = 36969u * (p_kiss->mwc_upper & 0xFFFFu) + (p_kiss->mwc_upper >> 16u);
}

static inline void kiss_next_mwc_lower(SimpleRandomKISS_t * p_kiss)
{
    p_kiss->mwc_lower = 18000u * (p_kiss->mwc_lower & 0xFFFFu) + (p_kiss->mwc_lower >> 16u);
}

static inline void kiss_next_cong(SimpleRandomKISS_t * p_kiss)
{
    p_kiss->cong = UINT32_C(69069) * p_kiss->cong + 12345u;
}

static inline void kiss_next_shr3(SimpleRandomKISS_t * p_kiss)
{
    uint32_t    shr3;

    shr3 = p_kiss->shr3;
    shr3 ^= (shr3 << 13);
    shr3 ^= (shr3 >> 17);
    shr3 ^= (shr3 << 5);
    p_kiss->shr3 = shr3;
}

uint32_t simplerandom_kiss_next(SimpleRandomKISS_t * p_kiss)
{
    kiss_next_mwc_upper(p_kiss);
    kiss_next_mwc_lower(p_kiss);
    kiss_next_cong(p_kiss);
    kiss_next_shr3(p_kiss);
    return kiss_current(p_kiss);
}

void simplerandom_kiss_mix(SimpleRandomKISS_t * p_kiss, const uint32_t * p_data, size_t num_data)
{
    uint32_t    current;

    if (p_data != NULL)
    {
        while (num_data)
        {
            --num_data;
            current = kiss_current(p_kiss);
            switch ((current >> 24u) & 0x3u)    /* Switch on 2 high bits */
            {
                case 0:
                    p_kiss->mwc_upper ^= *p_data;
                    kiss_sanitize_mwc_upper(p_kiss);
                    kiss_next_mwc_upper(p_kiss);
                    break;
                case 1:
                    p_kiss->mwc_lower ^= *p_data;
                    kiss_sanitize_mwc_lower(p_kiss);
                    kiss_next_mwc_lower(p_kiss);
                    break;
                case 2:
                    p_kiss->cong ^= *p_data;
                    /* Cong doesn't need sanitise; all states are valid. */
                    kiss_next_cong(p_kiss);
                    break;
                case 3:
                    p_kiss->shr3 ^= *p_data;
                    kiss_sanitize_shr3(p_kiss);
                    kiss_next_shr3(p_kiss);
                    break;
            }
            ++p_data;
        }
    }
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
        seed_upper = (num_seeds >= 1) ? p_seeds[0] : 0;
        seed_lower = (num_seeds >= 2) ? p_seeds[1] : seed_upper;
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
    uint64_t    state64_orig;
    uint64_t    temp64;
    bool        was_changed = false;

    state64_orig = ((uint64_t)p_mwc->mwc_upper << 32u) + p_mwc->mwc_lower;
    temp64 = state64_orig;
    if (temp64 >= UINT64_C(0x29A65EACFFFFFFFF))
    {
        temp64 %= UINT64_C(0x29A65EACFFFFFFFF);
        was_changed = true;
    }
    if (temp64 == 0)
    {
        /* Invert both upper and lower to get a good seed. */
        temp64 = state64_orig;
        temp64 ^= UINT64_C(0xFFFFFFFFFFFFFFFF);
        temp64 %= UINT64_C(0x29A65EACFFFFFFFF);
        was_changed = true;
    }
    if (was_changed)
    {
        p_mwc->mwc_upper = (uint32_t)(temp64 >> 32u);
        p_mwc->mwc_lower = (uint32_t)temp64;
    }
}

static inline uint32_t mwc64_current(SimpleRandomMWC64_t * p_mwc)
{
    return p_mwc->mwc_lower;
}

uint32_t simplerandom_mwc64_next(SimpleRandomMWC64_t * p_mwc)
{
    uint64_t    mwc64;


    mwc64 = UINT64_C(698769069) * p_mwc->mwc_lower + p_mwc->mwc_upper;
    p_mwc->mwc_upper = (mwc64 >> 32u);
    p_mwc->mwc_lower = (uint32_t)mwc64;

    return (uint32_t)mwc64;
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
            switch ((current >> 24u) & 0x1u)    /* Switch on 1 high bit */
            {
                case 0:
                    p_mwc->mwc_upper ^= *p_data;
                    break;
                case 1:
                    p_mwc->mwc_lower ^= *p_data;
                    break;
            }
            ++p_data;
            simplerandom_mwc64_sanitize(p_mwc);
            current = simplerandom_mwc64_next(p_mwc);
        }
    }
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
        seed_mwc_upper  = (num_seeds >= 1) ? p_seeds[0] : 0;
        seed_mwc_lower  = (num_seeds >= 2) ? p_seeds[1] : seed_mwc_upper;
        seed_cong       = (num_seeds >= 3) ? p_seeds[2] : seed_mwc_lower;
        seed_shr3       = (num_seeds >= 4) ? p_seeds[3] : seed_cong;
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
    uint64_t    state64_orig;
    uint64_t    temp64;
    bool        was_changed = false;

    state64_orig = ((uint64_t)p_kiss2->mwc_upper << 32u) + p_kiss2->mwc_lower;
    temp64 = state64_orig;
    if (temp64 >= UINT64_C(0x29A65EACFFFFFFFF))
    {
        temp64 %= UINT64_C(0x29A65EACFFFFFFFF);
        was_changed = true;
    }
    if (temp64 == 0)
    {
        /* Invert both upper and lower to get a good seed. */
        temp64 = state64_orig;
        temp64 ^= UINT64_C(0xFFFFFFFFFFFFFFFF);
        temp64 %= UINT64_C(0x29A65EACFFFFFFFF);
        was_changed = true;
    }
    if (was_changed)
    {
        p_kiss2->mwc_upper = (uint32_t)(temp64 >> 32u);
        p_kiss2->mwc_lower = (uint32_t)temp64;
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

static inline void kiss2_next_mwc64(SimpleRandomKISS2_t * p_kiss2)
{
    uint64_t    mwc64;

    mwc64 = UINT64_C(698769069) * p_kiss2->mwc_lower + p_kiss2->mwc_upper;
    p_kiss2->mwc_upper = (mwc64 >> 32u);
    p_kiss2->mwc_lower = (uint32_t)mwc64;
}

static inline void kiss2_next_cong(SimpleRandomKISS2_t * p_kiss2)
{
    p_kiss2->cong = UINT32_C(69069) * p_kiss2->cong + 12345u;
}

static inline void kiss2_next_shr3(SimpleRandomKISS2_t * p_kiss2)
{
    uint32_t    shr3;

    shr3 = p_kiss2->shr3;
    shr3 ^= (shr3 << 13);
    shr3 ^= (shr3 >> 17);
    shr3 ^= (shr3 << 5);
    p_kiss2->shr3 = shr3;
}

uint32_t simplerandom_kiss2_next(SimpleRandomKISS2_t * p_kiss2)
{
    kiss2_next_mwc64(p_kiss2);
    kiss2_next_cong(p_kiss2);
    kiss2_next_shr3(p_kiss2);
    return kiss2_current(p_kiss2);
}

void simplerandom_kiss2_mix(SimpleRandomKISS2_t * p_kiss2, const uint32_t * p_data, size_t num_data)
{
    uint32_t    current;

    if (p_data != NULL)
    {
        while (num_data)
        {
            --num_data;
            current = kiss2_current(p_kiss2);
            switch ((current >> 24u) & 0x3u)    /* Switch on 2 high bits */
            {
                case 0:
                    p_kiss2->mwc_upper ^= *p_data;
                    kiss2_sanitize_mwc64(p_kiss2);
                    kiss2_next_mwc64(p_kiss2);
                    break;
                case 1:
                    p_kiss2->mwc_lower ^= *p_data;
                    kiss2_sanitize_mwc64(p_kiss2);
                    kiss2_next_mwc64(p_kiss2);
                    break;
                case 2:
                    p_kiss2->cong ^= *p_data;
                    /* Cong doesn't need sanitise; all states are valid. */
                    kiss2_next_cong(p_kiss2);
                    break;
                case 3:
                    p_kiss2->shr3 ^= *p_data;
                    kiss2_sanitize_shr3(p_kiss2);
                    kiss2_next_shr3(p_kiss2);
                    break;
            }
            ++p_data;
        }
    }
}

#endif /* defined(UINT64_C) */


/*********
 * LFSR113
 ********/

#define LFSR_SEED_SHIFT         16u
#define LFSR_ALT_SEED_SHIFT     24u
#define LFSR_SEED(X)            ((X) ^ ((X) << LFSR_SEED_SHIFT))
#define LFSR_ALT_SEED(X)        ((X) << LFSR_ALT_SEED_SHIFT)

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
        seed_z1 = (num_seeds >= 1) ? p_seeds[0] : 0;
        seed_z2 = (num_seeds >= 2) ? p_seeds[1] : seed_z1;
        seed_z3 = (num_seeds >= 3) ? p_seeds[2] : seed_z2;
        seed_z4 = (num_seeds >= 4) ? p_seeds[3] : seed_z3;
    }
    simplerandom_lfsr113_seed(p_lfsr113, seed_z1, seed_z2, seed_z3, seed_z4);

    if (mix_extras && p_seeds != NULL)
    {
        simplerandom_lfsr113_mix(p_lfsr113, p_seeds + num_seeds_used, num_seeds - num_seeds_used);
        num_seeds_used = num_seeds;
    }
    return num_seeds_used;
}

/* Use a more complex seed function due to the unique state characteristics of
 * LFSR generators: some lowest 'n' bits of each state variable are discarded
 * for each 'next' calc.
 * We want the lowest bits of the seed values to have some contribution to the
 * initial state. So do some basic shift & XOR of seed values. Then do some
 * reasonable sanitising to ensure the state upper '32-n' bits aren't all zero.
 */
void simplerandom_lfsr113_seed(SimpleRandomLFSR113_t * p_lfsr113, uint32_t seed_z1, uint32_t seed_z2, uint32_t seed_z3, uint32_t seed_z4)
{
    uint32_t    working_seed;

    /* Seed z1 */
    working_seed = LFSR_SEED(seed_z1);
    if (working_seed < LFSR_SEED_Z1_MIN_VALUE)
    {
        working_seed = LFSR_ALT_SEED(seed_z1);
        if (working_seed < LFSR_SEED_Z1_MIN_VALUE)
        {
            working_seed = ~working_seed;
        }
    }
    p_lfsr113->z1 = working_seed;

    /* Seed z2 */
    working_seed = LFSR_SEED(seed_z2);
    if (working_seed < LFSR_SEED_Z2_MIN_VALUE)
    {
        working_seed = LFSR_ALT_SEED(seed_z2);
        if (working_seed < LFSR_SEED_Z2_MIN_VALUE)
        {
            working_seed = ~working_seed;
        }
    }
    p_lfsr113->z2 = working_seed;

    /* Seed z3 */
    working_seed = LFSR_SEED(seed_z3);
    if (working_seed < LFSR_SEED_Z3_MIN_VALUE)
    {
        working_seed = LFSR_ALT_SEED(seed_z3);
        if (working_seed < LFSR_SEED_Z3_MIN_VALUE)
        {
            working_seed = ~working_seed;
        }
    }
    p_lfsr113->z3 = working_seed;

    /* Seed z4 */
    working_seed = LFSR_SEED(seed_z4);
    if (working_seed < LFSR_SEED_Z4_MIN_VALUE)
    {
        working_seed = LFSR_ALT_SEED(seed_z4);
        if (working_seed < LFSR_SEED_Z4_MIN_VALUE)
        {
            working_seed = ~working_seed;
        }
    }
    p_lfsr113->z4 = working_seed;
}

/* For most simplerandom generators, sanitise functions are used for both
 * seeding and for sanitising the mix function results. But for LFSR generators
 * with a more complex seeding function, the sanitise functions are specified
 * separately, and are relatively simple compared to the seeding function.
 */
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

static inline void lfsr113_next_z1(SimpleRandomLFSR113_t * p_lfsr113)
{
    uint32_t    b;
    uint32_t    z1;

    z1 = p_lfsr113->z1;
    b  = ((z1 << 6) ^ z1) >> 13;
    z1 = ((z1 & UINT32_C(0xFFFFFFFE)) << 18) ^ b;
    p_lfsr113->z1 = z1;
}

static inline void lfsr113_next_z2(SimpleRandomLFSR113_t * p_lfsr113)
{
    uint32_t    b;
    uint32_t    z2;

    z2 = p_lfsr113->z2;
    b  = ((z2 << 2) ^ z2) >> 27;
    z2 = ((z2 & UINT32_C(0xFFFFFFF8)) << 2) ^ b;
    p_lfsr113->z2 = z2;
}

static inline void lfsr113_next_z3(SimpleRandomLFSR113_t * p_lfsr113)
{
    uint32_t    b;
    uint32_t    z3;

    z3 = p_lfsr113->z3;
    b  = ((z3 << 13) ^ z3) >> 21;
    z3 = ((z3 & UINT32_C(0xFFFFFFF0)) << 7) ^ b;
    p_lfsr113->z3 = z3;
}

static inline void lfsr113_next_z4(SimpleRandomLFSR113_t * p_lfsr113)
{
    uint32_t    b;
    uint32_t    z4;

    z4 = p_lfsr113->z4;
    b  = ((z4 << 3) ^ z4) >> 12;
    z4 = ((z4 & UINT32_C(0xFFFFFF80)) << 13) ^ b;
    p_lfsr113->z4 = z4;
}

static inline uint32_t lfsr113_current(SimpleRandomLFSR113_t * p_lfsr113)
{
    return (p_lfsr113->z1 ^ p_lfsr113->z2 ^ p_lfsr113->z3 ^ p_lfsr113->z4);
}

uint32_t simplerandom_lfsr113_next(SimpleRandomLFSR113_t * p_lfsr113)
{
    lfsr113_next_z1(p_lfsr113);
    lfsr113_next_z2(p_lfsr113);
    lfsr113_next_z3(p_lfsr113);
    lfsr113_next_z4(p_lfsr113);
    return lfsr113_current(p_lfsr113);
}

void simplerandom_lfsr113_mix(SimpleRandomLFSR113_t * p_lfsr113, const uint32_t * p_data, size_t num_data)
{
    uint32_t    current;

    if (p_data != NULL)
    {
        while (num_data)
        {
            --num_data;
            current = lfsr113_current(p_lfsr113);
            switch ((current >> 30) & 0x3)  /* Switch on 2 highest bits */
            {
                case 0:
                    p_lfsr113->z1 ^= *p_data;
                    lfsr113_sanitize_z1(p_lfsr113);
                    lfsr113_next_z1(p_lfsr113);
                    break;
                case 1:
                    p_lfsr113->z2 ^= *p_data;
                    lfsr113_sanitize_z2(p_lfsr113);
                    lfsr113_next_z2(p_lfsr113);
                    break;
                case 2:
                    p_lfsr113->z3 ^= *p_data;
                    lfsr113_sanitize_z3(p_lfsr113);
                    lfsr113_next_z3(p_lfsr113);
                    break;
                case 3:
                    p_lfsr113->z4 ^= *p_data;
                    lfsr113_sanitize_z4(p_lfsr113);
                    lfsr113_next_z4(p_lfsr113);
                    break;
            }
            ++p_data;
        }
    }
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
        seed_z1 = (num_seeds >= 1) ? p_seeds[0] : 0;
        seed_z2 = (num_seeds >= 2) ? p_seeds[1] : seed_z1;
        seed_z3 = (num_seeds >= 3) ? p_seeds[2] : seed_z2;
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
    working_seed = LFSR_SEED(seed_z1);
    if (working_seed < LFSR_SEED_Z1_MIN_VALUE)
    {
        working_seed = LFSR_ALT_SEED(seed_z1);
        if (working_seed < LFSR_SEED_Z1_MIN_VALUE)
        {
            working_seed = ~working_seed;
        }
    }
    p_lfsr88->z1 = working_seed;

    /* Seed z2 */
    working_seed = LFSR_SEED(seed_z2);
    if (working_seed < LFSR_SEED_Z2_MIN_VALUE)
    {
        working_seed = LFSR_ALT_SEED(seed_z2);
        if (working_seed < LFSR_SEED_Z2_MIN_VALUE)
        {
            working_seed = ~working_seed;
        }
    }
    p_lfsr88->z2 = working_seed;

    /* Seed z3 */
    working_seed = LFSR_SEED(seed_z3);
    if (working_seed < LFSR_SEED_Z3_MIN_VALUE)
    {
        working_seed = LFSR_ALT_SEED(seed_z3);
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

static inline void lfsr88_next_z1(SimpleRandomLFSR88_t * p_lfsr88)
{
    uint32_t    b;
    uint32_t    z1;

    z1 = p_lfsr88->z1;
    b  = ((z1 << 13) ^ z1) >> 19;
    z1 = ((z1 & UINT32_C(0xFFFFFFFE)) << 12) ^ b;
    p_lfsr88->z1 = z1;
}

static inline void lfsr88_next_z2(SimpleRandomLFSR88_t * p_lfsr88)
{
    uint32_t    b;
    uint32_t    z2;

    z2 = p_lfsr88->z2;
    b  = ((z2 << 2) ^ z2) >> 25;
    z2 = ((z2 & UINT32_C(0xFFFFFFF8)) << 4) ^ b;
    p_lfsr88->z2 = z2;
}

static inline void lfsr88_next_z3(SimpleRandomLFSR88_t * p_lfsr88)
{
    uint32_t    b;
    uint32_t    z3;

    z3 = p_lfsr88->z3;
    b  = ((z3 << 3) ^ z3) >> 11;
    z3 = ((z3 & UINT32_C(0xFFFFFFF0)) << 17) ^ b;
    p_lfsr88->z3 = z3;
}

static inline uint32_t lfsr88_current(SimpleRandomLFSR88_t * p_lfsr88)
{
    return (p_lfsr88->z1 ^ p_lfsr88->z2 ^ p_lfsr88->z3);
}

uint32_t simplerandom_lfsr88_next(SimpleRandomLFSR88_t * p_lfsr88)
{
    lfsr88_next_z1(p_lfsr88);
    lfsr88_next_z2(p_lfsr88);
    lfsr88_next_z3(p_lfsr88);
    return lfsr88_current(p_lfsr88);
}

void simplerandom_lfsr88_mix(SimpleRandomLFSR88_t * p_lfsr88, const uint32_t * p_data, size_t num_data)
{
    uint32_t    current;

    if (p_data != NULL)
    {
        while (num_data)
        {
            --num_data;
            /* Select which of the 3 state variables to modify, with
             * approximately equal probability. Unlike all other generators, we
             * don't have a power-of-2 number of state variables, so we have to
             * do this differently. Avoid using modulo or divide in case it's a
             * costly operation on target processor (especially embedded). */
            current = lfsr88_current(p_lfsr88);
            if (current < UINT32_C(1431655765))         /* constant is 2^32 / 3 */
            {
                p_lfsr88->z1 ^= *p_data;
                lfsr88_sanitize_z1(p_lfsr88);
                lfsr88_next_z1(p_lfsr88);
            }
            else if (current < UINT32_C(2863311531))    /* constant is 2^32 * 2 / 3 */
            {
                p_lfsr88->z2 ^= *p_data;
                lfsr88_sanitize_z2(p_lfsr88);
                lfsr88_next_z2(p_lfsr88);
            }
            else
            {
                p_lfsr88->z3 ^= *p_data;
                lfsr88_sanitize_z3(p_lfsr88);
                lfsr88_next_z3(p_lfsr88);
            }
            ++p_data;
        }
    }
}

