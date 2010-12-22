"""
Simple Pseudo-random number generators.

This module provides iterators that generate unsigned 32-bit PRNs.

Reference: http://www.cse.yorku.ca/~oz/marsaglia-rng.html
"""

__all__ = [
    "RandomCongIterator",
    "RandomSHR3Iterator",
    "RandomMWCIterator",
    "RandomKISSIterator",
    "RandomLFIB4Iterator",
    "RandomSWBIterator",
    "_RandomFibIterator",
]

try:
    from simplerandom.iterators._iterators_cython import *
    from simplerandom.iterators._iterators_cython import _RandomFibIterator
    _using_extension = True
except ImportError:
    from simplerandom.iterators._iterators_py import *
    from simplerandom.iterators._iterators_py import _RandomFibIterator
    _using_extension = False


