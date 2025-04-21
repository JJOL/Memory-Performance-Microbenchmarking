#!/bin/sh

#SBATCH --job-name=PeakBW
#SBATCH -D .
#SBATCH --output=p_%j.out
#SBATCH --error=p_%j.error
#SBATCH --time=00:05:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=56
#SBATCH --constrain=perfparanoid
#SBATCH --qos=gp_debug
#SBATCH -A nct_323

export SRUN_CPUS_PER_TASK=${SLURM_CPUS_PER_TASK}

numactl --cpunodebind=0 --membind=0 ./stream
numactl --cpunodebind=0 --membind=1 ./stream
#numactl --cpunodebind=0 --membind=0 ./mbw-O3
#numactl --cpunodebind=0 --membind=1 ./mbw-O3
