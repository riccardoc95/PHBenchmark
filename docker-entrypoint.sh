#!/usr/bin/env bash
set -euo pipefail

DATASETS_DIR="${DATASETS_DIR:-/app/datasets}"
RESULTS_DIR="${RESULTS_DIR:-/work/results}"
DATASET="${DATASET:-all}"
METHODS="${METHODS:-pixh cripser gudhi ripser ttk}"
DIMS="${DIMS:-0 1}"
DOWNLOAD="${DOWNLOAD:-0}"
RUN_BENCHMARKS="${RUN_BENCHMARKS:-1}"
COLLECT="${COLLECT:-1}"
REPORT="${REPORT:-1}"

mkdir -p "$DATASETS_DIR" "$RESULTS_DIR"

if [[ "$DOWNLOAD" == "1" ]]; then
    phbenchmark download --datasets_dir "$DATASETS_DIR" --dataset "$DATASET"
fi

if [[ "$RUN_BENCHMARKS" == "1" ]]; then
    shopt -s nullglob
    dataset_dirs=()

    if [[ "$DATASET" == "all" ]]; then
        dataset_dirs=("$DATASETS_DIR"/*_npy)
    else
        dataset_dirs=("$DATASETS_DIR/${DATASET}_npy")
    fi

    if [[ "${#dataset_dirs[@]}" -eq 0 ]]; then
        echo "No dataset directories found under $DATASETS_DIR" >&2
        exit 1
    fi

    for dataset_dir in "${dataset_dirs[@]}"; do
        dataset_name="$(basename "$dataset_dir")"
        dataset_name="${dataset_name%_npy}"

        for method in $METHODS; do
            for dim in $DIMS; do
                out_dir="$RESULTS_DIR/$method/$dataset_name"
                out_csv="$out_dir/dim${dim}.csv"
                mkdir -p "$out_dir"

                echo "Running method=$method dataset=$dataset_name dim=$dim"
                phbenchmark run \
                    --method "$method" \
                    --dataset "$dataset_dir" \
                    --maxdim "$dim" \
                    --output "$out_csv"
            done
        done
    done
fi

if [[ "$COLLECT" == "1" ]]; then
    phbenchmark collect \
        --results "$RESULTS_DIR" \
        --output "$RESULTS_DIR/benchmarks.json" \
        --summary "$RESULTS_DIR/benchmarks_summary.json"
fi

if [[ "$REPORT" == "1" ]]; then
    phbenchmark report \
        --summary "$RESULTS_DIR/benchmarks_summary.json" \
        --plots-dir "$RESULTS_DIR/plots" \
        --latex-output "$RESULTS_DIR/benchmark_table.tex"
fi
