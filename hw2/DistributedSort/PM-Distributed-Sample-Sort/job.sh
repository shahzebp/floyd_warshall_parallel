#!/bin/bash
#SBATCH -J Test        # Job Name
#SBATCH -o Test.o%j    # Output and error file name (%j expands to jobID)
#SBATCH -n 5           # Total number of  tasks requested
#SBATCH -N 5           # Total number of  tasks requested
#SBATCH -p development # Queue (partition) name -- normal, development, etc.
#SBATCH -t 1:00:00     # Run time (hh:mm:ss) - 1.5 hours

#export CILK_NWORKERS=32

export LD_LIBRARY_PATH=$HOME/cilk/lib64/:$PWD:$LD_LIBRARY_PATH

for i in 1000 97613800 195226600 292839400 390452200 488065000 585677800 683290600 780903400 878516200 878516200 976129000 1073741800
do 
	ibrun tacc_affinity a.out $i >> DSTTime
done
