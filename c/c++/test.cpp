
#include <iostream>
#include <cstdint>
#include <random>

int main()
{
#if 0
    /* This should work, but doesn't accept m == 0 in GCC g++ 4.7.3. */
    std::linear_congruential_engine<uint32_t, 69069u, 12345u, 0> cong;
#else
    std::linear_congruential_engine<uint64_t, 69069u, 12345u, 0x100000000u> cong;
#endif
    cong.seed(0);

    std::cout << "Min: " << cong.min() << std::endl;
    std::cout << "Max: " << cong.max() << std::endl;

    std::cout << std::endl
        << "Values:" << std::endl;
    for (size_t i = 0; i < 10; i++)
    {
        std::cout << cong() << std::endl;
    }
}

