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

#ifndef __TEST_COMMON_H__
#define __TEST_COMMON_H__


template<typename T>
void prints(const stream<T> &s, size_t n=10)
{
    typename stream<T>::iterator it=s.begin();
    for(size_t i=1; i<n; ++i) 
    {
        std::cout<<*it<<" ";
        ++it;
    }
    std::cout<<std::endl;
}

template<typename T>
void stress(const stream<T> &s)
{
    clock_t t1,t2;
    t1=clock();
    typename stream<T>::iterator it=s.begin();
    for(int i=1; i<10000; ++i) {
        *it;
        ++it;
    }
    t2=clock();
    std::cout<<(double(t2)-double(t1))/CLOCKS_PER_SEC<<std::endl;
}

template<typename T>
void compare(const stream<T> &s, const std::initializer_list<T> &a, int pass=3)
{

    typename stream<T>::iterator sit = s.begin();
    for(int i=0; i<pass; ++i)
    {
        typename std::initializer_list<T>::iterator ait = a.begin();

        while(ait != a.end())
        {
            assert(*ait==*sit);
            std::cout<<*ait<<" ";
            ++ait; ++sit;
        }
    }
    std::cout<<std::endl;
}

#endif//__TEST_COMMON_H__
