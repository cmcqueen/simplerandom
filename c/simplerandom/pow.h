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

uint32_t pow_uint32(uint32_t base, uintmax_t n);

/* Calculate power modulo a certain value.
 * This requires uint64_t calculations, assuming mod is not a power of 2.
 */
uint32_t pow_mod_uint32(uint32_t base, uintmax_t n, uint32_t mod);

uint64_t pow_uint64(uint64_t base, uintmax_t n);


#endif /* !defined(_SIMPLERANDOM_POW_H) */
