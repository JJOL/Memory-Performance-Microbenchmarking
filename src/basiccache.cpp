#include <omp.h>
#include <cstring>
#include <string>
#include "cache.hpp"
#include "mbench-common.h"
#include <thread>
#include <chrono>
#include <iostream>

volatile char l2buf[L2_SIZE/2];
volatile char l3buf[L3_SIZE/2];
void overwrite_l1(void)
{
    for (int i = 0; i < L2_SIZE/2; i++)
        l2buf[i] = (char)((i * 375 + 123) % 13);
}
void overwrite_l2(void)
{
    for (int i = 0; i < L3_SIZE/2; i++)
        l3buf[i] = (char)((i * 375ll + 123) % 13);
}

int main(void)
{
	uint64_t start, end;
	uint32_t  cycles_low, cycles_high, cycles_low1, cycles_high1;

    volatile double *x = new double;

    /* "warmup" TLB */
    for (int i = 0; i < 10; i++)
    {
        (*x) += i;
    }

    constexpr int num_trials = 16;

    std::cout << "From L1: ";
    for (int i = 0; i < num_trials; i++)
    {
        start_time(cycles_low, cycles_high);
        /**************************************/
        clread(x);
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
        cacheflush((double *)x, 8);

        start_time(cycles_low, cycles_high);
        /**************************************/
        clread(x);
        /**************************************/
        end_time(cycles_low1, cycles_high1);

        start = ((uint64_t)cycles_high << 32) | cycles_low;
        end = ((uint64_t)cycles_high1 << 32) | cycles_low1;

        std::cout << end - start << " ";
    }
    std::cout << std::endl;

    std::cout << "From L2/L3?: ";
    for (int i = 0; i < num_trials; i++)
    {
        cldemote((double *)x);
        
        start_time(cycles_low, cycles_high);
        /**************************************/
        clread(x);
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
        overwrite_l1();
        
        start_time(cycles_low, cycles_high);
        /**************************************/
        clread(x);
        /**************************************/
        end_time(cycles_low1, cycles_high1);

        start = ((uint64_t)cycles_high << 32) | cycles_low;
        end = ((uint64_t)cycles_high1 << 32) | cycles_low1;

        std::cout << end - start << " ";
    }
    std::cout << std::endl;

    std::cout << "From L3?: ";
    for (int i = 0; i < num_trials; i++)
    {
        overwrite_l2();
        
        start_time(cycles_low, cycles_high);
        /**************************************/
        clread(x);
        /**************************************/
        end_time(cycles_low1, cycles_high1);

        start = ((uint64_t)cycles_high << 32) | cycles_low;
        end = ((uint64_t)cycles_high1 << 32) | cycles_low1;

        std::cout << end - start << " ";
    }
    std::cout << std::endl;

    delete x;
    return 0;
}