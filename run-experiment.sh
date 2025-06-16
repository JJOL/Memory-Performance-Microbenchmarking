#!/bin/sh

#SBATCH --job-name=PeakBW
#SBATCH -D .
#SBATCH --output=memlat_%j.out
#SBATCH --error=memlat_%j.error
#SBATCH --time=00:15:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=112
#SBATCH --constraint=perfparanoid
#SBATCH --exclusive
#SBATCH --qos=gp_debug
#SBATCH -A nct_323

export SRUN_CPUS_PER_TASK=${SLURM_CPUS_PER_TASK}

# numactl --cpunodebind=0 --membind=0 ./stream
# numactl --cpunodebind=0 --membind=1 ./stream
# numactl --interleave=all ./stream
# numactl --cpunodebind=0 --membind=0 ./stream_v0
# numactl --interleave=all ./stream_v0

# ./pbw
# ./MemoryLatency -test mlp
numactl --cpunodebind=0 --membind=0 ./mpbw Local
mv mpbw.csv local-mpbw.csv 
sleep 1
numactl --cpunodebind=0 --membind=1 ./mpbw Remote
mv mpbw.csv remote-mpbw.csv
sleep 1
numactl --interleave=all ./mpbw _Inter
mv mpbw.csv interleave-mpbw.csv
# numactl --cpunodebind=0 --membind=0 ./pbw
# numactl --cpunodebind=0 --membind=1 ./pbw