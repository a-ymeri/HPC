#!/bin/bash -l

### COMP328 (lab03): example batch script
###           usage: "sbatch run.sh"
###         purpose: to run serial and then parallel OpenMP & MPI on requested number of cores
###    restrictions: this script can only handle single nodes
### (c) mkbane, University of Liverpool (2020)

# Specific course queue and max wallclock time
#SBATCH -p course -t 2

# Defaults on Barkla (but set to be safe)
## Specify the current working directory as the location for executables/files
#SBATCH -D ./
## Export the current environment to the compute node
#SBATCH --export=ALL

# load modules
## intel compiler
module load compilers/intel/2019u5 
## intel mpi wrapper and run time
module load mpi/intel-mpi/2019u5/bin

# SLURM terms
## nodes            relates to number of nodes
## ntasks-per-node  relates to MPI processes per node
## cpus-per-task    relates to OpenMP threads (per MPI process)

# determine number of cores requested (NB this is single node implementation)
## further options available via examples: /opt/apps/Slurm_Examples/sbatch*sh
echo "Node list                    : $SLURM_JOB_NODELIST"
echo "Number of nodes allocated    : $SLURM_JOB_NUM_NODES or $SLURM_NNODES"
echo "Number of threads or processes          : $SLURM_NTASKS"
echo "Number of processes per node : $SLURM_TASKS_PER_NODE"
echo "Requested tasks per node     : $SLURM_NTASKS_PER_NODE"
echo "Requested CPUs per task      : $SLURM_CPUS_PER_TASK"
echo "Scheduling priority          : $SLURM_PRIO_PROCESS"

# no check for expected inputs (note that more than 1 node is allowed for MPI)


# parallel using MPI
SRC=mpi_quad-REDUCE.c
EXE=${SRC%%.c}.exe
echo compiling $SRC to $EXE

export numMPI=${SLURM_NTASKS:-1} # if '-n' not used then default to 1

mpiicc -O0 $SRC func2.c -o $EXE && \
      (
      # run 3 times
      mpirun -np ${numMPI} ./${EXE};echo
      mpirun -np ${numMPI} ./${EXE};echo
      mpirun -np ${numMPI} ./${EXE};echo
  ) \
      || echo $SRC did not built to $EXE





## if wanted to check processor core speeds
# # run 3 times
# grep MHz /proc/cpuinfo|sort -nr|uniq -c; ./${EXE};echo
# grep MHz /proc/cpuinfo|sort -nr|uniq -c; ./${EXE};echo
# grep MHz /proc/cpuinfo|sort -nr|uniq -c; ./${EXE};echo
# grep MHz /proc/cpuinfo|sort -nr|uniq -c; 
