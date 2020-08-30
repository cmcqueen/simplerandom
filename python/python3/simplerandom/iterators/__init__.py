"""
Simple Pseudo-random number generators.

This module provides iterators that generate unsigned 32-bit PRNs.
"""

__all__ = [
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

try:
    from simplerandom.iterators._iterators_cython import *
    _using_extension = True
except ImportError:
    from simplerandom.iterators._iterators_py import *
    _using_extension = False


