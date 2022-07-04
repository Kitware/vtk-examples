### Description

Demonstrates the Parametric classes added by Andrew Maclean and additional classes added by Tim Meehan. The parametric spline is also included.

Options are provided to:

- Specify a single surface (**-s SURFACE_NAME**)
- Color the back-face (**-b**)
- Add normals (**-n**)
- Display the geometric bounds of the object (**-l**)

You can save a screenshot by pressing "k".

With respect to your VTK build you may need to specify one or more of:

```bash
-DVTK_MODULE_ENABLE_VTK_cli11=WANT
-DVTK_MODULE_ENABLE_VTK_fmt=WANT
```

If `-DVTK_BUILD_TESTING=ON` is specified when building VTK then `VTK:cli11` and `VTK::fmt` will be automatically enabled.

!!! Note
    To really appreciate the complexity of some of these surfaces, select a single surface, and use the options `-b -n`. Also try specifying wireframe (toggle "w" on the keyboard) and zooming in and out.
!!! Tip
    If you color the back face, the three-dimensional orientable surfaces will only show backface coloring inside the surface e.g `ConicSpiral` or `Torus`. For three dimensional non-orientable surfaces; backface coloring is visible because of the twisting used to generate these surfaces e.g `Boy` or `Figure8Klein`.

!!! Cite
    See: [Parametric Equations for Surfaces](http://www.vtk.org/wp-content/uploads/2015/11/ParametricSurfaces.pdf), for more information. This paper provides a description of fifteen surfaces, including their parametric equations and derivatives. Also provided is an example of how to create your own surface, namely the Figure-8 Torus.
