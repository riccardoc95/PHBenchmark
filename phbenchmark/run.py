import csv
import sys
import subprocess
import time
from pathlib import Path
import re
import platform
import typer

current_path = Path(__file__).resolve()

METHODS = {
    "pixh":    current_path.parent / "tests" / "test_pixh.py",
    "cripser": current_path.parent / "tests" / "test_cripser.py",
    "gudhi":   current_path.parent / "tests" / "test_gudhi.py",
    "ripser":  current_path.parent / "tests" / "test_ripser.py",
    "ttk":     current_path.parent / "tests" / "test_ttk.py",
}


def run_with_time(script_path: Path, npy_file: Path, maxdim: int) -> tuple[float, float]:
    """
    Run the Python script using `/usr/bin/time` and return:
      - execution time in seconds
      - peak memory in MB (including all subprocesses)
    """
    system = platform.system().lower()
    if system == "linux":
        time_command = "time"
        time_flag = "-v"
        regex = r"Maximum resident set size.*?:\s+(\d+)"
        divisor = 1024  # Linux reports KB
    else:
        time_command = "/usr/bin/time"
        time_flag = "-l"
        regex = r"(\d+)\s+peak memory footprint"
        divisor = 1024 * 1024

    cmd = [
        time_command, time_flag,
        sys.executable, str(script_path), str(npy_file), str(maxdim)
    ]

    start = time.time()
    process = subprocess.run(cmd, capture_output=True, text=True)
    elapsed = time.time() - start

    mem_match = re.search(regex, process.stderr)
    mem_mb = float(mem_match.group(1)) / divisor if mem_match else 0.0

    return elapsed, mem_mb


def run_benchmark(
    method: str = typer.Option(..., "--method", "-m", help=f"Method to benchmark. Available: {', '.join(METHODS.keys())}"),
    dataset_dir: Path = typer.Option(..., "--dataset", "-d", help="Path to directory containing .npy files."),
    maxdim: int = typer.Option(1, "--maxdim", "-k", help="Maximum homology dimension."),
    output_csv: Path = typer.Option("results.csv", "--output", "-o", help="Path to save benchmark results (CSV)."),
):
    """
    Run a benchmark for a given persistent homology method over a dataset of .npy files.
    """
    if method not in METHODS:
        typer.echo(f"Method '{method}' not found. Available: {list(METHODS.keys())}")
        raise typer.Exit(code=1)

    python_script = METHODS[method]
    typer.echo(f"Running {method} on {dataset_dir} (maxdim={maxdim})...")

    if not python_script.exists():
        typer.echo(f"Error: Python script {python_script} not found.")
        raise typer.Exit(code=1)
    if not dataset_dir.exists():
        typer.echo(f"Error: Dataset directory {dataset_dir} not found.")
        raise typer.Exit(code=1)

    npy_files = sorted(dataset_dir.glob("*.npy"))
    if not npy_files:
        typer.echo(f"No .npy files found in {dataset_dir}")
        raise typer.Exit()

    with open(output_csv, "w", newline="") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["filename", "time_s", "peak_memory_mb"])

        for npy_file in npy_files:
            typer.echo(f"  Running {npy_file.name} ...", nl=False)
            try:
                elapsed, mem_mb = run_with_time(python_script, npy_file, maxdim)
                writer.writerow([npy_file.name, f"{elapsed:.2f}", f"{mem_mb:.2f}"])
                typer.echo(f" done ({elapsed:.2f}s, {mem_mb:.2f}MB)")
            except Exception as e:
                writer.writerow([npy_file.name, "error", "error"])
                typer.echo(f" Error: {e}")

    typer.echo(f"\nResults saved into {output_csv}")


if __name__ == "__main__":
    app()
