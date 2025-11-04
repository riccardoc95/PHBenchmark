import sys
import numpy as np
from ripser import lower_star_img

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python test.py <file.npy>")
        sys.exit(1)

    path = sys.argv[1]
    maxdim = int(sys.argv[2])
    arr = np.load(path)
    if maxdim == 0:
        result = lower_star_img(arr)
    else:
        result = []
    print(f"Processed {path}, result={result}")
