"""
"""

try:
    from simplerandom.iterators._iterators_cython import *
    from simplerandom.iterators._iterators_cython import _RandomFibIterator
    _using_extension = True
except ImportError:
    from simplerandom.iterators._iterators_py import *
    from simplerandom.iterators._iterators_py import _RandomFibIterator
    _using_extension = False


