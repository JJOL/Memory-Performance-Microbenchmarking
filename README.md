# Memory Performance Microbenchmarking

Code and scripts used to microbenchmark the Memory Hierarchy in BSC's MareNostrum 5. The relevant code is
* stream.c: For benchmarking Peak Bandwidth (GB/s) using the STREAM benchmark.
* MemoryLatency.c: For measuring Cache Access Latency
* multithreaded_pbw.cpp: For measuring also Peak Bandwidth.

Experiments were run from script ```run-experiment.sh``` in SLURM and using _numactl_ to control NUMA policy.