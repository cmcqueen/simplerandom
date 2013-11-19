
#include "simplerandom.h"
#include "simplerandom/bitcolumnmatrix.h"

#include <stdint.h>

#include <iostream>
#include <iomanip>

int main()
{
#if 0
    simplerandom::cong rng(12345);
#elif 0
    simplerandom::mwc1 rng(12345);
#elif 0
    simplerandom::mwc2 rng(12345);
#elif 0
    simplerandom::shr3 rng(12345);
#else
    simplerandom::kiss rng(12345);
#endif

#if 1
    rng.discard(1000000);
#endif

#if 1
    std::cout << rng() << std::endl;
    std::cout << rng() << std::endl;
    std::cout << rng() << std::endl;
#endif

    typedef simplerandom::BitColumnMatrix<uint32_t> bcm_t;
    bcm_t shr3_matrix_a = bcm_t::unity() + bcm_t::shift(13);
    bcm_t shr3_matrix_b = bcm_t::unity() + bcm_t::shift(-17);
    bcm_t shr3_matrix_c = bcm_t::unity() + bcm_t::shift(5);
    bcm_t shr3_matrix = shr3_matrix_c * shr3_matrix_b * shr3_matrix_a;

#if 0
    std::cout << shr3_matrix * 4 << std::endl;
#elif 0
    shr3_matrix = shr3_matrix.pow(1);
    for (unsigned i = 0, value = 1u; i < 32u; ++i, value <<= 1u)
        std::cout << std::hex << shr3_matrix * value << std::endl;
    std::cout << std::dec << shr3_matrix * 12345u << std::endl;
#endif

    return 0;
}
