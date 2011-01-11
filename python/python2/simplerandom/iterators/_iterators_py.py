
class _RandomIteratorTemplate(object):
    '''Template class for random number generator'''

    def __init__(self, seed = None):
        raise NotImplemented

    def next(self):
        raise NotImplemented
        return 0

    def __iter__(self):
        return self


class RandomCongIterator(object):
    '''Congruential random number generator'''

    def __init__(self, seed = None):
        if seed==None:
            seed = 12344
        self.cong = int(seed) & 0xFFFFFFFF

    def seed(self, seed = None):
        self.__init__(seed)

    def next(self):
        self.cong = (69069 * self.cong + 1234567) & 0xFFFFFFFF
        return self.cong

    def __iter__(self):
        return self

    def getstate(self):
        return (self.cong, )

    def setstate(self, state):
        (self.cong, ) = (int(val) & 0xFFFFFFFF for val in state)


class RandomCong2Iterator(object):
    '''Congruential random number generator
    
    Very similar to RandomCongIterator, but with different added constant
    and different default seed.
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


class RandomSHR3Iterator(object):
    '''3-shift-register random number generator'''

    def __init__(self, seed = None):
        if seed==None or seed==0:
            seed = 34223
        self.shr3_j = int(seed) & 0xFFFFFFFF

    def seed(self, seed = None):
        self.__init__(seed)

    def next(self):
        shr3_j = self.shr3_j
        shr3_j ^= (shr3_j & 0x7FFF) << 17
        shr3_j ^= shr3_j >> 13
        shr3_j ^= (shr3_j & 0x7FFFFFF) << 5
        self.shr3_j = shr3_j
        return shr3_j

    def __iter__(self):
        return self

    def getstate(self):
        return (self.shr3_j, )

    def setstate(self, state):
        (self.shr3_j, ) = (int(val) & 0xFFFFFFFF for val in state)


class RandomSHR3_2Iterator(object):
    '''3-shift-register random number generator
    
    This differs from the SHR3 generator in the default seed value, and
    the values of the three shift operations.
    '''

    def __init__(self, seed = None):
        if seed==None or seed==0:
            seed = 362436000
        self.shr3_j = int(seed) & 0xFFFFFFFF

    def seed(self, seed = None):
        self.__init__(seed)

    def next(self):
        shr3_j = self.shr3_j
        shr3_j ^= (shr3_j & 0x7FFFF) << 13
        shr3_j ^= shr3_j >> 17
        shr3_j ^= (shr3_j & 0x7FFFFFF) << 5
        self.shr3_j = shr3_j
        return shr3_j

    def __iter__(self):
        return self

    def getstate(self):
        return (self.shr3_j, )

    def setstate(self, state):
        (self.shr3_j, ) = (int(val) & 0xFFFFFFFF for val in state)


class RandomMWCIterator(object):
    '''"Multiply-with-carry" random number generator

    This uses two MWC generators to generate high and low 16-bit parts,
    which are then combined to make a 32-bit value.
    '''

    def __init__(self, seed_z = None, seed_w = None):
        if seed_z==None:
            seed_z = 12344
        if seed_w==None:
            seed_w = 65437
        self.mwc_z = int(seed_z) & 0xFFFFFFFF
        self.mwc_w = int(seed_w) & 0xFFFFFFFF

    def seed(self, seed_z = None, seed_w = None):
        self.__init__(seed_z, seed_w)

    def next(self):
        self.mwc_z = 36969 * (self.mwc_z & 0xFFFF) + (self.mwc_z >> 16)
        self.mwc_w = 18000 * (self.mwc_w & 0xFFFF) + (self.mwc_w >> 16)
        return self._get_mwc()

    def _get_mwc(self):
        return (((self.mwc_z & 0xFFFF) << 16) + self.mwc_w) & 0xFFFFFFFF

    mwc = property(_get_mwc)

    def __iter__(self):
        return self

    def getstate(self):
        return (self.mwc_z, self.mwc_w)

    def setstate(self, state):
        (self.mwc_z, self.mwc_w) = (int(val) & 0xFFFFFFFF for val in state)


class RandomMWC64Iterator(object):
    '''"Multiply-with-carry" random number generator

    This uses a single MWC generator with 64 bits to generate a 32-bit value.
    The seed should be a 64-bit value.
    '''

    def __init__(self, seed = None):
        if seed==None:
            seed = 32875058889374645
        self.mwc_z = int(seed) & 0xFFFFFFFFFFFFFFFF

    def seed(self, seed = None):
        self.__init__(seed)

    def next(self):
        self.mwc_z = 698769069 * (self.mwc_z & 0xFFFFFFFF) + (self.mwc_z >> 32)
        return self._get_mwc()

    def _get_mwc(self):
        return self.mwc_z & 0xFFFFFFFF

    mwc = property(_get_mwc)

    def __iter__(self):
        return self

    def getstate(self):
        return (self.mwc_z, )

    def setstate(self, state):
        (self.mwc_z, ) = (int(val) & 0xFFFFFFFFFFFFFFFF for val in state)


class RandomKISSIterator(object):
    '''"Keep It Simple Stupid" random number generator
    
    It combines the RandomMWC, RandomCong, RandomSHR3
    generators. Period is about 2**123.
    '''

    def __init__(self, seed_mwc_z = None, seed_mwc_w = None, seed_cong = None, seed_shr3 = None):
        self.random_mwc = RandomMWCIterator(seed_mwc_z, seed_mwc_w)
        self.random_cong = RandomCongIterator(seed_cong)
        self.random_shr3 = RandomSHR3Iterator(seed_shr3)

    def seed(self, seed_mwc_z = None, seed_mwc_w = None, seed_cong = None, seed_shr3 = None):
        self.__init__(seed_mwc_z, seed_mwc_w, seed_cong, seed_shr3)

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

    def _get_mwc_z(self):
        return self.random_mwc.mwc_z
    def _set_mwc_z(self, value):
        self.random_mwc.mwc_z = value
    mwc_z = property(_get_mwc_z, _set_mwc_z)

    def _get_mwc_w(self):
        return self.random_mwc.mwc_w
    def _set_mwc_w(self, value):
        self.random_mwc.mwc_w = value
    mwc_w = property(_get_mwc_w, _set_mwc_w)

    def _get_mwc(self):
        return self.random_mwc.mwc
    mwc = property(_get_mwc)

    def _get_shr3_j(self):
        return self.random_shr3.shr3_j
    def _set_shr3_j(self, value):
        self.random_shr3.shr3_j = value
    shr3_j = property(_get_shr3_j, _set_shr3_j)

    def _get_cong(self):
        return self.random_cong.cong
    def _set_cong(self, value):
        self.random_cong.cong = value
    cong = property(_get_cong, _set_cong)


class RandomKISS2Iterator(object):
    '''"Keep It Simple Stupid" random number generator
    
    It combines the RandomMWC64, RandomCong2, RandomSHR3_2
    generators. Period is about 2**123.

    This is a slightly updated KISS generator design, from a newsgroup
    post in 2003:

    http://groups.google.com/group/comp.soft-sys.math.mathematica/msg/95a94c3b2aa5f077

    The Cong and SHR3 component generators are changed slightly. The
    MWC component uses a single 64-bit calculation, instead of two
    32-bit calculations that are combined.
    '''

    def __init__(self, seed_mwc = None, seed_cong = None, seed_shr3 = None):
        self.random_mwc = RandomMWC64Iterator(seed_mwc)
        self.random_cong = RandomCong2Iterator(seed_cong)
        self.random_shr3 = RandomSHR3_2Iterator(seed_shr3)

    def seed(self, seed_mwc = None, seed_cong = None, seed_shr3 = None):
        self.__init__(seed_mwc, seed_cong, seed_shr3)

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

    def _get_mwc_z(self):
        return self.random_mwc.mwc_z
    def _set_mwc_z(self, value):
        self.random_mwc.mwc_z = value
    mwc_z = property(_get_mwc_z, _set_mwc_z)

    def _get_mwc_w(self):
        return self.random_mwc.mwc_w
    def _set_mwc_w(self, value):
        self.random_mwc.mwc_w = value
    mwc_w = property(_get_mwc_w, _set_mwc_w)

    def _get_mwc(self):
        return self.random_mwc.mwc
    mwc = property(_get_mwc)

    def _get_shr3_j(self):
        return self.random_shr3.shr3_j
    def _set_shr3_j(self, value):
        self.random_shr3.shr3_j = value
    shr3_j = property(_get_shr3_j, _set_shr3_j)

    def _get_cong(self):
        return self.random_cong.cong
    def _set_cong(self, value):
        self.random_cong.cong = value
    cong = property(_get_cong, _set_cong)


class RandomLFIB4Iterator(object):
    '''"Lagged Fibonacci 4-lag" random number generator'''

    def __init__(self, seed = None):
        if not seed:
            random_kiss = RandomKISSIterator(12345, 65435, 12345, 34221)
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


class RandomSWBIterator(RandomLFIB4Iterator):
    '''"Subtract-With-Borrow" random number generator
    
    This is a Fibonacci 2-lag generator with an extra "borrow" operation.
    '''

    def __init__(self, seed = None):
        RandomLFIB4Iterator.__init__(self, seed)
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
        t_tuple = RandomLFIB4Iterator.getstate(self)
        return (t_tuple, self.borrow)

    def setstate(self, state):
        (t_tuple, borrow) = state
        RandomLFIB4Iterator.setstate(self, t_tuple)
        if borrow:
            self.borrow = 1
        else:
            self.borrow = 0


class _RandomFibIterator(object):
    '''Classical Fibonacci sequence

    x(n)=x(n-1)+x(n-2),but taken modulo 2^32. Its period is 3 * (2**31) if one
    of its two seeds is odd and not 1 mod 8.
    
    It has little worth as a RNG by itself, but provides a simple and fast
    component for use in combination generators.
    '''

    def __init__(self, seed_a = None, seed_b = None):
        if seed_a==None:
            seed_a = 1468761293
        if seed_b==None:
            seed_b = 3460192787
        self.a = int(seed_a) & 0xFFFFFFFF
        self.b = int(seed_b) & 0xFFFFFFFF

    def seed(self, seed_a = None, seed_b = None):
        self.__init__(seed_a, seed_b)

    def next(self):
        new_a = self.b
        new_b = (self.a + self.b) & 0xFFFFFFFF
        (self.a, self.b) = (new_a, new_b)
        return new_a

    def __iter__(self):
        return self

    def getstate(self):
        return (self.a, self.b)

    def setstate(self, state):
        (self.a, self.b) = (int(val) & 0xFFFFFFFF for val in state)


