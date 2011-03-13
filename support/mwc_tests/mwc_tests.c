#include "unif01.h"
#include "bbattery.h"


/*
========= Summary results of SmallCrush =========

 Version:          TestU01 1.2.3
 Generator:        mwc1
 Number of statistics:  15
 Total CPU time:   00:00:28.26
 The following tests gave p-values outside [0.001, 0.9990]:
 (eps  means a value < 1.0e-300):
 (eps1 means a value < 1.0e-15):

       Test                          p-value
 ----------------------------------------------
  2  Collision                      1 - eps1
  3  Gap                             1.2e-4
  6  MaxOft                           eps  
 ----------------------------------------------
 All other tests were passed
*/
unsigned int mwc1(void)
{
    static unsigned int mwc_upper = 362436069u;
    static unsigned int mwc_lower = 521288629u;

    mwc_upper = 36969u * (mwc_upper & 0xFFFFu) + (mwc_upper >> 16u);
    mwc_lower = 18000u * (mwc_lower & 0xFFFFu) + (mwc_lower >> 16u);

    return (mwc_upper << 16u) + (mwc_lower & 0xFFFF);
}

/*
========= Summary results of SmallCrush =========

 Version:          TestU01 1.2.3
 Generator:        mwc2
 Number of statistics:  15
 Total CPU time:   00:00:27.80
 The following tests gave p-values outside [0.001, 0.9990]:
 (eps  means a value < 1.0e-300):
 (eps1 means a value < 1.0e-15):

       Test                          p-value
 ----------------------------------------------
  3  Gap                             1.2e-4
 ----------------------------------------------
 All other tests were passed
*/
unsigned int mwc2(void)
{
    static unsigned int mwc_upper = 362436069u;
    static unsigned int mwc_lower = 521288629u;

    mwc_upper = 36969u * (mwc_upper & 0xFFFFu) + (mwc_upper >> 16u);
    mwc_lower = 18000u * (mwc_lower & 0xFFFFu) + (mwc_lower >> 16u);

    return (mwc_upper << 16u) + mwc_lower;
}

/*
========= Summary results of SmallCrush =========

 Version:          TestU01 1.2.3
 Generator:        mwc3
 Number of statistics:  15
 Total CPU time:   00:00:28.14

 All tests were passed
*/
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

