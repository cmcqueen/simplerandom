"""
Simple Pseudo-random number generators.

This module provides PRNG that sub-class the ``Random`` class in the
standard Python ``random`` module.

Reference: http://www.cse.yorku.ca/~oz/marsaglia-rng.html
"""

__all__ = [
    "_StandardRandomTemplate",
    "Cong",
    "SHR3",
    "MWC1",
    "MWC2",
    "MWC64",
    "KISS",
    "KISS2",
    "LFSR113",
    "LFSR88",
]


from simplerandom.random._random_py import *
from simplerandom.random._random_py import _StandardRandomTemplate

