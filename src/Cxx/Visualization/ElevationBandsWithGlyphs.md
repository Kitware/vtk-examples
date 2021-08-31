### Description

In this example we are coloring the surface by partitioning the elevation into bands and using arrows to display the normals on the surface.

Rather beautiful surfaces are generated.

The banded contour filter and an indexed lookup table are used along with the elevation filter to generate the banding on the surface. To further enhance the surface, the surface normals are glyphed and colored by elevation using an ordinal lookup table.

Feel free to experiment with different color schemes and/or the other sources from the parametric function group or a cone etc.

You will usually need to adjust the parameters for `maskPts`, `arrow` and `glyph` for a nice appearance.
Do this in the function `MakeGlyphs()`.

You may also need to add an elevation filter to generate the scalars as demonstrated in `MakeSphere()`.

`PrintBandsFrequencies()` allows you to inspect the bands and the number of scalars in each band. This are useful if you want to get an idea of the distribution of the scalars in each band.
