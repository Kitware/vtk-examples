#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkSphere
from vtkmodules.vtkIOImage import vtkImageReader2Factory
from vtkmodules.vtkImagingCore import (
    vtkImageCast,
    vtkImageShiftScale
)
from vtkmodules.vtkImagingGeneral import vtkImageGaussianSmooth
from vtkmodules.vtkImagingHybrid import vtkSampleFunction
from vtkmodules.vtkImagingMath import vtkImageMathematics
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

    cast = vtkImageCast()
    cast.SetInputConnection(reader.GetOutputPort())
    cast.SetOutputScalarTypeToDouble()

    # Get rid of the discrete scalars.
    smooth = vtkImageGaussianSmooth()
    smooth.SetInputConnection(cast.GetOutputPort())
    smooth.SetStandardDeviations(0.8, 0.8, 0)

    m1 = vtkSphere()
    m1.SetCenter(310, 130, 0)
    m1.SetRadius(0)

    m2 = vtkSampleFunction()
    m2.SetImplicitFunction(m1)
    m2.SetModelBounds(0, 264, 0, 264, 0, 1)
    m2.SetSampleDimensions(264, 264, 1)

    m3 = vtkImageShiftScale()
    m3.SetInputConnection(m2.GetOutputPort())
    m3.SetScale(0.000095)

    div = vtkImageMathematics()
    div.SetInputConnection(0, smooth.GetOutputPort())
    div.SetInputConnection(1, m3.GetOutputPort())
    div.SetOperationToMultiply()

    # Create the actors.
    colorWindow = 256.0
    colorLevel = 127.5
    originalActor = vtkImageActor()
    originalActor.GetMapper().SetInputConnection(cast.GetOutputPort())
    originalActor.GetProperty().SetColorWindow(colorWindow)
    originalActor.GetProperty().SetColorLevel(colorLevel)

    filteredActor = vtkImageActor()
    filteredActor.GetMapper().SetInputConnection(div.GetOutputPort())

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
    renderWindow.AddRenderer(originalRenderer)
    renderWindow.AddRenderer(filteredRenderer)
    renderWindow.SetWindowName('Attenuation')

    renderWindowInteractor = vtkRenderWindowInteractor()
    style = vtkInteractorStyleImage()

    renderWindowInteractor.SetInteractorStyle(style)

    renderWindowInteractor.SetRenderWindow(renderWindow)
    renderWindowInteractor.Initialize()

    renderWindowInteractor.Start()


def get_program_parameters():
    import argparse
    description = 'This MRI image illustrates attenuation that can occur due to sensor position.'
    epilogue = '''
    The artifact is removed by dividing by the attenuation profile determined manually.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='AttenuationArtifact.pgm.')
    args = parser.parse_args()
    return args.filename


if __name__ == '__main__':
    main()
