"""
Simple random

Unit Tests
"""

import unittest

import simplerandom.iterators as sri
#import simplerandom.iterators._iterators_py as sri


class Marsaglia1999Tests(unittest.TestCase):
    """Tests as in Marsaglia 1999 post
    
    The Marsaglia 1999 post didn't explicitly set seed values for each RNG,
    but relied on the seed values that were side-effects of previous RNG
    executions. But we want to run each test as a stand-alone unit. So we
    have obtained the seed values from the C execution, and set them
    explicitly in each test here. The exception is LFIB4 followed by SWB--
    it would be painful to set the SWB seed table explicitly.
    """

    def test_lfib4_swb(self):
        # Set up KISS RNG to initialise seeds for LFIB4 and SWB RNGs.
        random_kiss = sri.RandomKISSIterator(12345, 65435, 12345, 34221)
        t = [ random_kiss.next() for i in range(256) ]

        # Test LFIB4
        lfib4 = sri.RandomLFIB4Iterator(t)
        for i in range(1000000):
            k = lfib4.next()
        self.assertEqual(k, 1064612766, "LFIB4 test returned %d instead of expected value" % k)

        # Test SWB
        swb = sri.RandomSWBIterator(lfib4.t)
        swb.c = lfib4.c
        for i in range(1000000):
            k = swb.next()
        self.assertEqual(k, 627749721, "SWB test returned %d instead of expected value" % k)

    def test_kiss(self):
        random_kiss = sri.RandomKISSIterator(2247183469, 99545079, 1017008441, 3259917390)
        for i in range(1000000):
            k = random_kiss.next()
        self.assertEqual(k, 1372460312, "KISS test returned %d instead of expected value" % k)

    def test_cong(self):
        cong = sri.RandomCongIterator(2524969849)
        for i in range(1000000):
            k = cong.next()
        self.assertEqual(k, 1529210297, "Cong test returned %d instead of expected value" % k)

    def test_shr3(self):
        shr3 = sri.RandomSHR3Iterator(4176875757)
        for i in range(1000000):
            k = shr3.next()
        self.assertEqual(k, 2642725982, "SHR3 test returned %d instead of expected value" % k)

    def test_mwc(self):
        mwc = sri.RandomMWCIterator(2374144069, 1046675282)
        for i in range(1000000):
            k = mwc.next()
        self.assertEqual(k, 904977562, "MWC test returned %d instead of expected value" % k)

    def test_fib(self):
        fib = sri._RandomFibIterator(9983651,95746118)
        for i in range(1000000):
            k = fib.next()
        self.assertEqual(k, 3519793928, "Fib test returned %d instead of expected value" % k)


class KISS2Test(unittest.TestCase):
    def runTest(self):
        kiss2 = sri.RandomKISS2Iterator()
        for i in range(1000000):
            k = kiss2.next()
        self.assertEqual(k, 1010846401, "KISS2 test returned %d instead of expected value" % k)


class MWC64Test(unittest.TestCase):
    def test_seed_with_MSbit_set(self):
        """Test MWC64 with MS-bit of mwc_c seed set.
        
        This caused an exception an earlier version of the Cython code (0.7.0)
        when built with Cython 0.14.
        """
        mwc64 = sri.RandomMWC64Iterator(2**31, 1)
        mwc64.next()


def runtests():
    unittest.main()


if __name__ == '__main__':
    runtests()
