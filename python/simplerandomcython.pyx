
cdef extern from "stdint.h":
    ctypedef int uint32_t


cdef class RandomCongIterator(object):
    '''Congruential random number generator'''

    cdef public uint32_t cong

    def __init__(self, seed = None):
        if seed==None:
            seed = 12344
        self.cong = int(seed)

    def seed(self, seed = None):
        self.__init__(seed)

    def __next__(self):
        self.cong = 69069 * self.cong + 1234567
        return self.cong

    def __iter__(self):
        return self

    def getstate(self):
        return (self.cong, )

    def setstate(self, state):
        self.cong = int(state[0])


cdef class RandomSHR3Iterator(object):
    '''3-shift-register random number generator'''

    cdef public uint32_t shr3_j

    def __init__(self, seed = None):
        if seed==None:
            seed = 34223
        self.shr3_j = int(seed)

    def seed(self, seed = None):
        self.__init__(seed)

    def __next__(self):
        cdef uint32_t shr3_j
        shr3_j = self.shr3_j
        shr3_j ^= shr3_j << 17
        shr3_j ^= shr3_j >> 13
        shr3_j ^= shr3_j << 5
        self.shr3_j = shr3_j
        return shr3_j

    def __iter__(self):
        return self

    def getstate(self):
        return (self.shr3_j, )

    def setstate(self, state):
        self.shr3_j = int(state[0])

