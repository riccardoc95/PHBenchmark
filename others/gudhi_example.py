import numpy as np
import time

import cripser as cr
import pixhomology as px
from gudhi.sklearn.cubical_persistence import CubicalPersistence
import gudhi as gd
import matplotlib.pyplot as plt

np.random.seed(1234)

n = 4000
x = np.linspace(-1, 1, n)
y = np.linspace(-1, 1, n)
X, Y = np.meshgrid(x, y)
Z = np.random.randn(n, n)

#plt.imshow(Z)
#plt.title("Input image")
#plt.colorbar()
#plt.show()

# --------------------------------------------------
# PixHomology
# --------------------------------------------------
print("\n=== PixHomology ===")
t0 = time.perf_counter()

px_dgm0, px_dgm1 = px.computePH(Z, maxdim=1)

t1 = time.perf_counter()
print(f"Time: {t1 - t0:.6f} s")

print("DGM0:", len(px_dgm0))
print("DGM1:", len(px_dgm1))

# --------------------------------------------------
# CubicalRipser (cripser)
# --------------------------------------------------
print("\n=== CubicalRipser (cripser) ===")
t0 = time.perf_counter()

cr_result = cr.compute_ph(Z, maxdim=1, filtration="T")

t1 = time.perf_counter()
print(f"Time: {t1 - t0:.6f} s")

cr_dgm0 = cr_result[cr_result[:, 0] == 0][:, [1, 2]]
cr_dgm1 = cr_result[cr_result[:, 0] == 1][:, [1, 2]]

print("DGM0:", len(cr_dgm0))
print("DGM1:", len(cr_dgm1))

# --------------------------------------------------
# GUDHI CubicalPersistence (sklearn wrapper)
# --------------------------------------------------
print("\n=== GUDHI CubicalPersistence (sklearn) ===")
t0 = time.perf_counter()

cp = CubicalPersistence(
    homology_dimensions=[0, 1],
    input_type="top_dimensional_cells",
    homology_coeff_field=2,
    min_persistence=0.0
)

diags = cp.fit_transform([Z])

g_dgm0 = diags[0][0]
g_dgm1 = diags[0][1]

t1 = time.perf_counter()
print(f"Time: {t1 - t0:.6f} s")

print("DGM0:", len(g_dgm0))
print("DGM1:", len(g_dgm1))

# --------------------------------------------------
# GUDHI CubicalComplex (core API)
# --------------------------------------------------
print("\n=== GUDHI CubicalComplex ===")
t0 = time.perf_counter()

cc = gd.CubicalComplex(top_dimensional_cells=Z)
cc.persistence(homology_coeff_field=2, min_persistence=0.0)

g_c_dgm0 = cc.persistence_intervals_in_dimension(0)
g_c_dgm1 = cc.persistence_intervals_in_dimension(1)

t1 = time.perf_counter()
print(f"Time: {t1 - t0:.6f} s")

print("DGM0:", len(g_c_dgm0))
print("DGM1:", len(g_c_dgm1))



