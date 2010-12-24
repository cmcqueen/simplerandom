
from simplerandom.iterators import *
from simplerandom.iterators import _RandomFibIterator

def marsaglia_test():
    random_kiss = RandomKISSIterator(12345, 65435, 12345, 34221)
    t = [ random_kiss.next() for i in range(256) ]
#        for i in t: print i

    lfib4 = RandomLFIB4Iterator(t)
    if 1:
        for i in range(1000000):
            k = lfib4.next()
#                if i < 256: print k
        print k - 1064612766

#    swb = RandomSWBIterator(lfib4.getstate())
    swb = RandomSWBIterator(lfib4.t)
    swb.c = lfib4.c
    if 1:
        for i in range(1000000):
            k = swb.next()
        print k - 627749721

    if 1:
        for i in range(1000000):
            k = random_kiss.next()
        print k - 1372460312

    cong = RandomCongIterator(random_kiss.cong)
    if 1:
        for i in range(1000000):
            k = cong.next()
        print k - 1529210297

    shr3 = RandomSHR3Iterator(random_kiss.shr3_j)
    if 1:
        for i in range(1000000):
            k = shr3.next()
        print k - 2642725982

    mwc = RandomMWCIterator(random_kiss.mwc_z, random_kiss.mwc_w)
    if 1:
        for i in range(1000000):
            k = mwc.next()
        print k - 904977562

    fib = _RandomFibIterator(9983651,95746118)
    if 1:
        for i in range(1000000):
            k = fib.next()
        print k - 3519793928


def main():
    if 0:
        mwc = RandomMWCIterator(12345, 65435)
        for i in range(256):
            print mwc.next()
    if 0:
        cong = RandomCongIterator(12345)
        for i in range(256):
            print cong.next()
    if 0:
        shr3 = RandomSHR3Iterator(34221)
        for i in range(256):
            print shr3.next()
    if 0:
        shr3 = RandomSHR3Iterator(34221)
        for i in range(2000000):
            k = shr3.next()
        print k
    if 0:
        kiss = RandomKISSIterator(12345, 65435, 12345, 34221)
        for i in range(256):
            kiss_val = kiss.next()
            print kiss.mwc
            print kiss.cong
            print kiss.shr3_j
            print kiss_val
            print
    if 1:
        marsaglia_test()

if __name__ == "__main__":
    main()

