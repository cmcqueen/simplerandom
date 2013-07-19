/*
 * simplerandom.h
 *
 * Simple Pseudo-random Number Generators
 *
 * Most of these are from two newsgroup posts by
 * George Marsaglia.
 *
 * The first was in 1999 [1]. From that newsgroup post,
 * the following RNGs are defined:
 *     MWC
 *     KISS (however, instead we use Cong and SHR3 defined in the 2003 post)
 *
 * Due to analysis of SHR3 in ref [3], I believe the SHR3
 * as defined in the 1999 post should not be used. It
 * doesn't actually have a period of 2^32-1 as expected,
 * but has 64 cycles, some with very short periods. The
 * SHR3 in the 2003 post is very similar, but with two
 * shift values swapped. My suspicion is that the SHR3
 * shift values in the 1999 post are a typo.
 *
 * Since we don't use the 1999 SHR3, we may as well not
 * use the 1999 Cong either, since the 2003 Cong is almost
 * identical, but with a different added constant.
 * Therefore, we define a KISS function that uses the
 * 1999 MWC, but the 2003 SHR3 and Cong.
 * The advantage of using the 1999 MWC and KISS at all is
 * that the MWC uses only 32-bit math, while the 2003 MWC
 * uses 64-bit math which may not be suitable for small
 * embedded systems.
 *
 * The second Marsaglia post was in 2003. From that
 * newsgroup post, the following RNGs are defined:
 *     MWC64
 *     Cong
 *     SHR3
 *     KISS2
 *
 * The LFSR113 generator by L'Ecuyer is also implemented
 * [4], along with his earlier LFSR88.
 *
 * References:
 *
 * [1] Random Numbers for C: End, at last?
 *     George Marsaglia
 *     Newsgroup post, sci.stat.math and others, Thu, 21 Jan 1999
 *     http://www.cse.yorku.ca/~oz/marsaglia-rng.html
 *
 * [2] RNGs
 *     George Marsaglia
 *     Newsgroup post, sci.math, 26 Feb 2003
 *     http://groups.google.com/group/sci.math/msg/9959175f66dd138f
 *
 * [3] "KISS: A Bit Too Simple"
 *     Greg Rose
 *     Qualcomm Inc.
 *     http://eprint.iacr.org/2011/007.pdf
 *
 * [4] "Tables of Maximally-Equidistributed Combined LFSR Generators"
 *     Pierre L'Ecuyer
 *     http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.43.3639
 *     http://www.iro.umontreal.ca/~simardr/rng/lfsr113.c
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
} SimpleRandomMWC1_t;

typedef SimpleRandomMWC1_t SimpleRandomMWC2_t;

typedef struct
{
    uint32_t mwc_upper;
    uint32_t mwc_lower;
    uint32_t cong;
    uint32_t shr3;
} SimpleRandomKISS_t;

#ifdef UINT64_C

typedef struct
{
    uint32_t mwc_upper;
    uint32_t mwc_lower;
} SimpleRandomMWC64_t;

typedef struct
{
    uint32_t mwc_upper;
    uint32_t mwc_lower;
    uint32_t cong;
    uint32_t shr3;
} SimpleRandomKISS2_t;

#endif /* defined(UINT64_C) */

typedef struct
{
    uint32_t    z1;
    uint32_t    z2;
    uint32_t    z3;
    uint32_t    z4;
} SimpleRandomLFSR113_t;

typedef struct
{
    uint32_t    z1;
    uint32_t    z2;
    uint32_t    z3;
} SimpleRandomLFSR88_t;


/*****************************************************************************
 * Function prototypes
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* Cong -- Congruential random number generator
 *
 * This is a congruential generator with the widely used
 * 69069 multiplier: x[n]=69069x[n-1]+12345. It has
 * period 2^32.
 *
 * The leading half of its 32 bits seem to pass tests,
 * but bits in the last half are too regular. It fails
 * tests for which those bits play a significant role.
 * But keep in mind that it is a rare application for
 * which the trailing bits play a significant role. Cong
 * is one of the most widely used generators of the last
 * 30 years, as it was the system generator for VAX and 
 * was incorporated in several popular software packages,
 * all seemingly without complaint.
 */
void simplerandom_cong_seed(SimpleRandomCong_t * p_cong, uint32_t seed);
uint32_t simplerandom_cong_next(SimpleRandomCong_t * p_cong);

/* SHR3 -- 3-shift-register random number generator
 *
 * Reading between the lines, I believe the SHR3 defined
 * in Marsaglia's 1999 post actually has a typo: the
 * shift values defined don't actually produce a period of
 * 2^32-1, but have 64 possible cycles, some extremely
 * short. But the swapped values from Marsaglia's 2003
 * post produce the full 2^32-1 period. So we use that
 * definition of SHR3.
 *
 * SHR3 is a 3-shift-register generator with period
 * 2^32-1. It uses y[n]=y[n-1](I+L^13)(I+R^17)(I+L^5),
 * with the y's viewed as binary vectors, L the 32x32
 * binary matrix that shifts a vector left 1, and R its
 * transpose.
 *
 * SHR3 seems to pass all except those related to the
 * binary rank test, since 32 successive values, as
 * binary vectors, must be linearly independent, while
 * 32 successive truly random 32-bit integers, viewed
 * as binary vectors, will be linearly independent only
 * about 29% of the time.
 */
void simplerandom_shr3_seed(SimpleRandomSHR3_t * p_shr3, uint32_t seed);
uint32_t simplerandom_shr3_next(SimpleRandomSHR3_t * p_shr3);

/* MWC1 -- "Multiply-with-carry" random number generator
 *
 * This is the MWC as defined in Marsaglia's 1999
 * newsgroup post.
 *
 * It uses two MWC generators to generate high and
 * low 16-bit parts, which are then combined to make a
 * 32-bit value.
 *
 * The MWC generator concatenates two 16-bit multiply-
 * with-carry generators:
 *
 *     x[n]=36969x[n-1]+carry,
 *     y[n]=18000y[n-1]+carry mod 2^16,
 *
 * It has a period about 2^60.
 *
 * This seems to pass all Marsaglia's Diehard tests.
 * However, it fails many of L'Ecuyer's TestU01
 * tests. The modified MWC2 generator passes many more
 * tests in TestU01, and should probably be preferred,
 * unless backwards compatibility is required.
 */
void simplerandom_mwc1_seed(SimpleRandomMWC1_t * p_mwc, uint32_t seed_upper, uint32_t seed_lower);
uint32_t simplerandom_mwc1_next(SimpleRandomMWC1_t * p_mwc);

/* MWC2 -- "Multiply-with-carry" random number generator
 *
 * Very similar to MWC1, except that it concatenates the
 * two 16-bit MWC generators differently. The 'x'
 * generator is rotated 16 bits instead of just shifted
 * 16 bits.
 *
 * This gets much better test results than MWC1 in
 * L'Ecuyer's TestU01 test suite, so it should probably
 * be preferred.
 */
void simplerandom_mwc2_seed(SimpleRandomMWC2_t * p_mwc, uint32_t seed_upper, uint32_t seed_lower);
uint32_t simplerandom_mwc2_next(SimpleRandomMWC2_t * p_mwc);

/* KISS -- "Keep It Simple Stupid" random number generator
 *
 * It combines the MWC2, Cong, SHR3 generators. Period is
 * about 2^123.
 *
 * This is based on, but not identical to, Marsaglia's
 * KISS generator as defined in his 1999 newsgroup post.
 * That generator most significantly has problems with its
 * SHR3 component (see notes on SHR3 above). Since we are
 * not keeping compatibility with the 1999 KISS generator
 * for that reason, we take the opportunity to slightly
 * update the MWC and Cong generators too.
 */
void simplerandom_kiss_seed(SimpleRandomKISS_t * p_kiss, uint32_t seed_mwc_upper, uint32_t seed_mwc_lower, uint32_t seed_cong, uint32_t seed_shr3);
uint32_t simplerandom_kiss_next(SimpleRandomKISS_t * p_kiss);


#ifdef UINT64_C

/* MWC64 -- "Multiply-with-carry" random number generator
 *
 * This is a different MWC generator design, from
 * the newsgroup post in 2003.
 *
 * This uses a single MWC generator with a 64-bit
 * calculation to generate a 32-bit value. The seeds
 * should still be 32-bit values.
 */
void simplerandom_mwc64_seed(SimpleRandomMWC64_t * p_mwc, uint32_t seed_upper, uint32_t seed_lower);
uint32_t simplerandom_mwc64_next(SimpleRandomMWC64_t * p_mwc);

/* KISS2 -- "Keep It Simple Stupid" random number generator
 *
 * It combines the MWC64, Cong, SHR3 generators. Period
 * is about 2^123.
 *
 * This is a slightly updated KISS generator design, from
 * the newsgroup post in 2003. The MWC component uses a
 * single 64-bit calculation, instead of two 32-bit
 * calculations that are combined.
 */
void simplerandom_kiss2_seed(SimpleRandomKISS2_t * p_kiss2, uint32_t seed_mwc_upper, uint32_t seed_mwc_lower, uint32_t seed_cong, uint32_t seed_shr3);
uint32_t simplerandom_kiss2_next(SimpleRandomKISS2_t * p_kiss2);

#endif /* defined(UINT64_C) */


/* LFSR113 -- Combined LFSR random number generator by L'Ecuyer
 *
 * It combines 4 LFSR generators. The generators have been
 * chosen for maximal equidistribution.
 *
 * The period is approximately 2^113.
 *
 * "Tables of Maximally-Equidistributed Combined Lfsr Generators"
 * P. L'Ecuyer
 * Mathematics of Computation, 68, 225 (1999), 261–269.

 */
void simplerandom_lfsr113_seed(SimpleRandomLFSR113_t * p_lfsr113, uint32_t seed_z1, uint32_t seed_z2, uint32_t seed_z3, uint32_t seed_z4);
uint32_t simplerandom_lfsr113_next(SimpleRandomLFSR113_t * p_lfsr113);

/* LFSR88 -- Combined LFSR random number generator by L'Ecuyer
 *
 * It combines 3 LFSR generators. The generators have been
 * chosen for maximal equidistribution.
 *
 * The period is approximately 2^88.
 *
 * "Maximally Equidistributed Combined Tausworthe Generators"
 * P. L'Ecuyer
 * Mathematics of Computation, 65, 213 (1996), 203–213. 
 */
void simplerandom_lfsr88_seed(SimpleRandomLFSR88_t * p_lfsr88, uint32_t seed_z1, uint32_t seed_z2, uint32_t seed_z3);
uint32_t simplerandom_lfsr88_next(SimpleRandomLFSR88_t * p_lfsr88);


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* !defined(_SIMPLERANDOM_H) */

