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
#else

#include <limits>

#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include <simplerandom/tools.h>
#include <simplerandom/maths.h>
#include <simplerandom/bitcolumnmatrix.h>


/*****************************************************************************
 *
 ****************************************************************************/

namespace simplerandom
{

/*****************************************************************************
 *
 ****************************************************************************/

template<typename UIntType, UIntType a, UIntType c, UIntType m = 0>
class cong_engine
{
    StaticAssert< (std::numeric_limits<UIntType>::is_signed == 0) > _type_must_be_unsigned;
    StaticAssert< (m == 0 || (m >= 2 && a < m && c < m)) > _a_c_or_m_parameter_values;

public:
    /** The type of the generated random value. */
    typedef UIntType result_type;

    static const result_type multiplier     = a;
    static const result_type increment      = c;
    static const result_type modulus        = m;
    static const result_type default_seed   = (c != 0) ? 0 :
                                                ((m == 0) ? (~(UIntType)0) : ((~(UIntType)0) % m));

    /** Constructors */
    cong_engine(result_type s = default_seed)
    { seed(s); }

    /** Seed functions */
    void seed(result_type s = default_seed)
    {
        if (m != 0)
        {
            s %= m;
        }
        if (c == 0 && s == 0)
        {
            s = default_seed;
        }
        x = s;
    }

    /** Generation function */
    result_type operator()()
    {
        if (m == 0)
        {
            x = a * x + c;
        }
        else
        {
            x = (mul_mod(a, x, m) + c) % m;
        }
        return x;
    }
    result_type current() const
    {
        return x;
    }

    static result_type min()
    {
        /* If c is zero, then min is 1.
         * If c is non-zero, then min is 0.
         */
        return (c == 0);
    }

    static result_type max()
    {
        if (m == 0)
            return std::numeric_limits<UIntType>::max();
        else
            return m - 1;
    }

    void discard(uintmax_t n)
    {
        result_type     mult_exp;
        result_type     add_const;
        result_type     cong;

        if (m == 0)
        {
            mult_exp = pow(a, n);
            add_const = geom_series(a, n) * c;
            x = mult_exp * x + add_const;
        }
        else
        {
            mult_exp = pow_mod(a, n, m);
            add_const = mul_mod(geom_series(a, n, m), c, m);
            x = (mul_mod(mult_exp, x, m) + add_const) % m;
        }
    }

private:
    result_type     x;
};

typedef cong_engine<uint32_t, 69069u, 12345u> cong;


/*****************************************************************************
 *
 ****************************************************************************/

/* SHR3 engine, aka XorShift, from Marsaglia
 *
 * The three shift values have to be carefully chosen so that the resulting
 * sequence has a period of 2^32-1, and has decent statistical properties.
 *
 * SHR3 is a 3-shift-register generator with period 2^32-1. It uses
 *     y[n]=y[n-1](I+L^a)(I+L^b)(I+L^c)
 * with the y's viewed as binary vectors, L the 32x32 binary matrix that
 * shifts a vector left 1, and R its transpose. a, b and c are shift values,
 * and may be negative meaning right-shift i.e. L^a == R^(-a).
 *
 * Assuming a, b and c are well chosen, SHR3 should pass all Marsaglia's
 * Diehard tests except those related to the binary rank test, since
 * 32 successive values, as binary vectors, must be linearly independent,
 * while 32 successive truly random 32-bit integers, viewed as binary vectors
 * will be linearly independent only about 29% of the time.
 */
template<typename UIntType, int sh1, int sh2, int sh3, unsigned word_bits = 0>
class shr3_engine
{
    StaticAssert< (std::numeric_limits<UIntType>::is_signed == 0) > _type_must_be_unsigned;
    StaticAssert< (word_bits <= std::numeric_limits<UIntType>::digits) > _word_bits_must_fit_in_uinttype;

public:
    /** The type of the generated random value. */
    typedef UIntType result_type;

    static const int shift1                 = sh1;
    static const int shift2                 = sh2;
    static const int shift3                 = sh3;
    static const unsigned _word_bits        = (word_bits == 0) ? std::numeric_limits<UIntType>::digits : word_bits;
    static const result_type _word_mask     = (word_bits == 0) ? static_cast<result_type>(0xFFFFFFFFFFFFFFFFu) : ((1u << _word_bits) - 1u);
    static const result_type default_seed   = _word_mask;

    /** Constructors */
    shr3_engine(result_type s = default_seed)
    { seed(s); }

    /** Seed functions */
    void seed(result_type s = default_seed)
    {
        if (s == 0)
        {
            s = default_seed;
        }
        x = s;
    }

    /** Generation function */
    result_type operator()()
    {
        x ^= signed_left_shift(x, sh1) & _word_mask;
        x ^= signed_left_shift(x, sh2) & _word_mask;
        x ^= signed_left_shift(x, sh3) & _word_mask;
        return x;
    }
    result_type current() const
    {
        return x;
    }

    static result_type min()
    {
        return 1u;
    }

    static result_type max()
    {
        return _word_mask;
    }

    void discard(uintmax_t n)
    {
        typedef BitColumnMatrix<result_type> bcm;

        bcm shr3_matrix_a = bcm::unity() + bcm::shift(sh1);
        bcm shr3_matrix = shr3_matrix_a;
        bcm shr3_matrix_b = bcm::unity() + bcm::shift(sh2);
        shr3_matrix = shr3_matrix_b * shr3_matrix;
        bcm shr3_matrix_c = bcm::unity() + bcm::shift(sh3);
        shr3_matrix = shr3_matrix_c * shr3_matrix;

        x = shr3_matrix.pow(n) * x;
    }

private:
    result_type     x;
};

/* SHR3 from Marsaglia's 2003 post
 *
 * Reading between the lines, I believe the SHR3 defined in Marsaglia's 1999
 * post actually has a typo: the shift values defined don't actually produce
 * a period of 2^32-1, but have 64 possible cycles, some extremely short. But
 * the swapped values from Marsaglia's 2003 post produce the full 2^32-1
 * period. So we use that definition of SHR3.
 *
 * SHR3 is a 3-shift-register generator with period 2^32-1. It uses
 *     y[n]=y[n-1](I+L^13)(I+R^17)(I+L^5)
 * with the y's viewed as binary vectors, L the 32x32 binary matrix that
 * shifts a vector left 1, and R its transpose.
 *
 * SHR3 seems to pass all except those related to the binary rank test, since
 * 32 successive values, as binary vectors, must be linearly independent,
 * while 32 successive truly random 32-bit integers, viewed as binary
 * vectors, will be linearly independent only about 29% of the time.
 */
typedef shr3_engine<uint32_t, 13, -17, 5> shr3;


/*****************************************************************************
 *
 ****************************************************************************/

/* If template parameter word_bits == 0, this is treated as if
 * word_bits = std::numeric_limits<UIntType>::digits (that is, number of bits
 * of the type). E.g., 32 for uint32_t.
 *
 * mwc_engine<UIntType, a, word_bits> is a special case of linear congruential
 * generator. It is equivalent to
 * cong_engine<UIntType, a, 0, a * (1 << (word_bits / 2)) - 1>
 * The generator calculation uses a numeric trick which makes the calculation
 * faster for this special case.
 */
template<typename UIntType, UIntType a, unsigned _word_bits = 0>
class mwc_engine
{
private:
    StaticAssert< (std::numeric_limits<UIntType>::is_signed == 0) > _type_must_be_unsigned;
    StaticAssert< (_word_bits <= std::numeric_limits<UIntType>::digits) > _word_bits_must_fit_in_uinttype;
    StaticAssert< ((_word_bits % 2u) == 0) > _word_bits_must_be_multiple_2;

public:
    /** The type of the generated random value. */
    typedef UIntType result_type;

    static const unsigned word_bits        = (_word_bits == 0) ? std::numeric_limits<UIntType>::digits : _word_bits;
    static const unsigned _half_word_bits   = word_bits / 2u;
    static const result_type multiplier     = a;
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
    result_type current() const
    {
        return x;
    }

    static result_type min()
    {
        return 1u;
    }

    static result_type max()
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
protected:
#if 1
    mwc_engine<uint32_t, 36969u> mwc_upper;
    mwc_engine<uint32_t, 18000u> mwc_lower;
#else
    /* Just to demonstrate that mwc_engine is equivalent to cong_engine with
     * certain parameters. */
    cong_engine<uint32_t, 36969u, 0, 36969u * 65536u - 1> mwc_upper;
    cong_engine<uint32_t, 18000u, 0, 18000u * 65536u - 1> mwc_lower;
#endif

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
        mwc_upper();
        mwc_lower();
        return current();
    }

    virtual result_type current() const
    {
        result_type m_u = mwc_upper.current();
        result_type m_l = mwc_lower.current();

        return (m_u << 16u) + (m_u >> 16u) + m_l;
    }

    static result_type min()
    {
        return 0;
    }

    static result_type max()
    {
        return std::numeric_limits<uint32_t>::max();
    }

    void discard(uintmax_t n)
    {
        mwc_upper.discard(n);
        mwc_lower.discard(n);
    }
};

class mwc1 : public mwc2
{
public:
    /** Constructors */
    mwc1(result_type seed_upper, result_type seed_lower)
        : mwc2(seed_upper, seed_lower)
    {}
    mwc1(result_type s)
        : mwc2(s)
    {}
    mwc1()
        : mwc2()
    {}

    result_type current() const
    {
        result_type m_u = mwc_upper.current();
        result_type m_l = mwc_lower.current();

        return (m_u << 16u) + m_l;
    }
};


} // namespace simplerandom


#endif /* !defined(__cplusplus) */

#endif /* !defined(_SIMPLERANDOM_CPP_H) */

