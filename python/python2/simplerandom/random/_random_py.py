
# Standard Python library
import random

from simplerandom.iterators import *


class _StandardRandomTemplate(random.Random):
    BPF = random.BPF
    RECIP_BPF = random.RECIP_BPF
    RNG_BITS = 32
    RNG_RANGE = (1 << RNG_BITS)
    RNG_SEEDS = 1

    def __init__(self, x = None):
        self.rng_iterator = self.RNG_CLASS()
        self.seed(x)

    def seed(self, seed=None):
        seeder = random.Random(seed)
        # Make some random seed values
        seeds = [ seeder.randrange(self.RNG_RANGE) for _i in range(self.RNG_SEEDS) ]
        self.rng_iterator.seed(*seeds)
        self.f = 0
        self.bits = 0

    def getrandbits(self, k):
        while self.bits < k:
            self.f = (self.f << self.RNG_BITS) | self.rng_iterator.next()
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


class RandomCong(_StandardRandomTemplate):
    '''Congruential random number generator'''
    RNG_CLASS = RandomCongIterator


class RandomSHR3(_StandardRandomTemplate):
    '''3-shift-register random number generator'''
    RNG_CLASS = RandomSHR3Iterator


class RandomMWC(_StandardRandomTemplate):
    '''"Multiply-with-carry" random number generator'''
    RNG_CLASS = RandomMWCIterator
    RNG_SEEDS = 2


class RandomMWC64(_StandardRandomTemplate):
    '''"Multiply-with-carry" random number generator'''
    RNG_CLASS = RandomMWC64Iterator
    RNG_SEEDS = 2


class RandomKISS(_StandardRandomTemplate):
    '''"Keep It Simple Stupid" random number generator
    
    It combines the RandomMWC, RandomCong, RandomSHR3
    generators. Period is about 2**123.
    '''
    RNG_CLASS = RandomKISSIterator
    RNG_SEEDS = 4


class RandomKISS2(_StandardRandomTemplate):
    '''"Keep It Simple Stupid" random number generator
    
    It combines the RandomMWC64, RandomCong, RandomSHR3
    generators. Period is about 2**123.
    '''
    RNG_CLASS = RandomKISS2Iterator
    RNG_SEEDS = 4


class RandomLFIB4(_StandardRandomTemplate):
    '''"Lagged Fibonacci 4-lag" random number generator'''

    RNG_CLASS = RandomLFIB4Iterator

    def seed(self, seed=None):
        seeder = random.Random(seed)
        seed_t = [ seeder.randrange(self.RNG_RANGE) for _i in range(256) ]
        self.rng_iterator.seed(seed_t)
        self.f = 0
        self.bits = 0


class RandomSWB(RandomLFIB4):
    '''"Subtract-With-Borrow" random number generator
    
    This is a Fibonacci 2-lag generator with an extra "borrow" operation.
    '''

    RNG_CLASS = RandomSWBIterator


class RandomLFSR113(_StandardRandomTemplate):
    '''Combined LFSR random number generator by L'Ecuyer

    It combines 4 LFSR generators. The generators have been
    chosen for maximal equidistribution.

    The period is approximately 2**113.
    '''
    RNG_CLASS = RandomLFSR113Iterator
    RNG_SEEDS = 4
