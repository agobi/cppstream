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

extern stream<double> s2;
stream<double> s1(1.0<<=s2),s2(.0<<=s1);

stream<int> s3 = 1<<=s3;
stream<int> s4 = s3+s3;
stream<int> s5 = s3+s5;
stream<int> s6 = s6+s3;
stream<int> pm = 1<<=(-pm);

int main()
{
    compare(s1, {1., 0.});
    compare(s2, {0., 1.});
    compare(s3, {1});
    compare(pm, {1,-1});
	return 0;
}
