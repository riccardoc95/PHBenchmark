import numpy as np
import vtk
from vtk.util import numpy_support
import cripser as cr
import topologytoolkit as ttk
import sys


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python test.py <file.npy>")
        sys.exit(1)

    path = sys.argv[1]
    maxdim = int(sys.argv[2])
    arr = np.load(path)

    nx, ny = arr.shape

    image = vtk.vtkImageData()
    image.SetDimensions(nx, ny, 1)
    image.SetExtent(0, nx - 1, 0, ny - 1, 0, 0)
    image.SetSpacing(1.0, 1.0, 1.0)
    image.SetOrigin(0.0, 0.0, 0.0)
    image.AllocateScalars(vtk.VTK_DOUBLE, 1)

    arr = numpy_support.numpy_to_vtk(
        num_array=np.asfortranarray(arr).ravel(order="F"),
        deep=True,
        array_type=vtk.VTK_DOUBLE
    )
    arr.SetName("Scalars")
    image.GetPointData().SetScalars(arr)

    pd = ttk.ttkPersistenceDiagram()
    # pd.SetComputeSadMaxPairs(False)
    # pd.SetComputeMinSadPairs(True)
    pd.SetInputData(image)
    pd.SetInputArrayToProcess(
        0, 0, 0, vtk.vtkDataObject.FIELD_ASSOCIATION_POINTS, "Scalars"
    )
    pd.Update()

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
    ttk_dgm = np.vstack([PairType, Birth, Birth + Persistence, ]).T

    if maxdim == 0:
        result = ttk_dgm[ttk_dgm[:, 0] == 0][:, [1, 2]]
    else:
        H0 = ttk_dgm[ttk_dgm[:, 0] == 0][:, [1, 2]]
        H1 = ttk_dgm[ttk_dgm[:, 0] == 1][:, [1, 2]]
        result = (H0, H1)
        print(H0.shape, H1.shape)
    print(f"Processed {path}, result={result}")