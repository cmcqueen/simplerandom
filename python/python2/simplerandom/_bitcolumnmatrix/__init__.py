"""
Simple Pseudo-random number generators.

This module provides iterators that generate unsigned 32-bit PRNs.
"""

__all__ = [
    "BitColumnMatrix",
]

try:
    from simplerandom._bitcolumnmatrix._bitcolumnmatrix_cython import *
    _using_extension = True
except ImportError:
    from simplerandom._bitcolumnmatrix._bitcolumnmatrix_py import *
    _using_extension = False
