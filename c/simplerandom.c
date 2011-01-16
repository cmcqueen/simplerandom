
#include "simplerandom.h"


void simplerandom_cong_seed(SimpleRandomCong_t * p_cong, uint32_t seed)
{
    if (seed == 0)
    {
        seed = UINT32_C(380116160);
    }

    *p_cong = seed;
}

uint32_t simplerandom_cong_next(SimpleRandomCong_t * p_cong)
{
    *p_cong = UINT32_C(69069) * *p_cong + UINT32_C(1234567);

    return *p_cong;
}

void simplerandom_mwc_seed(SimpleRandomMWC_t * p_mwc, uint32_t seed_upper, uint32_t seed_lower)
{
    if (seed_upper == 0)
    {
        seed_upper = UINT32_C(362436069);
    }
    if (seed_lower == 0)
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

