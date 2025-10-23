import typer
import h5py
import numpy as np
import os
import subprocess
import pandas as pd
from typing import List
from pathlib import Path
import gdown

app = typer.Typer()
BASE_DIR = Path(__file__).resolve().parent.parent

# --- Predefined lists ---
DATASETS = {
    "test": "19GnE8Qw375kXTHmG_35GaXabvHVXSfUc",
    "mnist": "https://drive.google.com/uc?id=YOUR_MNIST_FILE_ID",
    "cifar10": "https://drive.google.com/uc?id=YOUR_CIFAR10_FILE_ID",
}

SOFTWARES = {
    "pixh": BASE_DIR / "libs" / "PixHomology/",
    "cripser": BASE_DIR / "libs" / "CubicalRipser_3dim-0.0.21/",
    "gudhi": BASE_DIR / "libs" / "gudhi-devel/",
    "ripserpy": BASE_DIR / "libs" / "ripser.py/",
}


# ==========================================================
# 1️⃣ DOWNLOAD AND CONVERT DATASETS
# ==========================================================
@app.command()
def download_dataset(name: str):
    """
    Download a dataset (HDF5) from Google Drive and export images as .npy files.
    """
    if name not in DATASETS:
        typer.echo(f"Dataset '{name}' not found. Available: {list(DATASETS.keys())}")
        raise typer.Exit()

    typer.echo(f"Downloading dataset '{name}'...")
    url = DATASETS[name]
    h5_path = f"datasets/{name}.h5"
    npy_dir = Path(f"datasets/{name}_npy")
    npy_dir.mkdir(parents=True, exist_ok=True)

    # Download the file from Google Drive
    gdown.download(id=url, output=h5_path, quiet=False)

    typer.echo(f"Extracting images from {h5_path}...")
    with h5py.File(h5_path, "r") as f:
        images = f["images"][:]

    for i, img in enumerate(images):
        np.save(npy_dir / f"img_{i}.npy", img)

    typer.echo(f"✅ Done! Extracted {len(images)} images to {npy_dir}")


# ==========================================================
# 2️⃣ INSTALL SOFTWARE LIBRARIES
# ==========================================================
@app.command()
def install(software: str = typer.Argument(None, help="Software name or 'all' to install everything")):
    """
    Install local Python packages (software tools).
    """
    if software == "all":
        for name, path in SOFTWARES.items():
            typer.echo(f"Installing {name} from {path}...")
            subprocess.run(["pip", "install", "-e", path], check=True)
        typer.echo("✅ All software installed successfully.")
    elif software in SOFTWARES:
        typer.echo(f"Installing {software} from {SOFTWARES[software]}...")
        subprocess.run(["pip", "install", "-e", SOFTWARES[software]], check=True)
        typer.echo(f"✅ {software} installed successfully.")
    else:
        typer.echo(f"Unknown software '{software}'. Available: {list(SOFTWARES.keys())}")


# ==========================================================
# 3️⃣ TEST SOFTWARE AND PRINT STATISTICS
# ==========================================================
@app.command()
def test(software: str, dataset: str, maxdim: int):
    """
    Run a test for a given software and dataset.
    Executes a script.sh that generates a CSV file, then computes statistics.
    """
    if software not in SOFTWARES:
        typer.echo(f"Unknown software '{software}'. Available: {list(SOFTWARES.keys())}")
        raise typer.Exit()

    csv_output = f"results/{dataset}_{software}_{maxdim}_results.csv"
    Path("results").mkdir(exist_ok=True)

    dataset = f"datasets/{dataset}_npy"
    typer.echo(f"Running test for {software} on {dataset} (maxdim={maxdim})...")
    phbenchmarkdir = BASE_DIR / "phbenchmark"
    subprocess.run(["bash", phbenchmarkdir/"script.sh", phbenchmarkdir, software, dataset, str(maxdim), csv_output], check=True)

    if not os.path.exists(csv_output):
        typer.echo("❌ CSV output not found. The test script may have failed.")
        raise typer.Exit()

    df = pd.read_csv(csv_output).drop("filename", axis=1)
    typer.echo("\n📊 Results Summary:")
    for col in df.columns:
        mean = df[col].mean()
        std = df[col].std()
        typer.echo(f"  {col}: mean = {mean:.4f}, std = {std:.4f}")


if __name__ == "__main__":
    app()
