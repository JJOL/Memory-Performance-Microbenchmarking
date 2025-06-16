#!/bin/bash
#SBATCH --constraint=perfparanoid
#SBATCH --account=nct_323
#SBATCH --qos=gp_training
#SBATCH --time=00-00:10:00
#SBATCH --nodes=1
#SBATCH --exclusive

##running on basic GPP partition

module load likwid

outfile=load2use.txt
echo 'Load to use latency (L1 cache hit)' > $outfile
echo 'Cycles, with respect to base frequency' >> $outfile

for i in $(seq 1 10)
do
    likwid-pin -q -c 1 ./src/build/l2u >> $outfile
done