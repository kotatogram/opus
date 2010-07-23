/* Copyright (c) 2007-2008 CSIRO
   Copyright (c) 2007-2009 Xiph.Org Foundation
   Written by Jean-Marc Valin */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef RATE_H
#define RATE_H

#define MAX_PSEUDO 40
#define LOG_MAX_PSEUDO 6

#define MAX_PULSES 128
#define LOG_MAX_PULSES 7

#define BITRES 3
#define FINE_OFFSET 25
#define QTHETA_OFFSET 18

#define BITOVERFLOW 30000

#include "cwrs.h"

static inline int get_pulses(int i)
{
   return i<8 ? i : (8 + (i&7)) << ((i>>3)-1);
}

static inline int bits2pulses(const CELTMode *m, const celt_int16 *cache, int N, int bits)
{
   int i;
   int lo, hi;

   lo = 0;
   hi = MAX_PSEUDO-1;
   for (i=0;i<LOG_MAX_PSEUDO;i++)
   {
      int mid = (lo+hi)>>1;
      /* OPT: Make sure this is implemented with a conditional move */
      if (cache[mid] >= bits)
         hi = mid;
      else
         lo = mid;
   }
   if (bits-cache[lo] <= cache[hi]-bits)
      return lo;
   else
      return hi;
   
   lo = 0;
   hi = MAX_PULSES-1;
   
   /* Instead of using the "bisection condition" we use a fixed number of 
   iterations because it should be faster */
   /*while (hi-lo != 1)*/
   for (i=0;i<LOG_MAX_PULSES;i++)
   {
      int mid = (lo+hi)>>1;
      /* OPT: Make sure this is implemented with a conditional move */
      if (cache[mid] >= bits)
         hi = mid;
      else
         lo = mid;
   }
   if (bits-cache[lo] <= cache[hi]-bits)
      return lo;
   else
      return hi;
}


static inline int pulses2bits(const celt_int16 *cache, int N, int pulses)
{
   return cache[pulses];
}

/** Computes a cache of the pulses->bits mapping in each band */
celt_int16 **compute_alloc_cache(CELTMode *m, int M);

/** Compute the pulse allocation, i.e. how many pulses will go in each
  * band. 
 @param m mode
 @param offsets Requested increase or decrease in the number of bits for
                each band
 @param total Number of bands
 @param pulses Number of pulses per band (returned)
 @return Total number of bits allocated
*/
void compute_allocation(const CELTMode *m, int start, int end, int *offsets, int total, int *pulses, int *ebits, int *fine_priority, int _C, int M);


#endif
