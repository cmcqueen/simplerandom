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
 */
#ifndef _SIMPLERANDOM_BITCOLUMNMATRIX_H
#define _SIMPLERANDOM_BITCOLUMNMATRIX_H

#ifndef __cplusplus
#error This header simplerandom-cpp.h is only suitable for C++ not C
#else

/*****************************************************************************
 * Includes
 ****************************************************************************/

#include <simplerandom/tools.h>
#include <ostream>


/*****************************************************************************
 *
 ****************************************************************************/

namespace simplerandom {

/*****************************************************************************
 * Types
 ****************************************************************************/

template <typename UIntType, unsigned _word_bits = 0>
class BitColumnMatrix
{
    StaticAssert< (std::numeric_limits<UIntType>::is_signed == 0) > _type_must_be_unsigned;
    StaticAssert< (_word_bits <= std::numeric_limits<UIntType>::digits) > _word_bits_must_fit_in_uinttype;

public:
    /** The type of the generated random value. */
    typedef UIntType result_type;

    static const unsigned word_bits        = (_word_bits == 0) ? (std::numeric_limits<UIntType>::digits) : (_word_bits);

    /* Constructors */
    BitColumnMatrix() { }

    template<typename InputIterator>
    BitColumnMatrix(InputIterator begin, InputIterator end)
    {
        unsigned i;
        InputIterator p;
        for (i = 0, p = begin;
             p != end && i < word_bits;
             ++i, ++p)
        {
            matrix[i] = static_cast<result_type>(*p);
        }
    }

    static BitColumnMatrix unity()
    {
        result_type     result_matrix[word_bits];
        unsigned        i;
        uint32_t        value;

        value = 1u;
        for (i = 0; i < word_bits; i++)
        {
            result_matrix[i] = value;
            value <<= 1u;
        }
        return BitColumnMatrix(&result_matrix[0], &result_matrix[word_bits]);
    }

    static BitColumnMatrix shift(int shift_value)
    {
        result_type     result_matrix[word_bits];
        unsigned        i;
        uint32_t        value;

        if (shift_value >= 0)
            value = 1u << shift_value;
        else
            value = 0;
        for (i = 0; i < word_bits; i++)
        {
            result_matrix[i] = value;
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
        return BitColumnMatrix(&result_matrix[0], &result_matrix[word_bits]);
    }

    result_type operator[](unsigned i) const
    {
        if (i < word_bits)
            return matrix[i];
        else
            return 0;
    }

    /* Add matrices */
    BitColumnMatrix operator+ (const BitColumnMatrix & bcm_b)
    {
        result_type         result_matrix[word_bits];
        unsigned            i;

        for (i = 0; i < word_bits; i++)
        {
            result_matrix[i] = matrix[i] ^ bcm_b[i];
        }

        return BitColumnMatrix(&result_matrix[0], &result_matrix[word_bits]);
    }

    /* Multiply matrix by integer */
    result_type operator* (result_type right)
    {
        unsigned    i;
        result_type result;

        result = 0;
        for (i = 0; i < word_bits; ++i)
        {
            if (right & 1u)
                result ^= matrix[i];
            right >>= 1u;
        }
        return result;
    }

    /* Multiply matrices */
    BitColumnMatrix operator* (const BitColumnMatrix & bcm_b)
    {
        result_type         result_matrix[word_bits];
        unsigned            i;

        for (i = 0; i < word_bits; i++)
        {
            result_matrix[i] = (*this) * bcm_b[i];
        }

        return BitColumnMatrix(&result_matrix[0], &result_matrix[word_bits]);
    }

    /* Raise matrix to the power of 'n'.
     * Algorithm is "exponentiation by squaring". Time complexity is O(log n).
     */
    BitColumnMatrix pow(uintmax_t n)
    {
        BitColumnMatrix     matrix_result;
        BitColumnMatrix     matrix_exp;

        matrix_result = BitColumnMatrix::unity();
        matrix_exp = *this;

        for (;;)
        {
            if (n & 1u)
            {
                matrix_result = matrix_result * matrix_exp;
            }
            n >>= 1;
            if (n == 0)
                break;
            matrix_exp = matrix_exp * matrix_exp;
        }
        return matrix_result;
    }

protected:
    result_type     matrix[word_bits];
};

template <typename charT, class traits, typename UIntType, unsigned _word_bits>
std::basic_ostream<charT,traits>& operator<< (std::basic_ostream<charT,traits>& os,
                                              const BitColumnMatrix<UIntType,_word_bits>& bcm)
{
    for (unsigned bit_mask = 1u;
            bit_mask != 0 && bit_mask <= (1u << (bcm.word_bits - 1));
            bit_mask <<= 1u)
    {
        for (unsigned j = 0; j < bcm.word_bits; ++j)
        {
            if (j > 0)
                os << " ";
            os << ((bcm[j] & bit_mask) ? "1" : "0");
        }
//        if (bit_mask > 1u)
            os << std::endl;
    }
    return os;
}


} // namespace simplerandom

#endif /* defined(__cplusplus) */

#endif /* !defined(_SIMPLERANDOM_BITCOLUMNMATRIX_H) */
