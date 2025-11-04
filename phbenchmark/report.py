import json
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
import typer
import os


def load_summary(json_path: str | Path):
    """
    Load a summary JSON (benchmarks_summary.json)
    and return a tidy pandas DataFrame.
    """
    with open(json_path, "r") as f:
        data = json.load(f)

    records = []
    for method, datasets in data.items():
        for dataset, dims in datasets.items():
            for dim, stats in dims.items():
                records.append({
                    "method": method,
                    "dataset": dataset,
                    "dim": int(dim),
                    **stats
                })

    df = pd.DataFrame(records)
    return df


def summary_to_latex_table(df: pd.DataFrame, output_path: Path | None = None):
    table = (
        df.pivot_table(
            index=["dataset", "dim"],
            columns="method",
            values=["avg_time_s", "avg_peak_memory_mb"]
        )
        .round(2)
    )

    latex_str = table.to_latex(escape=False, multirow=True)

    # Manually add caption and label
    latex_str = (
        "\\begin{table}[htbp]\n"
        "\\centering\n"
        "\\caption{Benchmark summary: average time (s) and memory (MB).}\n"
        "\\label{tab:bench_summary}\n"
        + latex_str +
        "\\end{table}\n"
    )

    if output_path:
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(latex_str)
        typer.echo(f"LaTeX table saved to {output_path}")
    else:
        typer.echo("\n" + latex_str)

    return latex_str


def plot_time_by_method(df: pd.DataFrame, output_path="avg_time.pdf"):
    """
    Plot average execution time per method for each dataset.
    """
    plt.figure(figsize=(8, 5))
    for dataset, subset in df.groupby("dataset"):
        plt.bar(
            subset["method"] + " (dim" + subset["dim"].astype(str) + ")",
            subset["avg_time_s"],
            label=dataset
        )

    plt.xticks(rotation=45, ha="right")
    plt.ylabel("Average time [s]")
    plt.title("Average Execution Time per Method and Dimension")
    plt.tight_layout()
    plt.savefig(output_path, bbox_inches="tight")
    plt.show()


def plot_memory_by_method(df: pd.DataFrame, output_path="avg_memory.pdf"):
    """
    Plot average memory usage per method for each dataset.
    """
    plt.figure(figsize=(8, 5))
    for dataset, subset in df.groupby("dataset"):
        plt.bar(
            subset["method"] + " (dim" + subset["dim"].astype(str) + ")",
            subset["avg_peak_memory_mb"],
            label=dataset
        )

    plt.xticks(rotation=45, ha="right")
    plt.ylabel("Average peak memory [MB]")
    plt.title("Average Peak Memory Usage per Method and Dimension")
    plt.tight_layout()
    plt.savefig(output_path, bbox_inches="tight")
    plt.show()


def make_report(
    summary_json: Path = typer.Option(
        Path("results/benchmarks_summary.json"),
        "--summary",
        "-s",
        help="Path to the summary JSON file created by collect-results.",
    ),
    plots_dir: Path = typer.Option(
        Path("results/plots"),
        "--plots-dir",
        "-p",
        help="Directory where generated plots will be saved.",
    ),
    latex_output: Path = typer.Option(
        Path("results/benchmark_table.tex"),
        "--latex-output",
        "-l",
        help="Path to save the LaTeX table file (optional).",
    ),
):
    """
    Generate LaTeX tables and PDF plots from a summary JSON file.
    """
    typer.echo(f"Loading summary from {summary_json}")
    if not summary_json.exists():
        typer.echo(f"Error: {summary_json} not found.")
        raise typer.Exit(code=1)

    df = load_summary(summary_json)

    os.makedirs(plots_dir, exist_ok=True)

    typer.echo("Generating LaTeX table and plots...")
    summary_to_latex_table(df, latex_output)
    plot_time_by_method(df, plots_dir / "avg_time.pdf")
    plot_memory_by_method(df, plots_dir / "avg_memory.pdf")
    typer.echo("Report generation complete.")
