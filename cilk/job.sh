#!/bin/bash
#SBATCH -J Test        # Job Name
#SBATCH -o Test.o%j    # Output and error file name (%j expands to jobID)
#SBATCH -n 32           # Total number of  tasks requested
#SBATCH -p normal  # Queue (partition) name -- normal, development, etc.
#SBATCH -t 01:30:00     # Run time (hh:mm:ss) - 1.5 hours
#SBATCH --mail-user=shahpatel@cs.stonybrook.edu
#SBATCH --mail-type=begin  # email me when the job starts
#SBATCH --mail-type=end    # email me when the job finishes

export CILK_NWORKERS=32

rm *.csv *.plt Test*

export N_VERTICES=$1

for i in {2 4 8 16 32 64 128 256 512 1024}
do 
	M_VAL=$i cilkview fw > a_fw_output
	M_VAL=$i cilkview fwr1 > a_fwr1_output
	M_VAL=$i cilkview fwr2 > a_fwr2_output
	M_VAL=$i cilkview fwr3 > a_fwr3_output
done
