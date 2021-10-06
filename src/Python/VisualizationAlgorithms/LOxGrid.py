#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkLookupTable
from vtkmodules.vtkFiltersCore import (
    vtkStructuredGridOutlineFilter,
    vtkTubeFilter
)
from vtkmodules.vtkFiltersFlowPaths import vtkStreamTracer
from vtkmodules.vtkFiltersGeometry import vtkStructuredGridGeometryFilter
from vtkmodules.vtkFiltersSources import vtkPointSource
from vtkmodules.vtkIOParallel import vtkMultiBlockPLOT3DReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkCamera,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    xyxFile, qFile = get_program_parameters()

    # Read the data.
    #
    pl3d = vtkMultiBlockPLOT3DReader()
    pl3d.AutoDetectFormatOn()
    pl3d.SetXYZFileName(xyxFile)
    pl3d.SetQFileName(qFile)
    pl3d.SetScalarFunctionNumber(153)
    pl3d.SetVectorFunctionNumber(200)
    pl3d.Update()

    sg = pl3d.GetOutput().GetBlock(0)

    # blue to red lut
    #
    lut = vtkLookupTable()
    lut.SetHueRange(0.667, 0.0)

    # Computational planes.
    floorComp = vtkStructuredGridGeometryFilter()
    floorComp.SetExtent(0, 37, 0, 75, 0, 0)
    floorComp.SetInputData(sg)
    floorComp.Update()

    floorMapper = vtkPolyDataMapper()
    floorMapper.SetInputConnection(floorComp.GetOutputPort())
    floorMapper.ScalarVisibilityOff()
    floorMapper.SetLookupTable(lut)

    floorActor = vtkActor()
    floorActor.SetMapper(floorMapper)
    floorActor.GetProperty().SetRepresentationToWireframe()
    floorActor.GetProperty().SetColor(colors.GetColor3d('Beige'))
    floorActor.GetProperty().SetLineWidth(2)

    subFloorComp = vtkStructuredGridGeometryFilter()

    subFloorComp.SetExtent(0, 37, 0, 15, 22, 22)
    subFloorComp.SetInputData(sg)

    subFloorMapper = vtkPolyDataMapper()
    subFloorMapper.SetInputConnection(subFloorComp.GetOutputPort())
    subFloorMapper.SetLookupTable(lut)
    subFloorMapper.SetScalarRange(sg.GetScalarRange())

    subFloorActor = vtkActor()

    subFloorActor.SetMapper(subFloorMapper)

    subFloor2Comp = vtkStructuredGridGeometryFilter()
    subFloor2Comp.SetExtent(0, 37, 60, 75, 22, 22)
    subFloor2Comp.SetInputData(sg)

    subFloor2Mapper = vtkPolyDataMapper()
    subFloor2Mapper.SetInputConnection(subFloor2Comp.GetOutputPort())
    subFloor2Mapper.SetLookupTable(lut)
    subFloor2Mapper.SetScalarRange(sg.GetScalarRange())

    subFloor2Actor = vtkActor()

    subFloor2Actor.SetMapper(subFloor2Mapper)

    postComp = vtkStructuredGridGeometryFilter()

    postComp.SetExtent(10, 10, 0, 75, 0, 37)
    postComp.SetInputData(sg)

    postMapper = vtkPolyDataMapper()
    postMapper.SetInputConnection(postComp.GetOutputPort())
    postMapper.SetLookupTable(lut)
    postMapper.SetScalarRange(sg.GetScalarRange())

    postActor = vtkActor()
    postActor.SetMapper(postMapper)
    postActor.GetProperty().SetColor(colors.GetColor3d('Beige'))

    fanComp = vtkStructuredGridGeometryFilter()
    fanComp.SetExtent(0, 37, 38, 38, 0, 37)
    fanComp.SetInputData(sg)

    fanMapper = vtkPolyDataMapper()
    fanMapper.SetInputConnection(fanComp.GetOutputPort())
    fanMapper.SetLookupTable(lut)
    fanMapper.SetScalarRange(sg.GetScalarRange())

    fanActor = vtkActor()

    fanActor.SetMapper(fanMapper)
    fanActor.GetProperty().SetColor(colors.GetColor3d('Beige'))

    # streamers
    #
    # spherical seed points
    rake = vtkPointSource()
    rake.SetCenter(-0.74, 0, 0.3)
    rake.SetNumberOfPoints(10)

    # a line of seed points
    seedsComp = vtkStructuredGridGeometryFilter()
    seedsComp.SetExtent(10, 10, 37, 39, 1, 35)
    seedsComp.SetInputData(sg)

    streamers = vtkStreamTracer()
    streamers.SetInputConnection(pl3d.GetOutputPort())

    # streamers SetSource [rake GetOutput]
    streamers.SetSourceConnection(seedsComp.GetOutputPort())
    streamers.SetMaximumPropagation(250)
    streamers.SetInitialIntegrationStep(.2)
    streamers.SetMinimumIntegrationStep(.01)
    streamers.SetIntegratorType(2)
    streamers.Update()

    tubes = vtkTubeFilter()
    tubes.SetInputConnection(streamers.GetOutputPort())
    tubes.SetNumberOfSides(8)
    tubes.SetRadius(0.08)
    tubes.SetVaryRadius(0)

    mapTubes = vtkPolyDataMapper()

    mapTubes.SetInputConnection(tubes.GetOutputPort())
    mapTubes.SetScalarRange(sg.GetScalarRange())

    tubesActor = vtkActor()
    tubesActor.SetMapper(mapTubes)

    # outline
    outline = vtkStructuredGridOutlineFilter()
    outline.SetInputData(sg)

    outlineMapper = vtkPolyDataMapper()
    outlineMapper.SetInputConnection(outline.GetOutputPort())

    outlineActor = vtkActor()
    outlineActor.SetMapper(outlineMapper)
    outlineActor.GetProperty().SetColor(colors.GetColor3d('Beige'))

    # Create graphics stuff.
    ren1 = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren1)

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Add the actors to the renderer, set the background and size.
    #
    ren1.AddActor(outlineActor)
    ren1.AddActor(floorActor)
    ren1.AddActor(subFloorActor)
    ren1.AddActor(subFloor2Actor)
    ren1.AddActor(postActor)
    ren1.AddActor(fanActor)
    ren1.AddActor(tubesActor)

    aCam = vtkCamera()
    aCam.SetFocalPoint(0.00657892, 0, 2.41026)
    aCam.SetPosition(-1.94838, -47.1275, 39.4607)
    aCam.SetViewUp(0.00653193, 0.617865, 0.786257)
    ren1.ResetCamera()
    aCam.Dolly(1.)
    aCam.SetClippingRange(1, 100)

    ren1.SetBackground(colors.GetColor3d('SlateGray'))
    ren1.SetActiveCamera(aCam)
    renWin.SetSize(640, 480)
    renWin.SetWindowName('LOxGrid')

    renWin.Render()
    iren.Start()


def get_program_parameters():
    import argparse
    description = 'LOx post CFD case study.'
    epilogue = '''
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('xyz_file', help='postxyz.bin.')
    parser.add_argument('q_file', help='postq.bin.')
    args = parser.parse_args()
    return args.xyz_file, args.q_file


if __name__ == '__main__':
    main()
