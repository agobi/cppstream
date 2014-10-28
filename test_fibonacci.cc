/*
 * Copyright (c) 2011-2012, Attila Gobi
 * All rights reserved.
 *
 * This software was developed by Attila Gobi.
 * The project was supported by the European Union and co-financed by the
 * European Social Fund (grant agreement no. TAMOP 4.2.1./B-09/1/KMR-2010-0003)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "stream.h"
#include "test_common.h"
#include <sys/time.h>

long timeval_diff(struct timeval &tv1, struct timeval &tv2)
{
    long ret = tv1.tv_usec - tv2.tv_usec;
    if(ret<0) ret += 1000000;
    return ret;
}


long fib(int n)
{
    long a=0,b=1;
    for(int i=0; i<n; ++i) {
        b+=a;
        a=b-a;
    }
    return a;
}


int main()
{
  stream<long> s = 0l<<=s+(1l<<=s);

  compare(s,  {0l,1l,1l,2l,3l,5l,8l,13l,21l,34l}, 1);

  const int n = 200000;
  struct timeval tv1, tv2;
  struct timezone tz;

  gettimeofday(&tv1, &tz);
  stream<long>::iterator it = s.begin();
  for(int i=0; i<n; ++i) ++it;
  *it;
  gettimeofday(&tv2, &tz);
  std::cout<<timeval_diff(tv2, tv1)<<std::endl;

  gettimeofday(&tv1, &tz);
  fib(n);
  gettimeofday(&tv2, &tz);
  std::cout<<timeval_diff(tv2, tv1)<<std::endl;

  return 0;
}
