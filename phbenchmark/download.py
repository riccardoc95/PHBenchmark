from pathlib import Path
import h5py
import numpy as np
import gdown
import typer
from tqdm import tqdm
from concurrent.futures import ThreadPoolExecutor


DATASETS = {
    # "test": "19GnE8Qw375kXTHmG_35GaXabvHVXSfUc",
    "mnist": "1pG1jzW8qNh5PRWT8kC4XT_udWQNSFLFo",
    "cifar10": "17lULKw6WX546SJxcTYl0qsLZLqyPO1Hk",
    "imagenet": "1BNl5VivoFJpevkH7b0sYuVnN39d830T9",
    "div2k": "1XkeGHk9ObsDoChMSrunozZHyLeKGiJ5N",
    "kather": "1Enb3EHrumPNcPwEUoe8Eon8wXAynONBD",
    "mast": "11_lydKNfvE889bsRlU9nIRw9n4GgEYTs",
}

IMAGE_SIZE = {
    #"test": 28*28,
    "mnist": 28*28,
    "cifar10": 32*32,
    "imagenet": 300*300,
    "div2k": 1000*1000,
    "kather": 5000*5000,
    "mast": 4800*5000,
}

def extract_images(h5_path: Path, output_dir: Path, max_workers: int = 8):
    """Extract all images from an HDF5 file and save as .npy files."""
    with h5py.File(h5_path, "r") as f:
        images = f["images"][:]

    typer.echo(f"Extracting {len(images)} images to {output_dir}...")

    def save_image(i, img):
        np.save(output_dir / f"img_{i}.npy", img)

    output_dir.mkdir(parents=True, exist_ok=True)
    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        list(tqdm(executor.map(lambda args: save_image(*args), enumerate(images)),
                  total=len(images),
                  desc="Saving images",
                  ncols=80))

    typer.echo(f"Done! Saved {len(images)} images.")


def download_dataset(
    data_dir: str = typer.Option(
        "datasets",
        "--datasets_dir",
        "-ddir",
        help=f"Dataset directory",
    ),
    dataset: str = typer.Option(
        "all",
        "--dataset",
        "-d",
        help=f"Dataset name to download. Available: {', '.join(DATASETS.keys())} or 'all'",
    ),
):
    """
    Download dataset(s) from Google Drive and export images as .npy files.
    """
    DATA_DIR = Path(data_dir)
    DATA_DIR.mkdir(exist_ok=True)

    datasets = DATASETS.keys() if dataset == "all" else [dataset]

    for name in datasets:
        if name not in DATASETS:
            typer.echo(f"Dataset '{name}' not found. Available: {list(DATASETS.keys())}")
            raise typer.Exit(code=1)

        typer.echo(f"\nDownloading dataset '{name}'...")
        file_id = DATASETS[name]
        h5_path = DATA_DIR / f"{name}.h5"
        output_dir = DATA_DIR / f"{name}_npy"

        try:
            gdown.download(id=file_id, output=str(h5_path), quiet=False)
        except Exception as e:
            typer.echo(f"Failed to download '{name}': {e}")
            continue

        if not h5_path.exists():
            typer.echo(f"Download failed: {h5_path} not found.")
            continue

        try:
            extract_images(h5_path, output_dir)
        except Exception as e:
            typer.echo(f"Failed to extract images from '{name}': {e}")
            continue


if __name__ == "__main__":
    app()
