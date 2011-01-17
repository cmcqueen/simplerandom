/*
 * simplerandom.h
 *
 * Simple Pseudo-random Number Generators
 *
 * All of these are from two newsgroup posts by
 * George Marsaglia.
 *
 * The first was in 1999:
 *     Random Numbers for C\: End, at last?
 *     George Marsaglia
 *     Newsgroup post, sci.stat.math and others, Thu, 21 Jan 1999
 *     http://www.cse.yorku.ca/~oz/marsaglia-rng.html
 * From that newsgroup post, the following RNGs are defined:
 *     MWC
 *     Cong
 *     SHR3
 *     KISS
 *     Fib
 *     LFIB4
 *     SWB
 *
 * The second was in 2003:
 *     RNGs
 *     George Marsaglia
 *     Newsgroup post, sci.math, 26 Feb 2003
 *     http://groups.google.com/group/sci.math/msg/9959175f66dd138f
 * From that newsgroup post, the following RNGs are defined:
 *     MWC64
 *     Cong2
 *     SHR3_2
 *     KISS2
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

typedef struct
{
    uint32_t    t[256u];
    uint8_t     c;
} SimpleRandomLFIB4_t;

typedef struct
{
    uint32_t    t[256u];
    uint8_t     c;
    uint8_t     borrow;
} SimpleRandomSWB_t;

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

/* Cong -- Congruential random number generator
 *
 * This is a congruential generator with the widely used
 * 69069 multiplier: x[n]=69069x[n-1]+1234567. It has
 * period 2^32.
 *
 * The leading half of its 32 bits seem to pass tests,
 * but bits in the last half are too regular. It fails
 * tests for which those bits play a significant role.
 * Cong+Fib will also have too much regularity in
 * trailing bits, as each does. But keep in mind that
 * it is a rare application for which the trailing
 * bits play a significant role. Cong is one of the
 * most widely used generators of the last 30 years,
 * as it was the system generator for VAX and was
 * incorporated in several popular software packages,
 * all seemingly without complaint.
 */
void simplerandom_cong_seed(SimpleRandomCong_t * p_cong, uint32_t seed);
uint32_t simplerandom_cong_next(SimpleRandomCong_t * p_cong);

/* SHR3 -- 3-shift-register random number generator
 *
 * SHR3 is a 3-shift-register generator with period
 * 2^32-1. It uses y[n]=y[n-1](I+L^17)(I+R^13)(I+L^5),
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

/* MWC -- "Multiply-with-carry" random number generator
 *
 * This uses two MWC generators to generate high and
 * low 16-bit parts, which are then combined to make a
 * 32-bit value.
 *
 * The MWC generator concatenates two 16-bit multiply-
 * with-carry generators:
 *
 *     x[n]=36969x[n-1]+carry,
 *     y[n]=18000y[n-1]+carry mod 2^16,
 *
 * It has a period about 2^60 and seems to pass all
 * tests of randomness. A favorite stand-alone generator,
 * and faster than KISS, which contains it.
 */
void simplerandom_mwc_seed(SimpleRandomMWC_t * p_mwc, uint32_t seed_upper, uint32_t seed_lower);
uint32_t simplerandom_mwc_next(SimpleRandomMWC_t * p_mwc);

/* KISS -- "Keep It Simple Stupid" random number generator
 *
 * It combines the RandomMWC, RandomCong, RandomSHR3
 * generators. Period is about 2^123. It is one of
 * Marsaglia's favourite generators.
 */
void simplerandom_kiss_seed(SimpleRandomKISS_t * p_kiss, uint32_t seed_mwc_upper, uint32_t seed_mwc_lower, uint32_t seed_cong, uint32_t seed_shr3);
uint32_t simplerandom_kiss_next(SimpleRandomKISS_t * p_kiss);

/* Fib -- Classical Fibonacci sequence
 * 
 * x[n]=x[n-1]+x[n-2], but taken modulo 2^32. Its
 * period is 3 * (2^31) if one of its two seeds is
 * odd and not 1 mod 8.
 * 
 * It has little worth as a RNG by itself, since it
 * fails several tests. But it provides a simple and
 * fast component for use in combination generators.
 */
void simplerandom_fib_seed(SimpleRandomFib_t * p_fib, uint32_t seed_a, uint32_t seed_b);
uint32_t simplerandom_fib_next(SimpleRandomFib_t * p_fib);

/* LFIB4 -- "Lagged Fibonacci 4-lag" random number generator
 * 
 * LFIB4 is an extension of what Marsaglia has previously
 * defined as a lagged Fibonacci generator:
 * 
 *     x[n]=x[n-r] op x[n-s]
 * 
 * with the x's in a finite set over which there is a
 * binary operation op, such as +,- on integers mod 2^32,
 * * on odd such integers, exclusive-or(xor) on binary
 * vectors. Except for those using multiplication, lagged
 * Fibonacci generators fail various tests of randomness,
 * unless the lags are very long. (See SWB).
 * 
 * To see if more than two lags would serve to overcome
 * the problems of 2-lag generators using +,- or xor,
 * Marsaglia developed the 4-lag generator LFIB4 using
 * addition:
 * 
 *     x[n]=x[n-256]+x[n-179]+x[n-119]+x[n-55] mod 2^32
 * 
 * Its period is 2^31*(2^256-1), about 2^287,
 * and it seems to pass all tests---in particular,
 * those of the kind for which 2-lag generators using
 * +,-,xor seem to fail.
 * 
 * For even more confidence in its suitability, LFIB4
 * can be combined with KISS, with a resulting period
 * of about 2^410.
 */
void simplerandom_lfib4_seed(SimpleRandomLFIB4_t * p_lfib4);
uint32_t simplerandom_lfib4_next(SimpleRandomLFIB4_t * p_lfib4);

/* SWB -- "Subtract-With-Borrow" random number generator
 * 
 * SWB is a subtract-with-borrow generator that Marsaglia
 * developed to give a simple method for producing
 * extremely long periods:
 * x[n]=x[n-222]-x[n-237]- borrow mod 2^32.
 * The 'borrow' is 0, or set to 1 if computing x[n-1]
 * caused overflow in 32-bit integer arithmetic. This
 * generator has a very long period, 2^7098(2^480-1),
 * about 2^7578.
 * 
 * It seems to pass all tests of randomness, except
 * for the Birthday Spacings test, which it fails
 * badly, as do all lagged Fibonacci generators using
 * +,- or xor. Marsaglia suggests combining SWB with
 * KISS, MWC, SHR3, or Cong. KISS+SWB has period
 * >2^7700 and is highly recommended.
 * 
 * Subtract-with-borrow has the same local behaviour
 * as lagged Fibonacci using +,-,xor---the borrow
 * merely provides a much longer period.
 * 
 * SWB fails the birthday spacings test, as do all
 * lagged Fibonacci and other generators that merely
 * combine two previous values by means of =,- or xor.
 * Those failures are for a particular case: m=512
 * birthdays in a year of n=2^24 days. There are
 * choices of m and n for which lags >1000 will also
 * fail the test. A reasonable precaution is to always
 * combine a 2-lag Fibonacci or SWB generator with
 * another kind of generator, unless the generator uses
 * *, for which a very satisfactory sequence of odd
 * 32-bit integers results.
 */
void simplerandom_swb_seed(SimpleRandomSWB_t * p_swb);
uint32_t simplerandom_swb_next(SimpleRandomSWB_t * p_swb);

/* Cong2 -- Congruential random number generator
 *
 * Very similar to Cong, but with different
 * added constant.
 */
void simplerandom_cong2_seed(SimpleRandomCong_t * p_cong2, uint32_t seed);
uint32_t simplerandom_cong2_next(SimpleRandomCong_t * p_cong2);

/* SHR3_2 -- 3-shift-register random number generator
 *
 * This differs from the SHR3 generator in the values of
 * the three shift operations.
 */
void simplerandom_shr3_2_seed(SimpleRandomSHR3_2_t * p_shr3_2, uint32_t seed);
uint32_t simplerandom_shr3_2_next(SimpleRandomSHR3_2_t * p_shr3_2);

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
void simplerandom_mwc64_seed(SimpleRandomMWC64_t * p_mwc64, uint32_t seed_upper, uint32_t seed_lower);
uint32_t simplerandom_mwc64_next(SimpleRandomMWC64_t * p_mwc64);

/* KISS2 -- "Keep It Simple Stupid" random number generator
 *
 * It combines the MWC64, Cong2, SHR3_2 generators. Period
 * is about 2^123.
 *
 * This is a slightly updated KISS generator design, from
 * the newsgroup post in 2003.
 *
 * The Cong and SHR3 component generators are changed
 * slightly. The MWC component uses a single 64-bit
 * calculation, instead of two 32-bit calculations that
 * are combined.
 */
void simplerandom_kiss2_seed(SimpleRandomKISS2_t * p_kiss2, uint32_t seed_mwc64_upper, uint32_t seed_mwc64_lower, uint32_t seed_cong2, uint32_t seed_shr3_2);
uint32_t simplerandom_kiss2_next(SimpleRandomKISS2_t * p_kiss2);

#endif /* defined(UINT64_C) */


#endif /* !defined(_SIMPLERANDOM_H) */


