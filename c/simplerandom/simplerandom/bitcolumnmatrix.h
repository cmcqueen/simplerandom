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


/*****************************************************************************
 *
 ****************************************************************************/

namespace simplerandom {

/*****************************************************************************
 * Types
 ****************************************************************************/

template <typename UIntType, unsigned word_bits = 0>
class BitColumnMatrix
{
    StaticAssert< (std::numeric_limits<UIntType>::is_signed == 0) > _type_must_be_unsigned;
    StaticAssert< (word_bits <= std::numeric_limits<UIntType>::digits) > _word_bits_must_fit_in_uinttype;

public:
    /** The type of the generated random value. */
    typedef UIntType result_type;

    static const unsigned _word_bits        = (word_bits == 0) ? std::numeric_limits<UIntType>::digits : word_bits;

    /* Constructors */
    BitColumnMatrix() { }

    template<class InputIterator>
    BitColumnMatrix(const InputIterator begin, const InputIterator end)
    {
        for (i = 0, const InputIterator p = begin;
             p != end && i < _word_bits;
             ++i, ++p)
        {
            matrix[i] = static_cast<result_type>(*p);
        }
    }

    static BitColumnMatrix unity()
    {
        result_type     matrix[_word_bits];
        size_t          i;
        uint32_t        value;

        value = 1u;
        for (i = 0; i < _word_bits; i++)
        {
            matrix[i] = value;
            value <<= 1u;
        }
        return BitColumnMatrix(&matrix, &matrix[_word_bits]);
    }

    static BitColumnMatrix shift(int shift_value)
    {
        result_type     matrix[_word_bits];
        size_t          i;
        uint32_t        value;

        if (shift_value >= 0)
            value = 1u << shift_value;
        else
            value = 0;
        for (i = 0; i < _word_bits; i++)
        {
            matrix[i] = value;
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
        return BitColumnMatrix(&matrix, &matrix[_word_bits]);
    }

protected:
    result_type     matrix[_word_bits];
};


} // namespace simplerandom

#endif /* defined(__cplusplus) */

#endif /* !defined(_SIMPLERANDOM_BITCOLUMNMATRIX_H) */
