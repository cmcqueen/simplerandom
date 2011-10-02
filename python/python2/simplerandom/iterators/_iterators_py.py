
def _init_default_and_int32(seed, default_value):
    if seed==None:
        return default_value
    else:
        # Ensure a 32-bit unsigned integer.
        return (int(seed) & 0xFFFFFFFF)


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

    def __init__(self, seed = None):
        self.cong = _init_default_and_int32(seed, 0)

    def seed(self, seed = None):
        self.__init__(seed)

    def next(self):
        self.cong = (69069 * self.cong + 12345) & 0xFFFFFFFF
        return self.cong

    def __iter__(self):
        return self

    def getstate(self):
        return (self.cong, )

    def setstate(self, state):
        (self.cong, ) = (int(val) & 0xFFFFFFFF for val in state)


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

    def __init__(self, seed = None):
        self.shr3 = _init_default_and_int32(seed, 0xFFFFFFFF)
        self._validate_seed()

    def seed(self, seed = None):
        self.__init__(seed)

    def _validate_seed(self):
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

    def __iter__(self):
        return self

    def getstate(self):
        return (self.shr3, )

    def setstate(self, state):
        (self.shr3, ) = (int(val) & 0xFFFFFFFF for val in state)
        self._validate_seed()


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

    def __init__(self, seed_upper = None, seed_lower = None):
        self.mwc_upper = _init_default_and_int32(seed_upper, 0xFFFFFFFF)
        self.mwc_lower = _init_default_and_int32(seed_lower, 0xFFFFFFFF)
        self._validate_seed()

    def seed(self, seed_upper = None, seed_lower = None):
        self.__init__(seed_upper, seed_lower)

    def _validate_seed(self):
        # There are a few bad seeds--that is, seeds that are a multiple of
        # 0x9068FFFF (which is 36969 * 2**16 - 1).
        if (self.mwc_upper % 0x9068ffff)==0:
            # Invert to get a good seed.
            self.mwc_upper ^= 0xFFFFFFFF
        # There are a few bad seeds--that is, seeds that are a multiple of
        # 0x464FFFFF (which is 18000 * 2**16 - 1).
        if (self.mwc_lower % 0x464FFFFF)==0:
            # Invert to get a good seed.
            self.mwc_lower ^= 0xFFFFFFFF

    def next(self):
        self.mwc_upper = 36969 * (self.mwc_upper & 0xFFFF) + (self.mwc_upper >> 16)
        self.mwc_lower = 18000 * (self.mwc_lower & 0xFFFF) + (self.mwc_lower >> 16)
        return self._get_mwc()

    def _get_mwc(self):
        return (((self.mwc_upper & 0xFFFF) << 16) + (self.mwc_upper >> 16) + self.mwc_lower) & 0xFFFFFFFF

    mwc = property(_get_mwc)

    def __iter__(self):
        return self

    def getstate(self):
        return (self.mwc_upper, self.mwc_lower)

    def setstate(self, state):
        (self.mwc_upper, self.mwc_lower) = (int(val) & 0xFFFFFFFF for val in state)
        self._validate_seed()


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

    def _get_mwc(self):
        return (((self.mwc_upper & 0xFFFF) << 16) + self.mwc_lower) & 0xFFFFFFFF


class MWC64(object):
    '''"Multiply-with-carry" random number generator

    This uses a single MWC generator with 64 bits to
    generate a 32-bit value. The seeds should be 32-bit
    values.
    '''

    def __init__(self, seed_upper = None, seed_lower = None):
        self.mwc_upper = _init_default_and_int32(seed_upper, 0xFFFFFFFF)
        self.mwc_lower = _init_default_and_int32(seed_lower, 0xFFFFFFFF)
        self._validate_seed()

    def seed(self, seed_upper = None, seed_lower = None):
        self.__init__(seed_upper, seed_lower)

    def _validate_seed(self):
        seed64 = (self.mwc_upper << 32) + self.mwc_lower
        # There are a few bad seeds--that is, seeds that are a multiple of
        # 0x29A65EACFFFFFFFF (which is 698769069 * 2**32 - 1).
        if seed64 % 0x29A65EACFFFFFFFF == 0:
            # Invert to get a good seed.
            self.mwc_upper ^= 0xFFFFFFFF
            self.mwc_lower ^= 0xFFFFFFFF

    def next(self):
        temp64 = 698769069 * self.mwc_lower + self.mwc_upper
        self.mwc_lower = temp64 & 0xFFFFFFFF
        self.mwc_upper = (temp64 >> 32) & 0xFFFFFFFF
        return self.mwc_lower

    def mwc(self):
        return self.mwc_lower

    mwc = property(mwc)

    def __iter__(self):
        return self

    def getstate(self):
        return (self.mwc_upper, self.mwc_lower)

    def setstate(self, state):
        (self.mwc_upper, self.mwc_lower) = (int(val) & 0xFFFFFFFF for val in state)
        self._validate_seed()


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

    def __init__(self, seed_mwc_upper = None, seed_mwc_lower = None, seed_cong = None, seed_shr3 = None):
        self.random_mwc = MWC2(seed_mwc_upper, seed_mwc_lower)
        self.random_cong = Cong(seed_cong)
        self.random_shr3 = SHR3(seed_shr3)

    def seed(self, seed_mwc_upper = None, seed_mwc_lower = None, seed_cong = None, seed_shr3 = None):
        self.__init__(seed_mwc_upper, seed_mwc_lower, seed_cong, seed_shr3)

    def next(self):
        mwc_val = self.random_mwc.next()
        cong_val = self.random_cong.next()
        shr3_val = self.random_shr3.next()
        return ((mwc_val ^ cong_val) + shr3_val) & 0xFFFFFFFF

    def __iter__(self):
        return self

    def getstate(self):
        return (self.random_mwc.getstate(), self.random_cong.getstate(), self.random_shr3.getstate())

    def setstate(self, state):
        (mwc_state, cong_state, shr3_state) = state
        self.random_mwc.setstate(mwc_state)
        self.random_cong.setstate(cong_state)
        self.random_shr3.setstate(shr3_state)

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

    def __init__(self, seed_mwc_upper = None, seed_mwc_lower = None, seed_cong = None, seed_shr3 = None):
        self.random_mwc = MWC64(seed_mwc_upper, seed_mwc_lower)
        self.random_cong = Cong(seed_cong)
        self.random_shr3 = SHR3(seed_shr3)

    def seed(self, seed_mwc_upper = None, seed_mwc_lower = None, seed_cong = None, seed_shr3 = None):
        self.__init__(seed_mwc_upper, seed_mwc_lower, seed_cong, seed_shr3)

    def next(self):
        mwc_val = self.random_mwc.next()
        cong_val = self.random_cong.next()
        shr3_val = self.random_shr3.next()
        return (mwc_val + cong_val + shr3_val) & 0xFFFFFFFF

    def __iter__(self):
        return self

    def getstate(self):
        return (self.random_mwc.getstate(), self.random_cong.getstate(), self.random_shr3.getstate())

    def setstate(self, state):
        (mwc_state, cong_state, shr3_state) = state
        self.random_mwc.setstate(mwc_state)
        self.random_cong.setstate(cong_state)
        self.random_shr3.setstate(shr3_state)

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


def lfsr_init_one_seed(seed, min_value_shift):
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
    if seed==None:
        working_seed = 0xFFFFFFFF
    else:
        seed = int(seed) & 0xFFFFFFFF

        working_seed = (seed ^ (seed << 16)) & 0xFFFFFFFF

        min_value = 1 << min_value_shift
        if working_seed < min_value:
            working_seed = (seed << 24) & 0xFFFFFFFF
            if working_seed < min_value:
                working_seed ^= 0xFFFFFFFF
    return working_seed

def lfsr_validate_one_seed(seed, min_value_shift):
    '''Validate seeds for LFSR generators
    
    The LFSR generator components discard a certain number of their lower bits
    when generating each output. The significant bits of their state must not
    all be zero. We must ensure that when seeding the generator.
    
    This is a light-weight validation of seeds, used from setstate().
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

    def __init__(self, seed_z1 = None, seed_z2 = None, seed_z3 = None, seed_z4 = None):
        self.z1 = lfsr_init_one_seed(seed_z1, 1)
        self.z2 = lfsr_init_one_seed(seed_z2, 3)
        self.z3 = lfsr_init_one_seed(seed_z3, 4)
        self.z4 = lfsr_init_one_seed(seed_z4, 7)

    def seed(self, seed_z1 = None, seed_z2 = None, seed_z3 = None, seed_z4 = None):
        self.__init__(seed_z1, seed_z2, seed_z3, seed_z4)

    def _validate_seed(self):
        self.z1 = lfsr_validate_one_seed(self.z1, 1)
        self.z2 = lfsr_validate_one_seed(self.z2, 3)
        self.z3 = lfsr_validate_one_seed(self.z3, 4)
        self.z4 = lfsr_validate_one_seed(self.z4, 7)

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

    def __iter__(self):
        return self

    def getstate(self):
        return (self.z1, self.z2, self.z3, self.z4)

    def setstate(self, state):
        (self.z1, self.z2, self.z3, self.z4) = (int(val) & 0xFFFFFFFF for val in state)
        self._validate_seed()


class LFSR88(object):
    '''Combined LFSR random number generator by L'Ecuyer

    It combines 3 LFSR generators. The generators have been
    chosen for maximal equidistribution.

    The period is approximately 2**88.

    "Maximally Equidistributed Combined Tausworthe Generators"
    P. L'Ecuyer
    Mathematics of Computation, 65, 213 (1996), 203-213. 
    '''

    def __init__(self, seed_z1 = None, seed_z2 = None, seed_z3 = None):
        self.z1 = lfsr_init_one_seed(seed_z1, 1)
        self.z2 = lfsr_init_one_seed(seed_z2, 3)
        self.z3 = lfsr_init_one_seed(seed_z3, 4)

    def seed(self, seed_z1 = None, seed_z2 = None, seed_z3 = None):
        self.__init__(seed_z1, seed_z2, seed_z3)

    def _validate_seed(self):
        self.z1 = lfsr_validate_one_seed(self.z1, 1)
        self.z2 = lfsr_validate_one_seed(self.z2, 3)
        self.z3 = lfsr_validate_one_seed(self.z3, 4)

    def next(self):
        b       = (((self.z1 & 0x0007FFFF) << 13) ^ self.z1) >> 19
        self.z1 = ((self.z1 & 0x000FFFFE) << 12) ^ b

        b       = (((self.z2 & 0x3FFFFFFF) << 2) ^ self.z2) >> 25
        self.z2 = ((self.z2 & 0x0FFFFFF8) << 4) ^ b

        b       = (((self.z3 & 0x1FFFFFFF) << 3) ^ self.z3) >> 11
        self.z3 = ((self.z3 & 0x00007FF0) << 17) ^ b

        return self.z1 ^ self.z2 ^ self.z3

    def __iter__(self):
        return self

    def getstate(self):
        return (self.z1, self.z2, self.z3)

    def setstate(self, state):
        (self.z1, self.z2, self.z3) = (int(val) & 0xFFFFFFFF for val in state)
        self._validate_seed()


