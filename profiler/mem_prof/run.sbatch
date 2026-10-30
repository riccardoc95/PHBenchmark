#!/bin/bash
# Job name:
#SBATCH --job-name=test
#
# Partition:
#SBATCH --partition=dss
#
# Request one node:
#SBATCH --nodes=1
#
# Specify one task:
#SBATCH --ntasks-per-node=1
#
# Number of processors for single task needed for use case (example):
#SBATCH --cpus-per-task=32
#
#SBATCH --exclusive
# Wall clock limit:
#SBATCH --time=24:00:00
#
## Command(s) to run (example):
./run.sh outputs
