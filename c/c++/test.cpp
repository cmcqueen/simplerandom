/* Test C++11 random API.
 * Compare C++11 equivalent of simplerandom.iterators.Cong.
 * Test C++11 uniform_real_distribution implementation with simplerandom.random.Cong.
 */


#include <iostream>
#include <iomanip>
#include <cstdint>
#include <random>

namespace simplerandom {

class SHR3
{
private:
    uint32_t shr3;
public:
    typedef uint32_t result_type;

    SHR3(uint32_t seed1 = 0xFFFFFFFF)
    {
        seed(seed1);
    }
    void seed(uint32_t seed1)
    {
        if (seed1 == 0)
            seed1 = 0xFFFFFFFF;
        shr3 = seed1;
    }
    uint32_t operator()()
    {
        shr3 ^= (shr3 << 13);
        shr3 ^= (shr3 >> 17);
        shr3 ^= (shr3 << 5);
        return shr3;
    }
    uint32_t min() const
    {
        return 1;
    }
    uint32_t max() const
    {
        return 0xFFFFFFFFu;
    }
};

};

int main()
{
    std::string rng_name;
#if 0
    /* This should work, but doesn't accept m == 0 in GCC g++ 4.7.3. */
    std::linear_congruential_engine<uint32_t, 69069u, 12345u, 0> rng;
    rng_name = "Cong";
#elif 1
    std::linear_congruential_engine<uint64_t, 69069u, 12345u, 0x100000000u> rng;
    rng_name = "Cong";
#else
    simplerandom::SHR3 rng;
    rng_name = "SHR3";
#endif
    rng.seed(0);

    std::cout << rng_name << " min: " << rng.min() << std::endl;
    std::cout << rng_name << " max: " << rng.max() << std::endl;

    std::cout << std::endl
        << rng_name << "(0) values:" << std::endl;
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << rng() << std::endl;
    }
    std::cout << std::endl
        << rng_name << "(0) real values:" << std::endl;
    std::uniform_real_distribution<> real_dist;
    rng.seed(0);
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << std::setprecision(17) << real_dist(rng) << std::endl;
    }
}

