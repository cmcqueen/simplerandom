"""
Reference: http://www.cse.yorku.ca/~oz/marsaglia-rng.html
"""

import random


class RandomIteratorTemplate(object):
    '''Template class for random number generator'''

    def __init__(self, seed = None):
        raise NotImplemented

    def next(self):
        raise NotImplemented
        return 0

    def __iter__(self):
        return self


class RandomCongIterator(object):
    '''Congruential random number generator'''

    def __init__(self, seed):
        self.cong = (seed & 0xFFFFFFFF)

    def next(self):
        self.cong = (69069 * self.cong + 1234567) & 0xFFFFFFFF
        return self.cong

    def __iter__(self):
        return self


class RandomSHR3Iterator(object):
    '''3-shift-register random number generator'''

    def __init__(self, seed):
        self.shr3_j = (seed & 0xFFFFFFFF)

    def next(self):
        shr3_j = self.shr3_j
        shr3_j ^= (shr3_j & 0x7FFF) << 17
        shr3_j ^= shr3_j >> 13
        shr3_j ^= (shr3_j & 0x7FFFFFF) << 5
        self.shr3_j = shr3_j
        return shr3_j

    def __iter__(self):
        return self


class RandomMWCIterator(object):
    '''"Multiply-with-carry" random number generator'''

    def __init__(self, seed_z, seed_w):
        self.mwc_z = (seed_z & 0xFFFFFFFF)
        self.mwc_w = (seed_w & 0xFFFFFFFF)

    def next(self):
        self.mwc_z = 36969 * (self.mwc_z & 0xFFFF) + (self.mwc_z >> 16)
        self.mwc_w = 18000 * (self.mwc_w & 0xFFFF) + (self.mwc_w >> 16)
        mwc = (((self.mwc_z & 0xFFFF) << 16) + self.mwc_w) & 0xFFFFFFFF
        self.mwc = mwc
        return mwc

    def __iter__(self):
        return self


class RandomKISSIterator(RandomMWCIterator, RandomCongIterator, RandomSHR3Iterator):
    '''"Keep It Simple Stupid" random number generator
    
    It combines the RandomMWC, RandomCong, RandomSHR3
    generators. Period is about 2**123.
    '''

    def __init__(self, seed_mwc_z, seed_mwc_w, seed_cong, seed_shr3):
#        self.random_mwc = RandomMWC(seed_mwc_z, seed_mwc_w)
#        self.random_cong = RandomCong(seed_cong)
#        self.random_shr3 = RandomSHR3(seed_shr3)
        RandomMWCIterator.__init__(self, seed_mwc_z, seed_mwc_w)
        RandomCongIterator.__init__(self, seed_cong)
        RandomSHR3Iterator.__init__(self, seed_shr3)

    def next(self):
#        mwc_val = next(self.random_mwc)
#        cong_val = next(self.random_cong)
#        shr3_val = next(self.random_shr3)
        mwc_val = RandomMWCIterator.next(self)
        cong_val = RandomCongIterator.next(self)
        shr3_val = RandomSHR3Iterator.next(self)
        return ((mwc_val ^ cong_val) + shr3_val) & 0xFFFFFFFF


class RandomLFIB4Iterator(object):
    '''"Lagged Fibonacci 4-lag" random number generator'''

    def __init__(self, seed = None):
        if not seed:
            random_kiss = RandomKISS(12345, 65435, 12345, 34221)
            seed = [ next(random_kiss) for i in range(256) ]
        else:
            if len(seed) != 256:
                raise Exception("seed length must be 256")
        self.t = list(seed)
        self.c = 0

    def next(self):
        t = self.t

        c = self.c
        c = (c + 1) % 256
        self.c = c

        new_value = (t[c] +
                     t[(c + 58) % 256] +
                     t[(c + 119) % 256] +
                     t[(c + 178) % 256]) & 0xFFFFFFFF

        t[c] = new_value

        return new_value

    def __iter__(self):
        return self

class RandomSWBIterator(RandomLFIB4Iterator):
    '''"Subtract-With-Borrow" random number generator
    
    This is a Fibonacci 2-lag generator with an extra "borrow" operation.
    '''

    def __init__(self, seed = None):
        RandomLFIB4Iterator.__init__(self, seed)
        self.borrow = 0

    def next(self):
        t = self.t

        c = self.c
        c = (c + 1) % 256
        self.c = c

        x = t[(c + 34) % 256]
        y = (t[(c + 19) % 256] + self.borrow) & 0xFFFFFFFF
        new_value = (x - y) & 0xFFFFFFFF

        t[c] = new_value

        self.borrow = 1 if (x < y) else 0

        return new_value

    def __iter__(self):
        return self


class RandomFibIterator(object):
    '''Classical Fibonacci sequence

    x(n)=x(n-1)+x(n-2),but taken modulo 2^32. Its period is 3 * (2**31) if one
    of its two seeds is odd and not 1 mod 8.
    
    It has little worth as a RNG by itself, but provides a simple and fast
    component for use in combination generators.
    '''

    def __init__(self, seed_a, seed_b):
        self.a = (seed_a & 0xFFFFFFFF)
        self.b = (seed_b & 0xFFFFFFFF)

    def next(self):
        new_a = self.b
        new_b = (self.a + self.b) & 0xFFFFFFFF
        (self.a, self.b) = (new_a, new_b)
        return new_a

    def __iter__(self):
        return self


class StandardRandomTemplate(random.Random):
    BPF = random.BPF
    RECIP_BPF = random.RECIP_BPF
    RNG_BITS = 32

    def getrandbits(self, k):
        while self.bits < k:
            self.f = (self.f << self.RNG_BITS) | next(self.rng_iterator)
            self.bits += self.RNG_BITS
        self.bits -= k
        x = self.f >> self.bits
        self.f &= ((1 << self.bits) - 1)
        return x

    def random(self, bpf=None, recip_bpf=None):
        if not bpf:
            bpf = self.BPF
            recip_bpf = self.RECIP_BPF
        else:
            if not recip_bpf:
                recip_bpf = 1./(1 << bpf)
        return self.getrandbits(bpf) * recip_bpf

def marsaglia_test():
    random_kiss = RandomKISSIterator(12345, 65435, 12345, 34221)
    t = [ next(random_kiss) for i in range(256) ]
#        for i in t: print i

    lfib4 = RandomLFIB4Iterator(t)
    if 1:
        for i in range(1000000):
            k = next(lfib4)
#                if i < 256: print k
        print k - 1064612766

    swb = RandomSWBIterator(lfib4.t)
    swb.c = lfib4.c
    if 1:
        for i in range(1000000):
            k = next(swb)
        print k - 627749721

    if 1:
        for i in range(1000000):
            k = next(random_kiss)
        print k - 1372460312

    cong = RandomCongIterator(random_kiss.cong)
    if 1:
        for i in range(1000000):
            k = next(cong)
        print k - 1529210297

    shr3 = RandomSHR3Iterator(random_kiss.shr3_j)
    if 1:
        for i in range(1000000):
            k = next(shr3)
        print k - 2642725982

    mwc = RandomMWCIterator(random_kiss.mwc_z, random_kiss.mwc_w)
    if 1:
        for i in range(1000000):
            k = next(mwc)
        print k - 904977562

    fib = RandomFibIterator(9983651,95746118)
    if 1:
        for i in range(1000000):
            k = next(fib)
        print k - 3519793928


if __name__ == "__main__":
    if 0:
        mwc = RandomMWCIterator(12345, 65435)
        for i in range(256):
            print next(mwc)
    if 0:
        cong = RandomCongIterator(12345)
        for i in range(256):
            print next(cong)
    if 0:
        shr3 = RandomSHR3Iterator(34221)
        for i in range(256):
            print next(shr3)
    if 0:
        kiss = RandomKISSIterator(12345, 65435, 12345, 34221)
        for i in range(256):
            kiss_val = next(kiss)
            print kiss.mwc
            print kiss.cong
            print kiss.shr3_j
            print kiss_val
            print
    if 1:
        marsaglia_test()
