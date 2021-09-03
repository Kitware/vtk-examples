### Description

This example demonstrates how to calculate Gaussian and Mean curvatures for a vtkPolyData source. A class called `ComputeCurvatures` is initialised with the vtkPolyData source. The method `update()` in the class will generate the Gaussian and Mean curvatures, adjusting curvature for edge variations (which can be quite large) and finally adding the vtkDoubleArray of curvatures into the original vtkPolyData.

You should be able to take this class and just drop it into your code.

In order for vtkCurvatures to work, the surface must be triangulated.

Some helper methods are provided which can be useful if you know the topology and geometry of the surface.

Here are some examples of using these extra C++ and Python methods in the class for particular surfaces. These methods must run **before** `Update()` is run.

We assume that the class has been instantiated:

``` C++
  ComputeCurvatures::ComputeCurvatures cc(source);
```

``` Python
cc = ComputeCurvatures(source)
```

#### Cube

For a Cube we have the intersection of six planes so we
would expect Gaussian Curvature to be 0. The corners and intersections of the planes still produce rather large variations.

``` C++
  cc.SetGaussCurvatureBounds(0.0, 0.0);
  cc.GaussBoundsOn();
```

``` Python
    cc.set_gauss_curvature_bounds(0.0, 0.0)
    cc.gauss_bounds_on()
```

#### Mobius

For Mobius we would expect Gaussian Curvature to be 0.
In fact the curvature is mostly around -0.2.

So we can apply:

``` C++
  cc.SetGaussCurvatureBounds(0.0, 0.0);
  cc.GaussBoundsOn();
```

``` Python
    cc.set_gauss_curvature_bounds(0.0, 0.0)
    cc.gauss_bounds_on()
```

#### Random Hills

If we set RandomHills to be a plane we would expect a Gaussian curvature of 0.0. If you do this you will see that the plane is perfectly smooth.

However the class by default treats very small values (less than 1.0e-8) as 0.0. If you apply the following method, you will see fluctuations occurring in a band across the surface for Gaussian curvatures:

``` C++
  cc.SetEpsilons(0.0, 0.0);
```

``` Python
    cc.set_epsilons(0.0, 0.0);
```

#### Sphere

For a sphere:

- gaussian curvature = 1/r^2
- mean_curvature = 1/r

So to eliminate large values near the pole
(Our sphere has a radius of 0.5):

``` C++
  cc.SetGaussCurvatureBounds(4, 4);
  cc.GaussBoundsOn();
  cc.SetMeanCurvatureBounds(2.0, 2.0);
  cc.MeanBoundsOn();
```

``` Python
    cc.set_gauss_curvature_bounds(4, 4)
    cc.gauss_bounds_on()
    cc.set_mean_curvature_bounds(2.0, 2.0)
    cc.mean_bounds_on()
```

This class was inspired by these discussions:

- [vtkCurvatures yields unreasonably large values along borders](https://discourse.vtk.org/t/vtkcurvatures-yields-unreasonably-large-values-along-borders/2527)
- [How to extract the ids of the boundary points of a surface?](https://discourse.vtk.org/t/2530/3)

Thanks to everyone involved in these discussions.
