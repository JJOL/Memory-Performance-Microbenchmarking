#include "mbench-common.h"

#include <inttypes.h>
#include <stdio.h>

/**
 * benchmark (adjust code here).
 * 
 * Measurements are repeated for num_ens ensembles each of size ens_size.
 * (Several ensembles can be useful to verify benchmark accuracy or increase/decrease
 * workload. Besides that the effect is the same as increasing ens_size.)
 * 
 * Parameters:
 * times		array to fill measured times with (needs to be preallocated, size == ens_size*num_ens)
 * ens_size		ensemble size
 * num_ens		number of ensembles
 */
void measure_load_to_use(double *times, int ens_size, int num_ens)
{
	int i, j;
	uint64_t start, end;
	uint32_t  cycles_low, cycles_high, cycles_low1, cycles_high1;

	/* warm up instruction cache */
	start_time(cycles_low, cycles_high);
	end_time(cycles_low1, cycles_high1);
	start_time(cycles_low, cycles_high);
	end_time(cycles_low1, cycles_high1);
	start_time(cycles_low, cycles_high);
	end_time(cycles_low1, cycles_high1);

	for (j=0; j<num_ens; j++) {
		for (i=0; i<ens_size; i++) {

			uint64_t x;
			uint64_t x_ptr = (uint64_t)&x;
			x = x_ptr;

			start_time(cycles_low, cycles_high);
			/**************************************/
			/* call the function to benchmark here*/
			asm volatile (
				".rept 10000				\n\t"
				"mov %[x], QWORD PTR [%[x]]	\n\t"
				".endr						\n\t"
				: [x] "+r" (x_ptr)
				: 
				: "rax", "memory"
			);
			/**************************************/
			end_time(cycles_low1, cycles_high1);

			start = ((uint64_t)cycles_high << 32) | cycles_low;
			end = ((uint64_t)cycles_high1 << 32) | cycles_low1;

			times[i + j*ens_size] = (double)(end - start) / 10000;
		}
	}
	return;
}
