import typer
import h5py
import numpy as np
import os
import subprocess
import pandas as pd
from pathlib import Path
import gdown

app = typer.Typer()
BASE_DIR = Path(__file__).resolve().parent.parent

# --- Predefined lists ---
DATASETS = {
    "test": "19GnE8Qw375kXTHmG_35GaXabvHVXSfUc",
    "mnist": "1pG1jzW8qNh5PRWT8kC4XT_udWQNSFLFo",
    "cifar10": "17lULKw6WX546SJxcTYl0qsLZLqyPO1Hk",
    "imagenet": "1BNl5VivoFJpevkH7b0sYuVnN39d830T9",
    "div2k": "1XkeGHk9ObsDoChMSrunozZHyLeKGiJ5N",
    "kather": "1Enb3EHrumPNcPwEUoe8Eon8wXAynONBD",
    "mast": "11_lydKNfvE889bsRlU9nIRw9n4GgEYTs",
}

SOFTWARES = {
    "pixh": BASE_DIR / "libs" / "PixHomology/",
    "cripser": BASE_DIR / "libs" / "CubicalRipser_3dim-0.0.21/",
    "gudhi": BASE_DIR / "libs" / "gudhi-devel/",
    "ripserpy": BASE_DIR / "libs" / "ripser.py/",
}


# ==========================================================
# 1 DOWNLOAD AND CONVERT DATASETS
# ==========================================================
@app.command()
def download_dataset(
    dataset: str = typer.Option(
        ...,
        "--dataset",
        "-d",
        help=f"Dataset name to download. Available: {', '.join(DATASETS.keys())}",
    )
):
    """
    Download a dataset (HDF5) from Google Drive and export images as .npy files.
    """
    if dataset not in DATASETS:
        typer.echo(f"Dataset '{dataset}' not found. Available: {list(DATASETS.keys())}")
        raise typer.Exit()

    typer.echo(f"Downloading dataset '{dataset}'...")
    url = DATASETS[dataset]
    h5_path = f"datasets/{dataset}.h5"
    npy_dir = Path(f"datasets/{dataset}_npy")
    npy_dir.mkdir(parents=True, exist_ok=True)

    # Download from Google Drive
    gdown.download(id=url, output=h5_path, quiet=False)

    typer.echo(f"🔍 Extracting images from {h5_path}...")
    with h5py.File(h5_path, "r") as f:
        images = f["images"][:]

    for i, img in enumerate(images):
        np.save(npy_dir / f"img_{i}.npy", img)

    typer.echo(f"Done! Extracted {len(images)} images to {npy_dir}")


# ==========================================================
# 2 INSTALL SOFTWARE LIBRARIES
# ==========================================================
@app.command()
def install(
    software: str = typer.Option(
        ...,
        "--software",
        "-s",
        help=f"Software to install (or 'all' to install everything). Available: {', '.join(SOFTWARES.keys())}",
    )
):
    """
    Install local Python packages (software tools).
    """
    if software == "all":
        for name, path in SOFTWARES.items():
            typer.echo(f"Installing {name} from {path}...")
            subprocess.run(["pip", "install", "-e", path], check=True)
        typer.echo("All software installed successfully.")
    elif software in SOFTWARES:
        typer.echo(f"Installing {software} from {SOFTWARES[software]}...")
        subprocess.run(["pip", "install", "-e", SOFTWARES[software]], check=True)
        typer.echo(f"{software} installed successfully.")
    else:
        typer.echo(f"Unknown software '{software}'. Available: {list(SOFTWARES.keys())}")


# ==========================================================
# 3 TEST SOFTWARE AND PRINT STATISTICS
# ==========================================================
@app.command()
def test(
    software: str = typer.Option(
        ...,
        "--software",
        "-s",
        help=f"Software name. Available: {', '.join(SOFTWARES.keys())}",
    ),
    dataset: str = typer.Option(
        ...,
        "--dataset",
        "-d",
        help=f"Dataset name. Available: {', '.join(DATASETS.keys())}",
    ),
    maxdim: int = typer.Option(
        1,
        "--maxdim",
        "-m",
        help="Maximum dimension (0 or 1).",
    ),
):
    """
    Run a test for a given software and dataset.
    Executes a script.sh that generates a CSV file, then computes statistics.
    """
    if software not in SOFTWARES:
        typer.echo(f"Unknown software '{software}'. Available: {list(SOFTWARES.keys())}")
        raise typer.Exit()

    csv_output = f"results/{dataset}_{software}_{maxdim}_results.csv"
    Path("results").mkdir(exist_ok=True)

    dataset_path = f"datasets/{dataset}_npy"
    typer.echo(f"Running test for {software} on {dataset_path} (maxdim={maxdim})...")
    phbenchmarkdir = BASE_DIR / "phbenchmark"

    subprocess.run(
        ["bash", phbenchmarkdir / "script.sh", phbenchmarkdir, software, dataset_path, str(maxdim), csv_output],
        check=True,
    )

    if not os.path.exists(csv_output):
        typer.echo("CSV output not found. The test script may have failed.")
        raise typer.Exit()

    df = pd.read_csv(csv_output).drop("filename", axis=1)
    typer.echo("\nResults Summary:")
    for col in df.columns:
        mean = df[col].mean()
        std = df[col].std()
        typer.echo(f"  {col}: mean = {mean:.4f}, std = {std:.4f}")


if __name__ == "__main__":
    app()
