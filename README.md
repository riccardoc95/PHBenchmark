# PHBenchmark

A **Python CLI tool** built with [Typer](https://typer.tiangolo.com/) for managing datasets, installing local software modules, and running benchmarking experiments.

---

## Features

- **Download datasets** from Google Drive in HDF5 format and export images as `.npy` files  
- **Install local software packages** (from the `libs/` folder) individually or all at once  
- **Run benchmarks** with a `.sh` script that generates results as CSV, and display statistics (mean, std)

---

## Project structure
```
phbenchmark/
│
├── phbenchmark/
│   └── app.py                ← main Typer CLI
│
├── libs/                     ← local software modules (methodA, methodB, etc.)
│   ├── methodA/
│   ├── methodB/
│   └── methodC/
│
├── pyproject.toml
└── README.md
```


## Installation

```bash
# Clone the repository
git clone https://github.com/yourusername/phbenchmark.git
cd phbenchmark

# Install in editable mode
pip install -e .
````

This will install the CLI command:

```bash
phbenchmark --help
```

---

## Usage

### 1. Download a dataset

```bash
phbenchmark download-dataset mnist
```

### 2. Install software

```bash
phbenchmark install all
```

or just one:

```bash
phbenchmark install methodA
```

### 3. Run benchmark

```bash
phbenchmark test methodA mnist 1
```
