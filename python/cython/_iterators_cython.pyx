
#cython: language_level=3

cdef extern from "types.h":
    ctypedef unsigned long uint64_t
    ctypedef unsigned int uint32_t

from simplerandom._bitcolumnmatrix import BitColumnMatrix

__all__ = [
    "Cong",
    "SHR3",
    "MWC1",
    "MWC2",
    "MWC64",
    "KISS",
    "KISS2",
    "LFSR113",
    "LFSR88",
    "_traverse_iter",
]

def _traverse_iter(o, tree_types=(list, tuple)):
    """Iterate over nested containers and/or iterators.
    This allows generator __init__() functions to be passed seeds either as
    a series of arguments, or as a list/tuple.
    """
    SIMPLERANDOM_BITS = 32
    SIMPLERANDOM_MOD = 2**SIMPLERANDOM_BITS
    SIMPLERANDOM_MASK = SIMPLERANDOM_MOD - 1
    if isinstance(o, tree_types) or getattr(o, '__iter__', False):
        for value in o:
            for subvalue in _traverse_iter(value):
                while True:
                    yield subvalue & SIMPLERANDOM_MASK
                    subvalue >>= SIMPLERANDOM_BITS
                    # If value is negative, then it effectively has infinitely extending
                    # '1' bits (modelled as a 2's complement representation). So when
                    # right-shifting it, it will eventually get to -1, and any further
                    # right-shifting will not change it.
                    if subvalue == 0 or subvalue == -1:
                        break
    else:
        yield o

def _repeat_iter(input_iter):
    """Iterate over the input iter values. Then repeat the last value
    indefinitely. This is useful to repeat seed values when an insufficient
    number of seeds are provided.

    E.g. KISS(1) effectively becomes KISS(1, 1, 1, 1), rather than (if we just
    used default values) KISS(1, default-value, default-value, default-value)

    It is better to repeat the last seed value, rather than just using default
    values. Given two generators seeded with an insufficient number of seeds,
    repeating the last seed value means their states are more different from
    each other, with less correlation between their generated outputs.
    """
    last_value = None
    for value in input_iter:
        last_value = value
        yield value
    if last_value is not None:
        while True:
            yield last_value

def _next_seed_int32_or_default(seed_iter, uint32_t default_value):
    try:
        seed_item = next(seed_iter)
    except StopIteration:
        return default_value
    else:
        if seed_item is None:
            return default_value
        else:
            return (int(seed_item) & 0xFFFFFFFFu)

def _geom_series_uint32(uint32_t r, n):
    """Unsigned integer calculation of sum of geometric series:
    1 + r + r^2 + r^3 + ... r^(n-1)
    summed to n terms.
    Calculated modulo 2**32.
    Use the formula (r**n - 1) / (r - 1)
    For calculating geometric series mod 2**32, see:
    http://www.codechef.com/wiki/tutorial-just-simple-sum#Back_to_the_geometric_series
    """
    cdef uint64_t m = 2**32
    cdef uint32_t common_factor
    cdef uint32_t other_factors
    cdef uint32_t other_factors_inverse
    cdef uint64_t numerator

    if n == 0:
        return 0
    if n == 1 or r == 0:
        return 1
    # Split (r - 1) into common factors with the modulo 2**32 -- i.e. all
    # factors of 2; and other factors which are coprime with the modulo 2**32.
    other_factors = r - 1u
    common_factor = 1u
    while (other_factors % 2u) == 0:
        other_factors //= 2u
        common_factor *= 2u
    other_factors_inverse = pow(other_factors, m - 1u, m)
    numerator = pow(r, n, common_factor * m) - 1u
    return (numerator // common_factor * other_factors_inverse) % m

cdef uint32_t SIMPLERANDOM_MAX = 2**32 - 1
cdef uint64_t SIMPLERANDOM_MOD = 2**32
cdef uint64_t CONG_CYCLE_LEN = 2**32
cdef uint32_t CONG_MULT = 69069u
cdef uint32_t CONG_CONST = 12345u

def _simplerandom_min():
    return 0
def _simplerandom_max():
    return SIMPLERANDOM_MAX
def _SHR3_min():
    return 1

cdef class Cong(object):
    '''Congruential random number generator

    This is a congruential generator with the widely used
    69069 multiplier: x[n]=69069x[n-1]+12345. It has
    period 2**32.

    The leading half of its 32 bits seem to pass tests,
    but bits in the last half are too regular. It fails
    tests for which those bits play a significant role.
    But keep in mind that it is a rare application for
    which the trailing bits play a significant role. Cong
    is one of the most widely used generators of the last
    30 years, as it was the system generator for VAX and
    was incorporated in several popular software packages,
    all seemingly without complaint.
    '''

    cdef public uint32_t cong

    min = staticmethod(_simplerandom_min)
    max = staticmethod(_simplerandom_max)

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        self.cong = _next_seed_int32_or_default(seed_iter, 0)
        if kwargs.pop('mix_extras', False):
            self.mix(seed_iter)
        for key in kwargs:
            raise TypeError("__init__() got an unexpected keyword argument '%s'" % key)

    def seed(self, *args, **kwargs):
        self.__init__(*args, **kwargs)

    def sanitise(self):
        pass

    def __next__(self):
        self.cong = CONG_MULT * self.cong + CONG_CONST
        return self.cong

    def current(self):
        return self.cong

    def mix(self, *args):
        cdef uint32_t value_int
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFFu
            self.cong ^= value_int
            self.__next__()
        return self.cong

    def __iter__(self):
        return self

    def getstate(self):
        return (self.cong, )

    def setstate(self, state):
        self.cong = int(state[0]) & 0xFFFFFFFFu

    def jumpahead(self, n):
        # Cong.jumpahead(n) = r**n * x mod 2**32 +
        #                      c * (1 + r + r**2 + ... + r**(n-1)) mod 2**32
        # where r = 69069 and c = 12345.
        #
        # The part c * (1 + r + r**2 + ... + r**(n-1)) is a geometric series.
        # For calculating geometric series mod 2**32, see:
        # http://www.codechef.com/wiki/tutorial-just-simple-sum#Back_to_the_geometric_series
        cdef uint32_t n_int
        cdef uint32_t mult_exp
        cdef uint32_t add_const_part
        cdef uint32_t add_const

        n_int = n % CONG_CYCLE_LEN
        mult_exp = CONG_MULT**n_int
        add_const_part = _geom_series_uint32(CONG_MULT, n_int)
        add_const = add_const_part * CONG_CONST
        self.cong = mult_exp * self.cong + add_const

    def __repr__(self):
        return self.__class__.__name__ + "(" + repr(int(self.cong)) + ")"


cdef uint32_t SHR3_CYCLE_LEN = 2u**32u - 1u
_SHR3_MATRIX_COLUMNS = [
    0x00042021, 0x00084042, 0x00108084, 0x00210108, 0x00420231, 0x00840462, 0x010808C4, 0x02101188,
    0x04202310, 0x08404620, 0x10808C40, 0x21011880, 0x42023100, 0x84046200, 0x0808C400, 0x10118800,
    0x20231000, 0x40462021, 0x808C4042, 0x01080084, 0x02100108, 0x04200210, 0x08400420, 0x10800840,
    0x21001080, 0x42002100, 0x84004200, 0x08008400, 0x10010800, 0x20021000, 0x40042000, 0x80084000,
]

_SHR3_MATRIX = BitColumnMatrix(_SHR3_MATRIX_COLUMNS)

cdef class SHR3(object):
    '''3-shift-register random number generator

    SHR3 is a 3-shift-register generator with period
    2**32-1. It uses y[n]=y[n-1](I+L^13)(I+R^17)(I+L^5),
    with the y's viewed as binary vectors, L the 32x32
    binary matrix that shifts a vector left 1, and R its
    transpose.

    SHR3 seems to pass all except those related to the
    binary rank test, since 32 successive values, as
    binary vectors, must be linearly independent, while
    32 successive truly random 32-bit integers, viewed
    as binary vectors, will be linearly independent only
    about 29% of the time.
    '''

    cdef public uint32_t shr3

    min = staticmethod(_SHR3_min)
    max = staticmethod(_simplerandom_max)

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        self.shr3 = _next_seed_int32_or_default(seed_iter, 0xFFFFFFFFu)
        self.sanitise()
        if kwargs.pop('mix_extras', False):
            self.mix(seed_iter)
        for key in kwargs:
            raise TypeError("__init__() got an unexpected keyword argument '%s'" % key)

    def seed(self, *args, **kwargs):
        self.__init__(*args, **kwargs)

    def sanitise(self):
        if self.shr3 == 0:
            # 0 is a bad seed. Invert to get a good seed.
            self.shr3 = 0xFFFFFFFFu

    def __next__(self):
        cdef uint32_t shr3
        shr3 = self.shr3
        shr3 ^= shr3 << 13u
        shr3 ^= shr3 >> 17u
        shr3 ^= shr3 << 5u
        self.shr3 = shr3
        return shr3

    def current(self):
        return self.shr3

    def mix(self, *args):
        cdef uint32_t value_int
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFFu
            self.shr3 ^= value_int
            self.sanitise()
            self.__next__()
        return self.shr3

    def __iter__(self):
        return self

    def getstate(self):
        return (self.shr3, )

    def setstate(self, state):
        self.shr3 = int(state[0]) & 0xFFFFFFFFu
        self.sanitise()

    def jumpahead(self, n):
        cdef uint32_t n_shr3
        n_shr3 = n % SHR3_CYCLE_LEN
        shr3 = pow(_SHR3_MATRIX, n_shr3) * self.shr3
        self.shr3 = shr3

    def __repr__(self):
        return self.__class__.__name__ + "(" + repr(int(self.shr3)) + ")"


cdef uint32_t _MWC_UPPER_MULT = 36969u
cdef uint32_t _MWC_LOWER_MULT = 18000u
cdef uint32_t _MWC_UPPER_MODULO = _MWC_UPPER_MULT * 2u**16u - 1u
cdef uint32_t _MWC_LOWER_MODULO = _MWC_LOWER_MULT * 2u**16u - 1u
cdef uint32_t _MWC_UPPER_CYCLE_LEN = _MWC_UPPER_MULT * 2u**16u // 2u - 1u
cdef uint32_t _MWC_LOWER_CYCLE_LEN = _MWC_LOWER_MULT * 2u**16u // 2u - 1u

cdef class MWC1(object):
    '''"Multiply-with-carry" random number generator

    This is the MWC as defined in Marsaglia's 1999
    newsgroup post.

    This uses two MWC generators to generate high and
    low 16-bit parts, which are then combined to make a
    32-bit value.

    The MWC generator concatenates two 16-bit multiply-
    with-carry generators:

        x[n]=36969x[n-1]+carry,
        y[n]=18000y[n-1]+carry mod 2**16,

    It has a period about 2**60.

    This seems to pass all Marsaglia's Diehard tests.
    However, it fails many of L'Ecuyer's TestU01
    tests. The modified MWC2 generator passes many more
    tests in TestU01, and should probably be preferred,
    unless backwards compatibility is required.
    '''

    cdef public uint32_t mwc_upper
    cdef public uint32_t mwc_lower

    min = staticmethod(_simplerandom_min)
    max = staticmethod(_simplerandom_max)

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        repeat_seed_iter = _repeat_iter(seed_iter)
        self.mwc_upper = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFFu)
        self.mwc_lower = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFFu)
        self.sanitise()
        if kwargs.pop('mix_extras', False):
            self.mix(seed_iter)
        for key in kwargs:
            raise TypeError("__init__() got an unexpected keyword argument '%s'" % key)

    def seed(self, *args, **kwargs):
        self.__init__(*args, **kwargs)

    def sanitise(self):
        self._sanitise_upper()
        self._sanitise_lower()

    def _sanitise_upper(self):
        mwc_upper_orig = self.mwc_upper
        # There are a few bad states--that is, any multiple of
        # 0x9068FFFF (which is 36969 * 2**16 - 1).
        sanitised_value = mwc_upper_orig % 0x9068FFFFu
        if sanitised_value == 0:
            # Invert to get a good seed.
            sanitised_value = (mwc_upper_orig ^ 0xFFFFFFFFu) % 0x9068FFFFu
        self.mwc_upper = sanitised_value
    def _sanitise_lower(self):
        mwc_lower_orig = self.mwc_lower
        # There are a few bad states--that is, any multiple of
        # 0x464FFFFF (which is 18000 * 2**16 - 1).
        sanitised_value = mwc_lower_orig % 0x464FFFFFu
        if sanitised_value == 0:
            # Invert to get a good seed.
            sanitised_value = (mwc_lower_orig ^ 0xFFFFFFFFu) % 0x464FFFFFu
        self.mwc_lower = sanitised_value

    def _next_upper(self):
        self.mwc_upper = 36969u * (self.mwc_upper & 0xFFFFu) + (self.mwc_upper >> 16u)
    def _next_lower(self):
        self.mwc_lower = 18000u * (self.mwc_lower & 0xFFFFu) + (self.mwc_lower >> 16u)

    def __next__(self):
        cdef uint32_t mwc
        self.mwc_upper = 36969u * (self.mwc_upper & 0xFFFFu) + (self.mwc_upper >> 16u)
        self.mwc_lower = 18000u * (self.mwc_lower & 0xFFFFu) + (self.mwc_lower >> 16u)
        mwc = (self.mwc_upper << 16u) + self.mwc_lower
        return mwc

    def current(self):
        cdef uint32_t mwc
        mwc = (self.mwc_upper << 16u) + self.mwc_lower
        return mwc

    property mwc:
        def __get__(self):
            cdef uint32_t mwc
            mwc = (self.mwc_upper << 16u) + self.mwc_lower
            return mwc

    def mix(self, *args):
        cdef uint32_t value_int
        cdef uint32_t current
        cdef uint32_t selector
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFFu
            current = self.current()
            selector = (current >> 24u) & 0x1u
            if selector == 0:
                self.mwc_upper ^= value_int
                self._sanitise_upper()
                self._next_upper()
            else:
                self.mwc_lower ^= value_int
                self._sanitise_lower()
                self._next_lower()
        return self.current()

    def __iter__(self):
        return self

    def getstate(self):
        return (self.mwc_upper, self.mwc_lower)

    def setstate(self, state):
        self.mwc_upper = int(state[0]) & 0xFFFFFFFFu
        self.mwc_lower = int(state[1]) & 0xFFFFFFFFu
        self.sanitise()

    def jumpahead(self, n):
        cdef uint64_t n_int
        n_int = n % _MWC_UPPER_CYCLE_LEN
        # The following calculation needs to be done in greater than 32-bit.
        self.mwc_upper = pow(<uint64_t>_MWC_UPPER_MULT, n_int, <uint64_t>_MWC_UPPER_MODULO) * self.mwc_upper % _MWC_UPPER_MODULO
        n_int = n % _MWC_LOWER_CYCLE_LEN
        # The following calculation needs to be done in greater than 32-bit.
        self.mwc_lower = pow(<uint64_t>_MWC_LOWER_MULT, n_int, <uint64_t>_MWC_LOWER_MODULO) * self.mwc_lower % _MWC_LOWER_MODULO

    def __repr__(self):
        return self.__class__.__name__ + "(" + repr(int(self.mwc_upper)) + "," + repr(int(self.mwc_lower)) + ")"


cdef class MWC2(object):
    '''"Multiply-with-carry" random number generator

    Very similar to MWC1, except that it concatenates the
    two 16-bit MWC generators differently. The 'x'
    generator is rotated 16 bits instead of just shifted
    16 bits.

    This gets much better test results than MWC1 in
    L'Ecuyer's TestU01 test suite, so it should probably
    be preferred.
    '''

    cdef public uint32_t mwc_upper
    cdef public uint32_t mwc_lower

    min = staticmethod(_simplerandom_min)
    max = staticmethod(_simplerandom_max)

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        repeat_seed_iter = _repeat_iter(seed_iter)
        self.mwc_upper = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFFu)
        self.mwc_lower = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFFu)
        self.sanitise()
        if kwargs.pop('mix_extras', False):
            self.mix(seed_iter)
        for key in kwargs:
            raise TypeError("__init__() got an unexpected keyword argument '%s'" % key)

    def seed(self, *args, **kwargs):
        self.__init__(*args, **kwargs)

    def sanitise(self):
        self._sanitise_upper()
        self._sanitise_lower()

    def _sanitise_upper(self):
        mwc_upper_orig = self.mwc_upper
        # There are a few bad states--that is, any multiple of
        # 0x9068FFFF (which is 36969 * 2**16 - 1).
        sanitised_value = mwc_upper_orig % 0x9068FFFFu
        if sanitised_value == 0:
            # Invert to get a good seed.
            sanitised_value = (mwc_upper_orig ^ 0xFFFFFFFFu) % 0x9068FFFFu
        self.mwc_upper = sanitised_value
    def _sanitise_lower(self):
        mwc_lower_orig = self.mwc_lower
        # There are a few bad states--that is, any multiple of
        # 0x464FFFFF (which is 18000 * 2**16 - 1).
        sanitised_value = mwc_lower_orig % 0x464FFFFFu
        if sanitised_value == 0:
            # Invert to get a good seed.
            sanitised_value = (mwc_lower_orig ^ 0xFFFFFFFFu) % 0x464FFFFFu
        self.mwc_lower = sanitised_value

    def _next_upper(self):
        self.mwc_upper = 36969u * (self.mwc_upper & 0xFFFFu) + (self.mwc_upper >> 16u)
    def _next_lower(self):
        self.mwc_lower = 18000u * (self.mwc_lower & 0xFFFFu) + (self.mwc_lower >> 16u)

    def __next__(self):
        cdef uint32_t mwc
        self.mwc_upper = 36969u * (self.mwc_upper & 0xFFFFu) + (self.mwc_upper >> 16u)
        self.mwc_lower = 18000u * (self.mwc_lower & 0xFFFFu) + (self.mwc_lower >> 16u)
        mwc = (self.mwc_upper << 16u) + (self.mwc_upper >> 16u) + self.mwc_lower
        return mwc

    def current(self):
        cdef uint32_t mwc
        mwc = (self.mwc_upper << 16u) + (self.mwc_upper >> 16u) + self.mwc_lower
        return mwc

    property mwc:
        def __get__(self):
            cdef uint32_t mwc
            mwc = (self.mwc_upper << 16u) + (self.mwc_upper >> 16u) + self.mwc_lower
            return mwc

    def mix(self, *args):
        cdef uint32_t value_int
        cdef uint32_t current
        cdef uint32_t selector
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFFu
            current = self.current()
            selector = (current >> 24u) & 0x1u
            if selector == 0:
                self.mwc_upper ^= value_int
                self._sanitise_upper()
                self._next_upper()
            else:
                self.mwc_lower ^= value_int
                self._sanitise_lower()
                self._next_lower()
        return self.current()

    def __iter__(self):
        return self

    def getstate(self):
        return (self.mwc_upper, self.mwc_lower)

    def setstate(self, state):
        self.mwc_upper = int(state[0]) & 0xFFFFFFFFu
        self.mwc_lower = int(state[1]) & 0xFFFFFFFFu
        self.sanitise()

    def jumpahead(self, n):
        cdef uint64_t n_int
        n_int = n % _MWC_UPPER_CYCLE_LEN
        # The following calculation needs to be done in greater than 32-bit.
        self.mwc_upper = pow(<uint64_t>_MWC_UPPER_MULT, n_int, <uint64_t>_MWC_UPPER_MODULO) * self.mwc_upper % _MWC_UPPER_MODULO
        n_int = n % _MWC_LOWER_CYCLE_LEN
        # The following calculation needs to be done in greater than 32-bit.
        self.mwc_lower = pow(<uint64_t>_MWC_LOWER_MULT, n_int, <uint64_t>_MWC_LOWER_MODULO) * self.mwc_lower % _MWC_LOWER_MODULO

    def __repr__(self):
        return self.__class__.__name__ + "(" + repr(int(self.mwc_upper)) + "," + repr(int(self.mwc_lower)) + ")"


cdef uint64_t _MWC64_MULT = 698769069u
cdef uint64_t _MWC64_MODULO = _MWC64_MULT * 2u**32u - 1u
cdef uint64_t _MWC64_CYCLE_LEN = _MWC64_MULT * 2u**32u // 2u - 1u

cdef class MWC64(object):
    '''"Multiply-with-carry" random number generator

    This uses a single MWC generator with 64 bits to
    generate a 32-bit value. The seeds should be 32-bit
    values.
    '''

    cdef public uint32_t mwc_upper
    cdef public uint32_t mwc_lower

    min = staticmethod(_simplerandom_min)
    max = staticmethod(_simplerandom_max)

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        repeat_seed_iter = _repeat_iter(seed_iter)
        self.mwc_upper = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFFu)
        self.mwc_lower = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFFu)
        self.sanitise()
        if kwargs.pop('mix_extras', False):
            self.mix(seed_iter)
        for key in kwargs:
            raise TypeError("__init__() got an unexpected keyword argument '%s'" % key)

    def seed(self, *args, **kwargs):
        self.__init__(*args, **kwargs)

    def sanitise(self):
        cdef uint64_t state64
        cdef uint64_t temp
        cdef bint was_changed

        state64 = (<uint64_t>self.mwc_upper << 32u) + self.mwc_lower
        temp = state64
        was_changed = False
        # There are a few bad seeds--that is, seeds that are a multiple of
        # 0x29A65EACFFFFFFFF (which is 698769069 * 2**32 - 1).
        if state64 >= 0x29A65EACFFFFFFFFu:
            was_changed = True
        temp = state64 % 0x29A65EACFFFFFFFFu
        if temp == 0:
            # Invert to get a good seed.
            temp = (state64 ^ 0xFFFFFFFFFFFFFFFFu) % 0x29A65EACFFFFFFFFu
            was_changed = True
        if was_changed:
            self.mwc_upper = temp >> 32u
            self.mwc_lower = temp & 0xFFFFFFFFu

    def __next__(self):
        cdef uint64_t temp64

        temp64 = <uint64_t>698769069u * self.mwc_lower + self.mwc_upper
        self.mwc_lower = temp64 & 0xFFFFFFFFu
        self.mwc_upper = temp64 >> 32u
        return self.mwc_lower

    def current(self):
        return self.mwc_lower

    property mwc:
        def __get__(self):
            return self.mwc_lower

    def mix(self, *args):
        cdef uint32_t value_int
        cdef uint32_t current
        cdef uint32_t selector
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFFu
            current = self.current()
            selector = (current >> 24u) & 0x1u
            if selector == 0:
                self.mwc_upper ^= value_int
            else:
                self.mwc_lower ^= value_int
            self.sanitise()
            self.__next__()
        return self.current()

    def __iter__(self):
        return self

    def getstate(self):
        return (self.mwc_upper, self.mwc_lower)

    def setstate(self, state):
        self.mwc_upper = int(state[0]) & 0xFFFFFFFFu
        self.mwc_lower = int(state[1]) & 0xFFFFFFFFu
        self.sanitise()

    def jumpahead(self, n):
        cdef uint64_t temp64

        n = n % _MWC64_CYCLE_LEN
        temp64 = (<uint64_t>self.mwc_upper << 32u) + self.mwc_lower
        # The following calculation needs to be done in greater than 64-bit.
        temp64 = pow(int(_MWC64_MULT), n, int(_MWC64_MODULO)) * temp64 % _MWC64_MODULO
        self.mwc_lower = temp64 & 0xFFFFFFFFu
        self.mwc_upper = (temp64 >> 32u) & 0xFFFFFFFFu

    def __repr__(self):
        return self.__class__.__name__ + "(" + repr(int(self.mwc_upper)) + "," + repr(int(self.mwc_lower)) + ")"


cdef class KISS(object):
    '''"Keep It Simple Stupid" random number generator

    It combines the MWC2, Cong, SHR3 generators. Period is
    about 2**123.

    This is based on, but not identical to, Marsaglia's
    KISS generator as defined in his 1999 newsgroup post.
    That generator most significantly has problems with its
    SHR3 component (see notes on SHR3). Since we are not
    keeping compatibility with the 1999 KISS generator for
    that reason, we take the opportunity to slightly
    update the MWC and Cong generators too.
    '''

    cdef public uint32_t cong
    cdef public uint32_t shr3
    cdef public uint32_t mwc_upper
    cdef public uint32_t mwc_lower

    min = staticmethod(_simplerandom_min)
    max = staticmethod(_simplerandom_max)

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        repeat_seed_iter = _repeat_iter(seed_iter)
        self.mwc_upper = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFFu)
        self.mwc_lower = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFFu)
        self.cong = _next_seed_int32_or_default(repeat_seed_iter, 0)
        self.shr3 = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFFu)
        self.sanitise()
        if kwargs.pop('mix_extras', False):
            self.mix(seed_iter)
        for key in kwargs:
            raise TypeError("__init__() got an unexpected keyword argument '%s'" % key)

    def seed(self, *args, **kwargs):
        self.__init__(*args, **kwargs)

    def sanitise(self):
        self._sanitise_mwc_upper()
        self._sanitise_mwc_lower()
        # Cong doesn't need any sanitising
        self._sanitise_shr3()

    def _sanitise_mwc_upper(self):
        mwc_upper_orig = self.mwc_upper
        # There are a few bad states--that is, any multiple of
        # 0x9068FFFF (which is 36969 * 2**16 - 1).
        sanitised_value = mwc_upper_orig % 0x9068FFFFu
        if sanitised_value == 0:
            # Invert to get a good seed.
            sanitised_value = (mwc_upper_orig ^ 0xFFFFFFFFu) % 0x9068FFFFu
        self.mwc_upper = sanitised_value
    def _sanitise_mwc_lower(self):
        mwc_lower_orig = self.mwc_lower
        # There are a few bad states--that is, any multiple of
        # 0x464FFFFF (which is 18000 * 2**16 - 1).
        sanitised_value = mwc_lower_orig % 0x464FFFFFu
        if sanitised_value == 0:
            # Invert to get a good seed.
            sanitised_value = (mwc_lower_orig ^ 0xFFFFFFFFu) % 0x464FFFFFu
        self.mwc_lower = sanitised_value

    def _sanitise_shr3(self):
        if self.shr3 == 0:
            # 0 is a bad seed. Invert to get a good seed.
            self.shr3 = 0xFFFFFFFFu

    def _next_mwc_upper(self):
        self.mwc_upper = 36969u * (self.mwc_upper & 0xFFFFu) + (self.mwc_upper >> 16u)
    def _next_mwc_lower(self):
        self.mwc_lower = 18000u * (self.mwc_lower & 0xFFFFu) + (self.mwc_lower >> 16u)
    def _next_cong(self):
        self.cong = 69069u * self.cong + 12345u
    def _next_shr3(self):
        cdef uint32_t shr3
        shr3 = self.shr3
        shr3 ^= shr3 << 13u
        shr3 ^= shr3 >> 17u
        shr3 ^= shr3 << 5u
        self.shr3 = shr3
        return shr3

    def __next__(self):
        cdef uint32_t mwc
        cdef uint32_t shr3

        # Update MWC RNG
        self.mwc_upper = 36969u * (self.mwc_upper & 0xFFFFu) + (self.mwc_upper >> 16u)
        self.mwc_lower = 18000u * (self.mwc_lower & 0xFFFFu) + (self.mwc_lower >> 16u)
        mwc = (self.mwc_upper << 16u) + (self.mwc_upper >> 16u) + self.mwc_lower

        # Update Cong RNG
        self.cong = 69069u * self.cong + 12345u

        # Update SHR3 RNG
        shr3 = self.shr3
        shr3 ^= shr3 << 13u
        shr3 ^= shr3 >> 17u
        shr3 ^= shr3 << 5u
        self.shr3 = shr3

        return (mwc ^ self.cong) + shr3

    property mwc:
        def __get__(self):
            cdef uint32_t mwc
            mwc = (self.mwc_upper << 16u) + (self.mwc_upper >> 16u) + self.mwc_lower
            return mwc

    def current(self):
        cdef uint32_t mwc
        mwc = (self.mwc_upper << 16u) + (self.mwc_upper >> 16u) + self.mwc_lower
        return (mwc ^ self.cong) + self.shr3

    def mix(self, *args):
        cdef uint32_t value_int
        cdef uint32_t current
        cdef uint32_t selector
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFFu
            current = self.current()
            selector = (current >> 24u) & 0x3u
            if selector == 0:
                self.mwc_upper ^= value_int
                self._sanitise_mwc_upper()
                self._next_mwc_upper()
            elif selector == 1:
                self.mwc_lower ^= value_int
                self._sanitise_mwc_lower()
                self._next_mwc_lower()
            elif selector == 2:
                self.cong ^= value_int
                # Cong doesn't need any sanitising
                self._next_cong()
            else:   # selector == 3
                self.shr3 ^= value_int
                self._sanitise_shr3()
                self._next_shr3()
        return self.current()

    def __iter__(self):
        return self

    def getstate(self):
        return ((self.mwc_upper, self.mwc_lower), (self.cong,), (self.shr3,))

    def setstate(self, state):
        (mwc_state, cong_state, shr3_state) = state
        self.mwc_upper = int(mwc_state[0]) & 0xFFFFFFFFu
        self.mwc_lower = int(mwc_state[1]) & 0xFFFFFFFFu
        self.cong = int(cong_state[0]) & 0xFFFFFFFFu
        self.shr3 = int(shr3_state[0]) & 0xFFFFFFFFu
        self.sanitise()

    def jumpahead(self, n):
        # Cong variables
        cdef uint32_t n_int
        cdef uint32_t mult_exp
        cdef uint32_t add_const_part
        cdef uint32_t add_const

        # MWC2
        n_int = n % _MWC_UPPER_CYCLE_LEN
        # The following calculation needs to be done in greater than 32-bit.
        self.mwc_upper = pow(<uint64_t>_MWC_UPPER_MULT, n_int, <uint64_t>_MWC_UPPER_MODULO) * self.mwc_upper % _MWC_UPPER_MODULO
        n_int = n % _MWC_LOWER_CYCLE_LEN
        # The following calculation needs to be done in greater than 32-bit.
        self.mwc_lower = pow(<uint64_t>_MWC_LOWER_MULT, n_int, <uint64_t>_MWC_LOWER_MODULO) * self.mwc_lower % _MWC_LOWER_MODULO

        # Cong
        n_int = n % CONG_CYCLE_LEN
        mult_exp = CONG_MULT**n_int
        add_const_part = _geom_series_uint32(CONG_MULT, n_int)
        add_const = add_const_part * CONG_CONST
        self.cong = mult_exp * self.cong + add_const

        # SHR3
        n_int = n % SHR3_CYCLE_LEN
        shr3 = pow(_SHR3_MATRIX, n_int) * self.shr3
        self.shr3 = shr3

    def __repr__(self):
        return (self.__class__.__name__ + "(" + repr(int(self.mwc_upper)) +
                                        "," + repr(int(self.mwc_lower)) +
                                        "," + repr(int(self.cong)) +
                                        "," + repr(int(self.shr3)) + ")")


cdef class KISS2(object):
    '''"Keep It Simple Stupid" random number generator

    It combines the MWC64, Cong, SHR3 generators. Period
    is about 2**123.

    This is a slightly updated KISS generator design, from
    a newsgroup post in 2003:

    http://groups.google.com/group/sci.math/msg/9959175f66dd138f

    The MWC component uses a single 64-bit calculation,
    instead of two 32-bit calculations that are combined.
    '''

    cdef public uint32_t cong
    cdef public uint32_t shr3
    cdef public uint32_t mwc_upper
    cdef public uint32_t mwc_lower

    min = staticmethod(_simplerandom_min)
    max = staticmethod(_simplerandom_max)

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        repeat_seed_iter = _repeat_iter(seed_iter)
        self.mwc_upper = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFFu)
        self.mwc_lower = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFFu)
        self.cong = _next_seed_int32_or_default(repeat_seed_iter, 0)
        self.shr3 = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFFu)
        self.sanitise()
        if kwargs.pop('mix_extras', False):
            self.mix(seed_iter)
        for key in kwargs:
            raise TypeError("__init__() got an unexpected keyword argument '%s'" % key)

    def seed(self, *args, **kwargs):
        self.__init__(*args, **kwargs)

    def sanitise(self):
        self._sanitise_mwc64()
        # Cong doesn't need any sanitising
        self._sanitise_shr3()

    def _sanitise_mwc64(self):
        cdef uint64_t state64
        cdef uint64_t temp
        cdef bint was_changed

        state64 = (<uint64_t>self.mwc_upper << 32u) + self.mwc_lower
        temp = state64
        was_changed = False
        # There are a few bad seeds--that is, seeds that are a multiple of
        # 0x29A65EACFFFFFFFF (which is 698769069 * 2**32 - 1).
        if state64 >= 0x29A65EACFFFFFFFFu:
            was_changed = True
        temp = state64 % 0x29A65EACFFFFFFFFu
        if temp == 0:
            # Invert to get a good seed.
            temp = (state64 ^ 0xFFFFFFFFFFFFFFFFu) % 0x29A65EACFFFFFFFFu
            was_changed = True
        if was_changed:
            self.mwc_upper = temp >> 32u
            self.mwc_lower = temp & 0xFFFFFFFFu

    def _sanitise_shr3(self):
        if self.shr3 == 0:
            # 0 is a bad seed. Invert to get a good seed.
            self.shr3 = 0xFFFFFFFFu

    def _next_mwc64(self):
        cdef uint64_t temp64

        temp64 = <uint64_t>698769069u * self.mwc_lower + self.mwc_upper
        self.mwc_lower = temp64 & 0xFFFFFFFFu
        self.mwc_upper = temp64 >> 32u
        return self.mwc_lower
    def _next_cong(self):
        self.cong = 69069u * self.cong + 12345u
    def _next_shr3(self):
        cdef uint32_t shr3
        shr3 = self.shr3
        shr3 ^= shr3 << 13u
        shr3 ^= shr3 >> 17u
        shr3 ^= shr3 << 5u
        self.shr3 = shr3
        return shr3

    def __next__(self):
        cdef uint64_t temp64
        cdef uint32_t shr3

        # Update MWC64 RNG
        temp64 = <uint64_t>698769069u * self.mwc_lower + self.mwc_upper
        self.mwc_lower = temp64 & 0xFFFFFFFFu
        self.mwc_upper = temp64 >> 32u

        # Update Cong RNG
        self.cong = 69069u * self.cong + 12345u

        # Update SHR3 RNG
        shr3 = self.shr3
        shr3 ^= shr3 << 13u
        shr3 ^= shr3 >> 17u
        shr3 ^= shr3 << 5u
        self.shr3 = shr3

        return self.mwc_lower + self.cong + shr3

    def current(self):
        return self.mwc_lower + self.cong + self.shr3

    property mwc:
        def __get__(self):
            return self.mwc_lower

    def mix(self, *args):
        cdef uint32_t value_int
        cdef uint32_t current
        cdef uint32_t selector
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFFu
            current = self.current()
            selector = (current >> 24u) & 0x3u
            if selector == 0:
                self.mwc_upper ^= value_int
                self._sanitise_mwc64()
                self._next_mwc64()
            elif selector == 1:
                self.mwc_lower ^= value_int
                self._sanitise_mwc64()
                self._next_mwc64()
            elif selector == 2:
                self.cong ^= value_int
                # Cong doesn't need any sanitising
                self._next_cong()
            else:   # selector == 3
                self.shr3 ^= value_int
                self._sanitise_shr3()
                self._next_shr3()
        return self.current()

    def __iter__(self):
        return self

    def getstate(self):
        return ((self.mwc_upper, self.mwc_lower), (self.cong,), (self.shr3,))

    def setstate(self, state):
        (mwc_state, cong_state, shr3_state) = state
        self.mwc_upper = int(mwc_state[0]) & 0xFFFFFFFFu
        self.mwc_lower = int(mwc_state[1]) & 0xFFFFFFFFu
        self.cong = int(cong_state[0]) & 0xFFFFFFFFu
        self.shr3 = int(shr3_state[0]) & 0xFFFFFFFFu
        self.sanitise()

    def jumpahead(self, n):
        # Cong variables
        cdef uint32_t n_int
        cdef uint32_t mult_exp
        cdef uint32_t add_const_part
        cdef uint32_t add_const
        cdef uint64_t temp64

        # MWC64
        n_mwc = n % _MWC64_CYCLE_LEN
        temp64 = (<uint64_t>self.mwc_upper << 32u) + self.mwc_lower
        # The following calculation needs to be done in greater than 64-bit.
        temp64 = pow(int(_MWC64_MULT), n_mwc, int(_MWC64_MODULO)) * temp64 % _MWC64_MODULO
        self.mwc_lower = temp64 & 0xFFFFFFFFu
        self.mwc_upper = (temp64 >> 32u) & 0xFFFFFFFFu

        # Cong
        n_int = n % CONG_CYCLE_LEN
        mult_exp = CONG_MULT**n_int
        add_const_part = _geom_series_uint32(CONG_MULT, n_int)
        add_const = add_const_part * CONG_CONST
        self.cong = mult_exp * self.cong + add_const

        # SHR3
        n_int = n % SHR3_CYCLE_LEN
        shr3 = pow(_SHR3_MATRIX, n_int) * self.shr3
        self.shr3 = shr3

    def __repr__(self):
        return (self.__class__.__name__ + "(" + repr(int(self.mwc_upper)) +
                                        "," + repr(int(self.mwc_lower)) +
                                        "," + repr(int(self.cong)) +
                                        "," + repr(int(self.shr3)) + ")")


def lfsr_next_one_seed(seed_iter, uint32_t min_value_shift):
    """High-quality seeding for LFSR generators.

    The LFSR generator components discard a certain number of their lower bits
    when generating each output. The significant bits of their state must not
    all be zero. We must ensure that when seeding the generator.

    In case generators are seeded from an incrementing input (such as a system
    timer), and between increments only the lower bits may change, we would
    also like the lower bits of the input to change the initial state, and not
    just be discarded. So we do basic manipulation of the seed input value to
    ensure that all bits of the seed input affect the initial state.
    """
    cdef uint32_t min_value
    cdef uint32_t working_seed

    try:
        seed = next(seed_iter)
    except StopIteration:
        return 0xFFFFFFFFu
    else:
        if seed is None:
            return 0xFFFFFFFFu
        else:
            seed = int(seed) & 0xFFFFFFFFu
            working_seed = (seed ^ (seed << 16)) & 0xFFFFFFFFu

            min_value = 1 << min_value_shift
            if working_seed < min_value:
                working_seed = (seed << 24) & 0xFFFFFFFFu
                if working_seed < min_value:
                    working_seed ^= 0xFFFFFFFFu
            return working_seed

def lfsr_validate_one_seed(seed, uint32_t min_value_shift):
    '''Validate seeds for LFSR generators

    The LFSR generator components discard a certain number of their lower bits
    when generating each output. The significant bits of their state must not
    all be zero. We must ensure that when seeding the generator.

    This is a light-weight validation of seeds, used from setstate().
    '''
    cdef uint32_t min_value

    min_value = 1 << min_value_shift
    if seed < min_value:
        seed ^= 0xFFFFFFFFu
    return seed

def lfsr_state_z(uint32_t z):
    cdef uint32_t work

    work = z ^ (z << 16)
    return int(work)

def lfsr_repr_z(uint32_t z):
    cdef uint32_t work

    work = z ^ (z << 16)
    return repr(int(work))

_LFSR113_1_MATRIX = BitColumnMatrix([
    0x00000000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000, 0x01000000, 0x02000001,
    0x04000002, 0x08000004, 0x10000008, 0x20000010, 0x40000020, 0x80000041, 0x00000082, 0x00000104,
    0x00000208, 0x00000410, 0x00000820, 0x00001040, 0x00002080, 0x00004100, 0x00008200, 0x00010400,
    0x00020800, 0x00041000, 0x00002000, 0x00004000, 0x00008000, 0x00010000, 0x00020000, 0x00040000
])
_LFSR113_1_CYCLE_LEN = 2**(32 - 1) - 1

_LFSR113_2_MATRIX = BitColumnMatrix([
    0x00000000, 0x00000000, 0x00000000, 0x00000020, 0x00000040, 0x00000080, 0x00000100, 0x00000200,
    0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000, 0x00010000, 0x00020000,
    0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000, 0x01000000, 0x02000000,
    0x04000000, 0x08000001, 0x10000002, 0x20000005, 0x4000000A, 0x80000014, 0x00000008, 0x00000010
])
_LFSR113_2_CYCLE_LEN = 2**(32 - 3) - 1

_LFSR113_3_MATRIX = BitColumnMatrix([
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000800, 0x00001000, 0x00002000, 0x00004000,
    0x00008001, 0x00010002, 0x00020004, 0x00040008, 0x00080010, 0x00100020, 0x00200040, 0x00400080,
    0x00800100, 0x01000200, 0x02000400, 0x04000000, 0x08000000, 0x10000001, 0x20000002, 0x40000004,
    0x80000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080, 0x00000100, 0x00000200, 0x00000400
])
_LFSR113_3_CYCLE_LEN = 2**(32 - 4) - 1

_LFSR113_4_MATRIX = BitColumnMatrix([
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00100000,
    0x00200000, 0x00400001, 0x00800002, 0x01000004, 0x02000009, 0x04000012, 0x08000024, 0x10000048,
    0x20000090, 0x40000120, 0x80000240, 0x00000480, 0x00000900, 0x00001200, 0x00002400, 0x00004800,
    0x00009000, 0x00012000, 0x00024000, 0x00048000, 0x00090000, 0x00020000, 0x00040000, 0x00080000
])
_LFSR113_4_CYCLE_LEN = 2**(32 - 7) - 1

cdef class LFSR113(object):
    '''Combined LFSR random number generator by L'Ecuyer

    It combines 4 LFSR generators. The generators have been
    chosen for maximal equidistribution.

    The period is approximately 2**113.

    "Tables of Maximally-Equidistributed Combined Lfsr Generators"
    P. L'Ecuyer
    Mathematics of Computation, 68, 225 (1999), 261–269.
    '''
    cdef public uint32_t z1
    cdef public uint32_t z2
    cdef public uint32_t z3
    cdef public uint32_t z4

    min = staticmethod(_simplerandom_min)
    max = staticmethod(_simplerandom_max)

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        repeat_seed_iter = _repeat_iter(seed_iter)
        self.z1 = lfsr_next_one_seed(repeat_seed_iter, 1)
        self.z2 = lfsr_next_one_seed(repeat_seed_iter, 3)
        self.z3 = lfsr_next_one_seed(repeat_seed_iter, 4)
        self.z4 = lfsr_next_one_seed(repeat_seed_iter, 7)
        if kwargs.pop('mix_extras', False):
            self.mix(seed_iter)
        for key in kwargs:
            raise TypeError("__init__() got an unexpected keyword argument '%s'" % key)

    def seed(self, *args, **kwargs):
        self.__init__(*args, **kwargs)

    def sanitise(self):
        self.z1 = lfsr_validate_one_seed(self.z1, 1)
        self.z2 = lfsr_validate_one_seed(self.z2, 3)
        self.z3 = lfsr_validate_one_seed(self.z3, 4)
        self.z4 = lfsr_validate_one_seed(self.z4, 7)

    def _next_z1(self):
        cdef uint32_t b
        b       = (((self.z1 & 0x03FFFFFFu) << 6) ^ self.z1) >> 13
        self.z1 = ((self.z1 & 0x00003FFEu) << 18) ^ b
    def _next_z2(self):
        cdef uint32_t b
        b       = (((self.z2 & 0x3FFFFFFFu) << 2) ^ self.z2) >> 27
        self.z2 = ((self.z2 & 0x3FFFFFF8u) << 2) ^ b
    def _next_z3(self):
        cdef uint32_t b
        b       = (((self.z3 & 0x0007FFFFu) << 13) ^ self.z3) >> 21
        self.z3 = ((self.z3 & 0x01FFFFF0u) << 7) ^ b
    def _next_z4(self):
        cdef uint32_t b
        b       = (((self.z4 & 0x1FFFFFFFu) << 3) ^ self.z4) >> 12
        self.z4 = ((self.z4 & 0x0007FF80u) << 13) ^ b
    def __next__(self):
        cdef uint32_t b

        b       = (((self.z1 & 0x03FFFFFFu) << 6) ^ self.z1) >> 13
        self.z1 = ((self.z1 & 0x00003FFEu) << 18) ^ b

        b       = (((self.z2 & 0x3FFFFFFFu) << 2) ^ self.z2) >> 27
        self.z2 = ((self.z2 & 0x3FFFFFF8u) << 2) ^ b

        b       = (((self.z3 & 0x0007FFFFu) << 13) ^ self.z3) >> 21
        self.z3 = ((self.z3 & 0x01FFFFF0u) << 7) ^ b

        b       = (((self.z4 & 0x1FFFFFFFu) << 3) ^ self.z4) >> 12
        self.z4 = ((self.z4 & 0x0007FF80u) << 13) ^ b

        return self.z1 ^ self.z2 ^ self.z3 ^ self.z4

    def current(self):
        return self.z1 ^ self.z2 ^ self.z3 ^ self.z4

    def mix(self, *args):
        cdef uint32_t value_int
        cdef uint32_t current
        cdef uint32_t selector
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFFu
            current = self.current()
            selector = (current >> 30) & 0x3
            if selector == 0:
                self.z1 = lfsr_validate_one_seed(self.z1 ^ value_int, 1)
                self._next_z1()
            elif selector == 1:
                self.z2 = lfsr_validate_one_seed(self.z2 ^ value_int, 3)
                self._next_z2()
            elif selector == 2:
                self.z3 = lfsr_validate_one_seed(self.z3 ^ value_int, 4)
                self._next_z3()
            else:   # selector == 3
                self.z4 = lfsr_validate_one_seed(self.z4 ^ value_int, 7)
                self._next_z4()
        return self.current()

    def __iter__(self):
        return self

    def getstate(self):
        return (lfsr_state_z(self.z1), lfsr_state_z(self.z2), lfsr_state_z(self.z3), lfsr_state_z(self.z4))

    def setstate(self, state):
        self.seed(state)

    def jumpahead(self, n):
        n_1 = int(n) % _LFSR113_1_CYCLE_LEN
        n_2 = int(n) % _LFSR113_2_CYCLE_LEN
        n_3 = int(n) % _LFSR113_3_CYCLE_LEN
        n_4 = int(n) % _LFSR113_4_CYCLE_LEN

        z1 = pow(_LFSR113_1_MATRIX, n_1) * self.z1
        self.z1 = z1
        z2 = pow(_LFSR113_2_MATRIX, n_2) * self.z2
        self.z2 = z2
        z3 = pow(_LFSR113_3_MATRIX, n_3) * self.z3
        self.z3 = z3
        z4 = pow(_LFSR113_4_MATRIX, n_4) * self.z4
        self.z4 = z4

    def __repr__(self):
        return (self.__class__.__name__ + "(" + lfsr_repr_z(self.z1) +
                                        "," + lfsr_repr_z(self.z2) +
                                        "," + lfsr_repr_z(self.z3) +
                                        "," + lfsr_repr_z(self.z4) + ")")


_LFSR88_1_MATRIX = BitColumnMatrix([
    0x00000000, 0x00002000, 0x00004000, 0x00008000, 0x00010000, 0x00020000, 0x00040001, 0x00080002,
    0x00100004, 0x00200008, 0x00400010, 0x00800020, 0x01000040, 0x02000080, 0x04000100, 0x08000200,
    0x10000400, 0x20000800, 0x40001000, 0x80000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010,
    0x00000020, 0x00000040, 0x00000080, 0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000
])
_LFSR88_1_CYCLE_LEN = 2**(32 - 1) - 1

_LFSR88_2_MATRIX = BitColumnMatrix([
    0x00000000, 0x00000000, 0x00000000, 0x00000080, 0x00000100, 0x00000200, 0x00000400, 0x00000800,
    0x00001000, 0x00002000, 0x00004000, 0x00008000, 0x00010000, 0x00020000, 0x00040000, 0x00080000,
    0x00100000, 0x00200000, 0x00400000, 0x00800000, 0x01000000, 0x02000000, 0x04000000, 0x08000001,
    0x10000002, 0x20000005, 0x4000000A, 0x80000014, 0x00000028, 0x00000050, 0x00000020, 0x00000040
])
_LFSR88_2_CYCLE_LEN = 2**(32 - 3) - 1

_LFSR88_3_MATRIX = BitColumnMatrix([
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200000, 0x00400000, 0x00800000, 0x01000000,
    0x02000001, 0x04000002, 0x08000004, 0x10000009, 0x20000012, 0x40000024, 0x80000048, 0x00000090,
    0x00000120, 0x00000240, 0x00000480, 0x00000900, 0x00001200, 0x00002400, 0x00004800, 0x00009000,
    0x00012000, 0x00024000, 0x00048000, 0x00090000, 0x00120000, 0x00040000, 0x00080000, 0x00100000
])
_LFSR88_3_CYCLE_LEN = 2**(32 - 4) - 1

cdef class LFSR88(object):
    '''Combined LFSR random number generator by L'Ecuyer

    It combines 3 LFSR generators. The generators have been
    chosen for maximal equidistribution.

    The period is approximately 2**88.

    "Maximally Equidistributed Combined Tausworthe Generators"
    P. L'Ecuyer
    Mathematics of Computation, 65, 213 (1996), 203–213.
    '''

    cdef public uint32_t z1
    cdef public uint32_t z2
    cdef public uint32_t z3

    min = staticmethod(_simplerandom_min)
    max = staticmethod(_simplerandom_max)

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        repeat_seed_iter = _repeat_iter(seed_iter)
        self.z1 = lfsr_next_one_seed(repeat_seed_iter, 1)
        self.z2 = lfsr_next_one_seed(repeat_seed_iter, 3)
        self.z3 = lfsr_next_one_seed(repeat_seed_iter, 4)
        if kwargs.pop('mix_extras', False):
            self.mix(seed_iter)
        for key in kwargs:
            raise TypeError("__init__() got an unexpected keyword argument '%s'" % key)

    def seed(self, *args, **kwargs):
        self.__init__(*args, **kwargs)

    def sanitise(self):
        self.z1 = lfsr_validate_one_seed(self.z1, 1)
        self.z2 = lfsr_validate_one_seed(self.z2, 3)
        self.z3 = lfsr_validate_one_seed(self.z3, 4)

    def _next_z1(self):
        cdef uint32_t b
        b       = (((self.z1 & 0x0007FFFFu) << 13) ^ self.z1) >> 19
        self.z1 = ((self.z1 & 0x000FFFFEu) << 12) ^ b
    def _next_z2(self):
        cdef uint32_t b
        b       = (((self.z2 & 0x3FFFFFFFu) << 2) ^ self.z2) >> 25
        self.z2 = ((self.z2 & 0x0FFFFFF8u) << 4) ^ b
    def _next_z3(self):
        cdef uint32_t b
        b       = (((self.z3 & 0x1FFFFFFFu) << 3) ^ self.z3) >> 11
        self.z3 = ((self.z3 & 0x00007FF0u) << 17) ^ b
    def __next__(self):
        cdef uint32_t b

        b       = (((self.z1 & 0x0007FFFFu) << 13) ^ self.z1) >> 19
        self.z1 = ((self.z1 & 0x000FFFFEu) << 12) ^ b

        b       = (((self.z2 & 0x3FFFFFFFu) << 2) ^ self.z2) >> 25
        self.z2 = ((self.z2 & 0x0FFFFFF8u) << 4) ^ b

        b       = (((self.z3 & 0x1FFFFFFFu) << 3) ^ self.z3) >> 11
        self.z3 = ((self.z3 & 0x00007FF0u) << 17) ^ b

        return self.z1 ^ self.z2 ^ self.z3

    def current(self):
        return self.z1 ^ self.z2 ^ self.z3

    def mix(self, *args):
        cdef uint32_t value_int
        cdef uint32_t current
        cdef uint32_t selector
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFFu
            current = self.current()
            if current < 1431655765u:       # constant is 2^32 / 3
                self.z1 = lfsr_validate_one_seed(self.z1 ^ value_int, 1)
                self._next_z1()
            elif current < 2863311531u:     # constant is 2^32 * 2 / 3
                self.z2 = lfsr_validate_one_seed(self.z2 ^ value_int, 3)
                self._next_z2()
            else:
                self.z3 = lfsr_validate_one_seed(self.z3 ^ value_int, 4)
                self._next_z3()
        return self.current()

    def __iter__(self):
        return self

    def getstate(self):
        return (lfsr_state_z(self.z1), lfsr_state_z(self.z2), lfsr_state_z(self.z3))

    def setstate(self, state):
        self.seed(state)

    def jumpahead(self, n):
        n_1 = int(n) % _LFSR88_1_CYCLE_LEN
        n_2 = int(n) % _LFSR88_2_CYCLE_LEN
        n_3 = int(n) % _LFSR88_3_CYCLE_LEN

        z1 = pow(_LFSR88_1_MATRIX, n_1) * self.z1
        self.z1 = z1
        z2 = pow(_LFSR88_2_MATRIX, n_2) * self.z2
        self.z2 = z2
        z3 = pow(_LFSR88_3_MATRIX, n_3) * self.z3
        self.z3 = z3

    def __repr__(self):
        return (self.__class__.__name__ + "(" + lfsr_repr_z(self.z1) +
                                        "," + lfsr_repr_z(self.z2) +
                                        "," + lfsr_repr_z(self.z3) + ")")
