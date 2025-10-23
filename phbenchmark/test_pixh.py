import sys
import numpy as np
import pixhomology as px

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python test.py <file.npy>")
        sys.exit(1)

    path = sys.argv[1]
    maxdim = int(sys.argv[2])
    arr = np.load(path)
    result = px.computePH(arr, maxdim=maxdim)
    print(f"Processed {path}, result={result}")
