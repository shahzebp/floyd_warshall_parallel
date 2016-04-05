#!/bin/bash
#SBATCH -J Test        # Job Name
#SBATCH -o Test.o%j    # Output and error file name (%j expands to jobID)
#SBATCH -n 16           # Total number of  tasks requested
#SBATCH -p development  # Queue (partition) name -- normal, development, etc.
#SBATCH -t 1:30:00     # Run time (hh:mm:ss) - 1.5 hours

export CILK_NWORKERS=16

rm *.csv *.plt Test*

for (( c = 2; c <= 32768; c = c*2))
do
	export N_VERTICES=$1
	echo -n $c" " >> PMSSM_out
	M_VAL=$c ./PMSSM >> PMSSM_out
done
