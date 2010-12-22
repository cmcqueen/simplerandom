"""
Simple Pseudo-random number generators.

This module provides PRNG that sub-class the ``Random`` class in the
standard Python ``random`` module.

Reference: http://www.cse.yorku.ca/~oz/marsaglia-rng.html
"""

__all__ = [
    "RandomCong",
    "RandomSHR3",
    "RandomMWC",
    "RandomKISS",
#    "RandomLFIB4",
#    "RandomSWB",
]


from simplerandom.random._random_py import *


