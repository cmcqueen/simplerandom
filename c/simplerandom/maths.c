/*
 * maths.c
 *
 * Integer functions for modulo power, multiplication and geometric series,
 * for uint32_t and uint64_t.
 */


/*****************************************************************************
 * Includes
 ****************************************************************************/

#include "maths.h"


/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/

/*****************************************************************************
 * Functions
 ****************************************************************************/

#ifdef UINT64_C

/* Simple implementation that uses 64-bit intermediate results */
uint32_t mul_mod_uint32(uint32_t a, uint32_t b, uint32_t mod)
{
    uint64_t    temp;

    temp = (uint64_t)a * b;
    return (uint32_t)(temp % mod);
}

#else /* !defined(UINT64_C) */

/* Slower implementation that fits all calculations within 32 bits. */
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

#ifdef UINT64_C

/* Calculate geometric series:
 *     1 + r + r^2 + r^3 + ... r^(n-1)
 * summed to n terms, modulo 2^32.
 *
 * This implementation is by the formula for sum of geometric series:
 *     (r^n - 1) / (r - 1)
 * For calculating geometric series mod 2^32, see:
 * http://www.codechef.com/wiki/tutorial-just-simple-sum#Back_to_the_geometric_series
 *
 * It is tricky to do this calculation mod 2^32.
 * Split the denominator into:
 *     - common factors with the modulo 2^32 (that is, all factors of 2)
 *     - other factors (which are then coprime with the modulo 2^32)
 * Calculate the numerator mod (common_factors * 2^32) (which requires 64-bit
 * calculations).
 * Then divide by the common factor.
 * Then multiply by the inverse mod 2^32 of the other factors.
 *
 */
uint32_t geom_series_uint32(uint32_t r, uintmax_t n)
{
    uint64_t    numerator;
    uint32_t    common_factor;
    uint32_t    other_factors;
    uint32_t    other_factors_inverse;

    if (n == 0)
        return 0;
    if (n == 1u || r == 0)
        return 1u;
    /* Split (r - 1) into common factors with the modulo 2**32 -- i.e. all
     * factors of 2; and other factors which are coprime with the modulo 2**32.
     */
    other_factors = r - 1u;
    common_factor = 1u;
    while ((other_factors & 1u) == 0)
    {
        /* Trailing zero -- it's a multiple of 2 */
        other_factors >>= 1u;
        common_factor <<= 1u;
    }
    other_factors_inverse = pow_uint32(other_factors, 0xFFFFFFFFu);
    numerator = pow_mod_uint64(r, n, common_factor * UINT64_C(0x100000000)) - 1u;
    return (numerator / common_factor * other_factors_inverse);
}

#else /* !defined(UINT64_C) */

/* Calculate geometric series:
 *     1 + r + r^2 + r^3 + ... r^(n-1)
 * summed to n terms, modulo 2^32.
 *
 * It makes use of the fact that the series can pair up terms:
 *     (1 + r) + (1 + r) r^2 + (1 + r) r^4 + ... + (1 + r) (r^2)^(n/2-1) + [ r^(n-1) if n is odd ]
 *     (1 + r) (1 + r^2 + r^4 + ... + (r^2)^(n/2-1)) + [ r^(n-1) if n is odd ]
 *
 * Which can be easily calculated by recursion, with time order O(log n), and
 * also stack depth O(log n).
 * This implementation is by recursion. Stack depth O(log n) isn't ideal; a
 * non-recursive implementation is preferable.
 */
uint32_t geom_series_uint32(uint32_t r, uintmax_t n)
{
    uint32_t    temp;

    if (n == 0)
        return 0;
    if (n == 1)
        return 1;
    temp = (1 + r) * geom_series_uint32(r * r, n / 2);
    if (n & 1)
        temp += pow_uint32(r, n - 1);
    return temp;
}

#endif /* defined(UINT64_C) */

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
