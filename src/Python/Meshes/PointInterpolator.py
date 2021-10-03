#!/usr/bin/env python

import numpy as np
# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import vtkImageData
from vtkmodules.vtkFiltersCore import vtkResampleWithDataSet
from vtkmodules.vtkFiltersGeneral import vtkTableToPolyData
from vtkmodules.vtkFiltersPoints import (
    vtkGaussianKernel,
    vtkPointInterpolator
)
from vtkmodules.vtkIOGeometry import vtkSTLReader
from vtkmodules.vtkIOInfovis import vtkDelimitedTextReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPointGaussianMapper,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def get_program_parameters():
    import argparse
    description = 'Plot the scalar field of points onto a PolyData surface.'
    epilogue = '''
This example uses vtkPointInterpolator with a Gaussian Kernel (or other kernel)
 to interpolate and extrapolate more smoothly the fields inside and outside the probed area.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('points_fn', help='sparsePoints.txt.')
    parser.add_argument('probe_fn', help='InterpolatingOnSTL_final.stl.')

    args = parser.parse_args()
    return args.points_fn, args.probe_fn


def main():
    points_fn, probe_fn = get_program_parameters()

    colors = vtkNamedColors()

    points_reader = vtkDelimitedTextReader()
    points_reader.SetFileName(points_fn)
    points_reader.DetectNumericColumnsOn()
    points_reader.SetFieldDelimiterCharacters('\t')
    points_reader.SetHaveHeaders(True)

    table_points = vtkTableToPolyData()
    table_points.SetInputConnection(points_reader.GetOutputPort())
    table_points.SetXColumn('x')
    table_points.SetYColumn('y')
    table_points.SetZColumn('z')
    table_points.Update()

    points = table_points.GetOutput()
    points.GetPointData().SetActiveScalars('val')
    range = points.GetPointData().GetScalars().GetRange()

    # Read a probe surface
    stl_reader = vtkSTLReader()
    stl_reader.SetFileName(probe_fn)
    stl_reader.Update()

    surface = stl_reader.GetOutput()
    bounds = np.array(surface.GetBounds())

    dims = np.array([101, 101, 101])
    box = vtkImageData()
    box.SetDimensions(dims)
    box.SetSpacing((bounds[1::2] - bounds[:-1:2]) / (dims - 1))
    box.SetOrigin(bounds[::2])

    # Gaussian kernel
    gaussian_kernel = vtkGaussianKernel()
    gaussian_kernel.SetSharpness(2)
    gaussian_kernel.SetRadius(12)

    interpolator = vtkPointInterpolator()
    interpolator.SetInputData(box)
    interpolator.SetSourceData(points)
    interpolator.SetKernel(gaussian_kernel)

    resample = vtkResampleWithDataSet()
    resample.SetInputData(surface)
    resample.SetSourceConnection(interpolator.GetOutputPort())

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(resample.GetOutputPort())
    mapper.SetScalarRange(range)

    actor = vtkActor()
    actor.SetMapper(mapper)

    point_mapper = vtkPointGaussianMapper()
    point_mapper.SetInputData(points)
    point_mapper.SetScalarRange(range)
    point_mapper.SetScaleFactor(0.6)
    point_mapper.EmissiveOff();
    point_mapper.SetSplatShaderCode(
        "//VTK::Color::Impl\n"
        "float dist = dot(offsetVCVSOutput.xy,offsetVCVSOutput.xy);\n"
        "if (dist > 1.0) {\n"
        "  discard;\n"
        "} else {\n"
        "  float scale = (1.0 - dist);\n"
        "  ambientColor *= scale;\n"
        "  diffuseColor *= scale;\n"
        "}\n"
    )

    point_actor = vtkActor()
    point_actor.SetMapper(point_mapper)

    renderer = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(renderer)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    renderer.AddActor(actor)
    renderer.AddActor(point_actor)
    renderer.SetBackground(colors.GetColor3d('SlateGray'))

    renWin.SetSize(640, 480)
    renWin.SetWindowName('PointInterpolator')

    renderer.ResetCamera()
    renderer.GetActiveCamera().Elevation(-45)

    iren.Initialize()

    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()
