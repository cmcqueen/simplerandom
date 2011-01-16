/*
 * simplerandom.h
 *
 * Simple Pseudo-random Number Generators
 */

#ifndef _SIMPLERANDOM_H
#define _SIMPLERANDOM_H


/*****************************************************************************
 * Includes
 ****************************************************************************/

#include <stdint.h>


/*****************************************************************************
 * Typedefs
 ****************************************************************************/

typedef uint32_t SimpleRandomCong_t;

typedef uint32_t SimpleRandomSHR3_t;

typedef struct
{
    uint32_t mwc_upper;
    uint32_t mwc_lower;
} SimpleRandomMWC_t;

typedef struct
{
    uint32_t cong;
    uint32_t shr3;
    uint32_t mwc_upper;
    uint32_t mwc_lower;
} SimpleRandomKISS_t;


/*****************************************************************************
 * Function prototypes
 ****************************************************************************/

/* Cong */
void simplerandom_cong_seed(SimpleRandomCong_t * p_cong, uint32_t seed);
uint32_t simplerandom_cong_next(SimpleRandomCong_t * p_cong);

/* SHR3 */
void simplerandom_shr3_seed(SimpleRandomSHR3_t * p_shr3, uint32_t seed);
uint32_t simplerandom_shr3_next(SimpleRandomSHR3_t * p_shr3);

/* MWC */
void simplerandom_mwc_seed(SimpleRandomMWC_t * p_mwc, uint32_t seed_upper, uint32_t seed_lower);
uint32_t simplerandom_mwc_next(SimpleRandomMWC_t * p_mwc);

/* KISS */
void simplerandom_kiss_seed(SimpleRandomKISS_t * p_kiss, uint32_t seed_mwc_upper, uint32_t seed_mwc_lower, uint32_t seed_cong, uint32_t seed_shr3);
uint32_t simplerandom_kiss_next(SimpleRandomKISS_t * p_kiss);


#endif /* !defined(_SIMPLERANDOM_H) */


