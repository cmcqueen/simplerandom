/*
This tests several variants of Marsaglia's MWC generator, using L'Ecuyer's
TestU01 RNG test suite.

References:

[1]: http://mathforum.org/kb/thread.jspa?threadID=499149&tstart=17850
[2]: http://mathforum.org/kb/thread.jspa?threadID=499163&tstart=17685
[3]: http://mathforum.org/kb/thread.jspa?threadID=499215&tstart=17685
     http://www.cse.yorku.ca/~oz/marsaglia-rng.html
*/

#include "unif01.h"
#include "bbattery.h"


/*
This is the MWC proposed in an early newsgroup post from Marsaglia in 1999 [1].
It was later changed by Marsaglia to the implementation shown in mwc2() below.

This generator doesn't perform well even in SmallCrush tests, and fails many
Crush tests. I didn't even bother running BigCrush on it.

========= Summary results of SmallCrush =========

 Version:          TestU01 1.2.3
 Generator:        mwc1
 Number of statistics:  15
 Total CPU time:   00:00:14.26
 The following tests gave p-values outside [0.001, 0.9990]:
 (eps  means a value < 1.0e-300):
 (eps1 means a value < 1.0e-015):

       Test                          p-value
 ----------------------------------------------
  2  Collision                      1 - eps1
  3  Gap                            1.2e-04
  6  MaxOft                           eps
 ----------------------------------------------
 All other tests were passed

========= Summary results of Crush =========

 Version:          TestU01 1.2.3
 Generator:        mwc1
 Number of statistics:  142
 Total CPU time:   00:51:02.75
 The following tests gave p-values outside [0.001, 0.9990]:
 (eps  means a value < 1.0e-300):
 (eps1 means a value < 1.0e-015):

       Test                          p-value
 ----------------------------------------------
  1  SerialOver, t = 2              1 - eps1
  2  SerialOver, t = 4                eps
  3  CollisionOver, t = 2           1 - eps1
  4  CollisionOver, t = 2           1 -  2.3e-05
  5  CollisionOver, t = 4             eps
  6  CollisionOver, t = 4           1 - eps1
  7  CollisionOver, t = 8           1 - eps1
  8  CollisionOver, t = 8           1 - eps1
  9  CollisionOver, t = 20          1 -  3.1e-11
 10  CollisionOver, t = 20          1 - eps1
 11  BirthdaySpacings, t = 2        3.3e-14
 12  BirthdaySpacings, t = 3          eps
 13  BirthdaySpacings, t = 4          eps
 14  BirthdaySpacings, t = 7          eps
 15  BirthdaySpacings, t = 7          eps
 16  BirthdaySpacings, t = 8          eps
 17  BirthdaySpacings, t = 8          eps
 18  ClosePairs NP, t = 2           1.9e-17
 18  ClosePairs mNP, t = 2         4.4e-127
 18  ClosePairs mNP1, t = 2        1.3e-143
 18  ClosePairs NJumps, t = 2       1 - eps1
 19  ClosePairs NP, t = 3           4.6e-21
 19  ClosePairs mNP, t = 3         3.2e-157
 19  ClosePairs mNP1, t = 3           eps
 19  ClosePairs mNP2, t = 3           eps
 19  ClosePairs NJumps, t = 3         eps
 19  ClosePairs mNP2S, t = 3          eps
 20  ClosePairs NP, t = 7           1.8e-79
 20  ClosePairs mNP, t = 7          3.1e-69
 20  ClosePairs mNP1, t = 7         6.9e-70
 20  ClosePairs NJumps, t = 7       1 - eps1
 21  ClosePairsBitMatch, t = 2      1 - eps1
 33  Gap, r = 0                     1.7e-10
 34  Gap, r = 22                      eps
 37  Permutation, r = 0             1 -  1.0e-12
 41  MaxOft, t = 5                    eps
 42  MaxOft, t = 10                   eps
 43  MaxOft, t = 20                   eps
 44  MaxOft, t = 30                   eps
 50  AppearanceSpacings, r = 20     1 -  8.6e-11
 68  RandomWalk1 R (L = 1000)       1.8e-04
 75  Fourier3, r = 20               3.0e-06
 82  HammingCorr, L = 30            5.3e-14
 86  HammingIndep, L = 30             eps
 88  HammingIndep, L = 300          3.0e-04
 91  Run of bits, r = 0             5.6e-05
 92  Run of bits, r = 20              eps
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
This is the MWC as eventually specified by Marsaglia in his 1999 newsgroup
post [3]. It passed Marsaglia's Diehard tests better than mwc1() above.
Quoting [2]:

    In my earlier work, done in Fortran, I had implemented
    two 16-bit multiply-with-carry generators, say z and w,
    as 32-bit integers, with the carry in the top 16 bits,
    the output in the bottom 16. They were combined by
    (z<<16)+w. (In Fortran, ishft(z,16)+w.) Such a
    combination seemed to pass all tests. In the above-
    mentioned post, I used (z<<16)+(w&65525), and that
    does not pass all tests. So (z<<16)+w seems
    preferable; it is used below, providing a MWC that
    seems to pass all tests.

It passes more of the TestU01 suite than mwc1(), but still fails a large
number of the Crush tests. So I didn't bother running the BigCrush tests.

========= Summary results of SmallCrush =========

 Version:          TestU01 1.2.3
 Generator:        mwc2
 Number of statistics:  15
 Total CPU time:   00:00:14.31
 The following tests gave p-values outside [0.001, 0.9990]:
 (eps  means a value < 1.0e-300):
 (eps1 means a value < 1.0e-015):

       Test                          p-value
 ----------------------------------------------
  3  Gap                            1.2e-04
 ----------------------------------------------
 All other tests were passed

========= Summary results of Crush =========

 Version:          TestU01 1.2.3
 Generator:        mwc2
 Number of statistics:  144
 Total CPU time:   00:50:56.17
 The following tests gave p-values outside [0.001, 0.9990]:
 (eps  means a value < 1.0e-300):
 (eps1 means a value < 1.0e-015):

       Test                          p-value
 ----------------------------------------------
  6  CollisionOver, t = 4           1 - eps1
  8  CollisionOver, t = 8           1 - eps1
 10  CollisionOver, t = 20          1 - eps1
 16  BirthdaySpacings, t = 8          eps
 17  BirthdaySpacings, t = 8          eps
 34  Gap, r = 22                      eps
 49  AppearanceSpacings, r = 0       0.9997
 50  AppearanceSpacings, r = 20     1 -  1.1e-10
 66  RandomWalk1 H (L = 90)         1.1e-04
 67  RandomWalk1 C (L = 1000)       7.1e-04
 75  Fourier3, r = 20               1.1e-06
 82  HammingCorr, L = 30            1.7e-10
 85  HammingIndep, L = 30           5.3e-06
 86  HammingIndep, L = 30             eps
 91  Run of bits, r = 0             3.8e-07
 92  Run of bits, r = 20              eps
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
This is my own modification to MWC, to rotate the "upper" MWC result rather
than shifting it (implemented in C by adding an extra shift of the result).
That means the upper 16 bits are included, rather than being thrown away.
That means the period of the lower 16 bits is 2^60 rather than just 2^29.

This performs much better in Crush and BigCrush tests, a significant
improvement.

========= Summary results of Crush =========

 Version:          TestU01 1.2.3
 Generator:        mwc3
 Number of statistics:  144
 Total CPU time:   00:51:04.82

 All tests were passed

========= Summary results of BigCrush =========

 Version:          TestU01 1.2.3
 Generator:        mwc3
 Number of statistics:  160
 Total CPU time:   06:21:05.18
 The following tests gave p-values outside [0.001, 0.9990]:
 (eps  means a value < 1.0e-300):
 (eps1 means a value < 1.0e-015):

       Test                          p-value
 ----------------------------------------------
 15  BirthdaySpacings, t = 4        1.4e-17
 ----------------------------------------------
 All other tests were passed
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

