#!/bin/bash
#SBATCH -J Test        # Job Name
#SBATCH -o Test.o%j    # Output and error file name (%j expands to jobID)
#SBATCH -n 16           # Total number of  tasks requested
#SBATCH -p development  # Queue (partition) name -- normal, development, etc.
#SBATCH -t 1:30:00     # Run time (hh:mm:ss) - 1.5 hours

export CILK_NWORKERS=16

rm *.csv *.plt Test*

export N_VERTICES=$1
M_VAL=256 cilkview PMSSM >> PMSSM_out
