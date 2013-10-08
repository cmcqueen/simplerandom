/*
 * simplerandom-jumpahead.c
 *
 * Simple Pseudo-random Number Generators -- jumpahead functions
 */


/*****************************************************************************
 * Includes
 ****************************************************************************/

#include "simplerandom.h"

#include "bitcolumnmatrix.h"
#include "pow.h"


/*****************************************************************************
 * Look-up tables
 ****************************************************************************/

static const BitColumnMatrix32_t shr3_matrix =
{
    {
        0x00042021, 0x00084042, 0x00108084, 0x00210108, 0x00420231, 0x00840462, 0x010808C4, 0x02101188,
        0x04202310, 0x08404620, 0x10808C40, 0x21011880, 0x42023100, 0x84046200, 0x0808C400, 0x10118800,
        0x20231000, 0x40462021, 0x808C4042, 0x01080084, 0x02100108, 0x04200210, 0x08400420, 0x10800840,
        0x21001080, 0x42002100, 0x84004200, 0x08008400, 0x10010800, 0x20021000, 0x40042000, 0x80084000,
    },
};

static const BitColumnMatrix32_t lfsr113_1_matrix =
{
    {
        0x00000000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000, 0x01000000, 0x02000001,
        0x04000002, 0x08000004, 0x10000008, 0x20000010, 0x40000020, 0x80000041, 0x00000082, 0x00000104,
        0x00000208, 0x00000410, 0x00000820, 0x00001040, 0x00002080, 0x00004100, 0x00008200, 0x00010400,
        0x00020800, 0x00041000, 0x00002000, 0x00004000, 0x00008000, 0x00010000, 0x00020000, 0x00040000
    },
};

static const BitColumnMatrix32_t lfsr113_2_matrix =
{
    {
        0x00000000, 0x00000000, 0x00000000, 0x00000020, 0x00000040, 0x00000080, 0x00000100, 0x00000200,
        0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000, 0x00010000, 0x00020000,
        0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000, 0x01000000, 0x02000000,
        0x04000000, 0x08000001, 0x10000002, 0x20000005, 0x4000000A, 0x80000014, 0x00000008, 0x00000010
    },
};

static const BitColumnMatrix32_t lfsr113_3_matrix =
{
    {
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000800, 0x00001000, 0x00002000, 0x00004000,
        0x00008001, 0x00010002, 0x00020004, 0x00040008, 0x00080010, 0x00100020, 0x00200040, 0x00400080,
        0x00800100, 0x01000200, 0x02000400, 0x04000000, 0x08000000, 0x10000001, 0x20000002, 0x40000004,
        0x80000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080, 0x00000100, 0x00000200, 0x00000400
    },
};

static const BitColumnMatrix32_t lfsr113_4_matrix =
{
    {
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00100000,
        0x00200000, 0x00400001, 0x00800002, 0x01000004, 0x02000009, 0x04000012, 0x08000024, 0x10000048,
        0x20000090, 0x40000120, 0x80000240, 0x00000480, 0x00000900, 0x00001200, 0x00002400, 0x00004800,
        0x00009000, 0x00012000, 0x00024000, 0x00048000, 0x00090000, 0x00020000, 0x00040000, 0x00080000
    },
};

static const BitColumnMatrix32_t lfsr88_1_matrix =
{
    {
        0x00000000, 0x00002000, 0x00004000, 0x00008000, 0x00010000, 0x00020000, 0x00040001, 0x00080002,
        0x00100004, 0x00200008, 0x00400010, 0x00800020, 0x01000040, 0x02000080, 0x04000100, 0x08000200,
        0x10000400, 0x20000800, 0x40001000, 0x80000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010,
        0x00000020, 0x00000040, 0x00000080, 0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000
    },
};

static const BitColumnMatrix32_t lfsr88_2_matrix =
{
    {
        0x00000000, 0x00000000, 0x00000000, 0x00000080, 0x00000100, 0x00000200, 0x00000400, 0x00000800,
        0x00001000, 0x00002000, 0x00004000, 0x00008000, 0x00010000, 0x00020000, 0x00040000, 0x00080000,
        0x00100000, 0x00200000, 0x00400000, 0x00800000, 0x01000000, 0x02000000, 0x04000000, 0x08000001,
        0x10000002, 0x20000005, 0x4000000A, 0x80000014, 0x00000028, 0x00000050, 0x00000020, 0x00000040
    },
};

static const BitColumnMatrix32_t lfsr88_3_matrix =
{
    {
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200000, 0x00400000, 0x00800000, 0x01000000,
        0x02000001, 0x04000002, 0x08000004, 0x10000009, 0x20000012, 0x40000024, 0x80000048, 0x00000090,
        0x00000120, 0x00000240, 0x00000480, 0x00000900, 0x00001200, 0x00002400, 0x00004800, 0x00009000,
        0x00012000, 0x00024000, 0x00048000, 0x00090000, 0x00120000, 0x00040000, 0x00080000, 0x00100000
    },
};


/*****************************************************************************
 * Functions
 ****************************************************************************/

/*********
 * SHR3
 ********/

void simplerandom_shr3_jumpahead(SimpleRandomSHR3_t * p_shr3, uintmax_t n)
{
    BitColumnMatrix32_t shr3_mult;
    uint32_t            shr3;

    bitcolumnmatrix32_pow(&shr3_mult, &shr3_matrix, n);
    shr3 = bitcolumnmatrix32_mul_uint32(&shr3_mult, p_shr3->shr3);
    p_shr3->shr3 = shr3;
}


/*********
 * MWC2
 *
 * MWC1 and MWC2 are very similar, apart from deriving the final random value
 * from the state. So they can share code. MWC2 is preferred, so we put MWC2
 * first, and then MWC1 can call some MWC2 functions.
 ********/

#define _MWC_UPPER_MULT         36969u
#define _MWC_LOWER_MULT         18000u
#define _MWC_UPPER_MODULO       (_MWC_UPPER_MULT * (UINT32_C(1) << 16u) - 1u)
#define _MWC_LOWER_MODULO       (_MWC_LOWER_MULT * (UINT32_C(1) << 16u) - 1u)
#define _MWC_UPPER_CYCLE_LEN    (_MWC_UPPER_MULT * (UINT32_C(1) << 16u) / 2u - 1u)
#define _MWC_LOWER_CYCLE_LEN    (_MWC_LOWER_MULT * (UINT32_C(1) << 16u) / 2u - 1u)

/*
 * This is almost identical to simplerandom_mwc1_jumpahead(), except that when
 * combining the upper and lower values in the last step, the upper 16 bits of
 * mwc_upper are added in too, instead of just being discarded.
 */
void simplerandom_mwc2_jumpahead(SimpleRandomMWC2_t * p_mwc, uintmax_t n)
{
    uint32_t    mwc;

    mwc = p_mwc->mwc_upper;
    mwc = mul_mod_uint32(pow_mod_uint32(_MWC_UPPER_MULT, n, _MWC_UPPER_MODULO), mwc, _MWC_UPPER_MODULO);
    p_mwc->mwc_upper = mwc;

    mwc = p_mwc->mwc_lower;
    mwc = mul_mod_uint32(pow_mod_uint32(_MWC_LOWER_MULT, n, _MWC_LOWER_MODULO), mwc, _MWC_LOWER_MODULO);
    p_mwc->mwc_lower = mwc;
}


/*********
 * MWC1
 *
 * MWC1 is very similar to MWC2, so many functions can be shared.
 ********/

void simplerandom_mwc1_jumpahead(SimpleRandomMWC1_t * p_mwc, uintmax_t n)
{
    simplerandom_mwc2_jumpahead(p_mwc, n);
}


/*********
 * Cong
 ********/

#define CONG_MULT                       69069u
#define CONG_CONST                      12345u

#ifdef UINT64_C

#define CONG_JUMPAHEAD_C_FACTOR         4u      /* (CONG_MULT - 1) == 4 * 17267 */
#define CONG_JUMPAHEAD_C_DENOM          17267u
#define CONG_JUMPAHEAD_C_MOD            ((UINT64_C(1) << 32u) * CONG_JUMPAHEAD_C_FACTOR)
/* Inverse of CONG_JUMPAHEAD_C_DENOM mod 2^32.
 * It can be calculated using extended Euclidean algorithm.
 * Or it can be calculated as (CONG_JUMPAHEAD_C_DENOM ^ (2^32 - 1)) mod 2^32.
 * In Python: pow(17267, 2**32 - 1, 2**32) */
#define CONG_JUMPAHEAD_C_DENOM_INVERSE  UINT32_C(1355127227)

/* Cong jumpahead(n) = r^n * x mod 2^32 + c * (r^n - 1) / (r - 1) mod 2^32
 * where r = 69069 and c = 12345.
 *
 * The part c * (r^n - 1) / (r - 1) is the formula for a geometric series
 *     c + c*r + c*r^2 + c*r^3 + ... + c*r^(n-1)
 * For calculating geometric series mod 2^32, see:
 * http://www.codechef.com/wiki/tutorial-just-simple-sum#Back_to_the_geometric_series
 *
 * The modulus 2^32 and (r - 1) have a common factor of 4 (see
 * CONG_JUMPAHEAD_C_FACTOR). So we calculate the numerator modulo 2^32 * 4,
 * i.e. 2^34. Unfortunately this requires more than 32-bit calculations--either
 * calculations with 64-bit integers, or multiple-precision arithmetic.
 * This function implements 64-bit integer calculations.
 *
 * After calculating the numerator, we divide by the common factor of 4,
 * then multiply by the inverse of the other part of the denominator.
 */
void simplerandom_cong_jumpahead(SimpleRandomCong_t * p_cong, uintmax_t n)
{
    uint32_t    mult_exp;
    uint64_t    add_const_exp;
    uint32_t    add_const_part;
    uint32_t    add_const;
    uint32_t    cong;

    mult_exp = pow_uint32(CONG_MULT, n);

    add_const_exp = pow_uint64(CONG_MULT, n) % CONG_JUMPAHEAD_C_MOD;
    add_const_part = (add_const_exp - 1) / CONG_JUMPAHEAD_C_FACTOR * CONG_JUMPAHEAD_C_DENOM_INVERSE;

    add_const = add_const_part * CONG_CONST;
    cong = mult_exp * p_cong->cong + add_const;
    p_cong->cong = cong;
}

#else /* !defined(UINT64_C) */

void simplerandom_cong_jumpahead(SimpleRandomCong_t * p_cong, uintmax_t n)
{
    uint32_t    mult_exp;
    uint32_t    add_const_part;
    uint32_t    add_const;
    uint32_t    cong;

    mult_exp = pow_uint32(CONG_MULT, n);

    add_const_part = geom_series_uint32(CONG_MULT, n);

    add_const = add_const_part * CONG_CONST;
    cong = mult_exp * p_cong->cong + add_const;
    p_cong->cong = cong;
}

#endif /* defined(UINT64_C) */

/*********
 * KISS
 ********/

void simplerandom_kiss_jumpahead(SimpleRandomKISS_t * p_kiss, uintmax_t n)
{
    SimpleRandomMWC2_t  rng_mwc;
    SimpleRandomCong_t  rng_cong;
    SimpleRandomSHR3_t  rng_shr3;

    rng_mwc.mwc_upper   = p_kiss->mwc_upper;
    rng_mwc.mwc_lower   = p_kiss->mwc_lower;
    rng_cong.cong       = p_kiss->cong;
    rng_shr3.shr3       = p_kiss->shr3;

    simplerandom_mwc2_jumpahead(&rng_mwc, n);
    simplerandom_cong_jumpahead(&rng_cong, n);
    simplerandom_shr3_jumpahead(&rng_shr3, n);

    p_kiss->mwc_upper   = rng_mwc.mwc_upper;
    p_kiss->mwc_lower   = rng_mwc.mwc_lower;
    p_kiss->cong        = rng_cong.cong;
    p_kiss->shr3        = rng_shr3.shr3;
}


#ifdef UINT64_C

/*********
 * MWC64
 ********/

#define _MWC64_MULT         UINT64_C(698769069)
#define _MWC64_MODULO       (_MWC64_MULT * (UINT64_C(1) << 32u) - 1u)
#define _MWC64_CYCLE_LEN    (_MWC64_MULT * (UINT64_C(1) << 32u) / 2u - 1u)

void simplerandom_mwc64_jumpahead(SimpleRandomMWC64_t * p_mwc, uintmax_t n)
{
    uint64_t    mwc;

    mwc = ((uint64_t)p_mwc->mwc_upper << 32u) + p_mwc->mwc_lower;
    mwc = mul_mod_uint64(pow_mod_uint64(_MWC64_MULT, n, _MWC64_MODULO), mwc, _MWC64_MODULO);
    p_mwc->mwc_upper = (uint32_t)(mwc >> 32u);
    p_mwc->mwc_lower = (uint32_t)mwc;
}


/*********
 * KISS2
 ********/

void simplerandom_kiss2_jumpahead(SimpleRandomKISS2_t * p_kiss2, uintmax_t n)
{
    SimpleRandomMWC64_t rng_mwc;
    SimpleRandomCong_t  rng_cong;
    SimpleRandomSHR3_t  rng_shr3;

    rng_mwc.mwc_upper   = p_kiss2->mwc_upper;
    rng_mwc.mwc_lower   = p_kiss2->mwc_lower;
    rng_cong.cong       = p_kiss2->cong;
    rng_shr3.shr3       = p_kiss2->shr3;

    simplerandom_mwc64_jumpahead(&rng_mwc, n);
    simplerandom_cong_jumpahead(&rng_cong, n);
    simplerandom_shr3_jumpahead(&rng_shr3, n);

    p_kiss2->mwc_upper  = rng_mwc.mwc_upper;
    p_kiss2->mwc_lower  = rng_mwc.mwc_lower;
    p_kiss2->cong       = rng_cong.cong;
    p_kiss2->shr3       = rng_shr3.shr3;
}

#endif /* defined(UINT64_C) */


/*********
 * LFSR113
 ********/

void simplerandom_lfsr113_jumpahead(SimpleRandomLFSR113_t * p_lfsr113, uintmax_t n)
{
    BitColumnMatrix32_t lfsr_mult;
    uint32_t            z1;
    uint32_t            z2;
    uint32_t            z3;
    uint32_t            z4;

    bitcolumnmatrix32_pow(&lfsr_mult, &lfsr113_1_matrix, n);
    z1 = bitcolumnmatrix32_mul_uint32(&lfsr_mult, p_lfsr113->z1);
    p_lfsr113->z1 = z1;

    bitcolumnmatrix32_pow(&lfsr_mult, &lfsr113_2_matrix, n);
    z2 = bitcolumnmatrix32_mul_uint32(&lfsr_mult, p_lfsr113->z2);
    p_lfsr113->z2 = z2;

    bitcolumnmatrix32_pow(&lfsr_mult, &lfsr113_3_matrix, n);
    z3 = bitcolumnmatrix32_mul_uint32(&lfsr_mult, p_lfsr113->z3);
    p_lfsr113->z3 = z3;

    bitcolumnmatrix32_pow(&lfsr_mult, &lfsr113_4_matrix, n);
    z4 = bitcolumnmatrix32_mul_uint32(&lfsr_mult, p_lfsr113->z4);
    p_lfsr113->z4 = z4;
}


/*********
 * LFSR88
 ********/

void simplerandom_lfsr88_jumpahead(SimpleRandomLFSR88_t * p_lfsr88, uintmax_t n)
{
    BitColumnMatrix32_t lfsr_mult;
    uint32_t            z1;
    uint32_t            z2;
    uint32_t            z3;

    bitcolumnmatrix32_pow(&lfsr_mult, &lfsr88_1_matrix, n);
    z1 = bitcolumnmatrix32_mul_uint32(&lfsr_mult, p_lfsr88->z1);
    p_lfsr88->z1 = z1;

    bitcolumnmatrix32_pow(&lfsr_mult, &lfsr88_2_matrix, n);
    z2 = bitcolumnmatrix32_mul_uint32(&lfsr_mult, p_lfsr88->z2);
    p_lfsr88->z2 = z2;

    bitcolumnmatrix32_pow(&lfsr_mult, &lfsr88_3_matrix, n);
    z3 = bitcolumnmatrix32_mul_uint32(&lfsr_mult, p_lfsr88->z3);
    p_lfsr88->z3 = z3;
}

