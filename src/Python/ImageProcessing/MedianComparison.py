#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonDataModel import vtkImageData
from vtkmodules.vtkIOImage import vtkImageReader2Factory
from vtkmodules.vtkImagingCore import (
    vtkImageCast,
    vtkImageThreshold
)
from vtkmodules.vtkImagingGeneral import (
    vtkImageGaussianSmooth,
    vtkImageMedian3D
)
from vtkmodules.vtkImagingMath import vtkImageMathematics
from vtkmodules.vtkImagingSources import vtkImageNoiseSource
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleImage
from vtkmodules.vtkRenderingCore import (
    vtkImageActor,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    # colors = vtkNamedColors()

    fileName = get_program_parameters()

    # Read the image.
    readerFactory = vtkImageReader2Factory()
    reader = readerFactory.CreateImageReader2(fileName)
    reader.SetFileName(fileName)
    reader.Update()

    scalarRange = [0] * 2
    scalarRange[0] = reader.GetOutput().GetPointData().GetScalars().GetRange()[0]
    scalarRange[1] = reader.GetOutput().GetPointData().GetScalars().GetRange()[1]
    print("Range:", scalarRange)
    middleSlice = (reader.GetOutput().GetExtent()[5] - reader.GetOutput().GetExtent()[4]) // 2

    # Work with double images
    cast = vtkImageCast()
    cast.SetInputConnection(reader.GetOutputPort())
    cast.SetOutputScalarTypeToDouble()
    cast.Update()

    originalData = vtkImageData()
    originalData.DeepCopy(cast.GetOutput())

    noisyData = vtkImageData()

    AddShotNoise(originalData, noisyData, 2000.0, 0.1, reader.GetOutput().GetExtent())
    median = vtkImageMedian3D()
    median.SetInputData(noisyData)
    median.SetKernelSize(5, 5, 1)

    gaussian = vtkImageGaussianSmooth()
    gaussian.SetDimensionality(2)
    gaussian.SetInputData(noisyData)
    gaussian.SetStandardDeviations(2.0, 2.0)
    gaussian.SetRadiusFactors(2.0, 2.0)

    colorWindow = (scalarRange[1] - scalarRange[0]) * 0.8
    colorLevel = colorWindow / 2
    originalActor = vtkImageActor()
    originalActor.GetMapper().SetInputData(originalData)
    originalActor.GetProperty().SetColorWindow(colorWindow)
    originalActor.GetProperty().SetColorLevel(colorLevel)
    originalActor.GetProperty().SetInterpolationTypeToNearest()
    originalActor.SetZSlice(middleSlice)

    noisyActor = vtkImageActor()
    noisyActor.GetMapper().SetInputData(noisyData)
    noisyActor.GetProperty().SetColorWindow(colorWindow)
    noisyActor.GetProperty().SetColorLevel(colorLevel)
    noisyActor.GetProperty().SetInterpolationTypeToNearest()
    noisyActor.SetZSlice(middleSlice)

    gaussianActor = vtkImageActor()
    gaussianActor.GetMapper().SetInputConnection(gaussian.GetOutputPort())
    gaussianActor.GetProperty().SetColorWindow(colorWindow)
    gaussianActor.GetProperty().SetColorLevel(colorLevel)
    gaussianActor.GetProperty().SetInterpolationTypeToNearest()
    gaussianActor.SetZSlice(middleSlice)

    medianActor = vtkImageActor()
    medianActor.GetMapper().SetInputConnection(median.GetOutputPort())
    medianActor.GetProperty().SetColorWindow(colorWindow)
    medianActor.GetProperty().SetColorLevel(colorLevel)
    medianActor.GetProperty().SetInterpolationTypeToNearest()
    medianActor.SetZSlice(middleSlice)

    # Setup the renderers.
    originalRenderer = vtkRenderer()
    originalRenderer.AddActor(originalActor)
    noisyRenderer = vtkRenderer()
    noisyRenderer.AddActor(noisyActor)
    gaussRenderer = vtkRenderer()
    gaussRenderer.AddActor(gaussianActor)
    medianRenderer = vtkRenderer()
    medianRenderer.AddActor(medianActor)

    renderers = list()
    renderers.append(originalRenderer)
    renderers.append(noisyRenderer)
    renderers.append(gaussRenderer)
    renderers.append(medianRenderer)

    # Setup viewports for the renderers.
    rendererSize = 400
    xGridDimensions = 2
    yGridDimensions = 2

    renderWindow = vtkRenderWindow()
    renderWindow.SetSize(
        rendererSize * xGridDimensions, rendererSize * yGridDimensions)
    for row in range(0, yGridDimensions):
        for col in range(xGridDimensions):
            index = row * xGridDimensions + col
            # (xmin, ymin, xmax, ymax)
            viewport = [float(col) / xGridDimensions, float(yGridDimensions - (row + 1)) / yGridDimensions,
                        float(col + 1) / xGridDimensions, float(yGridDimensions - row) / yGridDimensions]
            renderers[index].SetViewport(viewport)
            renderWindow.AddRenderer(renderers[index])
    renderWindow.SetWindowName('MedianComparison')

    renderWindowInteractor = vtkRenderWindowInteractor()
    style = vtkInteractorStyleImage()

    renderWindowInteractor.SetInteractorStyle(style)
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # The renderers share one camera.
    renderWindow.Render()
    renderers[0].GetActiveCamera().Dolly(1.5)
    renderers[0].ResetCameraClippingRange()
    for r in range(1, len(renderers)):
        renderers[r].SetActiveCamera(renderers[0].GetActiveCamera())
    renderWindowInteractor.Initialize()
    renderWindowInteractor.Start()


def get_program_parameters():
    import argparse
    description = 'Comparison of Gaussian and Median smoothing for reducing low-probability high-amplitude noise.'
    epilogue = '''
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='FullHead.mhd.')
    args = parser.parse_args()
    return args.filename


def AddShotNoise(inputImage, outputImage, noiseAmplitude, noiseFraction, extent):
    shotNoiseSource = vtkImageNoiseSource()
    shotNoiseSource.SetWholeExtent(extent)
    shotNoiseSource.SetMinimum(0.0)
    shotNoiseSource.SetMaximum(1.0)

    shotNoiseThresh1 = vtkImageThreshold()
    shotNoiseThresh1.SetInputConnection(shotNoiseSource.GetOutputPort())
    shotNoiseThresh1.ThresholdByLower(1.0 - noiseFraction)
    shotNoiseThresh1.SetInValue(0)
    shotNoiseThresh1.SetOutValue(noiseAmplitude)
    shotNoiseThresh2 = vtkImageThreshold()
    shotNoiseThresh2.SetInputConnection(shotNoiseSource.GetOutputPort())
    shotNoiseThresh2.ThresholdByLower(noiseFraction)
    shotNoiseThresh2.SetInValue(1.0 - noiseAmplitude)
    shotNoiseThresh2.SetOutValue(0.0)

    shotNoise = vtkImageMathematics()
    shotNoise.SetInputConnection(0, shotNoiseThresh1.GetOutputPort())
    shotNoise.SetInputConnection(1, shotNoiseThresh2.GetOutputPort())
    shotNoise.SetOperationToAdd()

    add = vtkImageMathematics()
    add.SetInputData(0, inputImage)
    add.SetInputConnection(1, shotNoise.GetOutputPort())
    add.SetOperationToAdd()
    add.Update()
    outputImage.DeepCopy(add.GetOutput())


if __name__ == '__main__':
    main()
