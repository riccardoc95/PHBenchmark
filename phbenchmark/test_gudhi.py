import numpy as np
import gudhi as gd
import matplotlib.pyplot as plt
from pathlib import Path
import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python test.py <file.npy>")
        sys.exit(1)

    path = sys.argv[1]
    maxdim = int(sys.argv[2])
    arr = np.load(path)
    cc = gd.CubicalComplex(top_dimensional_cells=arr)
    cc.persistence(homology_coeff_field=2, min_persistence=0.0)
    if maxdim == 0:
        result = cc.persistence_intervals_in_dimension(0)
    else:
        H0 = cc.persistence_intervals_in_dimension(0)
        H1 = cc.persistence_intervals_in_dimension(1)
        result = (H0, H1)
    print(f"Processed {path}, result={result}")
