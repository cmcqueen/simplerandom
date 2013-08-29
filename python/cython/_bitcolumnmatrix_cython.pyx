
from cython cimport view
import numbers

cdef extern from "types.h":
    ctypedef unsigned long uint64_t
    ctypedef unsigned int uint32_t
    ctypedef unsigned char uint8_t

cdef class BitColumnMatrix(object):

    #cdef public array columns
    cdef public view.array columns  # = view.array(shape=(32,), itemsize=sizeof(unsigned long), format="L")
    
    @staticmethod
    def unity(n):
        columns = []
        value = 1
        for _i in range(n):
            columns.append(value)
            value <<= 1
        return BitColumnMatrix(columns)

    @staticmethod
    def shift(n, shift_value):
        columns = []
        if shift_value >= 0:
            value = 1 << shift_value
        else:
            value = 0
        for _i in range(n):
            columns.append(value)
            if shift_value < 0:
                shift_value += 1
                if shift_value == 0:
                    value = 1
            else:
                value <<= 1
                if value >= (1 << n):
                    value = 0
        return BitColumnMatrix(columns)

    def __init__(self, columns=None, do_copy=True):
        if isinstance(columns, BitColumnMatrix):
            self.columns[:] = columns.columns[:]
        elif isinstance(columns, numbers.Integral):
            if columns > 32:
                raise NotImplementedError
            self.columns = view.array(shape=(columns,), itemsize=sizeof(unsigned long), format="L")
        else:
            columns_len = len(columns)
            if columns_len > 32:
                raise NotImplementedError
            self.columns = view.array(shape=(columns_len,), itemsize=sizeof(unsigned long), format="L")
            for i, column in enumerate(columns):
                self.columns[i] = int(column)

    def __len__(self):
        return self.columns.shape[0]

    def __iter__(self):
        return iter(self.columns)

    def __add__(x, y):
        if not isinstance(x, BitColumnMatrix) or not isinstance(y, BitColumnMatrix):
            return NotImplemented
        if len(x) != len(y):
            raise IndexError("Matrices are not of same width")
        result = BitColumnMatrix(len(x))
        for i in range(len(x)):
            result.columns[i] = x.columns[i] ^ y.columns[i]
        return result

    def __sub__(x, y):
        if not isinstance(x, BitColumnMatrix) or not isinstance(y, BitColumnMatrix):
            return NotImplemented
        if len(x) != len(y):
            raise IndexError("Matrices are not of same width")
        result = BitColumnMatrix(len(x))
        for i in range(len(x)):
            result.columns[i] = x.columns[i] ^ y.columns[i]
        return result

    def __mul__(x, y):
        cdef uint32_t yy
        cdef uint32_t value
        if not isinstance(x, BitColumnMatrix):
            return NotImplemented
        if isinstance(y, numbers.Integral):
            # Single value
            yy = int(y)
            value = 0
            for i in range(len(x)):
                if (yy & 1):
                    value ^= x.column[i]
                yy >>= 1
            return value
        else:
            # Matrix multiplication
            if len(x) != len(y):
                raise IndexError("Matrices are not of same width")
            result = BitColumnMatrix(len(x))
            for j in range(len(y)):
                yy = int(y.columns[j])
                value = 0
                for i in range(len(x)):
                    if (yy & 1):
                        value ^= x.column[i]
                    yy >>= 1
                result.columns[j] = value
            return result

    def __imul__(self, other):
        try:
            other_len = len(other)
        except TypeError:
            # Single value. Not suitable for __imul__.
            return NotImplemented
        else:
            # Matrix multiplication
            if len(self.columns) != other_len:
                raise IndexError("Matrices are not of same width")
            result_columns = []
            for other_column in other:
                x = int(other_column)
                value = 0
                for column in self.columns:
                    if (x & 1):
                        value ^= column
                    x >>= 1
                result_columns.append(value)
            self.columns = result_columns
            return self

    def __pow__(x, y, modulo):
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
