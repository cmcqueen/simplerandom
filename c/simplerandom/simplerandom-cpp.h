/*
 * simplerandom-cpp.h
 *
 * Simple Pseudo-random Number Generators
 *
 * Most of these are from two newsgroup posts by George Marsaglia.
 *
 * The first was in 1999 [1]. From that newsgroup post, the following RNGs are
 * defined:
 *     MWC1
 * We use the principles of SHR3 and KISS, but with several modifications.
 *
 * MWC2, a small modification of the 1999 MWC, yields a longer period for the
 * lower 16 bits, and therefore better test results in the L'Ecuyer TestU01
 * test suite.
 *
 * Due to analysis of SHR3 in ref [3], the SHR3 as defined in the 1999 post
 * should not be used. It doesn't actually have a period of 2^32-1 as expected,
 * but has 64 cycles, some with very short periods. The SHR3 in the 2003 post
 * is very similar, but with two shift values swapped. It is most likely that
 * the SHR3 shift values in the 1999 post are a typo.
 *
 * Since we don't use the 1999 SHR3, we may as well not use the 1999 Cong or
 * MWC either. The 2003 Cong is almost identical, but with a different added
 * constant. Therefore, we define a KISS function that uses MWC2 (the improved
 * version of the 1999 MWC), and the 2003 SHR3 and Cong. The advantage of using
 * the 1999 MWC and KISS at all is that the MWC uses only 32-bit math, while
 * the 2003 MWC uses 64-bit math which may not be suitable for small embedded
 * systems.
 *
 * The second Marsaglia post was in 2003. From that newsgroup post, the
 * following RNGs are defined:
 *     MWC64
 *     Cong
 *     SHR3
 *     KISS2
 *
 * The LFSR113 generator by L'Ecuyer is also implemented
 * [4], along with his earlier LFSR88.
 *
 * Some generators naturally have "bad" seed values, which if used will
 * not output a good-quality sequence. The most obvious is zero values for the
 * SHR3, MWC and LFSR generators, yielding constant zero output. But there are
 * also other less obvious "bad" seed values for the MWC and LFSR generators.
 *
 * So it is essential to avoid seeding the generators with any bad seed values.
 * The seed functions for all these generators take care of any "bad" seed
 * values, replacing them with a good alternative seed. So all these
 * generators' seed functions can safely be given _any_ unsigned 32-bit seed
 * values.
 *
 * References:
 *
 * [1] Random Numbers for C: End, at last?
 *     George Marsaglia
 *     Newsgroup post, sci.stat.math and others, Thu, 21 Jan 1999
 *     http://www.cse.yorku.ca/~oz/marsaglia-rng.html
 *
 * [2] RNGs
 *     George Marsaglia
 *     Newsgroup post, sci.math, 26 Feb 2003
 *     http://groups.google.com/group/sci.math/msg/9959175f66dd138f
 *
 * [3] "KISS: A Bit Too Simple"
 *     Greg Rose
 *     Qualcomm Inc.
 *     http://eprint.iacr.org/2011/007.pdf
 *
 * [4] "Tables of Maximally-Equidistributed Combined LFSR Generators"
 *     Pierre L'Ecuyer
 *     http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.43.3639
 *     http://www.iro.umontreal.ca/~simardr/rng/lfsr113.c
 */

#ifndef _SIMPLERANDOM_CPP_H
#define _SIMPLERANDOM_CPP_H

#ifndef __cplusplus
#error This header simplerandom-cpp.h is only suitable for C++ not C
#endif

#include <limits>

#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include <simplerandom/maths.h>


/*****************************************************************************
 *
 ****************************************************************************/

namespace simplerandom
{

template<bool> struct StaticAssert;
template<> struct StaticAssert<true> {};


template<typename UIntType, unsigned word_bits, UIntType a>
class mwc_engine
{
    StaticAssert< (std::numeric_limits<UIntType>::is_signed == 0) > _type_must_be_unsigned;
    StaticAssert< ((word_bits % 2u) == 0) > _word_bits_must_be_multiple_2;

public:
    /** The type of the generated random value. */
    typedef UIntType result_type;

    /** The multiplier. */
    static const result_type multiplier     = a;
    /** The modulus. */
    static const unsigned _half_word_bits   = word_bits / 2u;
    static const result_type modulus        = (a * (1u << _half_word_bits)) - 1u;
    static const result_type default_seed   = static_cast<result_type>(0xFFFFFFFFFFFFFFFFu) % modulus;
    static const result_type _lower_mask    = (1u << _half_word_bits) - 1u;

    /** Constructors */
    mwc_engine(result_type s = default_seed)
    { seed(s); }

    /** Seed functions */
    void seed(result_type s = default_seed)
    {
        s %= modulus;
        if (s == 0)
        {
            s = default_seed;
        }
        x = s;
    }

    /** Generation function */
    result_type operator()()
    {
        x = multiplier * (x & _lower_mask) + (x >> _half_word_bits);
        return x;
    }

    result_type min()
    {
        return 0;
    }

    result_type max()
    {
        return modulus - 1u;
    }

    void discard(uintmax_t n)
    {
        x = mul_mod(pow_mod(a, n, modulus), x, modulus);
    }

private:
    result_type     x;
};

class mwc2
{
    mwc_engine<uint32_t, 32u, 36969u> mwc_upper;
    mwc_engine<uint32_t, 32u, 18000u> mwc_lower;

public:
    /** The type of the generated random value. */
    typedef uint32_t result_type;

    /** Constructors */
    mwc2(result_type seed_upper, result_type seed_lower)
        : mwc_upper(seed_upper), mwc_lower(seed_lower)
    {}
    mwc2(result_type s)
        : mwc_upper(s), mwc_lower(s)
    {}
    mwc2()
        : mwc_upper(), mwc_lower()
    {}

    /** Generation function */
    result_type operator()()
    {
        result_type m_u = mwc_upper();
        result_type m_l = mwc_lower();
        return (m_u << 16u) + (m_u >> 16u) + m_l;
    }

    result_type min()
    {
        return 0;
    }

    result_type max()
    {
        return std::numeric_limits<uint32_t>::max();
    }

    void discard(uintmax_t n)
    {
        mwc_upper.discard(n);
        mwc_lower.discard(n);
    }
};

}


#endif /* !defined(_SIMPLERANDOM_CPP_H) */

