#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import (
    vtkPlane,
    vtkPolyData
)
from vtkmodules.vtkFiltersCore import (
    vtkCutter,
    vtkFeatureEdges,
    vtkStripper
)
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkProperty,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    # Create a cube
    cube = vtkSphereSource()
    cube.SetRadius(50)
    cube.SetThetaResolution(100)
    cube.SetPhiResolution(100)

    cubeMapper = vtkPolyDataMapper()
    cubeMapper.SetInputConnection(cube.GetOutputPort())

    # create a plane to cut,here it cuts in the XZ direction (xz normal=(1,0,0);XY =(0,0,1),YZ =(0,1,0)
    plane = vtkPlane()
    plane.SetOrigin(20, 0, 0)
    plane.SetNormal(1, 0, 0)

    # create cutter
    cutter = vtkCutter()
    cutter.SetCutFunction(plane)
    cutter.SetInputConnection(cube.GetOutputPort())
    cutter.Update()

    FeatureEdges = vtkFeatureEdges()
    FeatureEdges.SetInputConnection(cutter.GetOutputPort())
    FeatureEdges.BoundaryEdgesOn()
    FeatureEdges.FeatureEdgesOff()
    FeatureEdges.NonManifoldEdgesOff()
    FeatureEdges.ManifoldEdgesOff()
    FeatureEdges.Update()

    cutStrips = vtkStripper()  # Forms loops (closed polylines) from cutter
    cutStrips.SetInputConnection(cutter.GetOutputPort())
    cutStrips.Update()
    cutPoly = vtkPolyData()  # This trick defines polygons as polyline loop
    cutPoly.SetPoints((cutStrips.GetOutput()).GetPoints())
    cutPoly.SetPolys((cutStrips.GetOutput()).GetLines())

    cutMapper = vtkPolyDataMapper()
    # cutMapper.SetInput(FeatureEdges.GetOutput())
    cutMapper.SetInputData(cutPoly)

    backface = vtkProperty()
    backface.SetColor(colors.GetColor3d('Gold'))

    cutActor = vtkActor()
    cutActor.SetMapper(cutMapper)
    cutActor.GetProperty().SetColor(colors.GetColor3d('Yellow'))
    cutActor.GetProperty().SetEdgeColor(colors.GetColor3d('Red'))
    cutActor.GetProperty().SetLineWidth(2)
    cutActor.GetProperty().EdgeVisibilityOn()
    # cutActor.GetProperty().SetOpacity(0.7)
    cutActor.SetBackfaceProperty(backface)

    # create renderers and add actors of plane and cube
    ren = vtkRenderer()
    ren.AddActor(cutActor)

    # Add renderer to renderwindow and render
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren)
    renWin.SetSize(600, 400)
    renWin.SetWindowName('FilledPolygon')

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)
    ren.SetBackground(colors.GetColor3d('DarkSlateGray'))
    ren.GetActiveCamera().SetPosition(223, -122, -91)
    renWin.Render()

    camera = ren.GetActiveCamera()
    camera.SetPosition(151.519511, 12.795117, -223.586044)
    camera.SetFocalPoint(12.518283, 1.963242, 7.618042)
    camera.SetViewUp(0.740690, -0.523767, 0.420769)
    camera.SetDistance(269.988889)
    camera.SetClippingRange(175.347580, 366.490816)
    camera.Zoom(1.5)

    iren.Start()


if __name__ == '__main__':
    main()
