"""
Reference: http://www.cse.yorku.ca/~oz/marsaglia-rng.html
"""

class RandomTemplate(object):
    '''Template class for random number generator'''

    def __init__(self, seed = None):
        raise NotImplemented

    def next(self):
        raise NotImplemented
        return 0

    def __iter__(self):
        return self


class RandomCong(object):
    '''Congruential random number generator'''

    def __init__(self, seed):
        self.cong = (seed & 0xFFFFFFFF)

    def next(self):
        self.cong = (69069 * self.cong + 1234567) & 0xFFFFFFFF
        return self.cong

    def __iter__(self):
        return self


class RandomSHR3(object):
    '''3-shift-register random number generator'''

    def __init__(self, seed):
        self.jsr = (seed & 0xFFFFFFFF)

    def next(self):
        jsr = self.jsr
        jsr ^= (jsr & 0x7FFF) << 17
        jsr ^= jsr >> 13
        jsr ^= (jsr & 0x7FFFFFF) << 5
        self.jsr = jsr
        return jsr

    def __iter__(self):
        return self


class RandomMWC(object):
    '''"Multiply-with-carry" random number generator'''

    def __init__(self, seed_z, seed_w):
        self.z = (seed_z & 0xFFFFFFFF)
        self.w = (seed_w & 0xFFFFFFFF)

    def next(self):
        self.z = 36969 * (self.z & 0xFFFF) + (self.z >> 16)
        self.w = 18000 * (self.w & 0xFFFF) + (self.w >> 16)
        mwc = (((self.z & 0xFFFF) << 16) + self.w) & 0xFFFFFFFF
        self.mwc = mwc
        return mwc

    def __iter__(self):
        return self


class RandomKISS(RandomMWC, RandomCong, RandomSHR3):
    '''"Keep It Simple Stupid" random number generator
    
    It combines the RandomMWC, RandomCong, RandomSHR3
    generators. Period is about 2**123.
    '''

    def __init__(self, seed_mwc_z, seed_mwc_w, seed_cong, seed_shr3):
#        self.random_mwc = RandomMWC(seed_mwc_z, seed_mwc_w)
#        self.random_cong = RandomCong(seed_cong)
#        self.random_shr3 = RandomSHR3(seed_shr3)
        RandomMWC.__init__(self, seed_mwc_z, seed_mwc_w)
        RandomCong.__init__(self, seed_cong)
        RandomSHR3.__init__(self, seed_shr3)

    def next(self):
#        mwc_val = next(self.random_mwc)
#        cong_val = next(self.random_cong)
#        shr3_val = next(self.random_shr3)
        mwc_val = RandomMWC.next(self)
        cong_val = RandomCong.next(self)
        shr3_val = RandomSHR3.next(self)
        return ((mwc_val ^ cong_val) + shr3_val) & 0xFFFFFFFF


class RandomLFIB4(object):
    '''"Lagged Fibonacci 4-lag" random number generator'''

    def __init__(self, seed = None):
        if not seed:
            random_kiss = RandomKISS(12345, 65435, 12345, 34221)
            seed = [ next(random_kiss) for i in range(256) ]
        else:
            if len(seed) != 256:
                raise Exception("seed length must be 256")
        self.t = list(seed)
        self.c = 0

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


if __name__ == "__main__":
    if 0:
        mwc = RandomMWC(12345, 65435)
        for i in range(256):
            print next(mwc)
    if 0:
        cong = RandomCong(12345)
        for i in range(256):
            print next(cong)
    if 0:
        shr3 = RandomSHR3(34221)
        for i in range(256):
            print next(shr3)
    if 1:
        random_kiss = RandomKISS(12345, 65435, 12345, 34221)
        t = [ next(random_kiss) for i in range(256) ]
        if 0:
            for i in t:
                print i
        lfib4 = RandomLFIB4(t)
        if 0:
            for i in range(256):
                print next(lfib4)
        if 1:
            for i in range(1000000):
                k = next(lfib4)
            print k-1064612766
    if 0:
        kiss = RandomKISS(12345, 65435, 12345, 34221)
        for i in range(256):
            kiss_val = next(kiss)
            print kiss.mwc
            print kiss.cong
            print kiss.jsr
            print kiss_val
            print
