#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkPolyData
from vtkmodules.vtkFiltersCore import vtkGlyph3D
from vtkmodules.vtkFiltersSources import (
    vtkArrowSource,
    vtkSphereSource
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    sphereSource = vtkSphereSource()
    sphereSource.Update()

    input_data = vtkPolyData()
    input_data.ShallowCopy(sphereSource.GetOutput())

    arrowSource = vtkArrowSource()

    glyph3D = vtkGlyph3D()
    glyph3D.SetSourceConnection(arrowSource.GetOutputPort())
    glyph3D.SetVectorModeToUseNormal()
    glyph3D.SetInputData(input_data)
    glyph3D.SetScaleFactor(.2)
    glyph3D.Update()

    # Visualize
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(glyph3D.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('Gold'))

    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('OrientedGlyphs')

    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('DarkGreen'))

    renderWindow.Render()

    camera = renderer.GetActiveCamera()
    camera.SetPosition(-0.399941, -1.070475, 2.931458)
    camera.SetFocalPoint(-0.000000, -0.000000, 0.000000)
    camera.SetViewUp(-0.028450, 0.940195, 0.339448)
    camera.SetDistance(3.146318)
    camera.SetClippingRange(1.182293, 5.626211)

    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
