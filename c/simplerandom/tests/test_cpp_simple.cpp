
#include "simplerandom.h"

#include <stdint.h>

#include <iostream>

int main()
{
#if 0
    simplerandom::mwc2 rng(12345);
#else
    simplerandom::shr3 rng(12345);
#endif

#if 0
    rng.discard(1000000);
#endif

    std::cout << rng() << std::endl;
    std::cout << rng() << std::endl;
    std::cout << rng() << std::endl;

    return 0;
}
