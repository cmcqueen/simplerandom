
cdef extern from "stdint.h":
    ctypedef unsigned long uint64_t
    ctypedef unsigned int uint32_t
    ctypedef unsigned char uint8_t


from collections import defaultdict

class GotCycle(Exception):
    pass

def find_cycles(func, func_range):
    cdef uint64_t j

    RNG_BITS = 32u
    RNG_RANGE = 2u**RNG_BITS
    RECORD_STATE_INTERVAL = 10000u
    
    # key is lowest value of cycle; value is length of cycle
    cycles = {}
    # key is length of cycle; value is set of cycles identified by lowest value
    lengths = defaultdict(set)
    # key is a func value; dict values are cycles identified by lowest value
    values = set()

    def find_cycle_with_seed(func, start_value):
        cdef uint32_t i
        cdef uint32_t length
        cdef uint32_t value
        cdef uint32_t min_value

        local_values = set()
        value = start_value
        length = 0
        min_value = RNG_RANGE - 1
        try:
            while True:
                for i in xrange(RECORD_STATE_INTERVAL):
                    length += 1
                    value = func(value)
                    min_value = min(value, min_value)
                    if value == start_value:
                        print "found cycle min %d, length %d" % (min_value, length)
                        cycles[min_value] = length
                        local_values.add(min_value)
                        lengths[length].add(min_value)
                        raise GotCycle
                    if value in values:
                        raise GotCycle
                local_values.add(value)
        except GotCycle:
            values |= local_values

#    find_cycles_recurse(klass, num_seeds, [])
    for j in xrange(func_range):
        find_cycle_with_seed(func, j)

    for min_val in sorted(cycles):
        print "Cycle min %d, length %d" % (min_val, cycles[min_val])
    print

    for cycle_length in lengths:
        print "Length %d, %d cycles" % (cycle_length, len(lengths[cycle_length]))
    print

def mwc_upper(uint32_t val):
    return 36969u * (val & 0xFFFFu) + (val >> 16u)

def shr3(uint32_t val):
    val ^= (val & 0x7FFFu) << 17u
    val ^= val >> 13u
    val ^= (val & 0x7FFFFFFu) << 5u
    return val

def main():
    #find_cycles(mwc_upper, 2**16)
    find_cycles(shr3, 2**31 - 1)

