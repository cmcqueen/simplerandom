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

#define __STDC_CONSTANT_MACROS
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

/**
 * Cong RNG of the form x[n] = x[n-1] * a + c
 * a is the multiplier.
 * c is the added constant.
 * m is the modulus of the calculations.
 * m == 0 is a special case, to mean the modulus is the implicit modulus of
 * the integer type UIntType due to integer overflow.
 * That is to say, numeric_limits<UIntType>::max() + 1.
 */
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
    static const result_type _half_modulus_minus_1  = ((m == 0) ? (((result_type)-1) / 2u) : ((m - 1u) / 2u));
    static const result_type default_seed   = (c != 0) ? 0 :
                                                ((_half_modulus_minus_1 != 0) ? _half_modulus_minus_1 : 1u);

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
        return (c == 0) ? 1 : 0;
    }

    static result_type max()
    {
        if (m == 0)
            return std::numeric_limits<result_type>::max();
        else
            return m - 1;
    }

    /* Cong discard(n) = a^n * x mod m +
     *                      c * (1 + a + a^2 + ... + a^(n-1)) mod m
     *
     * The part c * (1 + a + a^2 + ... + a^(n-1)) is a geometric series.
     */
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
 * sequence has a maximal period, and has decent statistical properties. See
 * Marsaglia's paper on XorShift generators for details. However, note that
 * L'Ecuyer analysed Marsaglia's 3-shift XorShift generators, and found they
 * do not have good statistical properties.
 *
 * SHR3 is a 3-shift-register generator with period 2^word_bits-1. It uses
 *     y[n]=y[n-1](I+L^a)(I+L^b)(I+L^c)
 * with the y's viewed as binary vectors, L the word_bitsxword_bits binary
 * matrix that shifts a vector left 1, and R its transpose. a, b and c are
 * shift values, and may be negative meaning right-shift i.e. L^a == R^(-a).
 */
template<typename UIntType, int sh1, int sh2, int sh3, unsigned _word_bits = 0>
class shr3_engine
{
    StaticAssert< (std::numeric_limits<UIntType>::is_signed == 0) > _type_must_be_unsigned;
    StaticAssert< (_word_bits <= std::numeric_limits<UIntType>::digits) > _word_bits_must_fit_in_uinttype;

public:
    /** The type of the generated random value. */
    typedef UIntType result_type;

    static const int shift1                 = sh1;
    static const int shift2                 = sh2;
    static const int shift3                 = sh3;
    static const unsigned _type_bits        = std::numeric_limits<result_type>::digits;
    static const unsigned word_bits         = (_word_bits == 0) ? _type_bits : _word_bits;
    static const result_type _word_mask     = (word_bits >= _type_bits) ? (~(result_type)0) : (((result_type)1u << word_bits) - 1u);
    static const result_type _half_word_mask    = _word_mask / 2u;
    static const result_type default_seed   = (_half_word_mask != 0) ? _half_word_mask : 1u;

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
 * Marsaglia specified SHR3 in his 1999 newsgroup post. But it seems that the
 * SHR3 defined in Marsaglia's 1999 post actually has a typo: the shift values
 * defined don't actually produce a period of 2^32-1, but have 64 possible
 * cycles, some extremely short. But the swapped values from Marsaglia's 2003
 * post produce the full 2^32-1 period. So we use that definition of SHR3.
 *
 * SHR3 is a 3-shift-register generator with period 2^32-1. It uses
 *     y[n]=y[n-1](I+L^13)(I+R^17)(I+L^5)
 * with the y's viewed as binary vectors, L the 32x32 binary matrix that
 * shifts a vector left 1, and R its transpose.
 *
 * SHR3 seems to pass all Marsaglia's Diehard tests except those related to
 * the binary rank test, since 32 successive values, as binary vectors, must
 * be linearly independent, while 32 successive truly random 32-bit integers,
 * viewed as binary vectors, will be linearly independent only about 29% of
 * the time.
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

    static const unsigned word_bits         = (_word_bits == 0) ? std::numeric_limits<result_type>::digits : _word_bits;
    static const unsigned _half_word_bits   = word_bits / 2u;
    static const result_type multiplier     = a;
    static const result_type modulus        = (a * ((result_type)1u << _half_word_bits)) - 1u;
    static const result_type _half_modulus_minus_1  = (modulus - 1u) / 2u;
    static const result_type default_seed   = (_half_modulus_minus_1 != 0) ? _half_modulus_minus_1 : 1u;
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

/* mwc2 is a specific instantiation of two 32-bit mwc_engine.
 * The two generator's outputs are added after one is rotated by 16 bits.
 * The resulting RNG has quite good statistical properties, according to
 * TestU01 testing results.
 * It is a variation of the MWC (mwc1 below) specified by Marsaglia.
 * It is very similar to that, except the 2nd MWC component is rotated instead
 * of shifted, preserving more random bits and thus improving statistical
 * properties.
 */
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
        return std::numeric_limits<result_type>::max();
    }

    void discard(uintmax_t n)
    {
        mwc_upper.discard(n);
        mwc_lower.discard(n);
    }
};

/* mwc1 is a specific instantiation of two 32-bit mwc_engine, as specified by
 * Marsaglia.
 * The two generator's outputs are added after one is shifted by 16 bits.
 * It is very similar to mwc2, except the 2nd MWC component is shifted instead
 * of rotated. mwc2 (with better statistical properties) is to be preferred
 * over this one, except if the exact Marsaglia MWC algorithm is wanted.
 */
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

#ifdef UINT64_C

/* mwc64 is a specific instantiation of one 64-bit mwc_engine, as specified by
 * Marsaglia.
 * It may have the advantage of being fast on a 64-bit processor. But its
 * statistical properties are not as good as the mwc2 generator.
 */
class mwc64
{
protected:
#if 1
    mwc_engine<uint64_t, 698769069u> _mwc;
#else
    /* Just to demonstrate that mwc_engine is equivalent to cong_engine with
     * certain parameters. */
    cong_engine<uint64_t, 698769069u, 0, 698769069u * ((uint64_t)1u << 32u) - 1> _mwc;
#endif

public:
    /** The type of the generated random value. */
    typedef uint32_t result_type;

    /** Constructors */
    mwc64(result_type seed_upper, result_type seed_lower)
        : _mwc(((uint64_t)seed_upper << 32u) | seed_lower)
    {}
    mwc64(result_type s)
        : _mwc(((uint64_t)s << 32u) | s)
    {}
    mwc64()
        : _mwc()
    {}

    /** Generation function */
    result_type operator()()
    {
        return (uint32_t)_mwc();
    }

    result_type current() const
    {
        return (uint32_t)_mwc.current();
    }

    static result_type min()
    {
        return 0;
    }

    static result_type max()
    {
        return std::numeric_limits<result_type>::max();
    }

    void discard(uintmax_t n)
    {
        _mwc.discard(n);
    }
};

#endif /* defined(UINT64_C) */


/*****************************************************************************
 *
 ****************************************************************************/

class kiss
{
protected:
    mwc_engine<uint32_t, 36969u>    _mwc_upper;
    mwc_engine<uint32_t, 18000u>    _mwc_lower;
    cong                            _cong;
    shr3                            _shr3;

public:
    /** The type of the generated random value. */
    typedef uint32_t result_type;

    /** Constructors */
    kiss(result_type s1, result_type s2, result_type s3, result_type s4)
        : _mwc_upper(s1), _mwc_lower(s2), _cong(s3), _shr3(s4)
    {}
    kiss(result_type s1, result_type s2, result_type s3)
        : _mwc_upper(s1), _mwc_lower(s2), _cong(s3), _shr3(s3)
    {}
    kiss(result_type s1, result_type s2)
        : _mwc_upper(s1), _mwc_lower(s2), _cong(s2), _shr3(s2)
    {}
    kiss(result_type s)
        : _mwc_upper(s), _mwc_lower(s), _cong(s), _shr3(s)
    {}
    kiss()
        : _mwc_upper(), _mwc_lower(), _cong(), _shr3()
    {}

    /** Generation function */
    result_type operator()()
    {
        _mwc_upper();
        _mwc_lower();
        _cong();
        _shr3();
        return current();
    }

    result_type current() const
    {
        result_type m_upper = _mwc_upper.current();
        result_type m_lower = _mwc_lower.current();
        result_type mwc2 = (m_upper << 16u) + (m_upper >> 16u) + m_lower;
        result_type m_cong = _cong.current();
        result_type m_shr3 = _shr3.current();

        return ((mwc2 ^ m_cong) + m_shr3);
    }

    static result_type min()
    {
        return 0;
    }

    static result_type max()
    {
        return std::numeric_limits<result_type>::max();
    }

    void discard(uintmax_t n)
    {
        _mwc_upper.discard(n);
        _mwc_lower.discard(n);
        _cong.discard(n);
        _shr3.discard(n);
    }
};


#ifdef UINT64_C

/*****************************************************************************
 *
 ****************************************************************************/

class kiss2
{
protected:
    mwc64                           _mwc;
    cong                            _cong;
    shr3                            _shr3;

public:
    /** The type of the generated random value. */
    typedef uint32_t result_type;

    /** Constructors */
    kiss2(result_type s1, result_type s2, result_type s3, result_type s4)
        : _mwc(s1, s2), _cong(s3), _shr3(s4)
    {}
    kiss2(result_type s1, result_type s2, result_type s3)
        : _mwc(s1, s2), _cong(s3), _shr3(s3)
    {}
    kiss2(result_type s1, result_type s2)
        : _mwc(s1, s2), _cong(s2), _shr3(s2)
    {}
    kiss2(result_type s)
        : _mwc(s), _cong(s), _shr3(s)
    {}
    kiss2()
        : _mwc(), _cong(), _shr3()
    {}

    /** Generation function */
    result_type operator()()
    {
        _mwc();
        _cong();
        _shr3();
        return current();
    }

    result_type current() const
    {
        result_type mwc64 = _mwc.current();
        result_type m_cong = _cong.current();
        result_type m_shr3 = _shr3.current();

        return mwc64 + m_cong + m_shr3;
    }

    static result_type min()
    {
        return 0;
    }

    static result_type max()
    {
        return std::numeric_limits<result_type>::max();
    }

    void discard(uintmax_t n)
    {
        _mwc.discard(n);
        _cong.discard(n);
        _shr3.discard(n);
    }
};

#endif /* defined(UINT64_C) */

/*****************************************************************************
 *
 ****************************************************************************/

/*
 * Tables of Maximally-Equidistributed Combined LFSR Generators
 * http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.43.3639
 * Pierre L'Ecuyer
 * Mathematics of Computation, 68, 225 (1999), 261–269.
 *
 * Maximally Equidistributed Combined Tausworthe Generators
 * http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.43.4155
 * P. L'Ecuyer
 * Mathematics of Computation, 65, 213 (1996), 203–213.
 *
 * LFSR113 C double implementation
 * http://www.iro.umontreal.ca/~simardr/rng/lfsr113.c
 * Pierre L'Ecuyer
 */
template<typename UIntType, int k, int q, int s, unsigned _word_bits = 0>
class lfsr_engine
{
protected:
    StaticAssert< (std::numeric_limits<UIntType>::is_signed == 0) > _type_must_be_unsigned;
    StaticAssert< (_word_bits <= std::numeric_limits<UIntType>::digits) > _word_bits_must_fit_in_uinttype;
    StaticAssert< (k >= s) > _k_and_s_params;

public:
    /** The type of the generated random value. */
    typedef UIntType result_type;

    static const int exponent1              = k;
    static const int exponent2              = q;
    static const int step_size              = s;
    static const unsigned _type_bits        = std::numeric_limits<UIntType>::digits;
    static const unsigned word_bits         = (_word_bits == 0) ? _type_bits : _word_bits;
    static const UIntType _word_mask        = (word_bits >= _type_bits) ? (~(UIntType)0) : (((UIntType)1u << word_bits) - 1u);
    static const UIntType min_seed          = (k <= word_bits) ? (UIntType(1) << (word_bits - k)) : 1;
    static const UIntType default_seed      = min_seed;
    static const UIntType gen_mask          = _word_mask ^ (min_seed - 1u);

protected:
    StaticAssert< (k <= word_bits) > _k_and_word_bits_params;

    typedef BitColumnMatrix<UIntType, word_bits> bcm;

public:
    /** Constructors */
    lfsr_engine(UIntType s1 = default_seed)
    { seed(s1); }

    /** Seed functions */
    void seed(UIntType s1 = default_seed)
    {
        UIntType working_seed = s1 ^ (s1 << (word_bits / 2)) & _word_mask;
        if (working_seed < min_seed)
        {
            working_seed = (s1 << (word_bits / 2 + word_bits / 4)) & _word_mask;
            if (working_seed < min_seed)
            {
                working_seed ^= _word_mask;
            }
        }
        x = working_seed;
    }

    /** Generation function */
    result_type operator()()
    {
        UIntType    b;

        b  = (((x << q) ^ x) & _word_mask) >> (k - s);
        x = ((x & gen_mask) << s) ^ b;
        return x;
    }
    result_type current() const
    {
        return x;
    }

    static result_type min()
    {
        return min_seed;
    }

    static result_type max()
    {
        return _word_mask;
    }

    void discard(uintmax_t n)
    {
        bcm shr3_matrix_a = bcm::unity() + bcm::shift(q);
        bcm shr3_matrix = shr3_matrix_a;
        bcm shr3_matrix_b = bcm::shift(s - k);
        shr3_matrix = shr3_matrix_b * shr3_matrix;
        bcm shr3_matrix_c = bcm::mask(word_bits - k, word_bits);
        bcm shr3_matrix_d = bcm::shift(s);
        shr3_matrix = (shr3_matrix_d * shr3_matrix_c) + shr3_matrix;

        x = shr3_matrix.pow(n) * x;
    }

private:
    UIntType        x;
};

class lfsr113
{
protected:
#if 1
    lfsr_engine<uint32_t, 31, 6, 18> z1;
    lfsr_engine<uint32_t, 29, 2, 2> z2;
    lfsr_engine<uint32_t, 28, 13, 7> z3;
    lfsr_engine<uint32_t, 25, 3, 13> z4;
#else
    // Check implementation using uint64_t
    lfsr_engine<uint64_t, 31, 6, 18, 32> z1;
    lfsr_engine<uint64_t, 29, 2, 2, 32> z2;
    lfsr_engine<uint64_t, 28, 13, 7, 32> z3;
    lfsr_engine<uint64_t, 25, 3, 13, 32> z4;
#endif

public:
    /** The type of the generated random value. */
    typedef uint32_t result_type;

    /** Constructors */
    lfsr113(result_type seed1, result_type seed2, result_type seed3, result_type seed4)
        : z1(seed1), z2(seed2), z3(seed3), z4(seed4)
    {}
    lfsr113(result_type seed1, result_type seed2, result_type seed3)
        : z1(seed1), z2(seed2), z3(seed3), z4(seed3)
    {}
    lfsr113(result_type seed1, result_type seed2)
        : z1(seed1), z2(seed2), z3(seed2), z4(seed2)
    {}
    lfsr113(result_type seed1)
        : z1(seed1), z2(seed1), z3(seed1), z4(seed1)
    {}
    lfsr113()
        : z1(), z2(), z3(), z4()
    {}

    /** Generation function */
    result_type operator()()
    {
        z1();
        z2();
        z3();
        z4();
        return current();
    }

    virtual result_type current() const
    {
        result_type r1 = z1.current();
        result_type r2 = z2.current();
        result_type r3 = z3.current();
        result_type r4 = z4.current();

        return r1 ^ r2 ^ r3 ^ r4;
    }

    static result_type min()
    {
        return 0;
    }

    static result_type max()
    {
        return std::numeric_limits<result_type>::max();
    }

    void discard(uintmax_t n)
    {
        z1.discard(n);
        z2.discard(n);
        z3.discard(n);
        z4.discard(n);
    }
};

class lfsr88
{
protected:
    lfsr_engine<uint32_t, 31, 13, 12> z1;
    lfsr_engine<uint32_t, 29, 2, 4> z2;
    lfsr_engine<uint32_t, 28, 3, 17> z3;

public:
    /** The type of the generated random value. */
    typedef uint32_t result_type;

    /** Constructors */
    lfsr88(result_type seed1, result_type seed2, result_type seed3)
        : z1(seed1), z2(seed2), z3(seed3)
    {}
    lfsr88(result_type seed1, result_type seed2)
        : z1(seed1), z2(seed2), z3(seed2)
    {}
    lfsr88(result_type seed1)
        : z1(seed1), z2(seed1), z3(seed1)
    {}
    lfsr88()
        : z1(), z2(), z3()
    {}

    /** Generation function */
    result_type operator()()
    {
        z1();
        z2();
        z3();
        return current();
    }

    virtual result_type current() const
    {
        result_type r1 = z1.current();
        result_type r2 = z2.current();
        result_type r3 = z3.current();

        return r1 ^ r2 ^ r3;
    }

    static result_type min()
    {
        return 0;
    }

    static result_type max()
    {
        return std::numeric_limits<result_type>::max();
    }

    void discard(uintmax_t n)
    {
        z1.discard(n);
        z2.discard(n);
        z3.discard(n);
    }
};


} // namespace simplerandom


#endif /* !defined(__cplusplus) */

#endif /* !defined(_SIMPLERANDOM_CPP_H) */

