"""
Simple Pseudo-random number generators.

This module provides PRNG that sub-class the ``Random`` class in the
standard Python ``random`` module.

Reference: http://www.cse.yorku.ca/~oz/marsaglia-rng.html
"""

__all__ = [
    "_StandardRandomTemplate",
    "RandomCong",
    "RandomSHR3",
    "RandomMWC",
    "RandomMWC64",
    "RandomKISS",
    "RandomKISS2",
    "RandomLFIB4",
    "RandomSWB",
    "RandomLFSR113",
]


from simplerandom.random._random_py import *
from simplerandom.random._random_py import _StandardRandomTemplate

