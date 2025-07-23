
#cython: language_level=3

import numbers

from cython cimport view


cdef extern from "types.h":
    ctypedef unsigned int uint32_t

cdef class BitColumnMatrix(object):

    cdef uint32_t columns[32]
    cdef size_t columns_len

    @staticmethod
    def unity(uint32_t n):
        cdef uint32_t value
        cdef BitColumnMatrix result = BitColumnMatrix(int(n))
        if n > 32:
            raise NotImplementedError
        result.columns_len = n
        value = 1
        for i in range(n):
            result.columns[i] = value
            value <<= 1
        return result

    @staticmethod
    def mask(n, start, end):
        """Make a BitColumnMatrix that represents a mask.
        BitColumnMatrix size of n*n, to manipulate n binary bits.
        If start <= end, then bits in half-open range [start, end) are set.
        If end < start, then bits in ranges [0, end) and [start, n) are set,
        i.e. bits in range [end, start) are clear; others are set.
        """
        cdef uint32_t value
        cdef BitColumnMatrix result = BitColumnMatrix(int(n))
        if n > 32:
            raise NotImplementedError
        result.columns_len = n
        value = 1
        for i in range(n):
            if start <= end:
                result.columns[i] = value if (start <= i < end) else 0
            else:
                result.columns[i] = value if (start <= i or i < end) else 0
            value <<= 1
        return result

    @staticmethod
    def shift(uint32_t n, int shift_value):
        cdef uint32_t value
        cdef BitColumnMatrix result = BitColumnMatrix(int(n))
        if n > 32:
            raise NotImplementedError
        result.columns_len = n
        if shift_value >= 0:
            value = 1 << shift_value
        else:
            value = 0
        for i in range(n):
            result.columns[i] = value
            if shift_value < 0:
                shift_value += 1
                if shift_value == 0:
                    value = 1
            else:
                if n == 0 or value >= (1u << (n-1)):
                    value = 0
                else:
                    value <<= 1
        return result

    def __init__(self, columns=None, do_copy=True):
        if isinstance(columns, BitColumnMatrix):
            self.columns_len = (<BitColumnMatrix>columns).columns_len
            for i in range(32):
                self.columns[i] = (<BitColumnMatrix>columns).columns[i]
        elif isinstance(columns, numbers.Integral):
            if columns > 32:
                raise NotImplementedError
            self.columns_len = columns
            #self.columns = view.array(shape=(columns,), itemsize=sizeof(unsigned int), format="I")
            for i in range(32):
                self.columns[i] = 0
        else:
            self.columns_len = len(columns)
            if self.columns_len > 32:
                raise NotImplementedError
            #self.columns = view.array(shape=(self.columns_len,), itemsize=sizeof(unsigned int), format="I")
            for i, column in enumerate(columns):
                self.columns[i] = int(column)

    def __len__(self):
        #return self.columns.shape[0]
        return self.columns_len

#     def __iter__(self):
#         return iter(self.columns)

    def __richcmp__(BitColumnMatrix x, BitColumnMatrix y, int op):
        cdef BitColumnMatrix result
        if not isinstance(x, BitColumnMatrix) or not isinstance(y, BitColumnMatrix):
            raise NotImplementedError
        if op == 2:                 # ==
            if len(x) != len(y):
                return False
            for i in range(len(x)):
                if x.columns[i] != y.columns[i]:
                    return False
            return True
        elif op == 3:               # !=
            if len(x) != len(y):
                return True
            for i in range(len(x)):
                if x.columns[i] != y.columns[i]:
                    return True
            return False
        else:
            raise NotImplementedError

    def __add__(BitColumnMatrix x, BitColumnMatrix y):
        cdef BitColumnMatrix result
        if not isinstance(x, BitColumnMatrix) or not isinstance(y, BitColumnMatrix):
            return NotImplemented
        if len(x) != len(y):
            raise IndexError("Matrices are not of same width")
        result = BitColumnMatrix(len(x))
        for i in range(len(x)):
            result.columns[i] = x.columns[i] ^ y.columns[i]
        return result

    def __sub__(BitColumnMatrix x, BitColumnMatrix y):
        cdef BitColumnMatrix result
        if not isinstance(x, BitColumnMatrix) or not isinstance(y, BitColumnMatrix):
            return NotImplemented
        if len(x) != len(y):
            raise IndexError("Matrices are not of same width")
        result = BitColumnMatrix(len(x))
        for i in range(len(x)):
            result.columns[i] = (<BitColumnMatrix>x).columns[i] ^ (<BitColumnMatrix>y).columns[i]
        return result

    def __mul__(x, y):
        cdef BitColumnMatrix result
        cdef uint32_t yy
        cdef uint32_t value
        cdef size_t x_len
        cdef size_t y_len
        if not isinstance(x, BitColumnMatrix):
            return NotImplemented
        x_len = len(x)
        if isinstance(y, numbers.Integral):
            # Single value
            yy = int(y)
            value = 0
            for i in range(x_len):
                if (yy & 1):
                    value ^= (<BitColumnMatrix>x).columns[i]
                yy >>= 1
            return value
        elif not isinstance(y, BitColumnMatrix):
            return NotImplemented
        else:
            # Matrix multiplication
            y_len = len(y)
            if x_len != y_len:
                raise IndexError("Matrices are not of same width")
            result = BitColumnMatrix(y_len)
            for j in range(y_len):
                yy = (<BitColumnMatrix>y).columns[j]
                value = 0
                for i in range(x_len):
                    if (yy & 1):
                        value ^= (<BitColumnMatrix>x).columns[i]
                    yy >>= 1
                result.columns[j] = value
            return result

    def __imul__(self, BitColumnMatrix other):
        cdef BitColumnMatrix result
        cdef uint32_t yy
        cdef uint32_t value
        cdef size_t self_len
        cdef size_t other_len
        if not isinstance(other, BitColumnMatrix):
            # Single int value, or some other object. Not suitable for __imul__.
            return NotImplemented
        else:
            # Matrix multiplication
            self_len = len(self)
            other_len = len(other)
            if self_len != other_len:
                raise IndexError("Matrices are not of same width")
            result = BitColumnMatrix(other_len)
            for j in range(other_len):
                yy = (<BitColumnMatrix>other).columns[j]
                value = 0
                for i in range(self_len):
                    if (yy & 1):
                        value ^= self.columns[i]
                    yy >>= 1
                result.columns[j] = value
            for i in range(32):
                self.columns[i] = result.columns[i]
            return self

    def __pow__(x, y, modulo):
        cdef BitColumnMatrix result
        cdef BitColumnMatrix x_exp
        if modulo is not None:
            return NotImplemented
        if not isinstance(x, BitColumnMatrix):
            return NotImplemented
        n = int(y)
        result = BitColumnMatrix.unity(len(x))
        x_exp = BitColumnMatrix(x)
        while n != 0:
            if n & 1:
                result = x_exp * result
            x_exp *= x_exp
            n >>= 1
        return result
