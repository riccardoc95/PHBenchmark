import json
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path
import numpy as np
import os
import typer

from phbenchmark.download import IMAGE_SIZE
from phbenchmark.run import METHODS as RUN_METHODS

DATASETS = IMAGE_SIZE.keys()
METHODS = RUN_METHODS.keys()


def load_summary(json_path: str | Path):
    with open(json_path, "r") as f:
        data = json.load(f)

    records = []
    for method in METHODS:
        datasets = data[method]
        for dataset in DATASETS:
            try:
                dims = datasets[dataset]
                for dim, stats in dims.items():
                    records.append({
                        "method": method,
                        "dataset": dataset,
                        "dim": int(dim),
                        "image_size": IMAGE_SIZE.get(dataset, np.nan),
                        **stats
                    })
            except KeyError:
                records.append({
                    "method": method,
                    "dataset": dataset,
                    "dim": 0,
                    "image_size": IMAGE_SIZE.get(dataset, np.nan),
                    "avg_time_s":0,
                    "avg_peak_memory_mb": 0
                })

    return pd.DataFrame(records)


sns.set_theme(style="whitegrid", font_scale=1.15)
PALETTE = sns.color_palette("tab10", 10)


def summary_to_latex_table(
    df: pd.DataFrame,
    output_path: Path | None = None,
    caption: str = "Benchmark summary: average time (s) and memory (MB).",
    label: str = "tab:bench_summary",
    float_precision: int = 2,
):
    """
    Build a LaTeX table from the summary dataframe.

    The table has a multi-index on rows (dataset, dim) and two column blocks:
      - avg_time_s (one column per method)
      - avg_peak_memory_mb (one column per method)

    Notes:
    - Values == -1 are treated as missing (printed as '--').
    - Caption and label are injected manually to avoid pandas caption errors.
    """

    # Sanity & cleaning
    work = df.copy()
    # treat sentinel -1 as missing
    for col in ("avg_time_s", "avg_peak_memory_mb"):
        if col in work.columns:
            work.loc[work[col] < 0, col] = np.nan

    # Stable ordering
    method_order = sorted(work["method"].unique())
    dataset_order = sorted(work["dataset"].unique())

    work["dataset"] = pd.Categorical(work["dataset"], categories=dataset_order, ordered=True)
    work["method"] = pd.Categorical(work["method"], categories=method_order, ordered=True)
    work = work.sort_values(["dataset", "dim", "method"])

    # Pivot to wide with two top-level metric blocks
    table = (
        work.pivot_table(
            index=["dataset", "dim"],
            columns="method",
            values=["avg_time_s", "avg_peak_memory_mb"],
            aggfunc="first",
        )
        .reindex(columns=pd.MultiIndex.from_product(
            [["avg_time_s", "avg_peak_memory_mb"], method_order]
        ))
        .round(float_precision)
    )

    # Pretty column headers
    table.columns = pd.MultiIndex.from_tuples(
        [("avg_time_s", m) if top == "avg_time_s" else ("avg_peak_memory_mb", m)
         for (top, m) in table.columns]
    )

    # Build LaTeX body (without caption/label)
    latex_body = table.to_latex(
        escape=False,
        multirow=True,
        multicolumn=True,
        multicolumn_format="c",
        na_rep="--",
    )

    # Wrap with table env + caption/label
    latex_str = (
        "\\begin{table}[htbp]\n"
        "\\centering\n"
        f"\\caption{{{caption}}}\n"
        f"\\label{{{label}}}\n"
        f"{latex_body}"
        "\\end{table}\n"
    )

    if output_path:
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(latex_str)
        typer.echo(f"LaTeX table saved to {output_path}")
    else:
        typer.echo("\n" + latex_str)

    return latex_str

def plot_time_and_memory_per_dataset(df: pd.DataFrame, plots_dir: Path):
    datasets = sorted(df["dataset"].unique())
    n = len(datasets)
    fig, axes = plt.subplots(nrows=n, ncols=2, figsize=(12, 4 * n))
    if n == 1:
        axes = np.array([axes])

    # === scale globali ===
    time_min, time_max = df["avg_time_s"].min(), df["avg_time_s"].max()
    mem_min, mem_max = df["avg_peak_memory_mb"].min(), df["avg_peak_memory_mb"].max()

    for i, dataset in enumerate(DATASETS):
        subset = df[df["dataset"] == dataset]

        # Time subplot
        sns.barplot(
            data=subset,
            x="method",
            y="avg_time_s",
            hue="dim",
            ax=axes[i, 0],
            palette="Set2"
        )
        axes[i, 0].set_yscale("log")
        axes[i, 0].set_ylim(time_min * 0.9, time_max * 1.1)
        axes[i, 0].set_title(f"{dataset}: Execution time")
        axes[i, 0].set_ylabel("Time [s] (log)")
        axes[i, 0].legend(title="Dim")

        # Memory subplot
        sns.barplot(
            data=subset,
            x="method",
            y="avg_peak_memory_mb",
            hue="dim",
            ax=axes[i, 1],
            palette="Set2"
        )
        axes[i, 1].set_yscale("log")
        axes[i, 1].set_ylim(mem_min * 0.9, mem_max * 1.1)
        axes[i, 1].set_title(f"{dataset}: Peak memory")
        axes[i, 1].set_ylabel("Memory [MB] (log)")
        axes[i, 1].legend(title="Dim")

        for ax in axes[i]:
            ax.set_xlabel("")
            ax.tick_params(axis="x", rotation=30)

    plt.tight_layout()
    plt.savefig(plots_dir / "time_memory_per_dataset.pdf", bbox_inches="tight")
    plt.close()


def plot_scalability(df: pd.DataFrame, plots_dir: Path):
    plt.figure(figsize=(8, 6))
    ax = sns.lineplot(
        data=df,
        x="image_size",
        y="avg_time_s",
        hue="method",
        style="dim",
        dashes={0: "", 1: (4, 2)},  # 0=solid, 1=dashed
        markers=True,
        palette="tab10"
    )
    ax.set_xscale("log")
    ax.set_yscale("log")
    plt.xlabel("Image size (pixels)")
    plt.ylabel("Average time [s]")
    plt.title("Scalability: Time vs Image Size")
    plt.tight_layout()
    plt.savefig(plots_dir / "scalability_time.pdf")
    plt.close()

    plt.figure(figsize=(8, 6))
    ax = sns.lineplot(
        data=df,
        x="image_size",
        y="avg_peak_memory_mb",
        hue="method",
        style="dim",
        dashes={0: "", 1: (4, 2)},
        markers=True,
        palette="tab10"
    )
    ax.set_xscale("log")
    ax.set_yscale("log")
    plt.xlabel("Image size (pixels)")
    plt.ylabel("Peak memory [MB]")
    plt.title("Scalability: Memory vs Image Size")
    plt.tight_layout()
    plt.savefig(plots_dir / "scalability_memory.pdf")
    plt.close()


def plot_time_vs_memory(df: pd.DataFrame, plots_dir: Path, only_largest=False):
    if only_largest:
        dataset = max(df["dataset"].unique(), key=lambda d: df[df["dataset"] == d]["image_size"].mean())
        df = df[df["dataset"] == dataset]
        plt.figure(figsize=(7, 6))
        ax = sns.scatterplot(
            data=df,
            x="avg_time_s",
            y="avg_peak_memory_mb",
            hue="method",
            style="dim",
            s=150,
            palette="tab10"
        )
        ax.set_xscale("log")
        ax.set_yscale("log")
        plt.xlabel("Average time [s] (log)")
        plt.ylabel("Peak memory [MB] (log)")
        plt.title(f"Time vs Memory — {dataset}")
        plt.tight_layout()
        plt.savefig(plots_dir / f"time_vs_memory_{dataset}.pdf")
        plt.close()
        return

    datasets = sorted(df["dataset"].unique())
    n = len(datasets)
    rows, cols = int(np.ceil(n / 2)), 2
    fig, axes = plt.subplots(rows, cols, figsize=(12, 5 * rows))
    axes = axes.flatten()

    # === scale globali ===
    time_min, time_max = df["avg_time_s"].replace(-1, np.nan).min(), df["avg_time_s"].replace(-1, np.nan).max()
    mem_min, mem_max = df["avg_peak_memory_mb"].replace(-1, np.nan).min(), df["avg_peak_memory_mb"].replace(-1, np.nan).max()

    for i, dataset in enumerate(DATASETS):
        subset = df[df["dataset"] == dataset]
        ax = axes[i]
        sns.scatterplot(
            data=subset,
            x="avg_time_s",
            y="avg_peak_memory_mb",
            hue="method",
            style="dim",
            s=120,
            palette="tab10",
            ax=ax
        )
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.set_xlim(time_min * 0.9, time_max * 1.1)
        ax.set_ylim(mem_min * 0.9, mem_max * 1.1)
        ax.set_title(dataset)
        ax.set_xlabel("Time [s] (log)")
        ax.set_ylabel("Memory [MB] (log)")
        ax.legend(title="Method / Dim")
        #if i == 0:
        #    ax.legend(title="Method / Dim")
        #else:
        #    ax.legend().remove()

    # Nascondi subplot vuoti (se numero dispari)
    for j in range(i + 1, len(axes)):
        fig.delaxes(axes[j])

    plt.tight_layout()
    plt.savefig(plots_dir / "time_vs_memory_per_dataset.pdf", bbox_inches="tight")
    plt.close()



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
        help="Path to save the LaTeX summary table.",
    ),
):
    """
    Generate advanced benchmark reports including:
      - time and memory per dataset (dim0 vs dim1)
      - scalability vs image size
      - time vs memory tradeoff (per dataset)
      - LaTeX summary table (average time/memory per method)
    """
    typer.echo(f"Loading summary from {summary_json}")
    if not summary_json.exists():
        typer.echo(f"Error: {summary_json} not found.")
        raise typer.Exit(code=1)

    df = load_summary(summary_json)
    os.makedirs(plots_dir, exist_ok=True)

    typer.echo("Generating advanced plots...")
    plot_time_and_memory_per_dataset(df, plots_dir)
    plot_scalability(df, plots_dir)
    plot_time_vs_memory(df, plots_dir, only_largest=False)

    typer.echo("Generating LaTeX table...")
    summary_to_latex_table(
        df,
        output_path=latex_output,
        caption="Benchmark summary: average time (s) and peak memory (MB).",
        label="tab:bench_summary",
    )

    typer.echo("Report generation complete.")
    typer.echo(f"   Plots saved in: {plots_dir}")
    typer.echo(f"   LaTeX table: {latex_output}")
