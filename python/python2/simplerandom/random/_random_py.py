
# Standard Python library
import random
import numbers
import sys

import simplerandom.iterators as sri


class _StandardRandomTemplate(random.Random):
    BPF = random.BPF
    RECIP_BPF = random.RECIP_BPF
    RNG_BITS = 32
    RNG_RANGE = (1 << RNG_BITS)
    RNG_BITS_MASK = RNG_RANGE - 1
    RNG_SEEDS = 1

    def __init__(self, x=None, bpf=None):
        if not bpf:
            self.bpf = self.BPF
            self.recip_bpf = self.RECIP_BPF
        else:
            self.bpf = bpf
            self.recip_bpf = 1./(1 << bpf)

        self.rng_iterator = self.RNG_CLASS()
        self.seed(x)

    def seed(self, seed=None):
        if seed is None:
            seed = 0
        elif not isinstance(seed, numbers.Integral):
            seed = hash(seed)
            if seed < 0:
                seed %= ((sys.maxsize + 1) * 2)
        seeds = []
        while True:
            seeds.append(seed & self.RNG_BITS_MASK)
            seed >>= self.RNG_BITS
            if seed == 0 or seed == -1:
                break
        self.rng_iterator.seed(seeds, mix_extras=True)
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

    def random(self):
        return self.getrandbits(self.bpf) * self.recip_bpf

    def jumpahead(self, n):
        n_bits = n * self.bpf
        if n_bits < self.bits:
            self.bits -= n_bits
        else:
            n_more_bits = n_bits - self.bits
            n_rng_words = n_more_bits // self.RNG_BITS
            remove_bits = n_more_bits % self.RNG_BITS
            self.f = self.rng_iterator.jumpahead(n_rng_words + 1)
            self.bits = self.RNG_BITS - remove_bits
            self.f &= ((1 << self.bits) - 1)

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

