
from simplerandom._bitcolumnmatrix import BitColumnMatrix

def _traverse_iter(o, tree_types=(list, tuple)):
    """Iterate over nested containers and/or iterators.
    This allows generator __init__() functions to be passed seeds either as
    a series of arguments, or as a list/tuple.
    """
    if isinstance(o, tree_types) or getattr(o, '__iter__', False):
        for value in o:
            for subvalue in _traverse_iter(value):
                yield subvalue
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

def _next_seed_int32_or_default(seed_iter, default_value):
    try:
        seed_item = seed_iter.next()
    except StopIteration:
        return default_value
    else:
        if seed_item is None:
            return default_value
        else:
            return (int(seed_item) & 0xFFFFFFFF)

class Cong(object):
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
    SIMPLERANDOM_MOD = 2**32
    CONG_CYCLE_LEN = 2**32
    CONG_MULT = 69069
    CONG_CONST = 12345
    # The following are used to calculate Cong.jumpahead().
    JUMPAHEAD_C_FACTOR = 4      # (CONG_MULT - 1) == 4 * 17267
    JUMPAHEAD_C_DENOM = 17267
    JUMPAHEAD_C_MOD = SIMPLERANDOM_MOD * JUMPAHEAD_C_FACTOR
    # Inverse of CONG_JUMPAHEAD_C_DENOM mod 2^32.
    JUMPAHEAD_C_DENOM_INVERSE = pow(JUMPAHEAD_C_DENOM, SIMPLERANDOM_MOD - 1, SIMPLERANDOM_MOD)

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

    def next(self):
        self.cong = (69069 * self.cong + 12345) & 0xFFFFFFFF
        return self.cong

    def current(self):
        return self.cong

    def mix(self, *args):
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFF
            self.cong ^= value_int
            self.next()
        return self.cong

    def __iter__(self):
        return self

    def getstate(self):
        return (self.cong, )

    def setstate(self, state):
        (self.cong, ) = (int(val) & 0xFFFFFFFF for val in state)

    def jumpahead(self, n):
        # Cong.jumpahead(n) = r**n * x mod 2**32 + c * (r**n - 1) / (r - 1) mod 2**32
        # where r = 69069 and c = 12345.
        #
        # The part c * (r**n - 1) / (r - 1) is the formula for a geometric series
        #     c + c*r + c*r^2 + c*r^3 + ... + c*r^(n-1)
        # For calculating geometric series mod 2**32, see:
        # http://www.codechef.com/wiki/tutorial-just-simple-sum#Back_to_the_geometric_series
        #
        # The modulus 2^32 and (r - 1) have a common factor of 4 (see
        # CONG_JUMPAHEAD_C_FACTOR). So we calculate the numerator modulo 2^32 * 4,
        # i.e. 2^34.
        # After calculating the numerator, we divide by the common factor of 4,
        # then multiply by the inverse of the other part of the denominator.
        n = int(n) % self.CONG_CYCLE_LEN
        mult_exp = pow(self.CONG_MULT, n, self.SIMPLERANDOM_MOD)
        add_const = ((((pow(self.CONG_MULT, n, self.JUMPAHEAD_C_MOD) - 1) // self.JUMPAHEAD_C_FACTOR * self.JUMPAHEAD_C_DENOM_INVERSE) & 0xFFFFFFFF) * self.CONG_CONST) & 0xFFFFFFFF
        self.cong = (mult_exp * self.cong + add_const) & 0xFFFFFFFF
        return self.cong


class SHR3(object):
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

    SIMPLERANDOM_MOD = 2**32
    SHR3_CYCLE_LEN = 2**32 - 1

    _SHR3_MATRIX_a = BitColumnMatrix.unity(32) + BitColumnMatrix.shift(32,13)
    _SHR3_MATRIX_b = BitColumnMatrix.unity(32) + BitColumnMatrix.shift(32,-17)
    _SHR3_MATRIX_c = BitColumnMatrix.unity(32) + BitColumnMatrix.shift(32,5)
    _SHR3_MATRIX = _SHR3_MATRIX_c * _SHR3_MATRIX_b * _SHR3_MATRIX_a

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        self.shr3 = _next_seed_int32_or_default(seed_iter, 0xFFFFFFFF)
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
            self.shr3 = 0xFFFFFFFF

    def next(self):
        shr3 = self.shr3
        shr3 ^= (shr3 & 0x7FFFF) << 13
        shr3 ^= shr3 >> 17
        shr3 ^= (shr3 & 0x7FFFFFF) << 5
        self.shr3 = shr3
        return shr3

    def current(self):
        return self.shr3

    def mix(self, *args):
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFF
            self.shr3 ^= value_int
            self.sanitise()
            self.next()
        return self.shr3

    def __iter__(self):
        return self

    def getstate(self):
        return (self.shr3, )

    def setstate(self, state):
        (self.shr3, ) = (int(val) & 0xFFFFFFFF for val in state)
        self.sanitise()

    def jumpahead(self, n):
        n = int(n) % self.SHR3_CYCLE_LEN
        shr3 = pow(self._SHR3_MATRIX, n) * self.shr3
        self.shr3 = shr3
        return shr3

class MWC2(object):
    '''"Multiply-with-carry" random number generator

    Very similar to MWC1, except that it concatenates the
    two 16-bit MWC generators differently. The 'x'
    generator is rotated 16 bits instead of just shifted
    16 bits.

    This gets much better test results than MWC1 in
    L'Ecuyer's TestU01 test suite, so it should probably
    be preferred.
    '''
    _MWC_UPPER_MULT = 36969
    _MWC_LOWER_MULT = 18000
    _MWC_UPPER_MODULO = _MWC_UPPER_MULT * 2**16 - 1
    _MWC_LOWER_MODULO = _MWC_LOWER_MULT * 2**16 - 1
    _MWC_UPPER_CYCLE_LEN = _MWC_UPPER_MULT * 2**16 // 2 - 1
    _MWC_LOWER_CYCLE_LEN = _MWC_LOWER_MULT * 2**16 // 2 - 1

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        repeat_seed_iter = _repeat_iter(seed_iter)
        self.mwc_upper = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFF)
        self.mwc_lower = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFF)
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
        # _MWC_UPPER_MODULO -- that is 0x9068FFFF (which is 36969 * 2**16 - 1).
        sanitised_value = mwc_upper_orig % 0x9068FFFF
        if sanitised_value == 0:
            # Invert to get a good seed.
            sanitised_value = (mwc_upper_orig ^ 0xFFFFFFFF) % 0x9068FFFF
        self.mwc_upper = sanitised_value
    def _sanitise_lower(self):
        mwc_lower_orig = self.mwc_lower
        # There are a few bad states--that is, any multiple of
        # _MWC_LOWER_MODULO -- that is 0x464FFFFF (which is 18000 * 2**16 - 1).
        sanitised_value = mwc_lower_orig % 0x464FFFFF
        if sanitised_value == 0:
            # Invert to get a good seed.
            sanitised_value = (mwc_lower_orig ^ 0xFFFFFFFF) % 0x464FFFFF
        self.mwc_lower = sanitised_value

    def _next_upper(self):
        self.mwc_upper = 36969 * (self.mwc_upper & 0xFFFF) + (self.mwc_upper >> 16)
    def _next_lower(self):
        self.mwc_lower = 18000 * (self.mwc_lower & 0xFFFF) + (self.mwc_lower >> 16)

    def next(self):
        # Note: this is apparently equivalent to:
        # self.mwc_upper = (36969 * self.mwc_upper) % 0x9068FFFF
        # self.mwc_lower = (18000 * self.mwc_lower) % 0x464FFFFF
        # See Random Number Generation, Pierre L’Ecuyer, section 3.6 Linear Recurrences With Carry
        # http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.136.6898&rep=rep1&type=pdf
        self.mwc_upper = 36969 * (self.mwc_upper & 0xFFFF) + (self.mwc_upper >> 16)
        self.mwc_lower = 18000 * (self.mwc_lower & 0xFFFF) + (self.mwc_lower >> 16)
        return self.current()       # call self.current() so that MWC1 can over-ride it

    def current(self):
        return (((self.mwc_upper & 0xFFFF) << 16) + (self.mwc_upper >> 16) + self.mwc_lower) & 0xFFFFFFFF

    mwc = property(current)         # Note that this must be over-ridden again in MWC1

    def mix(self, *args):
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFF
            current = self.current()
            selector = (current >> 31) & 0x1
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
        (self.mwc_upper, self.mwc_lower) = (int(val) & 0xFFFFFFFF for val in state)
        self.sanitise()

    def jumpahead(self, n):
        # See next() note on functional equivalence.
        n_upper = int(n) % self._MWC_UPPER_CYCLE_LEN
        self.mwc_upper = pow(self._MWC_UPPER_MULT, n_upper, self._MWC_UPPER_MODULO) * self.mwc_upper % self._MWC_UPPER_MODULO
        n_lower = int(n) % self._MWC_LOWER_CYCLE_LEN
        self.mwc_lower = pow(self._MWC_LOWER_MULT, n_lower, self._MWC_LOWER_MODULO) * self.mwc_lower % self._MWC_LOWER_MODULO
        return self.current()


class MWC1(MWC2):
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

    def current(self):
        return (((self.mwc_upper & 0xFFFF) << 16) + self.mwc_lower) & 0xFFFFFFFF

    # We have to over-ride this again, because of the way property() works.
    mwc = property(current)


class MWC64(object):
    '''"Multiply-with-carry" random number generator

    This uses a single MWC generator with 64 bits to
    generate a 32-bit value. The seeds should be 32-bit
    values.
    '''
    _MWC64_MULT = 698769069
    _MWC64_MODULO = _MWC64_MULT * 2**32 - 1
    _MWC64_CYCLE_LEN = _MWC64_MULT * 2**32 // 2 - 1

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        repeat_seed_iter = _repeat_iter(seed_iter)
        self.mwc_upper = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFF)
        self.mwc_lower = _next_seed_int32_or_default(repeat_seed_iter, 0xFFFFFFFF)
        self.sanitise()
        if kwargs.pop('mix_extras', False):
            self.mix(seed_iter)
        for key in kwargs:
            raise TypeError("__init__() got an unexpected keyword argument '%s'" % key)

    def seed(self, *args, **kwargs):
        self.__init__(*args, **kwargs)

    def sanitise(self):
        state64 = (self.mwc_upper << 32) + self.mwc_lower
        temp = state64
        was_changed = False
        # There are a few bad seeds--that is, seeds that are a multiple of
        # 0x29A65EACFFFFFFFF (which is 698769069 * 2**32 - 1).
        if state64 >= 0x29A65EACFFFFFFFF:
            was_changed = True
        temp = state64 % 0x29A65EACFFFFFFFF
        if temp == 0:
            # Invert to get a good seed.
            temp = (state64 ^ 0xFFFFFFFFFFFFFFFF) % 0x29A65EACFFFFFFFF
            was_changed = True
        if was_changed:
            self.mwc_upper = temp >> 32
            self.mwc_lower = temp & 0xFFFFFFFF

    def next(self):
        # Note: this is apparently equivalent to:
        # temp64 = (self.mwc_upper << 32) + self.mwc_lower
        # temp64 = (698769069 * temp64) % 0x29A65EACFFFFFFFF
        # See reference in MWC2.next().
        temp64 = 698769069 * self.mwc_lower + self.mwc_upper
        self.mwc_lower = temp64 & 0xFFFFFFFF
        self.mwc_upper = (temp64 >> 32) & 0xFFFFFFFF
        return self.mwc_lower

    def current(self):
        return self.mwc_lower

    mwc = property(current)

    def mix(self, *args):
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFF
            current = self.current()
            selector = (current >> 31) & 0x1
            if selector == 0:
                self.mwc_upper ^= value_int
            else:
                self.mwc_lower ^= value_int
            self.sanitise()
            self.next()
        return self.current()

    def __iter__(self):
        return self

    def getstate(self):
        return (self.mwc_upper, self.mwc_lower)

    def setstate(self, state):
        (self.mwc_upper, self.mwc_lower) = (int(val) & 0xFFFFFFFF for val in state)
        self.sanitise()

    def jumpahead(self, n):
        # See MWC2.next() note on functional equivalence.
        n = int(n) % self._MWC64_CYCLE_LEN
        temp64 = (self.mwc_upper << 32) + self.mwc_lower
        temp64 = pow(self._MWC64_MULT, n, self._MWC64_MODULO) * temp64 % self._MWC64_MODULO
        self.mwc_lower = temp64 & 0xFFFFFFFF
        self.mwc_upper = (temp64 >> 32) & 0xFFFFFFFF
        return self.mwc_lower


class KISS(object):
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

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        repeat_seed_iter = _repeat_iter(seed_iter)
        self.random_mwc = MWC2(repeat_seed_iter)
        self.random_cong = Cong(repeat_seed_iter)
        self.random_shr3 = SHR3(repeat_seed_iter)
        if kwargs.pop('mix_extras', False):
            self.mix(seed_iter)
        for key in kwargs:
            raise TypeError("__init__() got an unexpected keyword argument '%s'" % key)

    def seed(self, *args, **kwargs):
        self.__init__(*args, **kwargs)

    def next(self):
        mwc_val = self.random_mwc.next()
        cong_val = self.random_cong.next()
        shr3_val = self.random_shr3.next()
        return ((mwc_val ^ cong_val) + shr3_val) & 0xFFFFFFFF

    def current(self):
        return ((self.random_mwc.current() ^ self.random_cong.cong) + self.random_shr3.shr3) & 0xFFFFFFFF

    def mix(self, *args):
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFF
            current = self.current()
            selector = (current >> 30) & 0x3
            if selector == 0:
                self.random_mwc.mwc_upper ^= value_int
                self.random_mwc._sanitise_upper()
                self.random_mwc._next_upper()
            elif selector == 1:
                self.random_mwc.mwc_lower ^= value_int
                self.random_mwc._sanitise_lower()
                self.random_mwc._next_lower()
            elif selector == 2:
                self.random_cong.cong ^= value_int
                # Cong doesn't need any sanitising
                self.random_cong.next()
            else:   # selector == 3
                self.random_shr3.shr3 ^= value_int
                self.random_shr3.sanitise()
                self.random_shr3.next()
        return self.current()

    def __iter__(self):
        return self

    def getstate(self):
        return (self.random_mwc.getstate(), self.random_cong.getstate(), self.random_shr3.getstate())

    def setstate(self, state):
        (mwc_state, cong_state, shr3_state) = state
        self.random_mwc.setstate(mwc_state)
        self.random_cong.setstate(cong_state)
        self.random_shr3.setstate(shr3_state)

    def jumpahead(self, n):
        self.random_mwc.jumpahead(n)
        self.random_cong.jumpahead(n)
        self.random_shr3.jumpahead(n)
        return self.current()

    def _get_mwc_upper(self):
        return self.random_mwc.mwc_upper
    def _set_mwc_upper(self, value):
        self.random_mwc.mwc_upper = value
    mwc_upper = property(_get_mwc_upper, _set_mwc_upper)

    def _get_mwc_lower(self):
        return self.random_mwc.mwc_lower
    def _set_mwc_lower(self, value):
        self.random_mwc.mwc_lower = value
    mwc_lower = property(_get_mwc_lower, _set_mwc_lower)

    def _get_mwc(self):
        return self.random_mwc.current()
    mwc = property(_get_mwc)

    def _get_shr3(self):
        return self.random_shr3.shr3
    def _set_shr3(self, value):
        self.random_shr3.shr3 = value
    shr3 = property(_get_shr3, _set_shr3)

    def _get_cong(self):
        return self.random_cong.cong
    def _set_cong(self, value):
        self.random_cong.cong = value
    cong = property(_get_cong, _set_cong)


class KISS2(object):
    '''"Keep It Simple Stupid" random number generator
    
    It combines the MWC64, Cong, SHR3 generators. Period
    is about 2**123.

    This is a slightly updated KISS generator design, from
    a newsgroup post in 2003:

    http://groups.google.com/group/sci.math/msg/9959175f66dd138f

    The MWC component uses a single 64-bit calculation,
    instead of two 32-bit calculations that are combined.
    '''

    def __init__(self, *args, **kwargs):
        '''Positional arguments are seed values
        Keyword-only arguments:
            mix_extras=False -- If True, then call mix() to 'mix' extra seed
                                values into the state.
        '''
        seed_iter = _traverse_iter(args)
        repeat_seed_iter = _repeat_iter(seed_iter)
        self.random_mwc = MWC64(repeat_seed_iter)
        self.random_cong = Cong(repeat_seed_iter)
        self.random_shr3 = SHR3(repeat_seed_iter)
        if kwargs.pop('mix_extras', False):
            self.mix(seed_iter)
        for key in kwargs:
            raise TypeError("__init__() got an unexpected keyword argument '%s'" % key)

    def seed(self, *args, **kwargs):
        self.__init__(*args, **kwargs)

    def next(self):
        mwc_val = self.random_mwc.next()
        cong_val = self.random_cong.next()
        shr3_val = self.random_shr3.next()
        return (mwc_val + cong_val + shr3_val) & 0xFFFFFFFF

    def current(self):
        return (self.random_mwc.current() + self.random_cong.cong + self.random_shr3.shr3) & 0xFFFFFFFF

    def mix(self, *args):
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFF
            current = self.current()
            selector = (current >> 30) & 0x3
            if selector == 0:
                self.random_mwc.mwc_upper ^= value_int
                self.random_mwc.sanitise()
                self.random_mwc.next()
            elif selector == 1:
                self.random_mwc.mwc_lower ^= value_int
                self.random_mwc.sanitise()
                self.random_mwc.next()
            elif selector == 2:
                self.random_cong.cong ^= value_int
                # Cong doesn't need any sanitising
                self.random_cong.next()
            else:   # selector == 3
                self.random_shr3.shr3 ^= value_int
                self.random_shr3.sanitise()
                self.random_shr3.next()
        return self.current()

    def __iter__(self):
        return self

    def getstate(self):
        return (self.random_mwc.getstate(), self.random_cong.getstate(), self.random_shr3.getstate())

    def setstate(self, state):
        (mwc_state, cong_state, shr3_state) = state
        self.random_mwc.setstate(mwc_state)
        self.random_cong.setstate(cong_state)
        self.random_shr3.setstate(shr3_state)

    def jumpahead(self, n):
        self.random_mwc.jumpahead(n)
        self.random_cong.jumpahead(n)
        self.random_shr3.jumpahead(n)
        return self.current()

    def _get_mwc_upper(self):
        return self.random_mwc.mwc_upper
    def _set_mwc_upper(self, value):
        self.random_mwc.mwc_upper = value
    mwc_upper = property(_get_mwc_upper, _set_mwc_upper)

    def _get_mwc_lower(self):
        return self.random_mwc.mwc_lower
    def _set_mwc_lower(self, value):
        self.random_mwc.mwc_lower = value
    mwc_lower = property(_get_mwc_lower, _set_mwc_lower)

    def _get_mwc(self):
        return self.random_mwc.mwc
    mwc = property(_get_mwc)

    def _get_shr3(self):
        return self.random_shr3.shr3
    def _set_shr3(self, value):
        self.random_shr3.shr3 = value
    shr3 = property(_get_shr3, _set_shr3)

    def _get_cong(self):
        return self.random_cong.cong
    def _set_cong(self, value):
        self.random_cong.cong = value
    cong = property(_get_cong, _set_cong)


def lfsr_next_one_seed(seed_iter, min_value_shift):
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
    try:
        seed = seed_iter.next()
    except StopIteration:
        return 0xFFFFFFFF
    else:
        if seed is None:
            return 0xFFFFFFFF
        else:
            seed = int(seed) & 0xFFFFFFFF
            working_seed = (seed ^ (seed << 16)) & 0xFFFFFFFF

            min_value = 1 << min_value_shift
            if working_seed < min_value:
                working_seed = (seed << 16) & 0xFFFFFFFF
                if working_seed < min_value:
                    working_seed ^= 0xFFFFFFFF
            return working_seed

def lfsr_validate_one_seed(seed, min_value_shift):
    '''Validate seeds for LFSR generators
    
    The LFSR generator components discard a certain number of their lower bits
    when generating each output. The significant bits of their state must not
    all be zero. We must ensure that when seeding the generator.
    
    This is a light-weight validation of state, used from setstate().
    '''
    min_value = 1 << min_value_shift
    if seed < min_value:
        seed ^= 0xFFFFFFFF
    return seed

class LFSR113(object):
    '''Combined LFSR random number generator by L'Ecuyer

    It combines 4 LFSR generators. The generators have been
    chosen for maximal equidistribution.

    The period is approximately 2**113.

    "Tables of Maximally-Equidistributed Combined Lfsr Generators"
    P. L'Ecuyer
    Mathematics of Computation, 68, 225 (1999), 261-269.
    '''
    _LFSR113_1_MATRIX_a = BitColumnMatrix.unity(32) + BitColumnMatrix.shift(32,6)
    _LFSR113_1_MATRIX_b = BitColumnMatrix.shift(32,-13)
    _LFSR113_1_MATRIX_c = BitColumnMatrix.unity(32)
    _LFSR113_1_MATRIX_c.columns[0] = 0
    _LFSR113_1_MATRIX_d = BitColumnMatrix.shift(32,18)
    _LFSR113_1_MATRIX = _LFSR113_1_MATRIX_d * _LFSR113_1_MATRIX_c + _LFSR113_1_MATRIX_b * _LFSR113_1_MATRIX_a
    _LFSR113_1_CYCLE_LEN = 2**(32 - 1) - 1

    _LFSR113_2_MATRIX_a = BitColumnMatrix.unity(32) + BitColumnMatrix.shift(32,2)
    _LFSR113_2_MATRIX_b = BitColumnMatrix.shift(32,-27)
    _LFSR113_2_MATRIX_c = BitColumnMatrix.unity(32)
    _LFSR113_2_MATRIX_c.columns[0:3] = [ 0, 0, 0 ]
    _LFSR113_2_MATRIX_d = BitColumnMatrix.shift(32,2)
    _LFSR113_2_MATRIX = _LFSR113_2_MATRIX_d * _LFSR113_2_MATRIX_c + _LFSR113_2_MATRIX_b * _LFSR113_2_MATRIX_a
    _LFSR113_2_CYCLE_LEN = 2**(32 - 3) - 1

    _LFSR113_3_MATRIX_a = BitColumnMatrix.unity(32) + BitColumnMatrix.shift(32,13)
    _LFSR113_3_MATRIX_b = BitColumnMatrix.shift(32,-21)
    _LFSR113_3_MATRIX_c = BitColumnMatrix.unity(32)
    _LFSR113_3_MATRIX_c.columns[0:4] = [ 0, 0, 0, 0 ]
    _LFSR113_3_MATRIX_d = BitColumnMatrix.shift(32,7)
    _LFSR113_3_MATRIX = _LFSR113_3_MATRIX_d * _LFSR113_3_MATRIX_c + _LFSR113_3_MATRIX_b * _LFSR113_3_MATRIX_a
    _LFSR113_3_CYCLE_LEN = 2**(32 - 4) - 1

    _LFSR113_4_MATRIX_a = BitColumnMatrix.unity(32) + BitColumnMatrix.shift(32,3)
    _LFSR113_4_MATRIX_b = BitColumnMatrix.shift(32,-12)
    _LFSR113_4_MATRIX_c = BitColumnMatrix.unity(32)
    _LFSR113_4_MATRIX_c.columns[0:7] = [ 0, 0, 0, 0, 0, 0, 0 ]
    _LFSR113_4_MATRIX_d = BitColumnMatrix.shift(32,13)
    _LFSR113_4_MATRIX = _LFSR113_4_MATRIX_d * _LFSR113_4_MATRIX_c + _LFSR113_4_MATRIX_b * _LFSR113_4_MATRIX_a
    _LFSR113_4_CYCLE_LEN = 2**(32 - 7) - 1

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
        b       = (((self.z1 & 0x03FFFFFF) << 6) ^ self.z1) >> 13
        self.z1 = ((self.z1 & 0x00003FFE) << 18) ^ b
    def _next_z2(self):
        b       = (((self.z2 & 0x3FFFFFFF) << 2) ^ self.z2) >> 27
        self.z2 = ((self.z2 & 0x3FFFFFF8) << 2) ^ b
    def _next_z3(self):
        b       = (((self.z3 & 0x0007FFFF) << 13) ^ self.z3) >> 21
        self.z3 = ((self.z3 & 0x01FFFFF0) << 7) ^ b
    def _next_z4(self):
        b       = (((self.z4 & 0x1FFFFFFF) << 3) ^ self.z4) >> 12
        self.z4 = ((self.z4 & 0x0007FF80) << 13) ^ b

    def next(self):
        b       = (((self.z1 & 0x03FFFFFF) << 6) ^ self.z1) >> 13
        self.z1 = ((self.z1 & 0x00003FFE) << 18) ^ b

        b       = (((self.z2 & 0x3FFFFFFF) << 2) ^ self.z2) >> 27
        self.z2 = ((self.z2 & 0x3FFFFFF8) << 2) ^ b

        b       = (((self.z3 & 0x0007FFFF) << 13) ^ self.z3) >> 21
        self.z3 = ((self.z3 & 0x01FFFFF0) << 7) ^ b

        b       = (((self.z4 & 0x1FFFFFFF) << 3) ^ self.z4) >> 12
        self.z4 = ((self.z4 & 0x0007FF80) << 13) ^ b

        return self.z1 ^ self.z2 ^ self.z3 ^ self.z4

    def current(self):
        return self.z1 ^ self.z2 ^ self.z3 ^ self.z4

    def mix(self, *args):
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFF
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
        return (self.z1, self.z2, self.z3, self.z4)

    def setstate(self, state):
        (self.z1, self.z2, self.z3, self.z4) = (int(val) & 0xFFFFFFFF for val in state)
        self.sanitise()

    def jumpahead(self, n):
        n_1 = int(n) % self._LFSR113_1_CYCLE_LEN
        n_2 = int(n) % self._LFSR113_2_CYCLE_LEN
        n_3 = int(n) % self._LFSR113_3_CYCLE_LEN
        n_4 = int(n) % self._LFSR113_4_CYCLE_LEN

        z1 = pow(self._LFSR113_1_MATRIX, n_1) * self.z1
        self.z1 = z1
        z2 = pow(self._LFSR113_2_MATRIX, n_2) * self.z2
        self.z2 = z2
        z3 = pow(self._LFSR113_3_MATRIX, n_3) * self.z3
        self.z3 = z3
        z4 = pow(self._LFSR113_4_MATRIX, n_4) * self.z4
        self.z4 = z4

        return self.current()


class LFSR88(object):
    '''Combined LFSR random number generator by L'Ecuyer

    It combines 3 LFSR generators. The generators have been
    chosen for maximal equidistribution.

    The period is approximately 2**88.

    "Maximally Equidistributed Combined Tausworthe Generators"
    P. L'Ecuyer
    Mathematics of Computation, 65, 213 (1996), 203-213. 
    '''
    _LFSR88_1_MATRIX_a = BitColumnMatrix.unity(32) + BitColumnMatrix.shift(32,13)
    _LFSR88_1_MATRIX_b = BitColumnMatrix.shift(32,-19)
    _LFSR88_1_MATRIX_c = BitColumnMatrix.unity(32)
    _LFSR88_1_MATRIX_c.columns[0] = 0
    _LFSR88_1_MATRIX_d = BitColumnMatrix.shift(32,12)
    _LFSR88_1_MATRIX = _LFSR88_1_MATRIX_d * _LFSR88_1_MATRIX_c + _LFSR88_1_MATRIX_b * _LFSR88_1_MATRIX_a
    _LFSR88_1_CYCLE_LEN = 2**(32 - 1) - 1

    _LFSR88_2_MATRIX_a = BitColumnMatrix.unity(32) + BitColumnMatrix.shift(32,2)
    _LFSR88_2_MATRIX_b = BitColumnMatrix.shift(32,-25)
    _LFSR88_2_MATRIX_c = BitColumnMatrix.unity(32)
    _LFSR88_2_MATRIX_c.columns[0:3] = [ 0, 0, 0 ]
    _LFSR88_2_MATRIX_d = BitColumnMatrix.shift(32,4)
    _LFSR88_2_MATRIX = _LFSR88_2_MATRIX_d * _LFSR88_2_MATRIX_c + _LFSR88_2_MATRIX_b * _LFSR88_2_MATRIX_a
    _LFSR88_2_CYCLE_LEN = 2**(32 - 3) - 1

    _LFSR88_3_MATRIX_a = BitColumnMatrix.unity(32) + BitColumnMatrix.shift(32,3)
    _LFSR88_3_MATRIX_b = BitColumnMatrix.shift(32,-11)
    _LFSR88_3_MATRIX_c = BitColumnMatrix.unity(32)
    _LFSR88_3_MATRIX_c.columns[0:4] = [ 0, 0, 0, 0 ]
    _LFSR88_3_MATRIX_d = BitColumnMatrix.shift(32,17)
    _LFSR88_3_MATRIX = _LFSR88_3_MATRIX_d * _LFSR88_3_MATRIX_c + _LFSR88_3_MATRIX_b * _LFSR88_3_MATRIX_a
    _LFSR88_3_CYCLE_LEN = 2**(32 - 4) - 1

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
        b       = (((self.z1 & 0x0007FFFF) << 13) ^ self.z1) >> 19
        self.z1 = ((self.z1 & 0x000FFFFE) << 12) ^ b
    def _next_z2(self):
        b       = (((self.z2 & 0x3FFFFFFF) << 2) ^ self.z2) >> 25
        self.z2 = ((self.z2 & 0x0FFFFFF8) << 4) ^ b
    def _next_z3(self):
        b       = (((self.z3 & 0x1FFFFFFF) << 3) ^ self.z3) >> 11
        self.z3 = ((self.z3 & 0x00007FF0) << 17) ^ b

    def next(self):
        b       = (((self.z1 & 0x0007FFFF) << 13) ^ self.z1) >> 19
        self.z1 = ((self.z1 & 0x000FFFFE) << 12) ^ b

        b       = (((self.z2 & 0x3FFFFFFF) << 2) ^ self.z2) >> 25
        self.z2 = ((self.z2 & 0x0FFFFFF8) << 4) ^ b

        b       = (((self.z3 & 0x1FFFFFFF) << 3) ^ self.z3) >> 11
        self.z3 = ((self.z3 & 0x00007FF0) << 17) ^ b

        return self.z1 ^ self.z2 ^ self.z3

    def current(self):
        return self.z1 ^ self.z2 ^ self.z3

    def mix(self, *args):
        for value in _traverse_iter(args):
            value_int = int(value) & 0xFFFFFFFF
            current = self.current()
            if current < 1431655765:        # constant is 2^32 / 3
                self.z1 = lfsr_validate_one_seed(self.z1 ^ value_int, 1)
                self._next_z1()
            elif current < 2863311531:      # constant is 2^32 * 2 / 3
                self.z2 = lfsr_validate_one_seed(self.z2 ^ value_int, 3)
                self._next_z2()
            else:
                self.z3 = lfsr_validate_one_seed(self.z3 ^ value_int, 4)
                self._next_z3()
        return self.current()

    def __iter__(self):
        return self

    def getstate(self):
        return (self.z1, self.z2, self.z3)

    def setstate(self, state):
        (self.z1, self.z2, self.z3) = (int(val) & 0xFFFFFFFF for val in state)
        self.sanitise()

    def jumpahead(self, n):
        n_1 = int(n) % self._LFSR88_1_CYCLE_LEN
        n_2 = int(n) % self._LFSR88_2_CYCLE_LEN
        n_3 = int(n) % self._LFSR88_3_CYCLE_LEN

        z1 = pow(self._LFSR88_1_MATRIX, n_1) * self.z1
        self.z1 = z1
        z2 = pow(self._LFSR88_2_MATRIX, n_2) * self.z2
        self.z2 = z2
        z3 = pow(self._LFSR88_3_MATRIX, n_3) * self.z3
        self.z3 = z3

        return self.current()
