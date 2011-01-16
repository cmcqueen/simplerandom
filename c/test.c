
#include <inttypes.h>
#include <stdio.h>

#include "simplerandom.h"


int main (void)
{
    SimpleRandomCong_t      cong;
    SimpleRandomSHR3_t      shr3;
    SimpleRandomMWC_t       mwc;
    SimpleRandomKISS_t      kiss;
    SimpleRandomFib_t       fib;
    SimpleRandomMWC64_t     mwc64;
    SimpleRandomCong2_t     cong2;
    SimpleRandomSHR3_2_t    shr3_2;
    SimpleRandomKISS2_t     kiss2;
    uint32_t                i;
    uint32_t                k;


    /* Cong */
    simplerandom_cong_seed(&cong, UINT32_C(2524969849));
    for (i = 0; i < 1000000; i++)
    {
        k = simplerandom_cong_next(&cong);
    }
    printf("%"PRIu32"\n", k - UINT32_C(1529210297));

    /* SHR3 */
    simplerandom_shr3_seed(&shr3, UINT32_C(4176875757));
    for (i = 0; i < 1000000; i++)
    {
        k = simplerandom_shr3_next(&shr3);
    }
    printf("%"PRIu32"\n", k - UINT32_C(2642725982));

    /* MWC */
    simplerandom_mwc_seed(&mwc, UINT32_C(2374144069), UINT32_C(1046675282));
    for (i = 0; i < 1000000; i++)
    {
        k = simplerandom_mwc_next(&mwc);
    }
    printf("%"PRIu32"\n", k - UINT32_C(904977562));

    /* KISS */
    simplerandom_kiss_seed(&kiss, UINT32_C(2247183469), UINT32_C(99545079), UINT32_C(1017008441), UINT32_C(3259917390));
    for (i = 0; i < 1000000; i++)
    {
        k = simplerandom_kiss_next(&kiss);
    }
    printf("%"PRIu32"\n", k - UINT32_C(1372460312));

    /* Fib */
    simplerandom_fib_seed(&fib, UINT32_C(9983651), UINT32_C(95746118));
    for (i = 0; i < 1000000; i++)
    {
        k = simplerandom_fib_next(&fib);
    }
    printf("%"PRIu32"\n", k - UINT32_C(3519793928));

    /* Cong2 */
    simplerandom_cong2_seed(&cong2, UINT32_C(123456789));
    for (i = 0; i < 1000000; i++)
    {
        k = simplerandom_cong2_next(&cong2);
    }
    printf("%"PRIu32"\n", k - UINT32_C(410693845));

    /* SHR3_2 */
    simplerandom_shr3_2_seed(&shr3_2, UINT32_C(362436000));
    for (i = 0; i < 1000000; i++)
    {
        k = simplerandom_shr3_2_next(&shr3_2);
    }
    printf("%"PRIu32"\n", k - UINT32_C(1517776246));

#ifdef UINT64_C

    /* MWC64 */
    simplerandom_mwc64_seed(&mwc64, UINT32_C(7654321), UINT32_C(521288629));
    for (i = 0; i < 1000000; i++)
    {
        k = simplerandom_mwc64_next(&mwc64);
    }
    printf("%"PRIu32"\n", k - UINT32_C(3377343606));

    /* KISS2 */
    simplerandom_kiss2_seed(&kiss2, UINT32_C(7654321), UINT32_C(521288629), UINT32_C(123456789), UINT32_C(362436000));
    for (i = 0; i < 1000000; i++)
    {
        k = simplerandom_kiss2_next(&kiss2);
    }
    printf("%"PRIu32"\n", k - UINT32_C(1010846401));

#endif /* defined(UINT64_C) */
}

