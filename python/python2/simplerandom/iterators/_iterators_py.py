
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
        if seed==None:
            seed = 123456789
        self.cong = int(seed) & 0xFFFFFFFF

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
        if seed==None or seed==0:
            seed = 362436000
        self.shr3 = int(seed) & 0xFFFFFFFF

    def seed(self, seed = None):
        self.__init__(seed)

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
        if seed_upper==None or (seed_upper % 0x9068ffff)==0:
            # Default seed, and avoid bad seeds
            # There are a few bad seeds--that is, seeds that are a multiple of
            # 0x9068FFFF (which is 36969 * 2**16 - 1).
            seed_upper = 12345
        if seed_lower==None or (seed_lower % 0x464FFFFF)==0:
            # Default seed, and avoid bad seeds
            # There are a few bad seeds--that is, seeds that are a multiple of
            # 0x464FFFFF (which is 18000 * 2**16 - 1).
            seed_lower = 65437
        self.mwc_upper = int(seed_upper) & 0xFFFFFFFF
        self.mwc_lower = int(seed_lower) & 0xFFFFFFFF

    def seed(self, seed_upper = None, seed_lower = None):
        self.__init__(seed_upper, seed_lower)

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
        if seed_upper==None:
            seed_upper = 7654321
        else:
            seed_upper = int(seed_upper) & 0xFFFFFFFF

        if seed_lower==None:
            seed_lower = 521288629
        else:
            seed_lower = int(seed_lower) & 0xFFFFFFFF

        # There are a few bad seeds--that is, seeds that are a multiple of
        # 0x29A65EACFFFFFFFF (which is 698769069 * 2**32 - 1).
        seed64 = (seed_upper << 32) + seed_lower
        if seed64 % 0x29A65EACFFFFFFFF == 0:
            seed_upper = 7654321
            seed_lower = 521288629

        self.mwc_upper = seed_upper
        self.mwc_lower = seed_lower

    def seed(self, seed_upper = None, seed_lower = None):
        self.__init__(seed_upper, seed_lower)

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


class LFIB4(object):
    '''"Lagged Fibonacci 4-lag" random number generator

    LFIB4 is an extension of what Marsaglia has previously
    defined as a lagged Fibonacci generator:

        x[n]=x[n-r] op x[n-s]

    with the x's in a finite set over which there is a
    binary operation op, such as +,- on integers mod 2**32,
    * on odd such integers, exclusive-or(xor) on binary
    vectors. Except for those using multiplication, lagged
    Fibonacci generators fail various tests of randomness,
    unless the lags are very long. (See SWB).

    To see if more than two lags would serve to overcome
    the problems of 2-lag generators using +,- or xor,
    Marsaglia developed the 4-lag generator LFIB4 using
    addition:

        x[n]=x[n-256]+x[n-179]+x[n-119]+x[n-55] mod 2**32

    Its period is 2**31*(2**256-1), about 2**287,
    and it seems to pass all tests---in particular,
    those of the kind for which 2-lag generators using
    +,-,xor seem to fail.

    For even more confidence in its suitability, LFIB4
    can be combined with KISS, with a resulting period
    of about 2**410.
    '''

    def __init__(self, seed = None):
        if not seed:
            random_kiss = KISS(12345, 65435, 12345, 34221)
            self.t = [ random_kiss.next() for i in range(256) ]
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


class SWB(LFIB4):
    '''"Subtract-With-Borrow" random number generator
    
    SWB is a subtract-with-borrow generator that Marsaglia
    developed to give a simple method for producing
    extremely long periods:
    x[n]=x[n-222]-x[n-237]- borrow mod 2**32.
    The 'borrow' is 0, or set to 1 if computing x[n-1]
    caused overflow in 32-bit integer arithmetic. This
    generator has a very long period, 2**7098(2**480-1),
    about 2**7578.

    It seems to pass all tests of randomness, except
    for the Birthday Spacings test, which it fails
    badly, as do all lagged Fibonacci generators using
    +,- or xor. Marsaglia suggests combining SWB with
    KISS, MWC, SHR3, or Cong. KISS+SWB has period
    >2**7700 and is highly recommended.

    Subtract-with-borrow has the same local behaviour
    as lagged Fibonacci using +,-,xor---the borrow
    merely provides a much longer period.

    SWB fails the birthday spacings test, as do all
    lagged Fibonacci and other generators that merely
    combine two previous values by means of =,- or xor.
    Those failures are for a particular case: m=512
    birthdays in a year of n=2**24 days. There are
    choices of m and n for which lags >1000 will also
    fail the test. A reasonable precaution is to always
    combine a 2-lag Fibonacci or SWB generator with
    another kind of generator, unless the generator uses
    *, for which a very satisfactory sequence of odd
    32-bit integers results.
    '''

    def __init__(self, seed = None):
        LFIB4.__init__(self, seed)
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

        if (x < y):
            self.borrow = 1
        else:
            self.borrow = 0

        return new_value

    def getstate(self):
        t_tuple = LFIB4.getstate(self)
        return (t_tuple, self.borrow)

    def setstate(self, state):
        (t_tuple, borrow) = state
        LFIB4.setstate(self, t_tuple)
        if borrow:
            self.borrow = 1
        else:
            self.borrow = 0


class LFSR113(object):
    '''Combined LFSR random number generator by L'Ecuyer

    It combines 4 LFSR generators. The generators have been
    chosen for maximal equidistribution.

    The period is approximately 2**113.

    "Tables of Maximally-Equidistributed Combined Lfsr Generators"
    P. L'Ecuyer
    Mathematics of Computation, 68, 225 (1999), 261–269.
    '''

    def __init__(self, seed_z1 = None, seed_z2 = None, seed_z3 = None, seed_z4 = None):
        def process_seed(seed, min_value):
            if seed==None:
                seed = 12345
            else:
                seed = int(seed) & 0xFFFFFFFF
                if seed < min_value:
                    seed += min_value
            return seed
        self.z1 = process_seed(seed_z1, 2)
        self.z2 = process_seed(seed_z2, 8)
        self.z3 = process_seed(seed_z3, 16)
        self.z4 = process_seed(seed_z4, 128)

    def seed(self, seed_z1 = None, seed_z2 = None, seed_z3 = None, seed_z4 = None):
        self.__init__(seed_z1, seed_z2, seed_z3, seed_z4)

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


class LFSR88(object):
    '''Combined LFSR random number generator by L'Ecuyer

    It combines 3 LFSR generators. The generators have been
    chosen for maximal equidistribution.

    The period is approximately 2**88.

    "Maximally Equidistributed Combined Tausworthe Generators"
    P. L'Ecuyer
    Mathematics of Computation, 65, 213 (1996), 203–213. 
    '''

    def __init__(self, seed_z1 = None, seed_z2 = None, seed_z3 = None):
        def process_seed(seed, min_value):
            if seed==None:
                seed = 12345
            else:
                seed = int(seed) & 0xFFFFFFFF
                if seed < min_value:
                    seed += min_value
            return seed
        self.z1 = process_seed(seed_z1, 2)
        self.z2 = process_seed(seed_z2, 8)
        self.z3 = process_seed(seed_z3, 16)

    def seed(self, seed_z1 = None, seed_z2 = None, seed_z3 = None):
        self.__init__(seed_z1, seed_z2, seed_z3)

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


