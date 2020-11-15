### Description

This example uses vtkMultiThreshold to extract voxels that are inside an isosurface and on the boundary of the isosurface. The result is a vtkUnstructuredGrid for each set of voxels. Before processing, vtkImageShrink3D reduces the resolution by a factor of 4.

Compare these results with 

!!! example "usage"
    VoxelsOnBoundary FullHead.mhd

!!! info
    Compare these results with [MedicalDemo1](../../Medical/MedicalDemo1) that extracts the surface using vtkMarchingCubes to extract an interpolated isosurface.

!!! info
    The example uses `src/Testing/Data/FullHead.mhd` which references `src/Testing/Data/FullHead.raw.gz`.
