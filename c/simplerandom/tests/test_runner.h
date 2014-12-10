
#include "test-wrapper-c.h"

#include <time.h>

#define CXXTEST_HAVE_EH
#include <cxxtest/TestSuite.h>
#include <iostream>


class SimplerandomCongTest : public CxxTest::TestSuite
{
protected:
    SimpleRandomWrapper * rng;
public:
    virtual SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomSeeder  seed_rng;
        return new SimpleRandomWrapperCong(seed_rng());
    }
    virtual SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperCong(2051391225u);
    }
    virtual SimpleRandomWrapper * factory(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        return new SimpleRandomWrapperCong(p_seeds, num_seeds, mix_extras);
    }
    virtual uint32_t get_million_result() { return 2416584377u; }
    virtual uint32_t get_mix_million_result() { return 2377132217u; }
    void setUp()
    {
        rng = factory((uint32_t)time(NULL));
    }
    void tearDown()
    {
        delete rng;
    }
    void testSeeds()
    {
        SimpleRandomSeeder      seed_rng(0);
        SimpleRandomWrapper *   array_seed_rng;
        SimpleRandomWrapper *   param_seed_rng;
        uint32_t                seeds[1000];

        for (size_t i = 0; i < rng->num_seeds(); i++)
        {
            seeds[i] = seed_rng();
        }
        array_seed_rng = factory(seeds, rng->num_seeds(), false);
        param_seed_rng = factory(0);
        TS_ASSERT_EQUALS((*array_seed_rng)(), (*param_seed_rng)());
        delete array_seed_rng;
        delete param_seed_rng;
    }
    void testMillion()
    {
        SimpleRandomWrapper * million_rng;
        uint32_t result;

        million_rng = factory();
        for (uint32_t i = 0; i < 1000000; i++)
        {
            result = (*million_rng)();
        }
        TS_ASSERT_EQUALS(result, get_million_result());
        delete million_rng;
    }
    void testDiscardMillion()
    {
        SimpleRandomWrapper * discard_rng;
        uint32_t result_discard;

        discard_rng = factory();
        discard_rng->discard(999999);
        result_discard = (*discard_rng)();
        TS_ASSERT_EQUALS(result_discard, get_million_result());
        delete discard_rng;
    }
    void testMixMillion()
    {
        uint32_t zero_seeds[32] = { 0 };
        SimpleRandomWrapper * rng;

        rng = factory(zero_seeds, 32, false);
        for (uint32_t i = 0; i < 1000000u; ++i)
        {
            rng->mix(&i, 1);
        }
        TS_ASSERT_EQUALS((*rng)(), get_mix_million_result());
    }
    void testDiscard()
    {
        SimpleRandomWrapper * discard_rng;
        SimpleRandomWrapper * next_rng;
        uint32_t result_discard;
        uint32_t result_next;

        CxxTest::setAbortTestOnFail(true);
        next_rng = factory();
        for (uint32_t i = 1; i < 10001; i++)
        {
            result_next = (*next_rng)();
            discard_rng = factory();
            discard_rng->discard(i - 1);
            result_discard = (*discard_rng)();
            TS_ASSERT_EQUALS(result_discard, result_next);
            if (result_discard != result_next)
            {
                std::ostringstream out_temp;
                out_temp << "failed at i = " << i;
                TS_TRACE(out_temp.str().c_str());
                break;
            }
            delete discard_rng;
        }
        delete next_rng;
    }
};

class SimplerandomSHR3Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomSeeder  seed_rng;
        return new SimpleRandomWrapperSHR3(seed_rng());
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperSHR3(3360276411u);
    }
    virtual SimpleRandomWrapper * factory(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        return new SimpleRandomWrapperSHR3(p_seeds, num_seeds, mix_extras);
    }
    uint32_t get_million_result() { return 1153302609u; }
    uint32_t get_mix_million_result() { return 2714084401u; }
};

class SimplerandomMWC1Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomSeeder  seed_rng;
        return new SimpleRandomWrapperMWC1(seed_rng(), seed_rng());
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperMWC1(2374144069u, 1046675282u);
    }
    virtual SimpleRandomWrapper * factory(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        return new SimpleRandomWrapperMWC1(p_seeds, num_seeds, mix_extras);
    }
    uint32_t get_million_result() { return 904977562u; }
    uint32_t get_mix_million_result() { return 560871203u; }
};

class SimplerandomMWC2Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomSeeder  seed_rng;
        return new SimpleRandomWrapperMWC2(seed_rng(), seed_rng());
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperMWC2();
    }
    virtual SimpleRandomWrapper * factory(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        return new SimpleRandomWrapperMWC2(p_seeds, num_seeds, mix_extras);
    }
    uint32_t get_million_result() { return 847931684u; }
    uint32_t get_mix_million_result() { return 2704875715u; }
};

class SimplerandomKISSTest : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomSeeder  seed_rng;
        return new SimpleRandomWrapperKISS(seed_rng(), seed_rng(), seed_rng(), seed_rng());
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperKISS(2247183469, 99545079, 3269400377, 3950144837);
    }
    virtual SimpleRandomWrapper * factory(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        return new SimpleRandomWrapperKISS(p_seeds, num_seeds, mix_extras);
    }
    uint32_t get_million_result() { return 2100752872u; }
    uint32_t get_mix_million_result() { return 3522308965u; }
};

#ifdef UINT64_C

class SimplerandomMWC64Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomSeeder  seed_rng;
        return new SimpleRandomWrapperMWC64(seed_rng(), seed_rng());
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperMWC64();
    }
    virtual SimpleRandomWrapper * factory(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        return new SimpleRandomWrapperMWC64(p_seeds, num_seeds, mix_extras);
    }
    uint32_t get_million_result() { return 2191957470u; }
    uint32_t get_mix_million_result() { return 3125681944u; }
};

class SimplerandomKISS2Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomSeeder  seed_rng;
        return new SimpleRandomWrapperKISS2(seed_rng(), seed_rng(), seed_rng(), seed_rng());
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperKISS2();
    }
    virtual SimpleRandomWrapper * factory(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        return new SimpleRandomWrapperKISS2(p_seeds, num_seeds, mix_extras);
    }
    uint32_t get_million_result() { return 4044786495u; }
    uint32_t get_mix_million_result() { return 73179452u; }
};

#endif

class SimplerandomLFSR113Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomSeeder  seed_rng;
        return new SimpleRandomWrapperLFSR113(seed_rng(), seed_rng(), seed_rng(), seed_rng());
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperLFSR113();
    }
    virtual SimpleRandomWrapper * factory(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        return new SimpleRandomWrapperLFSR113(p_seeds, num_seeds, mix_extras);
    }
    uint32_t get_million_result() { return 300959510u; }
    uint32_t get_mix_million_result() { return 1565144389u; }
};

class SimplerandomLFSR88Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomSeeder  seed_rng;
        return new SimpleRandomWrapperLFSR88(seed_rng(), seed_rng(), seed_rng());
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperLFSR88();
    }
    virtual SimpleRandomWrapper * factory(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        return new SimpleRandomWrapperLFSR88(p_seeds, num_seeds, mix_extras);
    }
    uint32_t get_million_result() { return 3774296834u; }
    uint32_t get_mix_million_result() { return 284026550u; }
};
