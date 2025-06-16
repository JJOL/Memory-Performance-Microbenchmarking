#include <omp.h>
#include <cstring>
#include <string>
#include "cache.hpp"
#include "mbench-common.h"
#include <thread>
#include <chrono>
#include <iostream>

/* Sum: a <- a + b, 2N loads, N stores */
void kernel(volatile double *a, volatile double *b, int N, int thrd_id, int num_threads, int num_iterations)
{
    int l = (N / num_threads) * thrd_id;
    int u = (N / num_threads) * (thrd_id+1);
    for (int it = 0; it < num_iterations; it++)
    {
        for (int i = l; i < u; i++)
        {
            a[i] = b[i] - a[i];
        }
    }
}

void measure_mem_latency(void)
{
	uint64_t start, end;
	uint32_t  cycles_low, cycles_high, cycles_low1, cycles_high1;

    /* timer overhead "calibration" (note: assuming that overhead does not overlap may be wrong assumption) */
    double avg_timer_overhead = 0.0;
    for (int i = 0; i < 1000; i++)
    {
        start_time(cycles_low, cycles_high);
        end_time(cycles_low1, cycles_high1);

        start = ((uint64_t)cycles_high << 32) | cycles_low;
        end = ((uint64_t)cycles_high1 << 32) | cycles_low1;

        avg_timer_overhead += end - start;
    }
    avg_timer_overhead /= 1000;

    volatile double *x = new double;

    /* "warmup" TLB */
    for (int i = 0; i < 10; i++)
    {
        (*x) += i;
    }
    
    constexpr int num_trials = 1000;
    double *times = new double[num_trials];
    for (int i = 0; i < num_trials; i++)
    {
        cacheflush((double *)x, 8);
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));

        start_time(cycles_low, cycles_high);
        /**************************************/
        clread(x);
        /**************************************/
        end_time(cycles_low1, cycles_high1);

        start = ((uint64_t)cycles_high << 32) | cycles_low;
        end = ((uint64_t)cycles_high1 << 32) | cycles_low1;

        times[i] = end - start - avg_timer_overhead;
    }

    print_result(times, num_trials, "Access latency");

    delete[] times;
    delete x;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Need to specify exactly one command line argument: The number of threads to use!\n";
        exit(0);
    }

    int num_threads = std::stoi(argv[1]);

    int N = 112*16*1024*1024; // enough to definitely not fit into L3
    double *a = new double[N];
    double *b = new double[N];

    std::memset(b, 1, N*sizeof(double));
    std::memset(b, 2, N*sizeof(double));

#pragma omp parallel num_threads(num_threads)
{
    int id = omp_get_thread_num();
    if (id == 0)
    {
        if (num_threads != omp_get_num_threads()) 
            exit(1);
        measure_mem_latency();
        exit(0); // abort (OS will clean up)
    }
    else
    {
        kernel(a,b,N, id, num_threads, 2000000000);
    }
}
    delete[] a;
    delete[] b;
    return 0;
}
