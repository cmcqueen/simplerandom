/*
 * tools.h
 */

#ifndef _SIMPLERANDOM_TOOLS_H
#define _SIMPLERANDOM_TOOLS_H

#ifndef __cplusplus
#error This header simplerandom-cpp.h is only suitable for C++ not C
#else


#include <limits>


/*****************************************************************************
 *
 ****************************************************************************/

namespace simplerandom
{

/*****************************************************************************
 *
 ****************************************************************************/

template<bool> struct StaticAssert;
template<> struct StaticAssert<true> {};


/*****************************************************************************
 *
 ****************************************************************************/

/* Left shift if shift_value is positive; right-shift if negative.
 * In C and C++, shift by negative value is undefined behaviour.
 */
template<typename UIntType>
inline UIntType signed_left_shift(UIntType a, int shift_value)
{
    StaticAssert< (std::numeric_limits<UIntType>::is_signed == 0) > _type_must_be_unsigned;

    if (shift_value >= 0)
    {
        return a << shift_value;
    }
    else
    {
        return a >> (-shift_value);
    }
}


} // namespace simplerandom


#endif /* !defined(__cplusplus) */

#endif /* !defined(_SIMPLERANDOM_TOOLS_H) */

