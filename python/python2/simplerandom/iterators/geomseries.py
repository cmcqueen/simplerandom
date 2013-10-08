SIMPLERANDOM_MOD = 2**32
CONG_CYCLE_LEN = 2**32
CONG_MULT = 69069
CONG_CONST = 12345
# The following are used to calculate Cong.jumpahead().
JUMPAHEAD_C_FACTOR = 4      # (CONG_MULT - 1) == 4 * 17267
JUMPAHEAD_C_DENOM = 17267
JUMPAHEAD_C_MOD = SIMPLERANDOM_MOD * JUMPAHEAD_C_FACTOR
# Inverse of CONG_JUMPAHEAD_C_DENOM mod 2^32.
JUMPAHEAD_C_DENOM_INVERSE = pow(JUMPAHEAD_C_DENOM, SIMPLERANDOM_MOD - 1, SIMPLERANDOM_MOD)

def geom_series(n):
    return ((pow(CONG_MULT, n, JUMPAHEAD_C_MOD) - 1) // JUMPAHEAD_C_FACTOR * JUMPAHEAD_C_DENOM_INVERSE) & 0xFFFFFFFF

def geom_series_slow(n):
    temp = 0
    for i in xrange(n):
        temp += pow(CONG_MULT, i, 2**32)
        temp &= 0xFFFFFFFF
    return temp

def geom_series_recurse(n, r=CONG_MULT):
    if n == 0:
        return 0
    if n == 1:
        return 1
    temp = (1 + r) * geom_series_recurse(n // 2, r * r)
    if (n & 1):
        temp += pow(r, n - 1, 2**32)
    return temp & 0xFFFFFFFF
