#include <omp.h>
#include <cstring>
#include <string>
#include "cache.hpp"
#include "Mbench-common.h"
#include <thread>
#include <chrono>
#include <iostream>

void read(volatile double *x, long num_iterations)
{
    for (int i = 0; i < num_iterations; i++)
    {
        asm volatile (
            "movsd xmm0, QWORD PTR [%[x]]	\n\t"
            : [x] "+r" (x)
            : 
            : "xmm0", "memory"
        );
    }
}

void readTwice(volatile double *x, long num_iterations)
{
    for (int i = 0; i < num_iterations; i+=2)
    {
        asm volatile (
            "movsd xmm0, QWORD PTR [%[x]]	\n\t"
            "movsd xmm1, QWORD PTR [%[x]]	\n\t"
            : [x] "+r" (x)
            : 
            : "xmm0", "xmm1", "memory"
        );
    }
}

void alternatingRead(volatile double *x, long num_iterations)
{
    volatile double *y = &x[1]; // any other entry
    for (int i = 0; i < num_iterations; i+=2)
    {
        asm volatile (
            "movsd xmm0, QWORD PTR [%[x]]	\n\t"
            "movsd xmm0, QWORD PTR [%[y]]	\n\t"
            : [x] "+r" (x), [y] "+r" (y)
            : 
            : "xmm0", "memory"
        );
    }
}

void fencedRead(volatile double *x, long num_iterations)
{
    for (int i = 0; i < num_iterations; i++)
    {
        asm volatile (
            "movsd xmm0, QWORD PTR [%[x]]	\n\t"
            "MFENCE                         \n\t"
            : [x] "+r" (x)
            : 
            : "xmm0", "memory"
        );
    }
}

void write(volatile double *x, long num_iterations)
{
    for (int i = 0; i < num_iterations; i++)
    {
        asm volatile (
            "movsd QWORD PTR [%[x]], xmm0	\n\t"
            : [x] "+r" (x)
            : 
            : "xmm0", "memory"
        );
    }
}

void writeTwo(volatile double *x, long num_iterations)
{
    for (int i = 0; i < num_iterations; i+=2)
    {
        asm volatile (
            "movsd QWORD PTR [%[x]], xmm0	\n\t"
            "movsd QWORD PTR [%[x]], xmm1	\n\t"
            : [x] "+r" (x)
            : 
            : "xmm0", "xmm1", "memory"
        );
    }
}

void alternatingWrite(volatile double *x, long num_iterations)
{
    volatile double *y = &x[64/sizeof(double)]; // at least one cache line apart
    for (int i = 0; i < num_iterations; i+=2)
    {
        asm volatile (
            "movsd QWORD PTR [%[x]], xmm0	\n\t"
            "movsd QWORD PTR [%[y]], xmm0	\n\t"
            : [x] "+r" (x), [y] "+r" (y)
            : 
            : "xmm0", "memory"
        );
    }
}

void fencedWrite(volatile double *x, long num_iterations)
{
    for (int i = 0; i < num_iterations; i++)
    {
        asm volatile (
            "movsd QWORD PTR [%[x]], xmm0	\n\t"
            "MFENCE                         \n\t"
            : [x] "+r" (x)
            : 
            : "xmm0", "memory"
        );
    }
}

void update(volatile double *x, long num_iterations)
{
    for (int i = 0; i < num_iterations; i++)
    {
        asm volatile (
            "movsd xmm0, QWORD PTR [%[x]]	\n\t"
            "movsd QWORD PTR [%[x]], xmm0	\n\t"
            : [x] "+r" (x)
            : 
            : "xmm0", "memory"
        );
    }
}

void atomicUpdate(volatile double *x, long num_iterations)
{
    for (int i = 0; i < num_iterations; i++)
    {
        asm volatile (
            "lock add QWORD PTR [%[x]], 1 	\n\t" // act like it is an integer
            : [x] "+r" (x)
            : 
            : "memory"
        );
    }
}

#ifdef OP //////////////////////////////////////////////////////////////

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x

int main(void)
{
    volatile double *x = new double[128];

    /* "warmup" TLB */
    for (int i = 0; i < 100; i++)
    {
        (*x) = i;
    }

    for (int num_threads = 1; num_threads <= 112; num_threads++)
    {
        /* number of threads sanity check */
        #pragma omp parallel num_threads(num_threads)
        {
            int id = omp_get_thread_num();
            if (id == 0)
            {
                if (num_threads != omp_get_num_threads()) 
                {
                    std::cerr << "Error when allocating OpenMP threads" << std::endl;
                    exit(1); // let OS clean up
                }
            }
        }

        /* make measurements, increase num_its until at least 1000 ms to ensure precision */
        long num_its = 1000000;
        long long start, end;
        while (true)
        {
            start = get_wtime();
            #pragma omp parallel num_threads(num_threads)
            {
                OP(x, num_its);
            }
            end = get_wtime();
            if (end - start < 1000)
            {
                if (end - start == 0) num_its *= 1000;
                else if (end - start < 10) num_its *= 100;
                else if (end - start < 100) num_its *= 10;
                else if (end - start < 334) num_its *= 3;
                else num_its *= 2;
            }
            else break;
        }
        std::cout << num_threads << "," << 1000000.0 * (double)(end - start) / (double)num_its << "," << STRINGIFY(OP) << std::endl;
    }

    delete[] x;
    return 0;
}

#else //////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Need to specify exactly one command line argument: The number of threads to use!\n";
        exit(0);
    }

    int num_threads = std::stoi(argv[1]);

    /* number of threads sanity check */
#pragma omp parallel num_threads(num_threads)
{
    int id = omp_get_thread_num();
    if (id == 0)
    {
        if (num_threads != omp_get_num_threads()) 
        {
            std::cerr << "Error when allocating OpenMP threads" << std::endl;
            exit(1);
        }
    }
}

    volatile double *x = new double;
    long long start, end;

    /* "warmup" TLB */
    for (int i = 0; i < 100; i++)
    {
        (*x) = i;
    }

    const int it_multiplier = 1;

    /* measure shared read bandwidth */
    constexpr long num_reads = 1000000000 * it_multiplier;
    start = get_wtime();
    #pragma omp parallel num_threads(num_threads)
    {
    read(x, num_reads);
    }
    end = get_wtime();
    std::cout << "READ" << std::endl;
    std::cout << "Walltime: " << end - start << "ms" << std::endl;
    std::cout << "Nanoseconds per load: " << 1000000.0 * (end - start) / num_reads << std::endl;


    /* measure shared read+fence bandwidth */
    constexpr long num_freads = 100000000 * it_multiplier;
    start = get_wtime();
    #pragma omp parallel num_threads(num_threads)
    {
    fencedRead(x, num_freads);
    }
    end = get_wtime();
    std::cout << "READ (with fence)" << std::endl;
    std::cout << "Walltime: " << end - start << "ms" << std::endl;
    std::cout << "Nanoseconds per load: " << 1000000.0 * (end - start) / num_freads << std::endl;


    /* measure shared write bandwidth */
    constexpr long num_writes = 1000000000 * it_multiplier;
    start = get_wtime();
    #pragma omp parallel num_threads(num_threads)
    {
    write(x, num_writes);
    }
    end = get_wtime();
    std::cout << "WRITE" << std::endl;
    std::cout << "Walltime: " << end - start << "ms" << std::endl;
    std::cout << "Nanoseconds per write: " << 1000000.0 * (end - start) / num_writes << std::endl;

    /* measure shared write+fence bandwidth */
    constexpr long num_fwrites = 100000000 * it_multiplier;
    start = get_wtime();
    #pragma omp parallel num_threads(num_threads)
    {
    fencedWrite(x, num_fwrites);
    }
    end = get_wtime();
    std::cout << "WRITE (with fence)" << std::endl;
    std::cout << "Walltime: " << end - start << "ms" << std::endl;
    std::cout << "Nanoseconds per write: " << 1000000.0 * (end - start) / num_fwrites << std::endl;


    /* measure shared update bandwidth */
    constexpr long num_updates = 10000000 * it_multiplier;
    start = get_wtime();
#pragma omp parallel num_threads(num_threads)
{
    update(x, num_updates);
}
    end = get_wtime();
    std::cout << "UPDATE" << std::endl;
    std::cout << "Walltime: " << end - start << "ms" << std::endl;
    std::cout << "Nanoseconds per update: " << 1000000.0 * (end - start) / num_updates << std::endl;

    delete x;
    return 0;
}

#endif /////////////////////////////////////////////////////////////////