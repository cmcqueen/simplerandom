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
 * Functions
 ****************************************************************************/

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
    uint64_t    result;
    uint64_t    temp_exp;

    result = 1u;
    temp_exp = base;
    for (;;)
    {
        if (n & 1u)
        {
            result *= temp_exp;
            result %= mod;
        }
        n >>= 1u;
        if (n == 0)
            break;
        temp_exp *= temp_exp;
        temp_exp %= mod;
    }
    return (uint32_t)result;
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
