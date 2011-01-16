
#ifndef _SIMPLERANDOM_H
#define _SIMPLERANDOM_H


#include <stdint.h>


typedef uint32_t SimpleRandomCong_t;

typedef struct
{
    uint32_t mwc_upper;
    uint32_t mwc_lower;
} SimpleRandomMWC_t;


void simplerandom_cong_seed(SimpleRandomCong_t * p_cong_t, uint32_t seed);
uint32_t simplerandom_cong_next(SimpleRandomCong_t * p_cong_t);

void simplerandom_mwc_seed(SimpleRandomMWC_t * p_mwc_t, uint32_t seed_upper, uint32_t seed_lower);
uint32_t simplerandom_mwc_next(SimpleRandomMWC_t * p_mwc_t);


#endif /* !defined(_SIMPLERANDOM_H) */


