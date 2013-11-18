
#include "simplerandom.h"

#include <stdint.h>

#include <iostream>

int main()
{
    simplerandom::mwc1 rng(12345);

    std::cout << rng() << std::endl;
    std::cout << rng() << std::endl;
    std::cout << rng() << std::endl;
    return 0;
}
