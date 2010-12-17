#!/usr/bin/env python2

from simplerandom import *
from simplerandom import _RandomFibIterator
import simplerandomcython as src

def marsaglia_test():
    random_kiss = RandomKISSIterator(12345, 65435, 12345, 34221)
    t = [ next(random_kiss) for i in range(256) ]
#        for i in t: print i

    lfib4 = RandomLFIB4Iterator(t)
    if 1:
        for i in range(1000000):
            k = next(lfib4)
#                if i < 256: print k
        print k - 1064612766

    swb = RandomSWBIterator(lfib4.t)
    swb.c = lfib4.c
    if 1:
        for i in range(1000000):
            k = next(swb)
        print k - 627749721

    if 1:
        for i in range(1000000):
            k = next(random_kiss)
        print k - 1372460312

    cong = src.RandomCongIterator(random_kiss.random_cong.cong)
    if 1:
        for i in range(1000000):
            k = next(cong)
        print k - 1529210297

    shr3 = src.RandomSHR3Iterator(random_kiss.random_shr3.shr3_j)
    if 1:
        for i in range(1000000):
            k = next(shr3)
        print k - 2642725982

    mwc = RandomMWCIterator(random_kiss.random_mwc.mwc_z, random_kiss.random_mwc.mwc_w)
    if 1:
        for i in range(1000000):
            k = next(mwc)
        print k - 904977562

    fib = _RandomFibIterator(9983651,95746118)
    if 1:
        for i in range(1000000):
            k = next(fib)
        print k - 3519793928


if __name__ == "__main__":
    if 0:
        mwc = RandomMWCIterator(12345, 65435)
        for i in range(256):
            print next(mwc)
    if 0:
        cong = src.RandomCongIterator(12345)
        for i in range(256):
            print next(cong)
    if 0:
        shr3 = src.RandomSHR3Iterator(34221)
        for i in range(256):
            print next(shr3)
    if 0:
        shr3 = src.RandomSHR3Iterator(34221)
        for i in range(2000000):
            k = next(shr3)
        print k
    if 0:
        kiss = RandomKISSIterator(12345, 65435, 12345, 34221)
        for i in range(256):
            kiss_val = next(kiss)
            print kiss.mwc
            print kiss.cong
            print kiss.shr3_j
            print kiss_val
            print
    if 1:
        marsaglia_test()
