### Description

This example demonstrates how to calculate Gaussian and Mean curvatures for a vtkPolyData source. Since edges can produce large discrepancies to curvatures, edge adjustment can be applied. If we know the geometry of the surface we can also modify the curvatures.

Functions are provided to achieve these aims.

A histogram of the frequencies is also output to the console. This is useful if you want to get an idea of the distribution of the scalars in each band.

This example was inspired by these discussions:

- [?vtkCurvatures? yields unreasonably large values along borders](https://discourse.vtk.org/t/?vtkcurvatures?-yields-unreasonably-large-values-along-borders/2527)
- [How to extract the ids of the boundary points of a surface?](https://discourse.vtk.org/t/2530/3)

Thanks to everyone involved in these discussions.
