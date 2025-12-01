#!/usr/bin/env python3
import argparse
import os
import csv
import numpy as np
import cripser



def count_positive_pairs(diagram):
    births = diagram[:, 0]
    deaths = diagram[:, 1]
    finite_death = np.isfinite(deaths)
    positive_length = deaths > births
    return int(np.sum(finite_death & positive_length))


def process_file(path):
    """Load .npy file, compute persistence, return counts for H0-H2."""
    arr = np.load(path)
    result = cripser.compute_ph(arr, maxdim=1, filtration="T")
    dgm0 = result[result[:, 0] == 0][:, [1,2]]
    dgm1 = result[result[:, 0] == 1][:, [1,2]]
    diagrams = [dgm0, dgm1]

    counts = []
    for k in range(2):  # H0, H1
        diag = diagrams[k] if k < len(diagrams) else np.zeros((0, 2))
        counts.append(count_positive_pairs(diag))

    return counts  # [H0, H1]


def main():
    parser = argparse.ArgumentParser(description="Count persistence pairs in datasets")
    parser.add_argument("--dataset", required=True,
                        help="Path to dataset directory containing .npy files")
    parser.add_argument("--output", required=True,
                        help="Path to output CSV")
    args = parser.parse_args()

    dataset_dir = args.dataset
    output_csv = args.output

    # Gather .npy files
    files = sorted([f for f in os.listdir(dataset_dir) if f.endswith(".npy")])
    if not files:
        raise RuntimeError(f"No .npy files found in {dataset_dir}")

    rows = []
    for fname in files:
        fpath = os.path.join(dataset_dir, fname)
        print(f"Processing {fname} ...")
        h0, h1 = process_file(fpath)
        total = h0 + h1

        rows.append([fname, h0, h1, total])

    # Write CSV
    with open(output_csv, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["file", "H0_pairs", "H1_pairs", "H2_pairs", "total_pairs"])
        writer.writerows(rows)

    print(f"\nSaved results to {output_csv}")


if __name__ == "__main__":
    main()
