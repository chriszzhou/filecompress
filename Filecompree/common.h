#pragma once
typedef unsigned char UCH;
typedef unsigned short USH;
typedef unsigned long long ULL;

const USH MIN_MATCH = 3;
const USH MAX_MATCH = 258;
const USH MIN_LOOKHEAD = MAX_MATCH + MIN_MATCH + 1;
const USH WSIZE = 32 * 1024;//32k
const USH MAX_DIST = WSIZE - MIN_LOOKHEAD;
const USH HASH_BITS = 15;
const USH HASH_SIZE = (1 << HASH_BITS);
const USH HASH_MASK = HASH_SIZE - 1;


