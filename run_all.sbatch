#!/bin/bash
#SBATCH --job-name=phbench
#SBATCH --output=logs/%x_%A_%a.out   # one log per job array element
#SBATCH --error=logs/%x_%A_%a.err
#SBATCH --time=02:00:00
#SBATCH --cpus-per-task=1
#SBATCH --mem=32G
#SBATCH --array=0-60
#SBATCH --ntasks=1

# ====== USER CONFIG ======
DATASETS=("mnist" "cifar10" "imagenet" "div2k" "kather" "mast")
METHODS=("pixh" "cripser" "ttk" "gudhi" "ripser")
DIMS=(0 1)

# ====== SETUP ENVIRONMENT ======
source ~/.bashrc
eval "$(~/miniforge3/condabin/conda shell.bash hook)"
conda activate phbenchmark

cd /ts3/home/rceccaroni/projects/PHBenchmark

# ====== COMPUTE COMBINATION ======
# Compute total combinations dynamically
COMBOS=()
for ds in "${DATASETS[@]}"; do
  for m in "${METHODS[@]}"; do
    for d in "${DIMS[@]}"; do
      COMBOS+=("$ds $m $d")
    done
  done
done

# Get combination for this job
combo="${COMBOS[$SLURM_ARRAY_TASK_ID]}"
set -- $combo
DATASET=$1
METHOD=$2
DIM=$3

# ====== OUTPUT PATHS ======
OUTDIR="results/${METHOD}/${DATASET}"
mkdir -p "$OUTDIR"

CSV_PATH="${OUTDIR}/dim${DIM}.csv"

echo "[$(date)] Running METHOD=$METHOD, DATASET=$DATASET, DIM=$DIM"
echo "Saving to $CSV_PATH"

# ====== RUN BENCHMARK ======
phbenchmark run \
    --method "$METHOD" \
    --dataset "datasets/${DATASET}_npy" \
    --maxdim "$DIM" \
    --output "$CSV_PATH"
