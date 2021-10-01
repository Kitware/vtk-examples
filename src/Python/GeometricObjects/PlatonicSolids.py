#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingFreeType
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkLookupTable
from vtkmodules.vtkFiltersSources import vtkPlatonicSolidSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkActor2D,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTextMapper,
    vtkTextProperty
)


def main():
    colors = vtkNamedColors()

    # Each face has a different cell scalar
    # Here we create a lookup table with a different colour
    # for each face. The colors have been carefully
    # chosen so that adjacent cells are colored distinctly.
    lut = vtkLookupTable()
    lut.SetNumberOfTableValues(20)
    lut.SetTableRange(0.0, 19.0)
    lut.Build()
    lut.SetTableValue(0, 0.1, 0.1, 0.1)
    lut.SetTableValue(1, 0, 0, 1)
    lut.SetTableValue(2, 0, 1, 0)
    lut.SetTableValue(3, 0, 1, 1)
    lut.SetTableValue(4, 1, 0, 0)
    lut.SetTableValue(5, 1, 0, 1)
    lut.SetTableValue(6, 1, 1, 0)
    lut.SetTableValue(7, 0.9, 0.7, 0.9)
    lut.SetTableValue(8, 0.5, 0.5, 0.5)
    lut.SetTableValue(9, 0.0, 0.0, 0.7)
    lut.SetTableValue(10, 0.5, 0.7, 0.5)
    lut.SetTableValue(11, 0, 0.7, 0.7)
    lut.SetTableValue(12, 0.7, 0, 0)
    lut.SetTableValue(13, 0.7, 0, 0.7)
    lut.SetTableValue(14, 0.7, 0.7, 0)
    lut.SetTableValue(15, 0, 0, 0.4)
    lut.SetTableValue(16, 0, 0.4, 0)
    lut.SetTableValue(17, 0, 0.4, 0.4)
    lut.SetTableValue(18, 0.4, 0, 0)
    lut.SetTableValue(19, 0.4, 0, 0.4)

    mappers = list()
    actors = list()
    textMappers = list()
    textActors = list()
    renderers = list()

    # Create a common text property.
    textProperty = vtkTextProperty()
    textProperty.SetFontSize(16)
    textProperty.SetJustificationToCentered()

    # Create the render window and interactor.
    renWin = vtkRenderWindow()
    renWin.SetWindowName('PlatonicSolids')

    iRen = vtkRenderWindowInteractor()
    iRen.SetRenderWindow(renWin)

    # Create the source, renderer, mapper
    # and actor for each object.
    PlatonicSolids = list()
    # There are five Platonic solids.
    names = ['Tetrahedron', 'Cube', 'Octahedron', 'Icosahedron', 'Dodecahedron']
    for i in range(0, len(names)):
        PlatonicSolids.append(vtkPlatonicSolidSource())
        PlatonicSolids[i].SetSolidType(i)

        mappers.append(vtkPolyDataMapper())
        mappers[i].SetInputConnection(PlatonicSolids[i].GetOutputPort())
        mappers[i].SetLookupTable(lut)
        mappers[i].SetScalarRange(0, 19)

        actors.append(vtkActor())
        actors[i].SetMapper(mappers[i])

        textMappers.append(vtkTextMapper())
        textMappers[i].SetInput(names[i])
        textMappers[i].SetTextProperty(textProperty)

        textActors.append(vtkActor2D())
        textActors[i].SetMapper(textMappers[i])
        textActors[i].SetPosition(120, 16)

        renderers.append(vtkRenderer())
        renderers[i].AddActor(actors[i])
        renderers[i].AddViewProp(textActors[i])

        renWin.AddRenderer(renderers[i])

    # Setup the viewports
    xGridDimensions = 3
    yGridDimensions = 2
    rendererSize = 300
    renWin.SetSize(rendererSize * xGridDimensions, rendererSize * yGridDimensions)
    for row in range(0, yGridDimensions):
        for col in range(0, xGridDimensions):
            index = row * xGridDimensions + col

            # (xmin, ymin, xmax, ymax)
            viewport = [float(col) / xGridDimensions,
                        float(yGridDimensions - (row + 1)) / yGridDimensions,
                        float(col + 1) / xGridDimensions,
                        float(yGridDimensions - row) / yGridDimensions]

            if index > (len(actors) - 1):
                # Add a renderer even if there is no actor.
                # This makes the render window background all the same color.
                ren = vtkRenderer()
                ren.SetBackground(colors.GetColor3d('SlateGray'))
                ren.SetViewport(viewport)
                renWin.AddRenderer(ren)
                continue

            renderers[index].SetViewport(viewport)
            renderers[index].SetBackground(colors.GetColor3d('SlateGray'))
            renderers[index].ResetCamera()
            renderers[index].GetActiveCamera().Azimuth(4.5)
            renderers[index].GetActiveCamera().Elevation(-18)
            renderers[index].ResetCameraClippingRange()

    iRen.Initialize()
    renWin.Render()
    iRen.Start()


if __name__ == '__main__':
    main()
