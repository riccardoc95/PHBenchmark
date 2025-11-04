#!/usr/bin/env python3
import typer
from phbenchmark.download import download_dataset
from phbenchmark.run import run_benchmark
from phbenchmark.collect import collect_results
from phbenchmark.report import make_report

app = typer.Typer(help="PHBenchmark CLI — tools for downloading datasets and benchmarking PH methods.")

# Mount sub-apps as commands
app.command("download")(download_dataset)
app.command("run")(run_benchmark)
app.command("collect")(collect_results)
app.command("report")(make_report)

if __name__ == "__main__":
    app()
