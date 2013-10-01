
from cython cimport view
import numbers

from libc.stdint cimport uint32_t

cdef class BitColumnMatrix(object):

    cdef uint32_t columns[32]
    cdef uint32_t columns_len
    
    @staticmethod
    def unity(uint32_t n):
        cdef uint32_t value
        cdef BitColumnMatrix result = BitColumnMatrix(int(n))
        value = 1
        for i in range(n):
            result.columns[i] = value
            value <<= 1
        return result

    @staticmethod
    def shift(uint32_t n, int shift_value):
        cdef uint32_t value
        cdef BitColumnMatrix result = BitColumnMatrix(int(n))
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
                value <<= 1
                if value >= (1 << n):
                    value = 0
        return result

    def __init__(self, columns=None, do_copy=True):
        if isinstance(columns, BitColumnMatrix):
            for i in range(32):
                self.columns[i] = (<BitColumnMatrix>columns).columns[i]
        elif isinstance(columns, numbers.Integral):
            if columns > 32:
                raise NotImplementedError
            #self.columns = view.array(shape=(columns,), itemsize=sizeof(unsigned int), format="I")
            for i in range(32):
                self.columns[i] = 0
        else:
            columns_len = len(columns)
            if columns_len > 32:
                raise NotImplementedError
            #self.columns = view.array(shape=(columns_len,), itemsize=sizeof(unsigned int), format="I")
            for i, column in enumerate(columns):
                self.columns[i] = int(column)

    def __len__(self):
        #return self.columns.shape[0]
        return 32

#     def __iter__(self):
#         return iter(self.columns)

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
        cdef uint32_t x_len
        cdef uint32_t y_len
        if not isinstance(x, BitColumnMatrix):
            return NotImplemented
        if isinstance(y, numbers.Integral):
            # Single value
            yy = int(y)
            value = 0
            for i in range(len(x)):
                if (yy & 1):
                    value ^= (<BitColumnMatrix>x).columns[i]
                yy >>= 1
            return value
        elif not isinstance(y, BitColumnMatrix):
            return NotImplemented
        else:
            # Matrix multiplication
            x_len = len(x)
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

    def __imul__(self, other):
        cdef BitColumnMatrix result
        cdef uint32_t yy
        cdef uint32_t value
        cdef uint32_t self_len
        cdef uint32_t other_len
        if isinstance(other, numbers.Integral):
            # Single value. Not suitable for __imul__.
            return NotImplemented
        elif not isinstance(other, BitColumnMatrix):
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
