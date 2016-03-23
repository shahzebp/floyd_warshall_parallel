#!/bin/bash
#SBATCH -J Test        # Job Name
#SBATCH -o Test.o%j    # Output and error file name (%j expands to jobID)
#SBATCH -n 32           # Total number of  tasks requested
#SBATCH -p normal  # Queue (partition) name -- normal, development, etc.
#SBATCH -t 1:30:00     # Run time (hh:mm:ss) - 1.5 hours

#export CILK_NWORKERS=32

rm *.csv *.plt Test*

export N_VERTICES=$1

for i in 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192
do 
#	M_VAL=$i ./fw 	> a_fw_output_$i
#	M_VAL=$i ./fwr1 > a_fwr1_output_$i
	M_VAL=$i ./fwr2 > a_fwr2_output_$i
#	M_VAL=$i ./fwr3 > a_fwr3_output_$i
done
