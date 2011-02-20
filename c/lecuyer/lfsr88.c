/* 
   32-bits Random number generator U[0,1): lfsr88
   Author: Pierre L'Ecuyer,
   Source: 
   ---------------------------------------------------------
*/

#include "lfsr88.h"


/**** VERY IMPORTANT **** :
  The initial seeds s1, s2, s3  MUST be larger than
  1, 7, and 15 respectively.
*/

#define SEED 12345
static unsigned int s1 = SEED, s2 = SEED, s3 = SEED, b;

double taus88_double ()
{                   /* Generates numbers between 0 and 1. */
b = (((s1 << 13) ^ s1) >> 19);
s1 = (((s1 & 4294967294) << 12) ^ b);
b = (((s2 << 2) ^ s2) >> 25);
s2 = (((s2 & 4294967288) << 4) ^ b);
b = (((s3 << 3) ^ s3) >> 11);
s3 = (((s3 & 4294967280) << 17) ^ b);
return ((s1 ^ s2 ^ s3) * 2.3283064365386963e-10);
}

unsigned int taus88 ()
{
b = (((s1 << 13) ^ s1) >> 19);
s1 = (((s1 & 4294967294) << 12) ^ b);
b = (((s2 << 2) ^ s2) >> 25);
s2 = (((s2 & 4294967288) << 4) ^ b);
b = (((s3 << 3) ^ s3) >> 11);
s3 = (((s3 & 4294967280) << 17) ^ b);
return (s1 ^ s2 ^ s3);
}

