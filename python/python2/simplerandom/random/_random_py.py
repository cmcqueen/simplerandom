
# Standard Python library
import random

from simplerandom.iterators import *


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


