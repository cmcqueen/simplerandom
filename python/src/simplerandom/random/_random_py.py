
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

    def __new__(cls, *args, **kwargs):
        return super (_StandardRandomTemplate, cls).__new__ (cls, random.random() )

    def __init__(self, x=None, *args, **kwargs):
        """x is a seed. For consistent cross-platform seeding, provide
        an integer seed.
        bpf is "bits per float", the number of bits of random data used
        to generate each output of random().
        """
        bpf = kwargs.pop('bpf', None)
        for key in kwargs:
            raise TypeError("__init__() got an unexpected keyword argument '%s'" % key)

        self.rng_iterator = self.RNG_CLASS()
        random.Random.__init__(self, x)         # This will call self.seed() with the 1 arg.
        if args:
            self.seed(x, *args)                 # Call our seed again with all args. Inefficient but necessary.

        if not bpf:
            bpf = self.BPF
        self.setbpf(bpf)

    def seed(self, x=None, *args):
        """For consistent cross-platform seeding, provide an integer seed.
        """
        if x is None:
            # Use same random seed code copied from Python's random.Random
            try:
                x = int(_hexlify(_urandom(16)), 16)
            except NotImplementedError:
                import time
                x = int(time.time() * 256) # use fractional seconds
        elif not isinstance(x, _Integral):
            # Use the hash of the input seed object. Note this does not give
            # consistent results cross-platform--between Python versions or
            # between 32-bit and 64-bit systems.
            x = hash(x)
        self.rng_iterator.seed(x, *args, mix_extras=True)

    def getbpf(self):
        """Get number of bits per float output"""
        return self._bpf

    def setbpf(self, bpf):
        """Set number of bits per float output"""
        self._bpf = min(bpf, self.BPF)
        self._rng_n = int((self._bpf + self.RNG_RANGE_BITS - 1) / self.RNG_RANGE_BITS)

    bpf = property(getbpf, setbpf, doc="bits per float")

    def getrandbits(self, k):
        accum = 0
        accum_bits = 0
        rng_bits = self.RNG_BITS
        k_div, k_remainder = divmod(k, rng_bits)
        if k_remainder:
            accum_bits = k_remainder
            accum = next(self.rng_iterator) >> (rng_bits - k_remainder)
        while k_div > 0:
            accum |= next(self.rng_iterator) << accum_bits
            accum_bits += rng_bits
            k_div -= rng_bits
        return accum

    def random(self):
        accum = 0.0
        accum_range = 1.0
        for _ in range(self._rng_n):
            accum += (next(self.rng_iterator) - self.RNG_MIN) * accum_range
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
        self.rng_iterator.jumpahead(n * self._rng_n)

    def getstate(self):
        return self.rng_iterator.getstate()

    def setstate(self, state):
        self.rng_iterator.setstate(state)

    def __repr__(self):
        """The object could be represented either as a single (potentially
        large) seed integer, or alternatively as a number of 32-bit seeds
        (which is not the standard Python random API, but it's consistent
        with the C init/seed API).
        Show the state as a single seed integer, for consistency with the
        Python API.
        """
        accum = 0
        accum_range = 1
        for x in sri._traverse_iter(self.getstate()):
            accum += x * accum_range
            accum_range <<= 32
        if 0:
            # Return a list of integers
            return self.__class__.__name__ + "(" + ",".join(repr(int(x)) for x in self.getstate()) + ")"
        else:
            # Return a single large integer
            return self.__class__.__name__ + "(" + repr(int(accum)) + ")"


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

