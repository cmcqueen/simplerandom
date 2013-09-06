
#include "simplerandom.h"

#include <time.h>

#include <cxxtest/TestSuite.h>
#include <iostream>

class SimpleRandomWrapper
{
public:
    virtual size_t num_seeds() = 0;
    virtual uint32_t get_million_result() = 0;
    virtual uint32_t next() = 0;
};

class SimpleRandomWrapperCong : public SimpleRandomWrapper
{
private:
    SimpleRandomCong_t  rng;
public:
    SimpleRandomWrapperCong() { simplerandom_cong_seed(&rng, 2051391225u); }
    SimpleRandomWrapperCong(uint32_t seed) { simplerandom_cong_seed(&rng, seed); }
    size_t num_seeds() { return 1; }
    uint32_t get_million_result() { return 2416584377u; }

    uint32_t next() { return simplerandom_cong_next(&rng); }
};

class SimpleRandomWrapperSHR3 : public SimpleRandomWrapper
{
private:
    SimpleRandomSHR3_t  rng;
public:
    SimpleRandomWrapperSHR3() { simplerandom_shr3_seed(&rng, 3360276411u); }
    SimpleRandomWrapperSHR3(uint32_t seed) { simplerandom_shr3_seed(&rng, seed); }
    size_t num_seeds() { return 1; }
    uint32_t get_million_result() { return 1153302609u; }

    uint32_t next() { return simplerandom_shr3_next(&rng); }
};

class SimpleRandomWrapperMWC1 : public SimpleRandomWrapper
{
private:
    SimpleRandomMWC1_t  rng;
public:
    SimpleRandomWrapperMWC1() { simplerandom_mwc1_seed(&rng, 2374144069u, 1046675282u); }
    SimpleRandomWrapperMWC1(uint32_t seed1, uint32_t seed2) { simplerandom_mwc1_seed(&rng, seed1, seed2); }
    size_t num_seeds() { return 2; }
    uint32_t get_million_result() { return 904977562u; }

    uint32_t next() { return simplerandom_mwc1_next(&rng); }
};

class SimpleRandomWrapperMWC2 : public SimpleRandomWrapper
{
private:
    SimpleRandomMWC2_t  rng;
public:
    SimpleRandomWrapperMWC2() { simplerandom_mwc2_seed(&rng, 0, 0); }
    SimpleRandomWrapperMWC2(uint32_t seed1, uint32_t seed2) { simplerandom_mwc2_seed(&rng, seed1, seed2); }
    size_t num_seeds() { return 2; }
    uint32_t get_million_result() { return 767834450u; }

    uint32_t next() { return simplerandom_mwc2_next(&rng); }
};


class SimplerandomCongTest : public CxxTest::TestSuite
{
protected:
    SimpleRandomWrapper * rng;
public:
    virtual SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomCong_t  seed_rng;

        simplerandom_cong_seed(&seed_rng, single_seed);

        return new SimpleRandomWrapperCong(simplerandom_cong_next(&seed_rng));
    }
    virtual SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperCong();
    }
    void setUp()
    {
        rng = factory((uint32_t)time(NULL));
    }
    void testMillion()
    {
        SimpleRandomWrapper * million_rng;
        uint32_t result;

        million_rng = factory();
        for (uint32_t i = 0; i < 1000000; i++)
        {
            result = million_rng->next();
        }
        TS_ASSERT_EQUALS(result, million_rng->get_million_result());
        delete million_rng;
    }
};

class SimplerandomSHR3Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomCong_t  seed_rng;

        simplerandom_cong_seed(&seed_rng, single_seed);

        return new SimpleRandomWrapperSHR3(simplerandom_cong_next(&seed_rng));
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperSHR3();
    }
};

class SimplerandomMWC1Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomCong_t  seed_rng;

        simplerandom_cong_seed(&seed_rng, single_seed);

        return new SimpleRandomWrapperMWC1(simplerandom_cong_next(&seed_rng),
                                            simplerandom_cong_next(&seed_rng));
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperMWC1();
    }
};

class SimplerandomMWC2Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomCong_t  seed_rng;

        simplerandom_cong_seed(&seed_rng, single_seed);

        return new SimpleRandomWrapperMWC2(simplerandom_cong_next(&seed_rng),
                                            simplerandom_cong_next(&seed_rng));
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperMWC2();
    }
};
