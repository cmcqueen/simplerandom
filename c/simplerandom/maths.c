/*
 * maths.c
 *
 * Integer functions for modulo power, multiplication and geometric series,
 * for uint32_t and uint64_t.
 */


/*****************************************************************************
 * Includes
 ****************************************************************************/

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include "maths.h"


/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/

/*****************************************************************************
 * Functions
 ****************************************************************************/

#ifdef UINT64_C

/* Multiplication of uint32_t values, modulo some uint32_t value.
 *
 * This is a simple implementation that uses 64-bit intermediate results.
 * As long as uint64_t capability is available, this is straight-forward.
 */
uint32_t mul_mod_uint32(uint32_t a, uint32_t b, uint32_t mod)
{
    uint64_t    temp;

    temp = (uint64_t)a * b;
    return (uint32_t)(temp % mod);
}

#else /* !defined(UINT64_C) */

/* Multiplication of uint32_t values, modulo some uint32_t value.
 *
 * This is an implementation that fits all calculations within 32 bits.
 * It is useful for limited platforms that don't have 64-bit types
 * available (smaller embedded systems).
 *
 * This same essential algorithm is used in the implementation of
 * mul_mod_uint64(), since a uint128_t is most likely not available.
 */
uint32_t mul_mod_uint32(uint32_t a, uint32_t b, uint32_t mod)
{
    uint32_t    result = 0;
    uint32_t    temp_b;

    if (b >= mod)
    {
        if (mod > UINT32_MAX / 2u)
            b -= mod;
        else
            b %= mod;
    }

    while (a != 0)
    {
        if (a & 1u)
        {
            if (b >= mod - result)
                result -= mod;
            result += b;
        }
        a >>= 1u;

        temp_b = b;
        if (b >= mod - temp_b)
            temp_b -= mod;
        b += temp_b;
    }
    return result;
}

#endif /* defined(UINT64_C) */

/* 32-bit calculation of 'base' to the power of 'n', modulo 2^32. */
uint32_t pow_uint32(uint32_t base, uintmax_t n)
{
    uint32_t    result;
    uint32_t    temp_exp;

    result = 1u;
    temp_exp = base;
    for (;;)
    {
        if (n & 1u)
            result *= temp_exp;
        n >>= 1u;
        if (n == 0)
            break;
        temp_exp *= temp_exp;
    }
    return result;
}

/* Calculate geometric series:
 *     1 + r + r^2 + r^3 + ... r^(n-1)
 * summed to n terms, modulo 2^32.
 *
 * It makes use of the fact that the series can pair up terms:
 *     (1 + r) + (1 + r) r^2 + (1 + r) r^4 + ... + (1 + r) (r^2)^(n/2-1) + [ r^(n-1) if n is odd ]
 *     (1 + r) (1 + r^2 + r^4 + ... + (r^2)^(n/2-1)) + [ r^(n-1) if n is odd ]
 *
 * Which can easily be calculated by recursion, with time order O(log n), and
 * also stack depth O(log n). However that stack depth isn't good, so a
 * non-recursive implementation is preferable.
 * This implementation is by a loop, not recursion, with time order
 * O(log n) and stack depth O(1).
 */
uint32_t geom_series_uint32(uint32_t r, uintmax_t n)
{
    uint32_t    temp_r;
    uint32_t    mult;
    uint32_t    result;

    if (n == 0)
        return 0;

    temp_r = r;
    mult = 1;
    result = 0;

    while (n > 1)
    {
        if (n & 1)
            result += mult * pow_uint32(temp_r, n - 1);
        mult *= (1 + temp_r);
        temp_r *= temp_r;
        n >>= 1;
    }
    result += mult;
    return result;
}

/* 32-bit calculation of 'base' to the power of an unsigned integer 'n',
 * modulo a uint32_t value 'mod'. */
uint32_t pow_mod_uint32(uint32_t base, uintmax_t n, uint32_t mod)
{
    uint32_t    result;
    uint32_t    temp_exp;

    result = 1u;
    temp_exp = base;
    for (;;)
    {
        if (n & 1u)
        {
            result = mul_mod_uint32(result, temp_exp, mod);
        }
        n >>= 1u;
        if (n == 0)
            break;
        temp_exp = mul_mod_uint32(temp_exp, temp_exp, mod);
    }
    return (uint32_t)result;
}

#ifdef UINT64_C

/* Multiplication of uint64_t values, modulo some uint64_t value.
 *
 * A "simple" implementation would require 128-bit intermediate values
 * (see the "simple" implementation of mul_mod_uint32 that uses 64-bit
 * intermediate calculations as a comparison). Given that a uint128_t isn't
 * commonly available, this is an implementation that fits all calculations
 * within 64 bits. It uses the same essential algorithm as the 32-bit-only
 * implementation of mul_mod_uint32().
 */
uint64_t mul_mod_uint64(uint64_t a, uint64_t b, uint64_t mod)
{
    uint64_t    result = 0;
    uint64_t    temp_b;

    if (b >= mod)
    {
        if (mod > UINT64_MAX / 2u)
            b -= mod;
        else
            b %= mod;
    }

    while (a != 0)
    {
        if (a & 1u)
        {
            if (b >= mod - result)
                result -= mod;
            result += b;
        }
        a >>= 1u;

        temp_b = b;
        if (b >= mod - temp_b)
            temp_b -= mod;
        b += temp_b;
    }
    return result;
}

/* 64-bit calculation of 'base' to the power of 'n', modulo 2^64. */
uint64_t pow_uint64(uint64_t base, uintmax_t n)
{
    uint64_t    result;
    uint64_t    temp_exp;

    result = 1u;
    temp_exp = base;
    for (;;)
    {
        if (n & 1u)
            result *= temp_exp;
        n >>= 1u;
        if (n == 0)
            break;
        temp_exp *= temp_exp;
    }
    return result;
}

/* 64-bit calculation of 'base' to the power of an unsigned integer 'n',
 * modulo a uint64_t value 'mod'. */
uint64_t pow_mod_uint64(uint64_t base, uintmax_t n, uint64_t mod)
{
    uint64_t    result;
    uint64_t    temp_exp;

    result = 1u;
    temp_exp = base;
    for (;;)
    {
        if (n & 1u)
        {
            result = mul_mod_uint64(result, temp_exp, mod);
        }
        n >>= 1u;
        if (n == 0)
            break;
        temp_exp = mul_mod_uint64(temp_exp, temp_exp, mod);
    }
    return (uint64_t)result;
}

#endif /* defined(UINT64_C) */
