
from collections import defaultdict

class GotCycle(Exception):
    pass

def find_cycles(func, func_range):
    RNG_BITS = 32
    RNG_RANGE = 2**RNG_BITS
    RECORD_STATE_INTERVAL = 1000
    
    # key is lowest value of cycle; value is length of cycle
    cycles = {}
    # key is length of cycle; value is set of cycles identified by lowest value
    lengths = defaultdict(set)
    # key is a func value; dict values are cycles identified by lowest value
    values = set()

    def find_cycle_with_seed(func, start_value):
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
    for i in xrange(func_range):
        find_cycle_with_seed(func, i)

    for min_value in sorted(cycles):
        print "Cycle min %d, length %d" % (min_value, cycles[min_value])
    print

    for length in lengths:
        print "Length %d, %d cycles" % (length, len(lengths[length]))
    print

def mwc_upper(val):
    return 36969 * (val & 0xFFFF) + (val >> 16)

def shr3(val):
    val ^= (val & 0x7FFF) << 17
    val ^= val >> 13
    val ^= (val & 0x7FFFFFF) << 5
    return val

#find_cycles(mwc_upper, 2**16)
find_cycles(shr3, 2**16)

