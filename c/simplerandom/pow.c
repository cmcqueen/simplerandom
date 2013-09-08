/*
 * pow.c
 *
 * Integer power functions for uint32_t and uint64_t.
 */


/*****************************************************************************
 * Includes
 ****************************************************************************/

#include "pow.h"


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