#!/bin/bash
#SBATCH --constraint=perfparanoid
#SBATCH --account=nct_323
#SBATCH --qos=gp_training
#SBATCH --time=00-01:00:00
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH -c 112

##running on basic GPP partition

module load likwid

outfile=cache_coherence.csv
echo 'Performance when sharing cache blocks between cores' > $outfile
echo '#cores,ns per access, operation' >> $outfile
likwid-pin -q -c E:N:112:1:2 ./src/build/cc_read >> $outfile
likwid-pin -q -c E:N:112:1:2 ./src/build/cc_readTwice >> $outfile
likwid-pin -q -c E:N:112:1:2 ./src/build/cc_aread >> $outfile
likwid-pin -q -c E:N:112:1:2 ./src/build/cc_fread >> $outfile

likwid-pin -q -c E:N:112:1:2 ./src/build/cc_write >> $outfile
likwid-pin -q -c E:N:112:1:2 ./src/build/cc_writeTwo >> $outfile
likwid-pin -q -c E:N:112:1:2 ./src/build/cc_awrite >> $outfile
likwid-pin -q -c E:N:112:1:2 ./src/build/cc_fwrite >> $outfile

likwid-pin -q -c E:N:112:1:2 ./src/build/cc_update >> $outfile
likwid-pin -q -c E:N:112:1:2 ./src/build/cc_atomicUpdate >> $outfile

