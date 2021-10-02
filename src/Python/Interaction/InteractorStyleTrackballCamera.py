#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkPolyData
from vtkmodules.vtkFiltersSources import (
    vtkPointSource,
    vtkSphereSource
)
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkGlyph3DMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # create a rendering window and renderer
    ren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren)
    renWin.SetWindowName('InteractorStyleTrackballCamera')

    # create a renderwindowinteractor
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    style = vtkInteractorStyleTrackballCamera()
    iren.SetInteractorStyle(style)

    # create source
    src = vtkPointSource()
    src.SetCenter(0, 0, 0)
    src.SetNumberOfPoints(50)
    src.SetRadius(5)
    src.Update()

    actor = point_to_glyph(src.GetOutput().GetPoints(), 0.05)
    actor.GetProperty().SetColor(colors.GetColor3d('Gold'))

    # assign actor to the renderer
    ren.AddActor(actor)
    ren.SetBackground(colors.GetColor3d('RoyalBLue'))

    # enable user interface interactor
    iren.Initialize()
    renWin.Render()
    iren.Start()


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
    for i in range(0, 3):
        max_len = max(bounds[i + 1] - bounds[i], max_len)

    sphere_source = vtkSphereSource()
    sphere_source.SetRadius(scale * max_len)

    pd = vtkPolyData()
    pd.SetPoints(points)

    mapper = vtkGlyph3DMapper()
    mapper.SetInputData(pd)
    mapper.SetSourceConnection(sphere_source.GetOutputPort())
    mapper.ScalarVisibilityOff()
    mapper.ScalingOff()

    actor = vtkActor()
    actor.SetMapper(mapper)

    return actor


if __name__ == '__main__':
    main()
