
#define __STDC_CONSTANT_MACROS
#include "simplerandom.h"

#include <time.h>

#define CXXTEST_HAVE_EH
#include <cxxtest/TestSuite.h>
#include <iostream>

class SimpleRandomWrapper
{
public:
    virtual size_t num_seeds() = 0;
    virtual uint32_t get_million_result() = 0;
    virtual uint32_t next() = 0;
    virtual uint32_t jumpahead(uintmax_t n) = 0;
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
    uint32_t jumpahead(uintmax_t n) { return simplerandom_cong_jumpahead(&rng, n); }
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
    uint32_t jumpahead(uintmax_t n) { return simplerandom_shr3_jumpahead(&rng, n); }
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
    uint32_t jumpahead(uintmax_t n) { return simplerandom_mwc1_jumpahead(&rng, n); }
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
    uint32_t jumpahead(uintmax_t n) { return simplerandom_mwc2_jumpahead(&rng, n); }
};

class SimpleRandomWrapperKISS : public SimpleRandomWrapper
{
private:
    SimpleRandomKISS_t  rng;
public:
    SimpleRandomWrapperKISS() { simplerandom_kiss_seed(&rng, 2247183469, 99545079, 3269400377, 3950144837); }
    SimpleRandomWrapperKISS(uint32_t seed1, uint32_t seed2, uint32_t seed3, uint32_t seed4)
    {
        simplerandom_kiss_seed(&rng, seed1, seed2, seed3, seed4);
    }
    size_t num_seeds() { return 4; }
    uint32_t get_million_result() { return 2100752872u; }

    uint32_t next() { return simplerandom_kiss_next(&rng); }
    uint32_t jumpahead(uintmax_t n) { return simplerandom_kiss_jumpahead(&rng, n); }
};

#ifdef UINT64_C

class SimpleRandomWrapperMWC64 : public SimpleRandomWrapper
{
private:
    SimpleRandomMWC64_t  rng;
public:
    SimpleRandomWrapperMWC64() { simplerandom_mwc64_seed(&rng, 0, 0); }
    SimpleRandomWrapperMWC64(uint32_t seed1, uint32_t seed2) { simplerandom_mwc64_seed(&rng, seed1, seed2); }
    size_t num_seeds() { return 2; }
    uint32_t get_million_result() { return 2191957470u; }

    uint32_t next() { return simplerandom_mwc64_next(&rng); }
    uint32_t jumpahead(uintmax_t n) { return simplerandom_mwc64_jumpahead(&rng, n); }
};

class SimpleRandomWrapperKISS2 : public SimpleRandomWrapper
{
private:
    SimpleRandomKISS2_t rng;
public:
    SimpleRandomWrapperKISS2() { simplerandom_kiss2_seed(&rng, 0, 0, 0, 0); }
    SimpleRandomWrapperKISS2(uint32_t seed1, uint32_t seed2, uint32_t seed3, uint32_t seed4)
    {
        simplerandom_kiss2_seed(&rng, seed1, seed2, seed3, seed4);
    }
    size_t num_seeds() { return 4; }
    uint32_t get_million_result() { return 4044786495u; }

    uint32_t next() { return simplerandom_kiss2_next(&rng); }
    uint32_t jumpahead(uintmax_t n) { return simplerandom_kiss2_jumpahead(&rng, n); }
};

#endif

class SimpleRandomWrapperLFSR113 : public SimpleRandomWrapper
{
private:
    SimpleRandomLFSR113_t  rng;
public:
    SimpleRandomWrapperLFSR113() { simplerandom_lfsr113_seed(&rng, 0, 0, 0, 0); }
    SimpleRandomWrapperLFSR113(uint32_t seed1, uint32_t seed2, uint32_t seed3, uint32_t seed4)
    {
        simplerandom_lfsr113_seed(&rng, seed1, seed2, seed3, seed4);
    }
    size_t num_seeds() { return 4; }
    uint32_t get_million_result() { return 300959510; }

    uint32_t next() { return simplerandom_lfsr113_next(&rng); }
    uint32_t jumpahead(uintmax_t n) { return simplerandom_lfsr113_jumpahead(&rng, n); }
};

class SimpleRandomWrapperLFSR88 : public SimpleRandomWrapper
{
private:
    SimpleRandomLFSR88_t  rng;
public:
    SimpleRandomWrapperLFSR88() { simplerandom_lfsr88_seed(&rng, 0, 0, 0); }
    SimpleRandomWrapperLFSR88(uint32_t seed1, uint32_t seed2, uint32_t seed3)
    {
        simplerandom_lfsr88_seed(&rng, seed1, seed2, seed3);
    }
    size_t num_seeds() { return 3; }
    uint32_t get_million_result() { return 3774296834; }

    uint32_t next() { return simplerandom_lfsr88_next(&rng); }
    uint32_t jumpahead(uintmax_t n) { return simplerandom_lfsr88_jumpahead(&rng, n); }
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
    void tearDown()
    {
        delete rng;
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
    void testJumpahead()
    {
        SimpleRandomWrapper * jumpahead_rng;
        SimpleRandomWrapper * thousand_rng;
        uint32_t result_jumpahead;
        uint32_t result_thousand;

        CxxTest::setAbortTestOnFail(true);
        thousand_rng = factory();
        for (uint32_t i = 1; i < 10001; i++)
        {
            result_thousand = thousand_rng->next();
            jumpahead_rng = factory();
            result_jumpahead = jumpahead_rng->jumpahead(i);
            TS_ASSERT_EQUALS(result_jumpahead, result_thousand);
            if (result_jumpahead != result_thousand)
            {
                std::ostringstream out_temp;
                out_temp << "failed at i = " << i;
                TS_TRACE(out_temp.str().c_str());
                break;
            }
            delete jumpahead_rng;
        }
        delete thousand_rng;
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

class SimplerandomKISSTest : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomCong_t  seed_rng;

        simplerandom_cong_seed(&seed_rng, single_seed);

        return new SimpleRandomWrapperKISS(simplerandom_cong_next(&seed_rng),
                                            simplerandom_cong_next(&seed_rng),
                                            simplerandom_cong_next(&seed_rng),
                                            simplerandom_cong_next(&seed_rng));
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperKISS();
    }
};

#ifdef UINT64_C

class SimplerandomMWC64Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomCong_t  seed_rng;

        simplerandom_cong_seed(&seed_rng, single_seed);

        return new SimpleRandomWrapperMWC64(simplerandom_cong_next(&seed_rng),
                                            simplerandom_cong_next(&seed_rng));
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperMWC64();
    }
};

class SimplerandomKISS2Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomCong_t  seed_rng;

        simplerandom_cong_seed(&seed_rng, single_seed);

        return new SimpleRandomWrapperKISS2(simplerandom_cong_next(&seed_rng),
                                            simplerandom_cong_next(&seed_rng),
                                            simplerandom_cong_next(&seed_rng),
                                            simplerandom_cong_next(&seed_rng));
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperKISS2();
    }
};

#endif

class SimplerandomLFSR113Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomCong_t  seed_rng;

        simplerandom_cong_seed(&seed_rng, single_seed);

        return new SimpleRandomWrapperLFSR113(simplerandom_cong_next(&seed_rng),
                                                simplerandom_cong_next(&seed_rng),
                                                simplerandom_cong_next(&seed_rng),
                                                simplerandom_cong_next(&seed_rng));
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperLFSR113();
    }
};

class SimplerandomLFSR88Test : public SimplerandomCongTest
{
public:
    SimpleRandomWrapper * factory(uint32_t single_seed)
    {
        SimpleRandomCong_t  seed_rng;

        simplerandom_cong_seed(&seed_rng, single_seed);

        return new SimpleRandomWrapperLFSR88(simplerandom_cong_next(&seed_rng),
                                                simplerandom_cong_next(&seed_rng),
                                                simplerandom_cong_next(&seed_rng));
    }
    SimpleRandomWrapper * factory()
    {
        return new SimpleRandomWrapperLFSR88();
    }
};
