### Description

Represent points as glyphs. The point is represented as a sphere.

To use the snippet, click the *Copy to clipboard* at the upper left of the code blocks.

### Implementation

``` Python

def point_to_glyph(points, scale):
    """
    Convert points to glyphs.
    :param points: The points to glyph.
    :param scale: The scale, used to determine the size of the
                  glyph representing the point, expressed as a
                  fraction of the largest side of the bounding
                  box surrounding the points. e.g. 0.05
    :return: The actor.
    """

    bounds = points.GetBounds()
    max_len = 0.0
    for i in range(0,3):
        max_len = max(bounds[i + 1] - bounds[i], max_len)

    sphere_source = vtk.vtkSphereSource()
    sphere_source.SetRadius(scale * max_len)

    pd = vtk.vtkPolyData()
    pd.SetPoints(points)

    mapper = vtk.vtkGlyph3DMapper()
    mapper.SetInputData(pd)
    mapper.SetSourceConnection(sphere_source.GetOutputPort())
    mapper.ScalarVisibilityOff()
    mapper.ScalingOff()

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)

    return actor

```

### Usage

``` Python

    # Map the points to spheres
    sphereActor = point_to_glyph(someFilter.GetOutput().GetPoints(), 0.05)
    # Set the actor color here

```
