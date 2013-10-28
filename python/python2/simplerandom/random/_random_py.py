
# Standard Python library
import random
from numbers import Integral as _Integral
from os import urandom as _urandom
from binascii import hexlify as _hexlify
from math import log

import simplerandom.iterators as sri

class _StandardRandomTemplate(random.Random):
    BPF = random.BPF
    RECIP_BPF = random.RECIP_BPF
    RNG_BITS = 32
    RNG_MIN = 0                 # with few exceptions (e.g. SHR3 with min 1)
    RNG_MAX = 2**32 - 1
    RNG_RANGE = RNG_MAX - RNG_MIN + 1
    RNG_RANGE_BITS = log(RNG_RANGE, 2)
    SEED_BITS_MASK = 2**32 - 1
    RNG_SEEDS = 1

    def __init__(self, x=None, bpf=None):
        """x is a seed. For consistent cross-platform seeding, provide
        an integer seed.
        bpf is "bits per float", the number of bits of random data used
        to generate each output of random().
        """
        self.rng_iterator = self.RNG_CLASS()
        self.seed(x)

        if not bpf:
            bpf = self.BPF
        self.setbpf(bpf)

    def seed(self, seed=None):
        """For consistent cross-platform seeding, provide an integer seed.
        """
        if seed is None:
            # Use same random seed code copied from Python's random.Random
            try:
                seed = long(_hexlify(_urandom(16)), 16)
            except NotImplementedError:
                import time
                seed = long(time.time() * 256) # use fractional seconds
        elif not isinstance(seed, _Integral):
            # Use the hash of the input seed object. Note this does not give
            # consistent results cross-platform--between Python versions or
            # between 32-bit and 64-bit systems.
            seed = hash(seed)
        seeds = []
        while True:
            seeds.append(seed & self.SEED_BITS_MASK)
            seed >>= self.RNG_BITS
            # If seed is negative, then it effectively has infinitely extending
            # '1' bits (modelled as a 2's complement representation). So when
            # right-shifting it, it will eventually get to -1, and any further
            # right-shifting will not change it.
            if seed == 0 or seed == -1:
                break
        self.rng_iterator.seed(seeds, mix_extras=True)
        self.f = 0
        self.bits = 0

    def getbpf(self):
        """Get number of bits per float output"""
        return self._bpf

    def setbpf(self, bpf):
        """Set number of bits per float output"""
        self._bpf = min(bpf, self.BPF)
        self._rng_n = int((self._bpf + self.RNG_RANGE_BITS - 1) / self.RNG_RANGE_BITS)

    bpf = property(getbpf, setbpf, doc="bits per float")

    def getrandbits(self, k):
        while self.bits < k:
            self.f = (self.f << self.RNG_BITS) | self.rng_iterator.next()
            self.bits += self.RNG_BITS
        self.bits -= k
        x = self.f >> self.bits
        self.f &= ((1 << self.bits) - 1)
        return x

    def random(self):
        accum = 0.0
        accum_range = 1.0
        for _ in range(self._rng_n):
            accum += (self.rng_iterator.next() - self.RNG_MIN) * accum_range
            accum_range *= self.RNG_RANGE
        return accum / accum_range

    def jumpahead(self, n):
        """Jump the random number generator ahead 'n' values of the
        random() function.
        This is a genuine jumpahead operation (unlike Python's standard
        library Mersene Twister implementation), implemented for all
        simplerandom generators. It is implemented in both Python 2
        and Python 3.
        """
        n_bits = n * self._bpf
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
    RNG_MIN = RNG_CLASS.min()
    RNG_RANGE = _StandardRandomTemplate.RNG_MAX - RNG_MIN + 1
    RNG_RANGE_BITS = log(RNG_RANGE, 2)
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

