#!/bin/bash
#$ -l mem=512M
#$ -l h_rt=30
#$ -pe mpi-fill  4
#$ -cwd
#$ -j y
#$ -N MpiExample
#$ -m abe
#$ -M kylehutson@ksu.edu
$HOME/beocatintro/mpi-example.out
