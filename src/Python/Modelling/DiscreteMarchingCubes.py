#!/usr/bin/env python

import vtk


def main():
    # vtkDiscreteFlyingEdges3D was introduced in VTK >= 8.2
    use_flying_edges = vtk_version_ok(8, 2, 0)

    n = 20
    radius = 8
    blob = make_blob(n, radius)

    if use_flying_edges:
        try:
            discrete = vtk.vtkDiscreteFlyingEdges3D()
        except AttributeError:
            discrete = vtk.vtkDiscreteMarchingCubes()
    else:
        discrete = vtk.vtkDiscreteMarchingCubes()
    discrete.SetInputData(blob)
    discrete.GenerateValues(n, 1, n)

    lut = make_colors(n)

    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(discrete.GetOutputPort())
    mapper.SetLookupTable(lut)
    mapper.SetScalarRange(0, lut.GetNumberOfColors())

    # Create the RenderWindow, Renderer and both Actors
    #
    ren = vtk.vtkRenderer()
    ren_win = vtk.vtkRenderWindow()
    ren_win.AddRenderer(ren)
    ren_win.SetWindowName('DiscreteMarchingCubes')

    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)

    ren.AddActor(actor)

    colors = vtk.vtkNamedColors()
    ren.SetBackground(colors.GetColor3d('Burlywood'))

    ren_win.Render()

    iren.Start()


def vtk_version_ok(major, minor, build):
    """
    Check the VTK version.

    :param major: Major version.
    :param minor: Minor version.
    :param build: Build version.
    :return: True if the requested VTK version is greater or equal to the actual VTK version.
    """
    needed_version = 10000000000 * int(major) + 100000000 * int(minor) + int(build)
    try:
        vtk_version_number = vtk.VTK_VERSION_NUMBER
    except AttributeError:  # as error:
        ver = vtk.vtkVersion()
        vtk_version_number = 10000000000 * ver.GetVTKMajorVersion() + 100000000 * ver.GetVTKMinorVersion() \
                             + ver.GetVTKBuildVersion()
    if vtk_version_number >= needed_version:
        return True
    else:
        return False


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
