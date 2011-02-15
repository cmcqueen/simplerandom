
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "lfsr113.h"


int main(void)
{
    uint32_t                i;
    uint32_t                k;


    for (i = 0; i < 1000000; i++)
    {
        k = lfsr113();
    }
    printf("%"PRIu32"\n", k - UINT32_C(1205173390));

}

