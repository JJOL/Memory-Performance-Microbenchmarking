#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <time.h>

void measure_load_to_use(double *times, int ens_size, int num_ens);

/* Return current time, in milliseconds */
long long get_wtime(void)
{
#ifdef _WIN32
    return GetTickCount64();
#else // assume __linux__
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    return time.tv_sec*1000 + time.tv_nsec/1000000;
#endif
}

/**
 * Calculate the sample mean of inputs[0:N].
 */
inline double mean(double *inputs, int N)
{
	double acc = 0.0;
	for (int i = 0; i < N; i++)
	{
		acc += inputs[i];
	}
	return acc / N;
}

/**
 * Calculate the (unbiased) sample variance of inputs[0:N].
 */
inline double var(double *inputs, int N)
{
	double acc = 0.0, acc2 = 0.0;
	for (int i = 0; i < N; i++)	
	{
		const double xi = inputs[i];
		acc += xi; 
		acc2 += xi*xi;
	}
	return (acc2 - acc*acc/N) / (N-1);
}

/**
 * Print mean and variance of a measurement to stdout
 */
static inline void print_result(double *times, int N, const char *text)
{
    printf("%s:,%f,variance,%f\n", text, mean(times, N), var(times, N));
}

void mjb_workhorse(int ensemble_size, int num_ensembles, int num_top_mins, bool out_raw, bool out_proces, int out_summary, bool out_nspurmins, const char *colsep);
