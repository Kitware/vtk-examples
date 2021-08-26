### Description

In this example we are coloring the surface by partitioning the gaussian curvature into bands and using arrows to display the normals on the surface.

Note that:

- If the regions on a surface have zero Gaussian curvature, then they can be flattened into a plane with no distortion, and the geometry of the region is Euclidean geometry.

- If the regions on a surface have positive Gaussian curvature, then the geometry of the surface is spherical geometry.

- If the regions on the surface have a negative Gaussian curvature, then the geometry of the surface is hyperbolic geometry.

In the above image you can see that the random hills incorporate all of these geometries.

We have used banding to colour the surface.
The banded contour filter and an indexed lookup table are used along with the gaussian curvature filter to generate the banding on the surface.
To further enhance the surface the surface normals are
glyphed and colored by elevation using the default lookup table.

For generating surfaces, the trick here is to return vtkPolyData for surfaces thereby hiding the particular surface properties in the implementation of the function.

The surface selected is the parametric random hills surface. The problem with
the random hills surface is:

- Most of the gaussian curvatures will lie in the range -1 to 0.2 (say) with a few large values say 20 to 40 at the peaks of the hills.
- The edges of the random hills surface also have large irregular values so we need to handle these also. This is actually done in the class, `ComputeCurvatures`.

So we need to mamually generate custom bands to group the curvatures. This is done by first running:

``` Python
bands = make_bands(scalarRange, number_of_bands, false)
```

then:

``` Python
freq = frequencies(bands, src)
print_bands_frequencies(bands, freq)
```

Tho output from this is used to specify each band as a list `[r1, r2]` where `r1 < r2` and
append these to a list.
The list should ultimately look
like this: `[[r1, r2], [r2, r3], [r3, r4]...]`

Feel free to experiment with different color schemes and/or the other sources from the parametric function group or the torus etc.

You will usually need to adjust the parameters for `mask_pts`, `arrow` and `glyph` for a nice appearance.
Do this in the function `make_glyphs()`.

print_bands_frequencies() allows you to inspect the bands and the number of scalars in each band. This are useful if you want to get an idea of the distribution of the scalars in each band.
