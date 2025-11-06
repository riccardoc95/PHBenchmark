# PixHomology Benchmark (PHBenchmark)

Tools to download 2D image datasets, benchmark multiple persistent homology (PH) implementations on them, and generate consolidated reports (plots + LaTeX table).

> This guide is written to reproduce the results end‑to‑end on a clean machine.

---

## What’s inside

* **CLI**: `phbenchmark` (Typer app) with four subcommands: `download`, `run`, `collect`, `report`.
* **Supported PH methods** (invoked via small Python wrappers in `phbenchmark/tests`):

  * `pixh` → PixHomology (in this repo, C++/pybind11 Python package)
  * `cripser` → [Cripser](https://github.com/shizuo-kaji/CubicalRipser_3dim)
  * `gudhi` → [GUDHI](https://gudhi.inria.fr/)
  * `ripser` → [Ripser.py](https://ripser.scikit-tda.org/en/latest/)
  * `ttk` → [Topology ToolKit](https://topology-tool-kit.github.io/) (via `topologytoolkit` Python module and VTK)
* **Datasets** (downloaded as `.h5` from Google Drive and expanded to `.npy`): `mnist`, `cifar10`, `imagenet`, `div2k`, `kather`.
* **Reports**: aggregated JSON, summary JSON, PDF plots, and a LaTeX table.

Repository layout (key files):

```
PHBenchmark/
├─ environment.yml               # fully reproducible conda env
├─ run_all.sbatch                # SLURM array job to run everything at scale
├─ phbenchmark/
│  ├─ app.py                     # CLI entry point
│  ├─ download.py                # dataset fetching/extraction
│  ├─ run.py                     # runs the benchmark with /usr/bin/time
│  ├─ collect.py                 # aggregates CSV to JSON summaries
│  ├─ report.py                  # plots + LaTeX table
│  └─ tests/                     # per‑method wrappers (pixh/gudhi/ripser/cripser/ttk)
└─ PixHomology/                  # local source for PixHomology (installed editable)
```

---

## 1) Installation (single command with conda)

This repository ships a **single** `environment.yml` that builds everything your need (Python + all PH libraries) and installs both PixHomology and PHBenchmark in *editable* mode.

### 1.1 Create and activate the environment

```bash
# from the repository root
conda env create -f environment.yml
conda activate phbenchmark
```

The environment file (for reference):

```yaml
name: phbenchmark
channels:
  - conda-forge
dependencies:
  - python==3.11
  - topologytoolkit==1.3.0
  - time
  - pip
  - pip:
      - ripser==0.6.12
      - cripser==0.0.21
      - gudhi==3.11.0
      - -e ./PixHomology
      - -e .
```

This will:

* Install Python 3.11 and `topologytoolkit` (TTK).
* Install GNU/BSD `time` (used to read peak memory and runtime).
* `pip`‑install PH libraries and this project:

  * `ripser`, `cripser`, `gudhi`.
  * `PixHomology` (local subfolder) and `phbenchmark` (this CLI) in editable mode.

> **Heads‑up about `/usr/bin/time`:** On Linux we call `time -v`; on macOS we call `/usr/bin/time -l`. If your system lacks a verbose `time`, install one (e.g. *GNU time* on Linux, or ensure `/usr/bin/time` exists on macOS). The conda package `time` typically provides this on Linux. If you still get errors, install GNU time from your distro (e.g. `apt install time`).

### 1.2 Verify the installation

```bash
# the CLI should now be on PATH
phbenchmark --help

# and the PixHomology Python package should import
python -c "import pixhomology as px; print('PixHomology OK', px.__version__)"
```

---

## 2) Datasets

`phbenchmark download` can fetch and unpack the following datasets (HDF5 → `.npy` image tensors):

* `mnist` (28×28)
* `cifar10` (32×32)
* `imagenet` (300×300 subset)
* `div2k` (1000×1000 subset)
* `kather` (5000×5000 subset)

Images are expanded as NumPy arrays under `datasets/<name>_npy/`.

### 2.1 Download usage

```bash
phbenchmark download \
  --datasets_dir datasets \
  --dataset mnist
```

Options:

* `--datasets_dir, -ddir` (default: `datasets`) — where to store the data.
* `--dataset, -d` — one of the names above **or** `all` to download everything.

*Example (download everything):*

```bash
phbenchmark download --datasets_dir datasets --dataset all
```

---

## 3) Running the benchmarks

PHBenchmark runs each method’s small wrapper on every `.npy` file in a dataset folder, while measuring **wall time** and **peak memory** using `/usr/bin/time`.

Supported methods (exact keys for `--method`): `pixh`, `cripser`, `gudhi`, `ripser`, `ttk`.

### 3.1 Run usage

```bash
phbenchmark run \
  --method pixh \
  --dataset datasets/mnist_npy \
  --maxdim 1 \
  --output results/pixh/mnist/dim1.csv
```

Options:

* `--method, -m` — one of `pixh|cripser|gudhi|ripser|ttk`.
* `--dataset, -d` — path to a directory of `.npy` images.
* `--maxdim, -k` — maximum homology dimension (e.g. `0` or `1`).
* `--output, -o` — path to the output CSV (created if missing).

The CSV schema is:

```
filename,time_s,peak_memory_mb
img_000001.npy,0.31,85.27
...
```

**Notes**

* On Linux we parse `Maximum resident set size` from `time -v` and convert kB → MB.
* On macOS we parse `peak memory footprint` from `/usr/bin/time -l` and convert bytes → MB.
* Environment variable `OMP_NUM_THREADS` is set to the machine/thread allocation (uses `SLURM_CPUS_PER_TASK` when present), so parallel libraries respect CPU limits.

---

## 4) Aggregating results

After you have multiple CSVs (e.g. across methods and/or dims), consolidate them into JSON and summary statistics.

### 4.1 Collect usage

```bash
phbenchmark collect \
  --results results \
  --output results/benchmarks.json \
  --summary results/benchmarks_summary.json
```

Inputs:

* It expects files at `results/<method>/<dataset>/dim<k>.csv` for each combination.

  * (The `run_all.sbatch` script below writes exactly in this layout.)

Outputs:

* **Full JSON** (`benchmarks.json`): groups raw timings/memory per file.
* **Summary JSON** (`benchmarks_summary.json`): per `(method, dataset, dim)` averages and standard deviations.

---

## 5) Making the report (plots + LaTeX)

Generate publication‑ready figures and a LaTeX table from the summary JSON.

### 5.1 Report usage

```bash
phbenchmark report \
  --summary results/benchmarks_summary.json \
  --plots-dir results/plots \
  --latex-output results/benchmark_table.tex
```

What you’ll get:

* `results/plots/time_memory_per_dataset.pdf` — for each dataset: log‑scaled bars of Avg Time [s] and Avg Peak Memory [MB], split by dim.
* `results/plots/scalability.pdf` — image size vs time [s] (log‑log) per method.
* `results/plots/time_vs_memory_per_dataset.pdf` — scatter of Time vs Memory per dataset, color = method, style = dim.
* `results/benchmark_table.tex` — LaTeX `tabular` with Avg Time and Avg Peak Memory for all methods/datasets/dims.

> The plotting code treats `-1` values as missing (rendered as `--` in the LaTeX table).

---

## 6) End‑to‑end example (MNIST)

Below is a minimal but complete run to help reviewers reproduce everything locally.

```bash
# 0) activate env
conda activate phbenchmark

# 1) download MNIST and expand to .npy
phbenchmark download --datasets_dir datasets --dataset mnist

# 2) run all methods for dim=0 and dim=1
for m in pixh cripser gudhi ripser ttk; do
  for k in 0 1; do
    out="results/${m}/mnist/dim${k}.csv"
    mkdir -p "$(dirname "$out")"
    phbenchmark run --method "$m" --dataset datasets/mnist_npy --maxdim "$k" --output "$out"
  done
done

# 3) aggregate results
phbenchmark collect --results results --output results/benchmarks.json --summary results/benchmarks_summary.json

# 4) build plots + LaTeX table
phbenchmark report --summary results/benchmarks_summary.json --plots-dir results/plots --latex-output results/benchmark_table.tex
```

You should now have:

```
results/
├─ <method>/<dataset>/dim<k>.csv
├─ benchmarks.json
├─ benchmarks_summary.json
├─ plots/
│  ├─ time_memory_per_dataset.pdf
│  ├─ scalability.pdf
│  └─ time_vs_memory_per_dataset.pdf
└─ benchmark_table.tex
```

---

## 7) Running everything on SLURM (batch arrays)

We include a ready‑to‑use SLURM array script: `run_all.sbatch`.

### 7.1 What it does

* Defines 3 axes: `DATASETS`, `METHODS`, `DIMS`.
* Computes the Cartesian product and maps `SLURM_ARRAY_TASK_ID` to one combination.
* Writes outputs to `results/<method>/<dataset>/dim<k>.csv`.
* Uses the `phbenchmark` CLI internally.

### 7.2 Customize it

Edit the **USER CONFIG** section near the top:

```bash
DATASETS=("mnist" "cifar10" "imagenet" "div2k" "kather")
METHODS=("pixh" "cripser" "ttk" "gudhi" "ripser")
DIMS=(0 1)
```

> The sample file contains `mast` in `DATASETS` by default; if you did not download/provide it, remove it.

Make sure the working directory and conda init lines match your cluster (search for the lines that `cd` into a path and `conda activate phbenchmark`).

### 7.3 Submit on the cluster

```bash
# set the array size to (#datasets × #methods × #dims - 1); example uses 0-60 in the template
sbatch --array=0-<N-1> run_all.sbatch
```

Logs go to `logs/<jobname>_<jobid>_<arrayid>.out/.err` (as defined in the file).

Once the array completes, continue with:

```bash
phbenchmark collect --results results --output results/benchmarks.json --summary results/benchmarks_summary.json
phbenchmark report  --summary results/benchmarks_summary.json --plots-dir results/plots --latex-output results/benchmark_table.tex
```

---

## 8) Software installed for the benchmark

Besides **PixHomology** (bundled here and installed in editable mode), the environment installs:

* **Cripser**: fast cubical persistent homology in Python. -> [https://github.com/shizuo-kaji/CubicalRipser_3dim](https://github.com/shizuo-kaji/CubicalRipser_3dim)
* **GUDHI**: Geometry Understanding in Higher Dimensions. -> [https://gudhi.inria.fr/](https://gudhi.inria.fr/)
* **Ripser.py**: Python interface for ripser. -> [https://ripser.scikit-tda.org/en/latest/](https://ripser.scikit-tda.org/en/latest/)
* **Topology ToolKit (TTK)**: topological analysis suite (Python module `topologytoolkit`, requires VTK). -> [https://topology-tool-kit.github.io/](https://topology-tool-kit.github.io/)
* **VTK**: provided transitively by the `topologytoolkit` conda package.

General Python utilities used by the CLI (installed via `pyproject.toml`):

* `typer`, `h5py`, `gdown`, `tqdm`, `pandas`, `seaborn`, `matplotlib`, `jinja2`.

If you hit build/runtime issues with any of the libraries, please refer to their respective documentation pages linked above.

---

## 9) Reproducibility and notes

* **Exact versions** are pinned in `environment.yml`. Re‑create the env if you change machines.
* **Timing/Memory** are collected via `/usr/bin/time`; ensure a compatible implementation is available.
* **CPU threads**: the CLI sets `OMP_NUM_THREADS` to the machine’s core count (or SLURM allocation) to improve reproducibility.
* **File formats**: datasets are expanded as `.npy` and treated as 2D arrays for lower‑star/cubical PH.

---

## 10) Short API reference (CLI)

```text
phbenchmark download  # fetch and expand datasets to .npy
phbenchmark run       # run a method over a dataset folder to CSV
phbenchmark collect   # consolidate CSVs to JSON + summary JSON
phbenchmark report    # build plots + LaTeX table from the summary
```

Run `phbenchmark <subcommand> --help` for all flags.

