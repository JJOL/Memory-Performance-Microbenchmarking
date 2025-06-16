#include <omp.h>
#include <cstring>
#include <string>
#include "cache.hpp"
#include "mbench-common.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <cstdlib>

__attribute__((optimize("O0")))
void warmup_tlb(double *x, int pg_size, int N)
{
    /* "warmup" TLB */
    for (int j = 0; j < 100; j++)
    {
        for (int i = 0; i < N; i++)
        {
            x[i*pg_size] += i;
        }
    }
}

__attribute__((optimize("O0")))
void cache_other_xis(volatile double *x, int pg_size, int N)
{
    /* "warmup" TLB */
    for (int j = 0; j < 100*N*N + (std::rand() % N); j++)
    {
        int i = 1 + (std::rand() % (N-1));
        x[i*pg_size] -= i;
    }
}

int main(void)
{
	uint64_t start, end;
	uint32_t  cycles_low, cycles_high, cycles_low1, cycles_high1;

    const int pg_size = 4096;
    const int N = 8;
    double *x = new (std::align_val_t(pg_size)) double[N*pg_size];

    warmup_tlb(x, pg_size, N);

    const int num_trials = 16;

    std::cout << "From L1: ";
    for (int i = 0; i < num_trials; i++)
    {
        clread(&x[0]);

        start_time(cycles_low, cycles_high);
        /**************************************/
        clread(&x[0]);
        /**************************************/
        end_time(cycles_low1, cycles_high1);

        start = ((uint64_t)cycles_high << 32) | cycles_low;
        end = ((uint64_t)cycles_high1 << 32) | cycles_low1;

        std::cout << end - start << " ";
    }
    std::cout << std::endl;

    std::cout << "From RAM: ";
    for (int i = 0; i < num_trials; i++)
    {
        cacheflush(&x[0], 8);

        start_time(cycles_low, cycles_high);
        /**************************************/
        clread(&x[0]);
        /**************************************/
        end_time(cycles_low1, cycles_high1);

        start = ((uint64_t)cycles_high << 32) | cycles_low;
        end = ((uint64_t)cycles_high1 << 32) | cycles_low1;

        std::cout << end - start << " ";
    }
    std::cout << std::endl;

    std::cout << "From L2?: ";
    for (int i = 0; i < num_trials; i++)
    {
        clread(&x[0]);
        _mm_mfence();
        cache_other_xis(x, pg_size,N);
        
        start_time(cycles_low, cycles_high);
        /**************************************/
        clread(&x[0]);
        /**************************************/
        end_time(cycles_low1, cycles_high1);

        start = ((uint64_t)cycles_high << 32) | cycles_low;
        end = ((uint64_t)cycles_high1 << 32) | cycles_low1;

        std::cout << end - start << " ";
    }
    std::cout << std::endl;

    operator delete[] (x, std::align_val_t(pg_size));
    return 0;
}