#!/usr/bin/env python3
import csv
import json
from pathlib import Path
import statistics
from collections import defaultdict
import typer


def collect_results(
    results_dir: Path = typer.Option(
        Path("results"),
        "--results",
        "-r",
        help="Directory containing benchmark result CSV files (organized by method/dataset).",
    ),
    output_json: Path = typer.Option(
        Path("results/benchmarks.json"),
        "--output",
        "-o",
        help="Path to save the full consolidated JSON file.",
    ),
    summary_json: Path = typer.Option(
        Path("results/benchmarks_summary.json"),
        "--summary",
        "-s",
        help="Path to save the JSON summary with averages and standard deviations.",
    ),
):
    """
    Read all benchmark CSVs and aggregate them into:
      1. Full JSON
      2. Summary JSON
    """
    typer.echo(f"Collecting results from: {results_dir}")
    data = defaultdict(lambda: defaultdict(dict))
    summary = defaultdict(lambda: defaultdict(dict))

    if not results_dir.exists():
        typer.echo(f"Error: directory {results_dir} not found.")
        raise typer.Exit(code=1)

    for method_dir in results_dir.iterdir():
        if not method_dir.is_dir():
            continue
        method = method_dir.name

        for dataset_dir in method_dir.iterdir():
            if not dataset_dir.is_dir():
                continue
            dataset = dataset_dir.name

            for csv_path in dataset_dir.glob("dim*.csv"):
                try:
                    dim = int(csv_path.stem.replace("dim", ""))
                except ValueError:
                    continue

                records = []
                with csv_path.open() as f:
                    reader = csv.DictReader(f)
                    for row in reader:
                        try:
                            if method == "ripser" and dim == 1:
                                records.append({
                                    "filename": row["filename"],
                                    "time_s": float(0),
                                    "peak_memory_mb": float(0),
                                })
                            else:
                                records.append({
                                    "filename": row["filename"],
                                    "time_s": float(row["time_s"]),
                                    "peak_memory_mb": float(row["peak_memory_mb"]),
                                })
                        except ValueError:
                            continue

                if not records:
                    continue



                data[method][dataset][dim] = records

                # --- Compute summary stats ---
                times = [r["time_s"] for r in records]
                mems = [r["peak_memory_mb"] for r in records]

                summary[method][dataset][dim] = {
                    "avg_time_s": statistics.mean(times),
                    "std_time_s": statistics.pstdev(times) if len(times) > 1 else 0.0,
                    "avg_peak_memory_mb": statistics.mean(mems),
                    "std_peak_memory_mb": statistics.pstdev(mems) if len(mems) > 1 else 0.0,
                    "num_files": len(records)
                }

    # Convert defaultdict → dict for JSON serialization
    data_dict = {m: {d: dict(vals) for d, vals in ds.items()} for m, ds in data.items()}
    summary_dict = {m: {d: dict(vals) for d, vals in ds.items()} for m, ds in summary.items()}

    # Save full results
    output_json.parent.mkdir(parents=True, exist_ok=True)
    with output_json.open("w", encoding="utf-8") as f:
        json.dump(data_dict, f, indent=2, sort_keys=True)
    typer.echo(f"Full results saved to {output_json}")

    # Save summary results
    summary_json.parent.mkdir(parents=True, exist_ok=True)
    with summary_json.open("w", encoding="utf-8") as f:
        json.dump(summary_dict, f, indent=2, sort_keys=True)
    typer.echo(f"Summary statistics saved to {summary_json}")
