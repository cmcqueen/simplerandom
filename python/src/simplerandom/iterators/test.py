"""
Simple random

Unit Tests
"""

import random
import unittest

import simplerandom.iterators as sri
#import simplerandom.iterators._iterators_py as sri


class Marsaglia1999Tests(unittest.TestCase):
    """Tests as in Marsaglia 1999 post

    The Marsaglia 1999 post didn't explicitly set seed values for each RNG,
    but relied on the seed values that were side-effects of previous RNG
    executions. But we want to run each test as a stand-alone unit. So we
    have obtained the seed values from the C execution, and set them
    explicitly in each test here.
    """

    def test_kiss_million(self):
        random_kiss = sri.KISS(2247183469, 99545079, 3269400377, 3950144837)
        for i in range(1000000):
            k = next(random_kiss)
        self.assertEqual(k, 2100752872)

    def test_cong_million(self):
        cong = sri.Cong(2051391225)
        for i in range(1000000):
            k = next(cong)
        self.assertEqual(k, 2416584377)

    def test_shr3_million(self):
        shr3 = sri.SHR3(3360276411)
        for i in range(1000000):
            k = next(shr3)
        self.assertEqual(k, 1153302609)

    def test_mwc1_million(self):
        mwc = sri.MWC1(2374144069, 1046675282)
        for i in range(1000000):
            k = next(mwc)
        self.assertEqual(k, 904977562)


class CongTest(unittest.TestCase):
    RNG_CLASS = sri.Cong
    RNG_SEEDS = 1
    RNG_BITS = 32
    RNG_RANGE = (1 << RNG_BITS)
    RNG_CYCLE_LEN = 2**32
    MIX_MILLION_STATE = (1888180864,)
    MIX_MILLION_RESULT = 2377132217

    def setUp(self):
        # Make some random seed values
        self.rng_seeds = [ random.randrange(self.RNG_RANGE) for _i in range(self.RNG_SEEDS) ]

        # Make the RNG instance
        self.rng = self.RNG_CLASS(*self.rng_seeds)

    def test_seed(self):
        # Record its initial state
        state_from_init = self.rng.getstate()
        # Use the RNG to make some random numbers
        num_next_calls = random.randrange(3, 10)
        data_from_init = tuple([ next(self.rng) for _i in range(num_next_calls) ])
        # Get its state again
        state_after_data_from_init = self.rng.getstate()

        # Use the seed function to set its state to the same as before
        self.rng.seed(*self.rng_seeds)
        # Get its state again. It should be the same as before.
        state_from_seed = self.rng.getstate()
        self.assertEqual(state_from_init, state_from_seed)
        # Get random numbers again. They should be the same as before.
        data_from_seed = tuple([ next(self.rng) for _i in range(num_next_calls) ])
        self.assertEqual(data_from_init, data_from_seed)
        # Get its state again. It should be the same as before.
        state_after_data_from_seed = self.rng.getstate()
        self.assertEqual(state_after_data_from_init, state_after_data_from_seed)

    def test_state(self):
        # Run it for some random count
        count1 = random.randrange(100,1000)
        for _i in range(count1):
            next(self.rng)

        # Get the state
        rng_state = self.rng.getstate()

        # Run it again for a while
        count2 = 1000
        rng_data = [ next(self.rng) for _i in range(count2) ]

        # Now reset to the previous state, and re-run the data
        self.rng.setstate(rng_state)
        for i in range(count2):
            self.assertEqual(next(self.rng), rng_data[i])

    def test_iter(self):
        """Test that __iter__ member function is present"""
        iter_object = iter(self.rng)

    def test_init(self):
        rng1 = self.RNG_CLASS(self.rng_seeds)
        rng2 = self.RNG_CLASS(*self.rng_seeds)
        self.assertEqual(rng1.getstate(), rng2.getstate())
        rng3 = self.RNG_CLASS(self.rng_seeds, 12345)
        self.assertEqual(rng1.getstate(), rng3.getstate())
        rng4 = self.RNG_CLASS(self.rng_seeds, 12345, mix_extras=True)
        self.assertNotEqual(rng1.getstate(), rng4.getstate())

    def test_mix(self):
        self.mix_values = [ random.randrange(self.RNG_RANGE) for _i in range(10001) ]
        rng1 = self.RNG_CLASS(self.rng_seeds)
        rng1.mix(self.mix_values)
        rng2 = self.RNG_CLASS(self.rng_seeds + self.mix_values, mix_extras=True)
        self.assertEqual(rng1.getstate(), rng2.getstate())

    def test_mix_million(self):
        rng = self.RNG_CLASS()
        rng.mix(range(1000000))
        self.assertEqual(rng.getstate(), self.MIX_MILLION_STATE)
        self.assertEqual(next(rng), self.MIX_MILLION_RESULT)

    def test_jumpahead(self):
        # Do one 'next', to get past any unusual initial state.
        next(self.rng)
        # Get the state
        rng_state = self.rng.getstate()
        # Jump ahead by cycle len
        self.rng.jumpahead(self.RNG_CYCLE_LEN)
        # See that state hasn't changed
        self.assertEqual(rng_state, self.rng.getstate())

        # Jump ahead 1, then back 1
        self.rng.jumpahead(1)
        # See that state HAS changed
        self.assertNotEqual(rng_state, self.rng.getstate())
        self.rng.jumpahead(-1)
        # See that state hasn't changed from original
        self.assertEqual(rng_state, self.rng.getstate())

        jumpahead_rng = self.RNG_CLASS()
        jumpahead_rng_start_state = self.rng.getstate()
        for i in range(10000):
            jumpahead_rng.setstate(jumpahead_rng_start_state)
            jumpahead_rng.jumpahead(i)
            self.assertEqual(next(self.rng), next(jumpahead_rng))

class SHR3Test(CongTest):
    RNG_CLASS = sri.SHR3
    RNG_CYCLE_LEN = 2**32 - 1
    MIX_MILLION_STATE = (3398451483,)
    MIX_MILLION_RESULT = 2497398786


class MWC1Test(CongTest):
    RNG_CLASS = sri.MWC1
    RNG_SEEDS = 2
    RNG_CYCLE_LEN = (36969 * 2**16 // 2 - 1) * (18000 * 2**16 // 2 - 1)
    MIX_MILLION_STATE = (2368777526, 228316256)
    MIX_MILLION_RESULT = 850045851


class KISS2Test(CongTest):
    RNG_CLASS = sri.KISS2
    RNG_SEEDS = 4
    MILLION_RESULT = 4044786495
    MWC64_CYCLE_LEN = 698769069 * 2**32 // 2 - 1
    RNG_CYCLE_LEN = MWC64_CYCLE_LEN * CongTest.RNG_CYCLE_LEN * SHR3Test.RNG_CYCLE_LEN
    MIX_MILLION_STATE = ((239137175, 3792736514), (3369301037,), (2168302343,))
    MIX_MILLION_RESULT = 73179452

    def test_million(self):
        rng = self.RNG_CLASS()
        for i in range(1000000):
            k = next(rng)
        self.assertEqual(k, self.MILLION_RESULT)


class MWC2Test(KISS2Test):
    RNG_CLASS = sri.MWC2
    RNG_SEEDS = 2
    RNG_CYCLE_LEN = MWC1Test.RNG_CYCLE_LEN
    MILLION_RESULT = 767834450
    MIX_MILLION_STATE = (877346697, 339013993)
    MIX_MILLION_RESULT = 2513067739

class MWC64Test(KISS2Test):
    RNG_CLASS = sri.MWC64
    RNG_SEEDS = 2
    RNG_CYCLE_LEN = 698769069 * 2**32 // 2 - 1
    MILLION_RESULT = 2191957470
    MIX_MILLION_STATE = (303682145, 2644015475)
    MIX_MILLION_RESULT = 3125681944

    def test_seed_with_MSbit_set(self):
        """Test MWC64 with MS-bit of mwc_c seed set.

        This caused an exception in an earlier version of the Cython code
        (0.7.0) when built with Cython 0.14.
        """
        mwc64 = sri.MWC64(2**31, 1)
        next(mwc64)


class KISSTest(CongTest):
    RNG_CLASS = sri.KISS
    RNG_SEEDS = 4
    RNG_CYCLE_LEN = MWC2Test.RNG_CYCLE_LEN * CongTest.RNG_CYCLE_LEN * SHR3Test.RNG_CYCLE_LEN
    MIX_MILLION_STATE = ((236260272, 836835745), (789422639,), (961402556,))
    MIX_MILLION_RESULT = 3522308965


class LFSR113Test(KISS2Test):
    RNG_CLASS = sri.LFSR113
    RNG_SEEDS = 4
    RNG_CYCLE_LEN = (2**(32 - 1) - 1) * (2**(32 - 3) - 1) * (2**(32 - 4) - 1) * (2**(32 - 7) - 1)
    MILLION_RESULT = 300959510
    MIX_MILLION_STATE = (2691024344, 4239946093, 3982661918, 2371033497)
    MIX_MILLION_RESULT = 1565144389


class LFSR88Test(KISS2Test):
    RNG_CLASS = sri.LFSR88
    RNG_SEEDS = 3
    RNG_CYCLE_LEN = (2**(32 - 1) - 1) * (2**(32 - 3) - 1) * (2**(32 - 4) - 1)
    MILLION_RESULT = 3774296834
    MIX_MILLION_STATE = (3899912688, 3314273521, 3192347695)
    MIX_MILLION_RESULT = 284026550


def runtests():
    unittest.main()


if __name__ == '__main__':
    runtests()

