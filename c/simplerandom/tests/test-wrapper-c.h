
#include "simplerandom-c.h"

#include <limits>


class SimpleRandomWrapper
{
public:
    // Non-standard API
    virtual size_t num_seeds() = 0;
    virtual void discard(uintmax_t n) = 0;
    virtual void mix(uint32_t * p_mix_array, size_t n) = 0;

    // Standard C++ random API
    typedef uint32_t result_type;
    virtual uint32_t operator()() = 0;
    virtual uint32_t min() const
    {
        return 0;
    }
    virtual uint32_t max() const
    {
        return std::numeric_limits<uint32_t>::max();
    }
};

class SimpleRandomWrapperCong : public SimpleRandomWrapper
{
private:
    SimpleRandomCong_t  rng;
public:
    SimpleRandomWrapperCong(uint32_t seed=0) { simplerandom_cong_seed(&rng, seed); }
    SimpleRandomWrapperCong(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        simplerandom_cong_seed_array(&rng, p_seeds, num_seeds, mix_extras);
    }
    size_t num_seeds() { return 1; }

    uint32_t operator()() { return simplerandom_cong_next(&rng); }
    void discard(uintmax_t n) { simplerandom_cong_discard(&rng, n); }
    void mix(uint32_t * p_mix_array, size_t n) { simplerandom_cong_mix(&rng, p_mix_array, n); }
};

typedef SimpleRandomWrapperCong SimpleRandomSeeder;

class SimpleRandomWrapperSHR3 : public SimpleRandomWrapper
{
private:
    SimpleRandomSHR3_t  rng;
public:
    SimpleRandomWrapperSHR3(uint32_t seed=0) { simplerandom_shr3_seed(&rng, seed); }
    SimpleRandomWrapperSHR3(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        simplerandom_shr3_seed_array(&rng, p_seeds, num_seeds, mix_extras);
    }
    size_t num_seeds() { return 1; }

    uint32_t operator()() { return simplerandom_shr3_next(&rng); }
    void discard(uintmax_t n) { simplerandom_shr3_discard(&rng, n); }
    void mix(uint32_t * p_mix_array, size_t n) { simplerandom_shr3_mix(&rng, p_mix_array, n); }
    uint32_t min() const
    {
        // SHR3 is exceptional in that it doesn't ever return 0.
        return 1;
    }
};

class SimpleRandomWrapperMWC1 : public SimpleRandomWrapper
{
private:
    SimpleRandomMWC1_t  rng;
public:
    SimpleRandomWrapperMWC1(uint32_t seed1=0, uint32_t seed2=0) { simplerandom_mwc1_seed(&rng, seed1, seed2); }
    SimpleRandomWrapperMWC1(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        simplerandom_mwc1_seed_array(&rng, p_seeds, num_seeds, mix_extras);
    }
    size_t num_seeds() { return 2; }

    uint32_t operator()() { return simplerandom_mwc1_next(&rng); }
    void discard(uintmax_t n) { simplerandom_mwc1_discard(&rng, n); }
    void mix(uint32_t * p_mix_array, size_t n) { simplerandom_mwc1_mix(&rng, p_mix_array, n); }
};

class SimpleRandomWrapperMWC2 : public SimpleRandomWrapper
{
private:
    SimpleRandomMWC2_t  rng;
public:
    SimpleRandomWrapperMWC2(uint32_t seed1=0, uint32_t seed2=0) { simplerandom_mwc2_seed(&rng, seed1, seed2); }
    SimpleRandomWrapperMWC2(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        simplerandom_mwc2_seed_array(&rng, p_seeds, num_seeds, mix_extras);
    }
    size_t num_seeds() { return 2; }

    uint32_t operator()() { return simplerandom_mwc2_next(&rng); }
    void discard(uintmax_t n) { simplerandom_mwc2_discard(&rng, n); }
    void mix(uint32_t * p_mix_array, size_t n) { simplerandom_mwc2_mix(&rng, p_mix_array, n); }
};

class SimpleRandomWrapperKISS : public SimpleRandomWrapper
{
private:
    SimpleRandomKISS_t  rng;
public:
    SimpleRandomWrapperKISS(uint32_t seed1, uint32_t seed2, uint32_t seed3, uint32_t seed4)
    {
        simplerandom_kiss_seed(&rng, seed1, seed2, seed3, seed4);
    }
    SimpleRandomWrapperKISS(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        simplerandom_kiss_seed_array(&rng, p_seeds, num_seeds, mix_extras);
    }
    size_t num_seeds() { return 4; }

    uint32_t operator()() { return simplerandom_kiss_next(&rng); }
    void discard(uintmax_t n) { simplerandom_kiss_discard(&rng, n); }
    void mix(uint32_t * p_mix_array, size_t n) { simplerandom_kiss_mix(&rng, p_mix_array, n); }
};

#ifdef UINT64_C

class SimpleRandomWrapperMWC64 : public SimpleRandomWrapper
{
private:
    SimpleRandomMWC64_t  rng;
public:
    SimpleRandomWrapperMWC64(uint32_t seed1=0, uint32_t seed2=0) { simplerandom_mwc64_seed(&rng, seed1, seed2); }
    SimpleRandomWrapperMWC64(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        simplerandom_mwc64_seed_array(&rng, p_seeds, num_seeds, mix_extras);
    }
    size_t num_seeds() { return 2; }

    uint32_t operator()() { return simplerandom_mwc64_next(&rng); }
    void discard(uintmax_t n) { simplerandom_mwc64_discard(&rng, n); }
    void mix(uint32_t * p_mix_array, size_t n) { simplerandom_mwc64_mix(&rng, p_mix_array, n); }
};

class SimpleRandomWrapperKISS2 : public SimpleRandomWrapper
{
private:
    SimpleRandomKISS2_t rng;
public:
    SimpleRandomWrapperKISS2(uint32_t seed1=0, uint32_t seed2=0, uint32_t seed3=0, uint32_t seed4=0)
    {
        simplerandom_kiss2_seed(&rng, seed1, seed2, seed3, seed4);
    }
    SimpleRandomWrapperKISS2(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        simplerandom_kiss2_seed_array(&rng, p_seeds, num_seeds, mix_extras);
    }
    size_t num_seeds() { return 4; }

    uint32_t operator()() { return simplerandom_kiss2_next(&rng); }
    void discard(uintmax_t n) { simplerandom_kiss2_discard(&rng, n); }
    void mix(uint32_t * p_mix_array, size_t n) { simplerandom_kiss2_mix(&rng, p_mix_array, n); }
};

#endif

class SimpleRandomWrapperLFSR113 : public SimpleRandomWrapper
{
private:
    SimpleRandomLFSR113_t  rng;
public:
    SimpleRandomWrapperLFSR113(uint32_t seed1=0, uint32_t seed2=0, uint32_t seed3=0, uint32_t seed4=0)
    {
        simplerandom_lfsr113_seed(&rng, seed1, seed2, seed3, seed4);
    }
    SimpleRandomWrapperLFSR113(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        simplerandom_lfsr113_seed_array(&rng, p_seeds, num_seeds, mix_extras);
    }
    size_t num_seeds() { return 4; }

    uint32_t operator()() { return simplerandom_lfsr113_next(&rng); }
    void discard(uintmax_t n) { simplerandom_lfsr113_discard(&rng, n); }
    void mix(uint32_t * p_mix_array, size_t n) { simplerandom_lfsr113_mix(&rng, p_mix_array, n); }
};

class SimpleRandomWrapperLFSR88 : public SimpleRandomWrapper
{
private:
    SimpleRandomLFSR88_t  rng;
public:
    SimpleRandomWrapperLFSR88(uint32_t seed1=0, uint32_t seed2=0, uint32_t seed3=0)
    {
        simplerandom_lfsr88_seed(&rng, seed1, seed2, seed3);
    }
    SimpleRandomWrapperLFSR88(const uint32_t * p_seeds, size_t num_seeds, bool mix_extras=false)
    {
        simplerandom_lfsr88_seed_array(&rng, p_seeds, num_seeds, mix_extras);
    }
    size_t num_seeds() { return 3; }

    uint32_t operator()() { return simplerandom_lfsr88_next(&rng); }
    void discard(uintmax_t n) { simplerandom_lfsr88_discard(&rng, n); }
    void mix(uint32_t * p_mix_array, size_t n) { simplerandom_lfsr88_mix(&rng, p_mix_array, n); }
};
