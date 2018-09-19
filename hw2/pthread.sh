#!/bin/sh

for i in {1..10}
do
  for j in 2 4 8 16
  do
    sbatch --time=1 --nodes=1 --ntasks-per-node=1 --cpus-per-task=$j --mem-per-cpu=512M --constraint=elves ./hw2-pthread.sh
  done
done