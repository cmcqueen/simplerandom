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
    uint32_t mwc_upper;
    uint32_t mwc_lower;
    uint32_t cong;
    uint32_t shr3;
} SimpleRandomKISS_t;

typedef struct
{
    uint32_t fib_a;
    uint32_t fib_b;
} SimpleRandomFib_t;

typedef uint32_t SimpleRandomSHR3_2_t;

typedef uint32_t SimpleRandomCong2_t;

#ifdef UINT64_C

typedef struct
{
    uint32_t mwc64_upper;
    uint32_t mwc64_lower;
} SimpleRandomMWC64_t;

typedef struct
{
    uint32_t mwc64_upper;
    uint32_t mwc64_lower;
    uint32_t cong2;
    uint32_t shr3_2;
} SimpleRandomKISS2_t;

#endif /* defined(UINT64_C) */


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

/* Fib */
void simplerandom_fib_seed(SimpleRandomFib_t * p_fib, uint32_t seed_a, uint32_t seed_b);
uint32_t simplerandom_fib_next(SimpleRandomFib_t * p_fib);

/* Cong2 */
void simplerandom_cong2_seed(SimpleRandomCong_t * p_cong2, uint32_t seed);
uint32_t simplerandom_cong2_next(SimpleRandomCong_t * p_cong2);

/* SHR3_2 */
void simplerandom_shr3_2_seed(SimpleRandomSHR3_2_t * p_shr3_2, uint32_t seed);
uint32_t simplerandom_shr3_2_next(SimpleRandomSHR3_2_t * p_shr3_2);

#ifdef UINT64_C

/* MWC64 */
void simplerandom_mwc64_seed(SimpleRandomMWC64_t * p_mwc64, uint32_t seed_upper, uint32_t seed_lower);
uint32_t simplerandom_mwc64_next(SimpleRandomMWC64_t * p_mwc64);

/* KISS2 */
void simplerandom_kiss2_seed(SimpleRandomKISS2_t * p_kiss2, uint32_t seed_mwc64_upper, uint32_t seed_mwc64_lower, uint32_t seed_cong2, uint32_t seed_shr3_2);
uint32_t simplerandom_kiss2_next(SimpleRandomKISS2_t * p_kiss2);

#endif /* defined(UINT64_C) */


#endif /* !defined(_SIMPLERANDOM_H) */


