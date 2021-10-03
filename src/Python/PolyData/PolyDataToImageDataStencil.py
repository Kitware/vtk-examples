#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonDataModel import vtkPlane
from vtkmodules.vtkFiltersCore import (
    vtkCutter,
    vtkImageAppend,
    vtkStripper,
    vtkTriangleFilter
)
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkIOImage import vtkPNGReader
from vtkmodules.vtkImagingStencil import (
    vtkImageStencil,
    vtkPolyDataToImageStencil
)
from vtkmodules.vtkInteractionImage import vtkImageViewer
from vtkmodules.vtkRenderingCore import vtkRenderWindowInteractor


def get_program_parameters():
    import argparse
    description = 'Converts the polydata to imagedata and masks the given imagedata.'
    epilogue = '''
        Contributed by: Peter Gruber

   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('filename', help='A filename e.g. fullhead15.png.')
    args = parser.parse_args()
    return args.filename


def main():
    fn = get_program_parameters()
    # A script to test the stencil filter with a polydata stencil.
    # Image pipeline
    reader = vtkPNGReader()
    reader.SetDataSpacing(0.8, 0.8, 1.5)
    reader.SetDataOrigin(0.0, 0.0, 0.0)
    reader.SetFileName(fn)
    sphere = vtkSphereSource()
    sphere.SetPhiResolution(12)
    sphere.SetThetaResolution(12)
    sphere.SetCenter(102, 102, 0)
    sphere.SetRadius(60)
    triangle = vtkTriangleFilter()
    triangle.SetInputConnection(sphere.GetOutputPort())

    stripper = vtkStripper()
    stripper.SetInputConnection(triangle.GetOutputPort())
    dataToStencil = vtkPolyDataToImageStencil()
    dataToStencil.SetInputConnection(stripper.GetOutputPort())
    dataToStencil.SetOutputSpacing(0.8, 0.8, 1.5)
    dataToStencil.SetOutputOrigin(0.0, 0.0, 0.0)

    stencil = vtkImageStencil()
    stencil.SetInputConnection(reader.GetOutputPort())
    stencil.SetStencilConnection(dataToStencil.GetOutputPort())
    stencil.ReverseStencilOn()
    stencil.SetBackgroundValue(500)

    # test again with a contour
    reader2 = vtkPNGReader()
    reader2.SetDataSpacing(0.8, 0.8, 1.5)
    reader2.SetDataOrigin(0.0, 0.0, 0.0)
    reader2.SetFileName(fn)
    plane = vtkPlane()
    plane.SetOrigin(0, 0, 0)
    plane.SetNormal(0, 0, 1)
    cutter = vtkCutter()
    cutter.SetInputConnection(sphere.GetOutputPort())
    cutter.SetCutFunction(plane)
    stripper2 = vtkStripper()
    stripper2.SetInputConnection(cutter.GetOutputPort())
    dataToStencil2 = vtkPolyDataToImageStencil()
    dataToStencil2.SetInputConnection(stripper2.GetOutputPort())
    dataToStencil2.SetOutputSpacing(0.8, 0.8, 1.5)
    dataToStencil2.SetOutputOrigin(0.0, 0.0, 0.0)
    stencil2 = vtkImageStencil()
    stencil2.SetInputConnection(reader2.GetOutputPort())
    stencil2.SetStencilConnection(dataToStencil2.GetOutputPort())
    stencil2.SetBackgroundValue(500)

    imageAppend = vtkImageAppend()
    imageAppend.SetInputConnection(stencil.GetOutputPort())
    imageAppend.AddInputConnection(stencil2.GetOutputPort())

    viewer = vtkImageViewer()
    interator = vtkRenderWindowInteractor()
    viewer.SetInputConnection(imageAppend.GetOutputPort())
    viewer.SetupInteractor(interator)
    viewer.SetZSlice(0)
    viewer.SetColorWindow(2000)
    viewer.SetColorLevel(1000)
    viewer.GetRenderWindow().SetWindowName('PolyDataToImageDataStencil')
    viewer.Render()

    interator.Start()


if __name__ == '__main__':
    main()
