#!/bin/bash
#SBATCH -J Test        # Job Name
#SBATCH -o Test.o%j    # Output and error file name (%j expands to jobID)
#SBATCH -n 16           # Total number of  tasks requested
#SBATCH -p normal  # Queue (partition) name -- normal, development, etc.
#SBATCH -t 01:30:00     # Run time (hh:mm:ss) - 1.5 hours

export CILK_NWORKERS=32

./fw > out
./fw1 > out1
./fw2 > out2

