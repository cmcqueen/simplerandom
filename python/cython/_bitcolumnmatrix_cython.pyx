
cdef extern from "types.h":
    ctypedef unsigned long uint64_t
    ctypedef unsigned int uint32_t
    ctypedef unsigned char uint8_t

cdef class BitColumnMatrix(object):

    cdef public list columns

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

    def __init__(self, columns, do_copy=True):
        self.columns = [ 0 ] * len(columns)
        for i, column in enumerate(columns):
            self.columns[i] = column

    def __len__(self):
        return len(self.columns)

    def __iter__(self):
        return iter(self.columns)

    def __add__(x, y):
        if not isinstance(x, BitColumnMatrix):
            return NotImplemented
        if len(x.columns) != len(y):
            raise IndexError("Matrices are not of same width")
        result_columns = []
        for i, column in enumerate(y):
            result_columns.append(x.columns[i] ^ int(column))
        return BitColumnMatrix(result_columns, do_copy=False)

    def __sub__(x, y):
        if not isinstance(x, BitColumnMatrix):
            return NotImplemented
        if len(x.columns) != len(y):
            raise IndexError("Matrices are not of same width")
        result_columns = []
        for i, column in enumerate(y):
            result_columns.append(x.columns[i] ^ int(column))
        return BitColumnMatrix(result_columns, do_copy=False)

    def __mul__(x, y):
        if not isinstance(x, BitColumnMatrix):
            return NotImplemented
        try:
            y_len = len(y)
        except TypeError:
            # Single value
            yy = int(y)
            value = 0
            for column in x.columns:
                if (yy & 1):
                    value ^= column
                yy >>= 1
            return value
        else:
            # Matrix multiplication
            if len(x.columns) != y_len:
                raise IndexError("Matrices are not of same width")
            result_columns = []
            for y_column in y:
                yy = int(y_column)
                value = 0
                for column in x.columns:
                    if (yy & 1):
                        value ^= column
                    yy >>= 1
                result_columns.append(value)
            return BitColumnMatrix(result_columns, do_copy=False)

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
        x_exp = BitColumnMatrix(x.columns)
        while n != 0:
            if n & 1:
                result = x_exp * result
            x_exp *= x_exp
            n >>= 1
        return result
