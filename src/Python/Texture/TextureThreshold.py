#!/usr/bin/env python

# Modified from VTK/Filters/Texture/Testing/Python/textureThreshold.py.

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import vtkStructuredGridOutlineFilter
from vtkmodules.vtkFiltersGeometry import vtkStructuredGridGeometryFilter
from vtkmodules.vtkFiltersTexture import vtkThresholdTextureCoords
from vtkmodules.vtkIOLegacy import vtkStructuredPointsReader
from vtkmodules.vtkIOParallel import vtkMultiBlockPLOT3DReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkCamera,
    vtkDataSetMapper,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTexture
)


def main():
    dataFn1, dataFn2, textureFn = get_program_parameters()
    colors = vtkNamedColors()

    # Read the data.
    pl3d = vtkMultiBlockPLOT3DReader()
    pl3d.SetXYZFileName(dataFn1)
    pl3d.SetQFileName(dataFn2)
    pl3d.SetScalarFunctionNumber(100)
    pl3d.SetVectorFunctionNumber(202)
    pl3d.Update()
    output = pl3d.GetOutput().GetBlock(0)

    # Make the wall (floor).
    wall = vtkStructuredGridGeometryFilter()
    wall.SetInputData(output)
    wall.SetExtent(0, 100, 0, 0, 0, 100)
    wallMap = vtkPolyDataMapper()
    wallMap.SetInputConnection(wall.GetOutputPort())
    wallMap.ScalarVisibilityOff()
    wallActor = vtkActor()
    wallActor.SetMapper(wallMap)
    wallActor.GetProperty().SetColor(colors.GetColor3d('PeachPuff'))

    # Make the fin (rear wall)
    fin = vtkStructuredGridGeometryFilter()
    fin.SetInputData(output)
    fin.SetExtent(0, 100, 0, 100, 0, 0)
    finMap = vtkPolyDataMapper()
    finMap.SetInputConnection(fin.GetOutputPort())
    finMap.ScalarVisibilityOff()
    finActor = vtkActor()
    finActor.SetMapper(finMap)
    finActor.GetProperty().SetColor(colors.GetColor3d('DarkSlateGray'))

    # Get the texture.
    tmap = vtkStructuredPointsReader()
    tmap.SetFileName(textureFn)
    texture = vtkTexture()
    texture.SetInputConnection(tmap.GetOutputPort())
    texture.InterpolateOff()
    texture.RepeatOff()

    # Create the rendering window, renderer, and interactive renderer.
    ren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Make the planes to threshold and texture.
    plane = list()
    thresh = list()
    planeMap = list()
    planeActor = list()
    # Define the extents of planes that we will use.
    planeExtents = [[10, 10, 0, 100, 0, 100],
                    [30, 30, 0, 100, 0, 100],
                    [35, 35, 0, 100, 0, 100]]
    # Now set up the pipeline.
    for i in range(0, len(planeExtents)):
        plane.append(vtkStructuredGridGeometryFilter())
        plane[i].SetInputData(output)
        plane[i].SetExtent(*planeExtents[i])
        thresh.append(vtkThresholdTextureCoords())
        thresh[i].SetInputConnection(plane[i].GetOutputPort())
        thresh[i].SetInputConnection(plane[i].GetOutputPort())
        # If you want an image similar to Fig 9-43(a) in the VTK textbook,
        # set thresh[i].ThresholdByUpper(1.5) for all planes.
        if i == 1:
            thresh[i].ThresholdByLower(1.5)
        elif i == 2:
            thresh[i].ThresholdBetween(1.5, 1.8)
        else:
            thresh[i].ThresholdByUpper(1.5)
        planeMap.append(vtkDataSetMapper())
        planeMap[i].SetInputConnection(thresh[i].GetOutputPort())
        planeMap[i].SetScalarRange(output.GetScalarRange())
        planeActor.append(vtkActor())
        planeActor[i].SetMapper(planeMap[i])
        planeActor[i].SetTexture(texture)
        #  The slight transparency gives a nice effect.
        planeActor[i].GetProperty().SetOpacity(0.999)
        ren.AddActor(planeActor[i])

    # Get an outline of the data set for context.
    outline = vtkStructuredGridOutlineFilter()
    outline.SetInputData(output)
    outlineMapper = vtkPolyDataMapper()
    outlineMapper.SetInputConnection(outline.GetOutputPort())
    outlineActor = vtkActor()
    outlineActor.SetMapper(outlineMapper)
    outlineProp = outlineActor.GetProperty()
    outlineProp.SetColor(colors.GetColor3d('Black'))

    # Add the remaining actors to the renderer, set the background and size.
    ren.AddActor(outlineActor)
    ren.AddActor(wallActor)
    ren.AddActor(finActor)
    ren.SetBackground(colors.GetColor3d('MistyRose'))
    renWin.SetSize(512, 512)
    renWin.SetWindowName('TextureThreshold')

    cam = vtkCamera()
    cam.SetClippingRange(1.51176, 75.5879)
    cam.SetFocalPoint(2.33749, 2.96739, 3.61023)
    cam.SetPosition(10.8787, 5.27346, 15.8687)
    cam.SetViewAngle(30)
    cam.SetViewUp(-0.0610856, 0.987798, -0.143262)
    ren.SetActiveCamera(cam)

    iren.Initialize()
    iren.Start()


def get_program_parameters():
    import argparse
    description = 'Demonstrating texture thresholding applied to scalar data from a simulation of fluid flow.'
    epilogue = '''
    There are three planes cutting the blunt fin with different thresholds set. 
     From the left, the scalar threshold is set so that:
       1) Only data with a scalar value greater than or equal to 1.5 is shown.
       2) Only data with a scalar value less than or equal to 1.5 is shown.
       3) Only data with a scalar value between 1.5 and 1.8 inclusive is shown.
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('dataFn1', help='bluntfinxyz.bin.')
    parser.add_argument('dataFn2', help='bluntfinq.bin.')
    parser.add_argument('textureFn', help='texThres2.bmp')
    args = parser.parse_args()
    return args.dataFn1, args.dataFn2, args.textureFn


if __name__ == '__main__':
    main()
