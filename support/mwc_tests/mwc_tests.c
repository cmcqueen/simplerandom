#include "unif01.h"
#include "bbattery.h"


unsigned int mwc1(void)
{
    static unsigned int mwc_upper = 362436069u;
    static unsigned int mwc_lower = 521288629u;

    mwc_upper = 36969u * (mwc_upper & 0xFFFFu) + (mwc_upper >> 16u);
    mwc_lower = 18000u * (mwc_lower & 0xFFFFu) + (mwc_lower >> 16u);

    return (mwc_upper << 16u) + (mwc_lower & 0xFFFF);
}

unsigned int mwc2(void)
{
    static unsigned int mwc_upper = 362436069u;
    static unsigned int mwc_lower = 521288629u;

    mwc_upper = 36969u * (mwc_upper & 0xFFFFu) + (mwc_upper >> 16u);
    mwc_lower = 18000u * (mwc_lower & 0xFFFFu) + (mwc_lower >> 16u);

    return (mwc_upper << 16u) + mwc_lower;
}

unsigned int mwc3(void)
{
    static unsigned int mwc_upper = 362436069u;
    static unsigned int mwc_lower = 521288629u;

    mwc_upper = 36969u * (mwc_upper & 0xFFFFu) + (mwc_upper >> 16u);
    mwc_lower = 18000u * (mwc_lower & 0xFFFFu) + (mwc_lower >> 16u);

    return (mwc_upper << 16u) + (mwc_upper >> 16u) + mwc_lower;
}

int main (void)
{
    unif01_Gen *gen;


    gen = unif01_CreateExternGenBits ("mwc1", mwc1);
    //gen = unif01_CreateExternGenBits ("mwc2", mwc2);
    //gen = unif01_CreateExternGenBits ("mwc3", mwc3);

    bbattery_SmallCrush (gen);
    //bbattery_Crush (gen);
    //bbattery_BigCrush (gen);

    unif01_DeleteExternGenBits (gen);

    return 0;
}

