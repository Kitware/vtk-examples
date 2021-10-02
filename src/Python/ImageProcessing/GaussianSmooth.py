#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkIOImage import vtkImageReader2Factory
from vtkmodules.vtkImagingCore import vtkImageCast
from vtkmodules.vtkImagingGeneral import vtkImageGaussianSmooth
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleImage
from vtkmodules.vtkRenderingCore import (
    vtkImageActor,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    fileName = get_program_parameters()

    # Read the image.
    readerFactory = vtkImageReader2Factory()
    reader = readerFactory.CreateImageReader2(fileName)
    reader.SetFileName(fileName)
    reader.Update()

    # Process the image.
    cast = vtkImageCast()
    cast.SetInputConnection(reader.GetOutputPort())
    cast.SetOutputScalarTypeToFloat()

    smoothing_filter = vtkImageGaussianSmooth()
    smoothing_filter.SetDimensionality(2)
    smoothing_filter.SetInputConnection(cast.GetOutputPort())
    smoothing_filter.SetStandardDeviations(4.0, 4.0)
    smoothing_filter.SetRadiusFactors(2.0, 2.0)

    # Create the actors.
    originalActor = vtkImageActor()
    originalActor.GetMapper().SetInputConnection(
        reader.GetOutputPort())

    filteredActor = vtkImageActor()
    filteredActor.GetMapper().SetInputConnection(
        smoothing_filter.GetOutputPort())

    # Define the viewport ranges.
    # (xmin, ymin, xmax, ymax)
    originalViewport = [0.0, 0.0, 0.5, 1.0]
    filteredViewport = [0.5, 0.0, 1.0, 1.0]

    # Setup the renderers.
    originalRenderer = vtkRenderer()
    originalRenderer.SetViewport(originalViewport)
    originalRenderer.AddActor(originalActor)
    originalRenderer.ResetCamera()
    originalRenderer.SetBackground(colors.GetColor3d("SlateGray"))

    filteredRenderer = vtkRenderer()
    filteredRenderer.SetViewport(filteredViewport)
    filteredRenderer.AddActor(filteredActor)
    filteredRenderer.ResetCamera()
    filteredRenderer.SetBackground(colors.GetColor3d("LightSlateGray"))

    renderWindow = vtkRenderWindow()
    renderWindow.SetSize(600, 300)
    renderWindow.SetWindowName('GaussianSmooth')
    renderWindow.AddRenderer(originalRenderer)
    renderWindow.AddRenderer(filteredRenderer)

    renderWindowInteractor = vtkRenderWindowInteractor()
    style = vtkInteractorStyleImage()

    renderWindowInteractor.SetInteractorStyle(style)

    renderWindowInteractor.SetRenderWindow(renderWindow)
    renderWindowInteractor.Initialize()

    renderWindowInteractor.Start()


def get_program_parameters():
    import argparse
    description = 'Low-pass filters can be implemented as convolution with a Gaussian kernel.'
    epilogue = '''
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='Gourds.png.')
    args = parser.parse_args()
    return args.filename


if __name__ == '__main__':
    main()
