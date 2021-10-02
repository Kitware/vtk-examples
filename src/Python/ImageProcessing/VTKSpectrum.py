#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkIOImage import vtkImageReader2Factory
from vtkmodules.vtkImagingCore import vtkImageMapToColors
from vtkmodules.vtkImagingFourier import (
    vtkImageFFT,
    vtkImageFourierCenter
)
from vtkmodules.vtkImagingMath import (
    vtkImageLogarithmicScale,
    vtkImageMagnitude
)
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleImage
from vtkmodules.vtkRenderingCore import (
    vtkImageActor,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkWindowLevelLookupTable
)


def main():
    colors = vtkNamedColors()

    fileName = get_program_parameters()

    # Read the image.
    readerFactory = vtkImageReader2Factory()
    reader = readerFactory.CreateImageReader2(fileName)
    reader.SetFileName(fileName)
    reader.Update()

    fft = vtkImageFFT()
    fft.SetInputConnection(reader.GetOutputPort())

    mag = vtkImageMagnitude()
    mag.SetInputConnection(fft.GetOutputPort())

    center = vtkImageFourierCenter()
    center.SetInputConnection(mag.GetOutputPort())

    compress = vtkImageLogarithmicScale()
    compress.SetInputConnection(center.GetOutputPort())
    compress.SetConstant(15)
    compress.Update()

    # Create the actors.
    originalActor = vtkImageActor()
    originalActor.GetMapper().SetInputConnection(reader.GetOutputPort())
    originalActor.GetProperty().SetInterpolationTypeToNearest()

    compressedActor = vtkImageActor()
    compressedActor.GetMapper().SetInputConnection(compress.GetOutputPort())
    compressedActor.GetProperty().SetInterpolationTypeToNearest()
    CreateImageActor(compressedActor, 160, 120)

    # Define the viewport ranges.
    # (xmin, ymin, xmax, ymax)
    originalViewport = [0.0, 0.0, 0.5, 1.0]
    compressedViewport = [0.5, 0.0, 1.0, 1.0]

    # Setup the renderers.
    originalRenderer = vtkRenderer()
    originalRenderer.SetViewport(originalViewport)
    originalRenderer.AddActor(originalActor)
    originalRenderer.ResetCamera()
    originalRenderer.SetBackground(colors.GetColor3d("SlateGray"))

    compressedRenderer = vtkRenderer()
    compressedRenderer.SetViewport(compressedViewport)
    compressedRenderer.AddActor(compressedActor)
    compressedRenderer.ResetCamera()
    compressedRenderer.SetBackground(colors.GetColor3d("LightSlateGray"))

    renderWindow = vtkRenderWindow()
    renderWindow.SetSize(600, 300)
    renderWindow.SetWindowName('VTKSpectrum')
    renderWindow.AddRenderer(originalRenderer)
    renderWindow.AddRenderer(compressedRenderer)

    renderWindowInteractor = vtkRenderWindowInteractor()
    style = vtkInteractorStyleImage()

    renderWindowInteractor.SetInteractorStyle(style)

    renderWindowInteractor.SetRenderWindow(renderWindow)
    renderWindowInteractor.Initialize()

    renderWindowInteractor.Start()


def get_program_parameters():
    import argparse
    description = 'The discrete Fourier transform.'
    epilogue = '''
    This changes an image from the spatial domain into the frequency domain,
     where each pixel represents a sinusoidal function.
    This figure shows an image and its power spectrum displayed using a logarithmic transfer function.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='vtks.pgm.')
    args = parser.parse_args()
    return args.filename


def CreateImageActor(actor, colorWindow, colorLevel):
    wlut = vtkWindowLevelLookupTable()
    wlut.SetWindow(colorWindow)
    wlut.SetLevel(colorLevel)
    wlut.Build()

    # Map the image through the lookup table.
    color = vtkImageMapToColors()
    color.SetLookupTable(wlut)
    color.SetInputData(actor.GetMapper().GetInput())

    actor.GetMapper().SetInputConnection(color.GetOutputPort())
    return


if __name__ == '__main__':
    main()
