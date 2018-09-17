#!/bin/sh
#SBATCH --ntasks=100
#SBATCH --mem-per-cpu=512M
#SBATCH --job-name=pt0
#SBATCH --time=10
#SBATCH --constraint=elves
#SBATCH --output=%x-%a.out
#SBATCH --partition=ksu-gen-reserved.q,batch.q
#SBATCH --array=2,20,200,2000,20000
RUNSIZE=$SLURM_ARRAY_TASK_ID

#srun ./pt0-unrolled $RUNSIZE
srun ./pt0 $RUNSIZE
#srun ./pt0-tiled $RUNSIZE
