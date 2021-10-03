#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkPoints
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkPolyData
)
from vtkmodules.vtkFiltersCore import vtkPolyDataNormals
from vtkmodules.vtkFiltersModeling import vtkRotationalExtrusionFilter
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Create the RenderWindow, Renderer and Interactor.
    #
    renderer = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(renderer)

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Create the spring profile (a circle).
    #
    points = vtkPoints()
    points.InsertPoint(0, 1.0, 0.0, 0.0)
    points.InsertPoint(1, 1.0732, 0.0, -0.1768)
    points.InsertPoint(2, 1.25, 0.0, -0.25)
    points.InsertPoint(3, 1.4268, 0.0, -0.1768)
    points.InsertPoint(4, 1.5, 0.0, 0.00)
    points.InsertPoint(5, 1.4268, 0.0, 0.1768)
    points.InsertPoint(6, 1.25, 0.0, 0.25)
    points.InsertPoint(7, 1.0732, 0.0, 0.1768)

    poly = vtkCellArray()
    poly.InsertNextCell(8)  # The number of points.
    poly.InsertCellPoint(0)
    poly.InsertCellPoint(1)
    poly.InsertCellPoint(2)
    poly.InsertCellPoint(3)
    poly.InsertCellPoint(4)
    poly.InsertCellPoint(5)
    poly.InsertCellPoint(6)
    poly.InsertCellPoint(7)

    profile = vtkPolyData()
    profile.SetPoints(points)
    profile.SetPolys(poly)

    # Extrude the profile to make a spring.
    #
    extrude = vtkRotationalExtrusionFilter()
    extrude.SetInputData(profile)
    extrude.SetResolution(360)
    extrude.SetTranslation(6)
    extrude.SetDeltaRadius(1.0)
    extrude.SetAngle(2160.0)  # six revolutions

    normals = vtkPolyDataNormals()
    normals.SetInputConnection(extrude.GetOutputPort())
    normals.SetFeatureAngle(60)

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(normals.GetOutputPort())

    spring = vtkActor()
    spring.SetMapper(mapper)
    spring.GetProperty().SetColor(colors.GetColor3d("PowderBlue"))
    spring.GetProperty().SetDiffuse(0.7)
    spring.GetProperty().SetSpecular(0.4)
    spring.GetProperty().SetSpecularPower(20)
    spring.GetProperty().BackfaceCullingOn()

    # Add the actors to the renderer, set the background and size.
    #
    renderer.AddActor(spring)
    renderer.SetBackground(colors.GetColor3d("Burlywood"))
    renWin.SetSize(640, 512)
    renWin.SetWindowName('Spring')

    renderer.ResetCamera()
    renderer.GetActiveCamera().Azimuth(90)

    # Render the image.
    #
    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()
