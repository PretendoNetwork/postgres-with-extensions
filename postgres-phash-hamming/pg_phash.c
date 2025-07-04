#include <postgres.h>
#include <fmgr.h>
#include <stdlib.h>
#include <string.h>
#include <utils/datum.h>
#include <varatt.h>

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 ulong64;
#else
typedef unsigned long long ulong64;
#endif

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

int32 ph_hamming_distance(const ulong64 hash1, const ulong64 hash2);

PG_FUNCTION_INFO_V1(phash_hamming);
Datum phash_hamming(PG_FUNCTION_ARGS) {
    VarChar *ptr1;
    VarChar *ptr2;
    unsigned char size1;
    unsigned char size2;
    char *varchar1;
    char *varchar2;
    ulong64 long1;
    ulong64 long2;
    int32 distance;
    
    // Prevent null pointers, which will cause segfault
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1)) {
        PG_RETURN_NULL();
    }

    // Get varchar pointers
    ptr1 = PG_GETARG_VARCHAR_P(0);
    ptr2 = PG_GETARG_VARCHAR_P(1);

    // Get varchar sizes
    size1 = VARSIZE(ptr1) - VARHDRSZ;
    size2 = VARSIZE(ptr2) - VARHDRSZ;

    // Extract content to new memory space
    varchar1 = palloc(sizeof (char) * (size1 + 1));
    memcpy(varchar1, (void*) VARDATA(ptr1), size1);
    varchar1[size1] = '\0';

    varchar2 = palloc(sizeof (char) * (size2 + 1));
    memcpy(varchar2, (void*) VARDATA(ptr2), size2);
    varchar2[size2] = '\0';

    // Convert number strings to ulong64
    long1 = strtoull(varchar1, NULL, 10);
    long2 = strtoull(varchar2, NULL, 10);

    pfree(varchar1);
    pfree(varchar2);

    // Compute hamming distance
    distance = ph_hamming_distance(long1, long2);

    PG_RETURN_INT32(distance);
}

/*
    pHash, the open source perceptual hash library
    Copyright (C) 2009 Aetilius, Inc.
    All rights reserved.
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Evan Klinger - eklinger@phash.org
    D Grant Starkweather - dstarkweather@phash.org
*/

int32 ph_hamming_distance(const ulong64 hash1, const ulong64 hash2){
    const ulong64 m1  = 0x5555555555555555ULL;
    const ulong64 m2  = 0x3333333333333333ULL;
    const ulong64 h01 = 0x0101010101010101ULL;
    const ulong64 m4  = 0x0f0f0f0f0f0f0f0fULL;
    
    ulong64 x = hash1 ^ hash2;
    x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    return (x * h01)>>56;
}
