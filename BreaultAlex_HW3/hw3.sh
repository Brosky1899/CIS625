#!/bin/bash
#SBATCH --time=24:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=4
#SBATCH --mem-per-cpu=3G
#SBATCH --constraint=elves
#SBATCH --output=4_1_100000.o

module load OpenMPI
mpirun hw3