import sys
import numpy as np
import cripser

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python test.py <file.npy>")
        sys.exit(1)

    path = sys.argv[1]
    maxdim = int(sys.argv[2])
    arr = np.load(path)
    result = cripser.compute_ph(arr, maxdim=maxdim, filtration="T")
    print(f"Processed {path}, result={result}")
