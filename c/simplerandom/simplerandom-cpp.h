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


/*****************************************************************************
 *
 ****************************************************************************/

namespace simplerandom
{

}


#endif /* !defined(_SIMPLERANDOM_CPP_H) */

