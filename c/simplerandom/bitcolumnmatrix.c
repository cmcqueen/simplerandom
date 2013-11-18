/*
 * bitcolumnmatrix.c
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
 */

/*****************************************************************************
 * Includes
 ****************************************************************************/

#include "bitcolumnmatrix.h"

#include <string.h>


/*****************************************************************************
 * Typedefs
 ****************************************************************************/

/*****************************************************************************
 * Functions
 ****************************************************************************/

/* Create a unity matrix -- that is, 1 on the main diagonal, 0 elsewhere. */
void bitcolumnmatrix32_unity(BitColumnMatrix32_t * p_matrix)
{
    size_t      i;
    uint32_t    value;

    if (p_matrix != NULL)
    {
        value = 1u;
        for (i = 0; i < 32u; i++)
        {
            p_matrix->matrix[i] = value;
            value <<= 1u;
        }
    }
}

/* Create a shift matrix -- which if multiplied into a vector, would be
 * equivalent to a left or right shift of the vector value.
 * That is, 1 on a super- or sub-diagonal, 0 elsewhere.
 */
void bitcolumnmatrix32_shift(BitColumnMatrix32_t * p_matrix, int_fast8_t shift_value)
{
    size_t      i;
    uint32_t    value;

    if (p_matrix != NULL)
    {
        if (shift_value >= 0)
            value = 1u << shift_value;
        else
            value = 0;

        for (i = 0; i < 32u; i++)
        {
            p_matrix->matrix[i] = value;
            if (shift_value < 0)
            {
                ++shift_value;
                if (shift_value == 0)
                    value = 1u;
            }
            else
            {
                value <<= 1u;
            }
        }
    }
}

/* Add two matrices, with the result put in the left matrix.
 * That is to say, left += right.
 */
void bitcolumnmatrix32_iadd(BitColumnMatrix32_t * p_left, const BitColumnMatrix32_t * p_right)
{
    size_t      i;

    if (p_left != NULL && p_right != NULL)
    {
        for (i = 0; i < 32u; i++)
        {
            p_left->matrix[i] ^= p_right->matrix[i];
        }
    }
}

/* Multiply a matrix with a vector, resulting in a vector result.
 * The input and result vectors are represented by a uint32_t value.
 * That is to say, result_vector = left_matrix * right_vector.
 */
uint32_t bitcolumnmatrix32_mul_uint32(const BitColumnMatrix32_t * p_left, uint32_t right)
{
    size_t      i;
    uint32_t    result;

    result = 0;
    if (p_left != NULL)
    {
        for (i = 0; i < 32u; i++)
        {
            if (right & 1u)
                result ^= p_left->matrix[i];
            right >>= 1u;
        }
    }
    return result;
}

/* Multiply two matrices, with the result put in the left matrix.
 * That is to say, left *= right.
 */
void bitcolumnmatrix32_imul(BitColumnMatrix32_t * p_left, const BitColumnMatrix32_t * p_right)
{
    BitColumnMatrix32_t matrix_result;
    size_t              i;

    if (p_left != NULL && p_right != NULL)
    {
        for (i = 0; i < 32u; i++)
        {
            matrix_result.matrix[i] = bitcolumnmatrix32_mul_uint32(p_left, p_right->matrix[i]);
        }

        /* Copy result to left matrix */
        memcpy(p_left, &matrix_result, sizeof(*p_left));
    }
}

/* Raise matrix to the power of 'n'.
 * Algorithm is "exponentiation by squaring". Time complexity is O(log n).
 */
void bitcolumnmatrix32_pow(BitColumnMatrix32_t * p_result, const BitColumnMatrix32_t * p_matrix, uintmax_t n)
{
    BitColumnMatrix32_t matrix_result;
    BitColumnMatrix32_t matrix_exp;

    if (p_result != NULL && p_matrix != NULL)
    {
        bitcolumnmatrix32_unity(&matrix_result);
        memcpy(&matrix_exp, p_matrix, sizeof(matrix_exp));

        for (;;)
        {
            if (n & 1u)
            {
                bitcolumnmatrix32_imul(&matrix_result, &matrix_exp);
            }
            n >>= 1;
            if (n == 0)
                break;
            bitcolumnmatrix32_imul(&matrix_exp, &matrix_exp);
        }
        /* Copy result to result matrix */
        memcpy(p_result, &matrix_result, sizeof(*p_result));
    }
}

