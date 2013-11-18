
#include "simplerandom.h"

#include <stdint.h>

#include <iostream>

int main()
{
    simplerandom::mwc2 rng(12345);

    rng.discard(1000000);
    std::cout << rng() << std::endl;
    std::cout << rng() << std::endl;
    std::cout << rng() << std::endl;
    return 0;
}
