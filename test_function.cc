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

template<typename T>
stream<int> helper(int x, T &&s)
{
    if(x==0) {
        return stream<int>(s);
	} else {
		--x;
        return helper(x, x<<=std::forward<T>(s));
	}
}

int main()
{
    stream<int> s =helper(1, s);
    stream<int> s1=helper(2, s1);
    stream<int> s2=helper(3, 9<<=s2);
    stream<int> s3=helper(0, 9<<=s3);
    stream<int> s4=helper(1, 9<<=s4);
    compare(s, {0});
    compare(s1, {0,1});
    compare(s2, {0,1,2,9});
    compare(s3, {9});
    compare(s4, {0,9});
}
