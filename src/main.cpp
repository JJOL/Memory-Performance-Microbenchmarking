#include "mbench-common.h"

#include <malloc.h>

/**
 * main (adjst parameters here)
 */
int main(void)
{
	/* benchmark metaparameters */
	const int Nmax = 1000;

	double* times = (double*)malloc(Nmax*sizeof(double));
	if (!times) {
		printf("unable to allocate memory for times\n");
		return 1;
	}

	measure_load_to_use(times, Nmax, 1);
	print_result(times, Nmax, "Load-to-use latency");

	free(times);
	return 0;
}
