#!/usr/bin/env python2

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

    def __init__(self, seed = None):
        if seed==None:
            seed = 12344
        self.cong = int(seed) & 0xFFFFFFFF

    def seed(self, seed = None):
        self.__init__(seed)

    def next(self):
        self.cong = (69069 * self.cong + 1234567) & 0xFFFFFFFF
        return self.cong

    def __iter__(self):
        return self

    def getstate(self):
        return (self.cong, )

    def setstate(self, state):
        (self.cong, ) = (int(val) & 0xFFFFFFFF for val in state)


class RandomSHR3Iterator(object):
    '''3-shift-register random number generator'''

    def __init__(self, seed = None):
        if seed==None:
            seed = 34223
        self.shr3_j = int(seed) & 0xFFFFFFFF

    def seed(self, seed = None):
        self.__init__(seed)

    def next(self):
        shr3_j = self.shr3_j
        shr3_j ^= (shr3_j & 0x7FFF) << 17
        shr3_j ^= shr3_j >> 13
        shr3_j ^= (shr3_j & 0x7FFFFFF) << 5
        self.shr3_j = shr3_j
        return shr3_j

    def __iter__(self):
        return self

    def getstate(self):
        return (self.shr3_j, )

    def setstate(self, state):
        (self.shr3_j, ) = (int(val) & 0xFFFFFFFF for val in state)


class RandomMWCIterator(object):
    '''"Multiply-with-carry" random number generator'''

    def __init__(self, seed_z = None, seed_w = None):
        if seed_z==None:
            seed_z = 12344
        if seed_w==None:
            seed_w = 65437
        self.mwc_z = int(seed_z) & 0xFFFFFFFF
        self.mwc_w = int(seed_w) & 0xFFFFFFFF

    def seed(self, seed_z = None, seed_w = None):
        self.__init__(seed_z, seed_w)

    def next(self):
        self.mwc_z = 36969 * (self.mwc_z & 0xFFFF) + (self.mwc_z >> 16)
        self.mwc_w = 18000 * (self.mwc_w & 0xFFFF) + (self.mwc_w >> 16)
        return self._get_mwc()

    def _get_mwc(self):
        return (((self.mwc_z & 0xFFFF) << 16) + self.mwc_w) & 0xFFFFFFFF

    mwc = property(_get_mwc)

    def __iter__(self):
        return self

    def getstate(self):
        return (self.mwc_z, self.mwc_w)

    def setstate(self, state):
        (self.mwc_z, self.mwc_w) = (int(val) & 0xFFFFFFFF for val in state)


class RandomKISSIterator(object):
    '''"Keep It Simple Stupid" random number generator
    
    It combines the RandomMWC, RandomCong, RandomSHR3
    generators. Period is about 2**123.
    '''

    def __init__(self, seed_mwc_z = None, seed_mwc_w = None, seed_cong = None, seed_shr3 = None):
        self.random_mwc = RandomMWCIterator(seed_mwc_z, seed_mwc_w)
        self.random_cong = RandomCongIterator(seed_cong)
        self.random_shr3 = RandomSHR3Iterator(seed_shr3)

    def seed(self, seed_mwc_z = None, seed_mwc_w = None, seed_cong = None, seed_shr3 = None):
        self.__init__(seed_mwc_z, seed_mwc_w, seed_cong, seed_shr3)

    def next(self):
        mwc_val = next(self.random_mwc)
        cong_val = next(self.random_cong)
        shr3_val = next(self.random_shr3)
        return ((mwc_val ^ cong_val) + shr3_val) & 0xFFFFFFFF

    def getstate(self):
        return (self.random_mwc.getstate(), self.random_cong.getstate(), self.random_shr3.getstate())

    def setstate(self, state):
        (mwc_state, cong_state, shr3_state) = state
        self.random_mwc.setstate(mwc_state)
        self.random_cong.setstate(cong_state)
        self.random_shr3.setstate(shr3_state)

    @property
    def mwc_z(self):
        return self.random_mwc.mwc_z
    @mwc_z.setter
    def mwc_z(self, value):
        self.random_mwc.mwc_z = value

    @property
    def mwc_w(self):
        return self.random_mwc.mwc_w
    @mwc_w.setter
    def mwc_w(self, value):
        self.random_mwc.mwc_w = value

    @property
    def mwc(self):
        return self.random_mwc.mwc

    @property
    def shr3_j(self):
        return self.random_shr3.shr3_j
    @shr3_j.setter
    def shr3_j(self, value):
        self.random_shr3.shr3_j = value

    @property
    def cong(self):
        return self.random_cong.cong
    @cong.setter
    def cong(self, value):
        self.random_cong.cong = value


class RandomLFIB4Iterator(object):
    '''"Lagged Fibonacci 4-lag" random number generator'''

    def __init__(self, seed = None):
        if not seed:
            random_kiss = RandomKISSIterator(12345, 65435, 12345, 34221)
            self.t = [ next(random_kiss) for i in range(256) ]
        else:
            if len(seed) != 256:
                raise Exception("seed length must be 256")
            self.t = [ int(val) & 0xFFFFFFFF for val in seed ]
        self.c = 0

    def seed(self, seed = None):
        self.__init__(seed)

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

    def getstate(self):
        return tuple(self.t[self.c :] + self.t[: self.c])

    def setstate(self, state):
        if len(state) != 256:
            raise Exception("state length must be 256")
        self.t = list(int(val) & 0xFFFFFFFF for val in state)
        self.c = 0


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

    def getstate(self):
        t_tuple = RandomLFIB4Iterator.getstate(self)
        return (t_tuple, self.borrow)

    def setstate(self, state):
        (t_tuple, borrow) = state
        RandomLFIB4Iterator.setstate(self, t_tuple)
        self.borrow = 1 if borrow else 0


class _RandomFibIterator(object):
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
    RNG_RANGE = (1 << RNG_BITS)

    def __init__(self, x = None):
        self.rng_iterator = self.RNG_CLASS(1)
        self.seed(x)

    def seed(self, seed=None):
        seeder = random.Random(seed)
        c = seeder.randrange(self.RNG_RANGE)
        self.rng_iterator.seed(c)
        self.f = 0
        self.bits = 0

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

    def getstate(self):
        return self.f, self.bits, self.rng_iterator.getstate()

    def setstate(self, state):
        (f, bits, rng_state) = state
        bits = int(bits)
        bits = max(bits, self.RNG_BITS)
        f %= (1 << bits)
        self.f, self.bits, = f,  bits
        self.rng_iterator.setstate(rng_state)


class RandomCong(StandardRandomTemplate):
    '''Congruential random number generator'''
    RNG_CLASS = RandomCongIterator


class RandomSHR3(StandardRandomTemplate):
    '''3-shift-register random number generator'''
    RNG_CLASS = RandomSHR3Iterator


class RandomMWC(StandardRandomTemplate):
    '''"Multiply-with-carry" random number generator'''
    RNG_CLASS = RandomMWCIterator

    def seed(self, seed=None):
        seeder = random.Random(seed)
        seed_z = seeder.randrange(self.RNG_RANGE)
        seed_w = seeder.randrange(self.RNG_RANGE)
        self.rng_iterator.seed(seed_z, seed_w)
        self.f = 0
        self.bits = 0


class RandomKISS(StandardRandomTemplate):
    '''"Keep It Simple Stupid" random number generator
    
    It combines the RandomMWC, RandomCong, RandomSHR3
    generators. Period is about 2**123.
    '''
    RNG_CLASS = RandomKISSIterator

    def seed(self, seed=None):
        seeder = random.Random(seed)
        seed_z = seeder.randrange(self.RNG_RANGE)
        seed_w = seeder.randrange(self.RNG_RANGE)
        seed_cong = seeder.randrange(self.RNG_RANGE)
        seed_shr3 = seeder.randrange(self.RNG_RANGE)
        self.rng_iterator.seed(seed_z, seed_w, seed_cong, seed_shr3)
        self.f = 0
        self.bits = 0

