/*
 * pow.h
 *
 * Integer power functions for uint32_t and uint64_t.
 */
#ifndef _SIMPLERANDOM_POW_H
#define _SIMPLERANDOM_POW_H


/*****************************************************************************
 * Includes
 ****************************************************************************/

#include <stdint.h>


/*****************************************************************************
 * Function prototypes
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

uint32_t mul_mod_uint32(uint32_t a, uint32_t b, uint32_t mod);
uint32_t pow_uint32(uint32_t base, uintmax_t n);
uint32_t geom_series_uint32(uint32_t r, uintmax_t n);
uint32_t pow_mod_uint32(uint32_t base, uintmax_t n, uint32_t mod);

#ifdef UINT64_C

uint64_t mul_mod_uint64(uint64_t a, uint64_t b, uint64_t mod);
uint64_t pow_uint64(uint64_t base, uintmax_t n);
uint64_t pow_mod_uint64(uint64_t base, uintmax_t n, uint64_t mod);

#endif /* defined(UINT64_C) */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* !defined(_SIMPLERANDOM_POW_H) */
