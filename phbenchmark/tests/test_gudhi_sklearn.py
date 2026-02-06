import numpy as np
import sys

from gudhi.sklearn.cubical_persistence import CubicalPersistence

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Uso: python test.py <file.npy> <maxdim>")
        sys.exit(1)

    path = sys.argv[1]
    maxdim = int(sys.argv[2])

    arr = np.load(path)

    dims = [0] if maxdim == 0 else [0, 1]

    cp = CubicalPersistence(
        homology_dimensions=dims,
        input_type="top_dimensional_cells",
        homology_coeff_field=2,
        min_persistence=0.0,
    )

    diags = cp.fit_transform([arr])

    if maxdim == 0:
        result = diags[0][0]   # H0: array Nx2 (birth, death)
    else:
        H0 = diags[0][0]
        H1 = diags[0][1]
        result = (H0, H1)

    print(f"Processed {path}, result={result}")
