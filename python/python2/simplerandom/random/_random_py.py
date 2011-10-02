
# Standard Python library
import random

import simplerandom.iterators as sri


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


class Cong(_StandardRandomTemplate):
    RNG_CLASS = sri.Cong
    __doc__ = RNG_CLASS.__doc__


class SHR3(_StandardRandomTemplate):
    RNG_CLASS = sri.SHR3
    __doc__ = RNG_CLASS.__doc__


class MWC1(_StandardRandomTemplate):
    RNG_CLASS = sri.MWC1
    RNG_SEEDS = 2
    __doc__ = RNG_CLASS.__doc__


class MWC2(_StandardRandomTemplate):
    RNG_CLASS = sri.MWC2
    RNG_SEEDS = 2
    __doc__ = RNG_CLASS.__doc__


class MWC64(_StandardRandomTemplate):
    RNG_CLASS = sri.MWC64
    RNG_SEEDS = 2
    __doc__ = RNG_CLASS.__doc__


class KISS(_StandardRandomTemplate):
    RNG_CLASS = sri.KISS
    RNG_SEEDS = 4
    __doc__ = RNG_CLASS.__doc__


class KISS2(_StandardRandomTemplate):
    RNG_CLASS = sri.KISS2
    RNG_SEEDS = 4
    __doc__ = RNG_CLASS.__doc__


class LFSR113(_StandardRandomTemplate):
    RNG_CLASS = sri.LFSR113
    RNG_SEEDS = 4
    __doc__ = RNG_CLASS.__doc__


class LFSR88(_StandardRandomTemplate):
    RNG_CLASS = sri.LFSR88
    RNG_SEEDS = 3
    __doc__ = RNG_CLASS.__doc__

