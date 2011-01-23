"""
Simple Pseudo-random number generators.

This module provides iterators that generate unsigned 32-bit PRNs.
"""

__all__ = [
    "RandomCongIterator",
    "RandomSHR3Iterator",
    "RandomMWCIterator",
    "RandomMWC64Iterator",
    "RandomKISSIterator",
    "RandomKISS2Iterator",
    "RandomLFIB4Iterator",
    "RandomSWBIterator",
    "RandomFibIterator",
]

try:
    from simplerandom.iterators._iterators_cython import *
    _using_extension = True
except ImportError:
    from simplerandom.iterators._iterators_py import *
    _using_extension = False


