/*
 * bitcolumnmatrix.h
 *
 * 32-by-32 Galois-2 matrix, represented by 32 uint32_t integers, representing
 * the bits in each column of the matrix.
 *
 * [ 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 ]
 * [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 ]
 *   ^ ^
 *   | + 2nd column is 2nd element of array
 *   + 1st column is 1st element of array
 *
 * This is represented by array:
 * { 0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, ...
 *   ..., 0x80000000 }
 *
 * This representation allows for fast calculations of add, subtract, multiply
 * and power functions for a Galois-2 32-by-32 matrix.
 *
 * Bit manipulations (and, or, xor, shift) can thus be represented as matrix
 * operations. A sequence of bit manipulations can be represented by a
 * sequence of matrix calculations.
 *
 *     Bit operation        Matrix operation                Code
 *
 *     a ^= b               a += b                          bitcolumnmatrix32_iadd(a, b);
 *
 *     a &= 0x0000FF00      a = mask_matrix(8, 16) * a      bitcolumnmatrix32_mask(mask, 8, 16);
 *                                                          bitcolumnmatrix32_imul(a, mask);
 *
 *     a <<= 5              a = shift_matrix(5) * a         bitcolumnmatrix32_shift(shift, 5);
 *                                                          bitcolumnmatrix32_imul(a, shift);
 *
 *     Do ops n times       where matrix f represents ops--
 *                          f * f * f * ... n times
 *                          i.e. pow(f, n)                  bitcolumnmatrix32_pow(result, f, n);
 */

#ifndef _BITCOLUMNMATRIX_H
#define _BITCOLUMNMATRIX_H


/*****************************************************************************
 * Includes
 ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


/*****************************************************************************
 * Typedefs
 ****************************************************************************/

typedef struct
{
    uint32_t    matrix[32u];
} BitColumnMatrix32_t;


/*****************************************************************************
 * Function prototypes
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void bitcolumnmatrix32_unity(BitColumnMatrix32_t * p_matrix);
void bitcolumnmatrix32_shift(BitColumnMatrix32_t * p_matrix, int_fast8_t shift_value);
void bitcolumnmatrix32_mask(BitColumnMatrix32_t * p_matrix, uint_fast8_t start, uint_fast8_t end);

void bitcolumnmatrix32_iadd(BitColumnMatrix32_t * p_left, const BitColumnMatrix32_t * p_right);
void bitcolumnmatrix32_imul(BitColumnMatrix32_t * p_left, const BitColumnMatrix32_t * p_right);
uint32_t bitcolumnmatrix32_mul_uint32(const BitColumnMatrix32_t * p_left, uint32_t right);
void bitcolumnmatrix32_pow(BitColumnMatrix32_t * p_left, const BitColumnMatrix32_t * p_matrix, uintmax_t n);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* !defined(_BITCOLUMNMATRIX_H) */

