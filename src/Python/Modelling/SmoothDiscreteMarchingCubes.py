#!/usr/bin/env python

import vtk


def main():
    n = 20
    radius = 8
    blob = make_blob(n, radius)

    discrete = vtk.vtkDiscreteMarchingCubes()
    discrete.SetInputData(blob)
    discrete.GenerateValues(n, 1, n)

    smoothing_iterations = 15
    pass_band = 0.001
    feature_angle = 120.0

    smoother = vtk.vtkWindowedSincPolyDataFilter()
    smoother.SetInputConnection(discrete.GetOutputPort())
    smoother.SetNumberOfIterations(smoothing_iterations)
    smoother.BoundarySmoothingOff()
    smoother.FeatureEdgeSmoothingOff()
    smoother.SetFeatureAngle(feature_angle)
    smoother.SetPassBand(pass_band)
    smoother.NonManifoldSmoothingOn()
    smoother.NormalizeCoordinatesOn()
    smoother.Update()

    lut = make_colors(n)

    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(smoother.GetOutputPort())
    mapper.SetLookupTable(lut)
    mapper.SetScalarRange(0, lut.GetNumberOfColors())

    # Create the RenderWindow, Renderer and both Actors
    #
    ren = vtk.vtkRenderer()
    ren_win = vtk.vtkRenderWindow()
    ren_win.AddRenderer(ren)
    ren_win.SetWindowName('SmoothDiscreteMarchingCubes')

    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)

    ren.AddActor(actor)

    colors = vtk.vtkNamedColors()
    ren.SetBackground(colors.GetColor3d('Burlywood'))

    ren_win.Render()

    iren.Start()


def make_blob(n, radius):
    blob_image = vtk.vtkImageData()

    max_r = 50 - 2.0 * radius
    random_sequence = vtk.vtkMinimalStandardRandomSequence()
    random_sequence.SetSeed(5071)
    for i in range(0, n):

        sphere = vtk.vtkSphere()
        sphere.SetRadius(radius)

        x = random_sequence.GetRangeValue(-max_r, max_r)
        random_sequence.Next()
        y = random_sequence.GetRangeValue(-max_r, max_r)
        random_sequence.Next()
        z = random_sequence.GetRangeValue(-max_r, max_r)
        random_sequence.Next()

        sphere.SetCenter(int(x), int(y), int(z))

        sampler = vtk.vtkSampleFunction()
        sampler.SetImplicitFunction(sphere)
        sampler.SetOutputScalarTypeToFloat()
        sampler.SetSampleDimensions(100, 100, 100)
        sampler.SetModelBounds(-50, 50, -50, 50, -50, 50)

        thres = vtk.vtkImageThreshold()
        thres.SetInputConnection(sampler.GetOutputPort())
        thres.ThresholdByLower(radius * radius)
        thres.ReplaceInOn()
        thres.ReplaceOutOn()
        thres.SetInValue(i + 1)
        thres.SetOutValue(0)
        thres.Update()
        if i == 0:
            blob_image.DeepCopy(thres.GetOutput())

        max_value = vtk.vtkImageMathematics()
        max_value.SetInputData(0, blob_image)
        max_value.SetInputData(1, thres.GetOutput())
        max_value.SetOperationToMax()
        max_value.Modified()
        max_value.Update()

        blob_image.DeepCopy(max_value.GetOutput())

    return blob_image


def make_colors(n):
    """
    Generate some random colors
    :param n: The number of colors.
    :return: The lookup table.
    """

    lut = vtk.vtkLookupTable()
    lut.SetNumberOfColors(n)
    lut.SetTableRange(0, n - 1)
    lut.SetScaleToLinear()
    lut.Build()
    lut.SetTableValue(0, 0, 0, 0, 1)

    random_sequence = vtk.vtkMinimalStandardRandomSequence()
    random_sequence.SetSeed(5071)
    for i in range(1, n):
        r = random_sequence.GetRangeValue(0.4, 1)
        random_sequence.Next()
        g = random_sequence.GetRangeValue(0.4, 1)
        random_sequence.Next()
        b = random_sequence.GetRangeValue(0.4, 1)
        random_sequence.Next()
        lut.SetTableValue(i, r, g, b, 1.0)

    return lut


if __name__ == '__main__':
    main()
