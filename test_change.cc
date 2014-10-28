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
#include <unordered_map>
#include <map>
#include <cstdlib>
#include <sys/time.h>
#include <libgen.h>

long timeval_diff(struct timeval &tv1, struct timeval &tv2)
{
    long ret = tv1.tv_usec - tv2.tv_usec;
    if(ret<0) ret += 1000000;
    return ret;
}


long change(int n, int max=50) {
    if(n<0)
        return 0;

    if(n==0)
        return 1;

    long ret = 0;
    if(n>=1 && max>=1) ret+=change(n-1, 1);
    if(n>=2 && max>=2) ret+=change(n-2, 2);
    if(n>=5 && max>=5) ret+=change(n-5, 5);
    if(n>=10 && max>=10) ret+=change(n-10, 10);
    if(n>=20 && max>=20) ret+=change(n-20, 20);
    if(n>=50 && max>=50) ret+=change(n-50, 50);
    return ret;
}

long changef(int n, int max=4) {
    if(n<0)
        return 0;

    long ret = 1;
    switch(max) {
        case 4: ret += changef(n-50, 4);
        case 3: ret += changef(n-20, 3);
        case 2: ret += changef(n-10, 2);
        case 1: ret += changef(n-5, 1);
        case 0: ret += changef(n-2, 0);
    }
    return ret;
}

long changedyn0(int n, int max=4) {
    if(n<0)
        return 0;

    if(max<0)
        return 1;

    static std::map<int,long> cache[5];
    std::map<int,long>::iterator it = cache[max].find(n);
    if(it != cache[max].end())
        return cache[max][n];

    long ret = 1;
    switch(max) {
        case 4: ret += changedyn0(n-50, 4);
        case 3: ret += changedyn0(n-20, 3);
        case 2: ret += changedyn0(n-10, 2);
        case 1: ret += changedyn0(n-5, 1);
        case 0: ret += changedyn0(n-2, 0);
    }
    cache[max][n] = ret;
    return ret;
}

long changedyn(int n, int max=4) {
    if(n<0)
        return 0;

    if(max<0)
        return 1;

    static std::unordered_map<int,long> cache[5];
    std::unordered_map<int,long>::iterator it = cache[max].find(n);
    if(it != cache[max].end())
        return cache[max][n];

    long ret = 1;
    switch(max) {
        case 4: ret += changedyn(n-50, 4);
        case 3: ret += changedyn(n-20, 3);
        case 2: ret += changedyn(n-10, 2);
        case 1: ret += changedyn(n-5, 1);
        case 0: ret += changedyn(n-2, 0);
    }
    cache[max][n] = ret;
    return ret;
}

template<typename ST>
stream<long> times(long n, long val, ST && s)
{
    if(n==0) return s;
    return times(n-1, val, val<<=std::forward<ST>(s));
}

stream<long> change1 = 1l<<=change1;
stream<long> change2 = times(2, 0, change2) + change1;
stream<long> change5 = times(5, 0, change5) + change2;
stream<long> change10 = times(10, 0, change10) + change5;
stream<long> change20 = times(20, 0, change20) + change10;
stream<long> change50 = times(50, 0, change50) + change20;

int main(int argc, char *argv[]) {
    if (argc != 3)
    {
        std::cout<<" Usage: "<<basename(argv[0])<<" N X"<<std::endl
                 <<"     N : Number to change"<<std::endl
                 <<"     X : Algorithm"<<std::endl
                 <<"         1 : map"<<std::endl
                 <<"         2 : unordered map"<<std::endl
                 <<"         3 : stream"<<std::endl
                 <<std::endl;
        return 1;
    }

    const int n = atoi(argv[1]);
    const int x = atoi(argv[2]);

    struct timeval tv1, tv2;
    struct timezone tz;
    switch(x) {
        case 1:
            gettimeofday(&tv1, &tz);
            changedyn0(n);
            gettimeofday(&tv2, &tz);
            break;

        case 2:
            gettimeofday(&tv1, &tz);
            changedyn(n);
            gettimeofday(&tv2, &tz);
            break;

        default:
            std::cerr<<"Unknown algorighm"<<std::endl;
            return 1;

        case 3:
            gettimeofday(&tv1, &tz);
            stream<long>::iterator it = change50.begin();
            for(int i=0; i<n; ++i) ++it;
            *it;
            gettimeofday(&tv2, &tz);
            break;

    }
    std::cout<<timeval_diff(tv2, tv1)<<std::endl;
}
