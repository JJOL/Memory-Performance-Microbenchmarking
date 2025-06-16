#pragma once
#include <x86intrin.h>

#define CACHE_LINE_SIZE 64

#define L1_SIZE (48*1024)
#define L2_SIZE (2048*1024)
#define L3_SIZE (105*1024*1024)

static inline void cacheflush(void *address, int num_bytes)
{
    char *addr = (char*)address;
    _mm_mfence(); // ensure that previous memory accesses complete before flushing cache lines
    for (int idx = 0; idx < num_bytes; idx+=CACHE_LINE_SIZE)
        _mm_clflushopt(&addr[idx]);
    _mm_clflushopt(&addr[num_bytes-1]);
    _mm_mfence(); // ensure that flushes complete before accessing further memery
}

static inline void clread(volatile void* address)
{
    volatile char* addr = (volatile char*)address;
    *addr;
}

static inline void cldemote(void *address)
{
    _cldemote(address);
}