#!/bin/bash
#SBATCH --constraint=perfparanoid
#SBATCH --account=nct_323
#SBATCH --qos=gp_training
#SBATCH --time=00-10:00:00
#SBATCH --nodes=1
#SBATCH --exclusive

##running on basic GPP partition

module load likwid

# Allocate all memory on S0
# With -w option, by default the first thread within a workgroup allocates all memory.
outfile=effective_mem_bandwidth_allMemOnS0.csv
echo 'Effective bandwidth (memory allocated on socket 0)' > $outfile
echo '#cores,MB/s,access pattern' >> $outfile
for benchmark in copy_mem_avx512 load_avx512 store_mem_avx512 stream_mem_avx512
do
	for num_threads in $(seq 1 112)
	do
		likwid-bench -t $benchmark -w N:$(($num_threads*1))GB:${num_threads}:1:2 | tee -a log-${outfile}.log | echo "$num_threads,$(grep -oP 'MByte/s:\s*\K[0-9.]+'),$benchmark" >> $outfile
	done
done

## Allocate memory local to threads
## Using -W option, each thread allocates its own memory locally.
outfile=effective_mem_bandwidth_MemLocal.csv
echo 'Effective bandwidth (memory allocated on local socket)' > $outfile
echo '#cores,MB/s,access pattern' >> $outfile
for benchmark in copy_mem_avx512 load_avx512 store_mem_avx512 stream_mem_avx512
do
	for num_threads in $(seq 1 112)
	do
		likwid-bench -t $benchmark -W N:$(($num_threads*1))GB:${num_threads}:1:2 | tee -a log-${outfile}.log | echo "$num_threads,$(grep -oP 'MByte/s:\s*\K[0-9.]+'),$benchmark" >> $outfile
	done
done

# Allocate memory on opposite node
# With -w option we can specify custom placement for each stream (note that
# streams refers to the arrays). 
# Placement is specified independently for different workgroups and has to be
# specified every time for each stream (or more, but not less).
outfile=effective_mem_bandwidth_MemRemote.csv
echo 'Effective bandwidth (memory allocated on remote socket)' > $outfile
echo '#cores,MB/s,access pattern' >> $outfile
for benchmark in copy_mem_avx512 load_avx512 store_mem_avx512 stream_mem_avx512
do
	for num_threads in $(seq 1 56)
	do
		likwid-bench -t $benchmark -w S0:$(($num_threads*1))GB:${num_threads}:1:2-0:S1,1:S1,2:S1 | tee -a log-${outfile}.log | echo "$num_threads,$(grep -oP 'MByte/s:\s*\K[0-9.]+'),$benchmark" >> $outfile
	done
	for num_threads in $(seq 1 56)
	do
		likwid-bench -t $benchmark -w S0:$((56*1))GB:56:1:2-0:S1,1:S1,2:S1 -w S1:$(($num_threads*1))GB:${num_threads}:1:2-0:S0,1:S0,2:S0 | tee -a log-${outfile}.log | echo "$(($num_threads+56)),$(grep -oP 'MByte/s:\s*\K[0-9.]+'),$benchmark" >> $outfile
	done
done

