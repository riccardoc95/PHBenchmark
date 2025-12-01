#!/bin/bash
#SBATCH --job-name=phbench
#SBATCH --output=logs/%x_%A_%a.out
#SBATCH --error=logs/%x_%A_%a.err
#SBATCH --time=02:00:00
#SBATCH --cpus-per-task=1
#SBATCH --mem=32G
#SBATCH --array=0-5
#SBATCH --ntasks=1

# ====== USER CONFIG ======
DATASETS=("mnist" "cifar10" "imagenet" "div2k" "kather" "mast")

# ====== SETUP ENVIRONMENT ======
source ~/.bashrc
eval "$(~/miniforge3/condabin/conda shell.bash hook)"
conda activate phbenchmark

cd /ts3/home/rceccaroni/projects/PHBenchmark/datasets_pairs

# SELECT DATASET BASED ON JOB ARRAY INDEX
DATASET=${DATASETS[$SLURM_ARRAY_TASK_ID]}

# ====== OUTPUT PATHS ======
OUTDIR="results"
mkdir -p "$OUTDIR"
CSV_PATH="${OUTDIR}/${DATASET}.csv"

echo "[$(date)] Running DATASET=$DATASET (ID=$SLURM_ARRAY_TASK_ID)"
echo "Saving to $CSV_PATH"

# ====== RUN BENCHMARK ======
python count_pairs.py --dataset "../datasets/${DATASET}_npy" --output "$CSV_PATH"
