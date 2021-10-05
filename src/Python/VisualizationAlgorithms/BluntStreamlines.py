#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import vtkStructuredGridOutlineFilter
from vtkmodules.vtkFiltersFlowPaths import vtkStreamTracer
from vtkmodules.vtkFiltersGeometry import vtkStructuredGridGeometryFilter
from vtkmodules.vtkFiltersSources import vtkLineSource
from vtkmodules.vtkIOParallel import vtkMultiBlockPLOT3DReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    xyzFilename, qFilename = get_program_parameters()

    colors = vtkNamedColors()

    aren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(aren)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    scalarRange = [0.0] * 2
    c = [0.0] * 3
    maxTime = 0.0

    reader = vtkMultiBlockPLOT3DReader()
    reader.SetXYZFileName(xyzFilename)
    reader.SetQFileName(qFilename)
    reader.Update()  # Force a read to occur.

    pd = reader.GetOutput().GetBlock(0)
    pd.GetCenter(c)
    if pd.GetPointData().GetScalars():
        pd.GetPointData().GetScalars().GetRange(scalarRange)
    if pd.GetPointData().GetVectors():
        maxVelocity = pd.GetPointData().GetVectors().GetMaxNorm()
        maxTime = 20.0 * pd.GetLength() / maxVelocity

    outlineF = vtkStructuredGridOutlineFilter()
    outlineF.SetInputData(pd)

    outlineMapper = vtkPolyDataMapper()
    outlineMapper.SetInputConnection(outlineF.GetOutputPort())

    outline = vtkActor()
    outline.SetMapper(outlineMapper)
    outline.GetProperty().SetColor(colors.GetColor3d('Moccasin'))
    outline.GetProperty().SetLineWidth(2.0)

    #
    # Some geometry for context
    #
    wall = vtkStructuredGridGeometryFilter()
    wall.SetInputData(pd)
    wall.SetExtent(0, 100, 0, 100, 0, 0)

    wallMap = vtkPolyDataMapper()
    wallMap.SetInputConnection(wall.GetOutputPort())
    wallMap.ScalarVisibilityOff()

    wallActor = vtkActor()
    wallActor.SetMapper(wallMap)
    wallActor.GetProperty().SetColor(colors.GetColor3d('Silver'))

    fin = vtkStructuredGridGeometryFilter()
    fin.SetInputData(pd)
    fin.SetExtent(0, 100, 0, 0, 0, 100)

    finMap = vtkPolyDataMapper()
    finMap.SetInputConnection(fin.GetOutputPort())
    finMap.ScalarVisibilityOff()

    finActor = vtkActor()
    finActor.SetMapper(finMap)
    finActor.GetProperty().SetColor(colors.GetColor3d('Silver'))
    #
    # regular streamlines
    #
    line1 = vtkLineSource()
    line1.SetResolution(25)
    line1.SetPoint1(-6.36, 0.25, 0.06)
    line1.SetPoint2(-6.36, 0.25, 5.37)

    rakeMapper = vtkPolyDataMapper()
    rakeMapper.SetInputConnection(line1.GetOutputPort())

    rake1 = vtkActor()
    rake1.SetMapper(rakeMapper)
    rake1.GetProperty().SetColor(colors.GetColor3d('Black'))
    rake1.GetProperty().SetLineWidth(5)

    streamers = vtkStreamTracer()
    # streamers.DebugOn()
    streamers.SetInputConnection(reader.GetOutputPort())
    streamers.SetSourceConnection(line1.GetOutputPort())
    streamers.SetMaximumPropagation(maxTime)
    streamers.SetInitialIntegrationStep(0.2)
    streamers.SetMinimumIntegrationStep(0.01)
    streamers.SetIntegratorType(2)
    streamers.Update()

    streamersMapper = vtkPolyDataMapper()
    streamersMapper.SetInputConnection(streamers.GetOutputPort())
    streamersMapper.SetScalarRange(scalarRange)

    lines = vtkActor()
    lines.SetMapper(streamersMapper)

    aren.AddActor(outline)
    aren.AddActor(wallActor)
    aren.AddActor(finActor)
    aren.AddActor(rake1)
    aren.AddActor(lines)
    aren.SetBackground(colors.GetColor3d('Gray'))

    aren.ResetCamera()
    aren.GetActiveCamera().Elevation(30.0)
    aren.GetActiveCamera().Azimuth(30.0)
    aren.GetActiveCamera().Dolly(1.2)
    aren.ResetCameraClippingRange()

    renWin.SetSize(640, 480)
    renWin.SetWindowName('BluntStreamlines')
    renWin.Render()

    # Interact with the data.
    iren.Start()


def get_program_parameters():
    import argparse
    description = 'This example shows airflow around a blunt fin.'
    epilogue = '''
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename1', help='bluntfinxyz.bin.')
    parser.add_argument('filename2', help='bluntfinq.bin.')
    args = parser.parse_args()
    return args.filename1, args.filename2


if __name__ == '__main__':
    main()
