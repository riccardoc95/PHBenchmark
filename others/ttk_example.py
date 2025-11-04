import numpy as np
import vtk
from vtk.util import numpy_support
import cripser as cr
import topologytoolkit as ttk

import matplotlib.pyplot as plt

np.random.seed(1234)

n = 5
x = np.linspace(-1, 1, n)
y = np.linspace(-1, 1, n)
X, Y = np.meshgrid(x, y)
Z = np.random.randn(n,n)#np.exp(-4*(X**2 + Y**2))

plt.imshow(Z)


print("\n=== CubicalRipser ===")
cr_result = cr.compute_ph(
    Z, maxdim=1, filtration="T"
)
cr_dgm0 = cr_result[cr_result[:,0] == 0][:,[1,2]]
cr_dgm1 = cr_result[cr_result[:,0] == 1][:,[1,2]]
print("----------")
print("DGM0")
print("----------")
print(len(cr_dgm0))
print()
print("----------")
print("DGM1")
print("----------")
print(len(cr_dgm1))


image = vtk.vtkImageData()
image.SetDimensions(n, n, 1)
image.SetExtent(0, n-1, 0, n-1, 0, 0)
image.SetSpacing(1.0, 1.0, 1.0)
image.SetOrigin(0.0, 0.0, 0.0)
image.AllocateScalars(vtk.VTK_DOUBLE, 1)

arr = numpy_support.numpy_to_vtk(
    num_array=np.asfortranarray(Z).ravel(order="F"),
    deep=True,
    array_type=vtk.VTK_DOUBLE
)
arr.SetName("Scalars")
image.GetPointData().SetScalars(arr)

pd = ttk.ttkPersistenceDiagram()
#pd.SetComputeSadMaxPairs(False)
#pd.SetComputeMinSadPairs(True)
pd.SetInputData(image)
pd.SetInputArrayToProcess(
    0, 0, 0, vtk.vtkDataObject.FIELD_ASSOCIATION_POINTS, "Scalars"
)
pd.Update()

print("\n=== TTK ===")
output = pd.GetOutput()
points = numpy_support.vtk_to_numpy(output.GetPoints().GetData())
Birth = numpy_support.vtk_to_numpy(
    output.GetCellData().GetArray("Birth")
)
Persistence = numpy_support.vtk_to_numpy(
    output.GetCellData().GetArray("Persistence")
)
PairType = numpy_support.vtk_to_numpy(
    output.GetCellData().GetArray("PairType")
)
ttk_dgm = np.vstack([PairType, Birth, Birth+Persistence, ]).T
ttk_dgm0 = ttk_dgm[ttk_dgm[:,0] == 0][:,[1,2]]
ttk_dgm1 = ttk_dgm[ttk_dgm[:,0] == 1][:,[1,2]]
print("----------")
print("DGM0")
print("----------")
print(len(ttk_dgm0))
print()
print("----------")
print("DGM1")
print("----------")
print(len(ttk_dgm1))

import pixhomology as px

px.computePH(Z, maxdim=1)
cr_dgm0
cr_dgm1