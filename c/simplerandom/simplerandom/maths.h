/*
 * maths.h
 *
 * Integer functions for modulo power, multiplication and geometric series,
 * for uint32_t and uint64_t.
 */
#ifndef SIMPLERANDOM_MATHS_H
#define SIMPLERANDOM_MATHS_H


/*****************************************************************************
 * Includes
 ****************************************************************************/

#ifndef __cplusplus
#error This header simplerandom-cpp.h is only suitable for C++ not C
#else

#define __STDC_LIMIT_MACROS
#include <stdint.h>


/*****************************************************************************
 * Functions
 ****************************************************************************/

namespace simplerandom {

/* Multiplication of uint32_t values, modulo some uint32_t value.
 *
 * This is an implementation that fits all calculations within 32 bits.
 * It is useful for limited platforms that don't have 64-bit types
 * available (smaller embedded systems).
 *
 * This same essential algorithm is used in the implementation of
 * mul_mod_uint64(), since a uint128_t is most likely not available.
 */
template <typename UIntType>
inline UIntType mul_mod(UIntType a, UIntType b, UIntType mod)
{
    UIntType    result = 0;
    UIntType    temp_b;

    if (b >= mod)
    {
        if (mod > std::numeric_limits<UIntType>::max() / 2u)
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

#ifdef UINT64_C

/* Multiplication of uint32_t values, modulo some uint32_t value.
 *
 * This specialisation is a simple implementation that uses 64-bit intermediate
 * results. As long as uint64_t capability is available, this is
 * straight-forward.
 */
template <>
inline UIntType mul_mod<uint32_t>(uint32_t a, uint32_t b, uint32_t mod)
{
    uint64_t    temp;

    temp = (uint64_t)a * b;
    return (uint32_t)(temp % mod);
}

#endif /* defined(UINT64_C) */

/* Calculation of 'base' to the power of 'n', modulo (max + 1) of UIntType. */
template <typename UIntType>
inline UIntType pow(UIntType base, uintmax_t n)
{
    UIntType    result;
    UIntType    temp_exp;

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

/* Calculation of 'base' to the power of an unsigned integer 'n',
 * modulo a value 'mod'. */
template <typename UIntType>
inline UIntType pow_mod(UIntType base, uintmax_t n, UIntType mod)
{
    UIntType    result;
    UIntType    temp_exp;

    result = 1u;
    temp_exp = base;
    for (;;)
    {
        if (n & 1u)
        {
            result = mul_mod(result, temp_exp, mod);
        }
        n >>= 1u;
        if (n == 0)
            break;
        temp_exp = mul_mod(temp_exp, temp_exp, mod);
    }
    return result;
}

/* Calculate geometric series:
 *     1 + r + r^2 + r^3 + ... r^(n-1)
 * summed to n terms, modulo (max + 1) of UIntType.
 *
 * This implementation fits all calculations within the UIntType.
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
template <typename UIntType>
inline UIntType geom_series(UIntType r, uintmax_t n)
{
    UIntType    temp_r;
    UIntType    mult;
    UIntType    result;

    if (n == 0)
        return 0;

    temp_r = r;
    mult = 1;
    result = 0;

    while (n > 1)
    {
        if (n & 1)
            result += mult * pow(temp_r, n - 1);
        mult *= (1 + temp_r);
        temp_r *= temp_r;
        n >>= 1;
    }
    result += mult;
    return result;
}

/* Calculate geometric series:
 *     1 + r + r^2 + r^3 + ... r^(n-1)
 * summed to n terms, modulo mod.
 *
 * This implementation fits all calculations within the UIntType.
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
template <typename UIntType>
inline UIntType geom_series(UIntType r, uintmax_t n, UIntType mod)
{
    UIntType    temp_r;
    UIntType    mult;
    UIntType    result;

    if (n == 0)
        return 0;

    temp_r = r;
    mult = 1;
    result = 0;

    while (n > 1)
    {
        if (n & 1)
            result += mul_mod(mult, pow_mod(temp_r, n - 1, mod), mod);
        mult = mul_mod(mult, (1 + temp_r), mod);
        temp_r = mul_mod(temp_r, temp_r, mod);
        n >>= 1;
    }
    result += mult;
    return result;
}

} // namespace simplerandom

#endif /* defined(__cplusplus) */

#endif /* !defined(SIMPLERANDOM_MATHS_H) */
