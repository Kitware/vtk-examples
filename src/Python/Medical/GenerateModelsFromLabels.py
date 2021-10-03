#!/usr/bin/env python

import os
import sys

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonCore import (
    VTK_VERSION_NUMBER,
    vtkVersion
)
from vtkmodules.vtkCommonDataModel import (
    vtkDataObject,
    vtkDataSetAttributes
)
from vtkmodules.vtkFiltersCore import (
    vtkMaskFields,
    vtkThreshold,
    vtkWindowedSincPolyDataFilter
)
from vtkmodules.vtkFiltersGeneral import (
    vtkDiscreteFlyingEdges3D,
    vtkDiscreteMarchingCubes
)
from vtkmodules.vtkFiltersGeometry import vtkGeometryFilter
from vtkmodules.vtkIOImage import vtkMetaImageReader
from vtkmodules.vtkIOXML import vtkXMLPolyDataWriter
from vtkmodules.vtkImagingStatistics import vtkImageAccumulate


def main():
    # vtkDiscreteFlyingEdges3D was introduced in VTK >= 8.2
    use_flying_edges = vtk_version_ok(8, 2, 0)

    file_name, start_label, end_label = get_program_parameters()
    if start_label > end_label:
        end_label, start_label = start_label, end_label

    # Create all of the classes we will need
    reader = vtkMetaImageReader()
    histogram = vtkImageAccumulate()
    if use_flying_edges:
        try:
            using_marching_cubes = False
            discrete_cubes = vtkDiscreteFlyingEdges3D()
        except AttributeError:
            using_marching_cubes = True
            discrete_cubes = vtkDiscreteMarchingCubes()
    else:
        using_marching_cubes = True
        discrete_cubes = vtkDiscreteMarchingCubes()
    smoother = vtkWindowedSincPolyDataFilter()
    selector = vtkThreshold()
    scalars_off = vtkMaskFields()
    geometry = vtkGeometryFilter()
    writer = vtkXMLPolyDataWriter()

    # Define all of the variables
    file_prefix = 'Label'
    smoothing_iterations = 15
    pass_band = 0.001
    feature_angle = 120.0

    # Generate models from labels
    # 1) Read the meta file
    # 2) Generate a histogram of the labels
    # 3) Generate models from the labeled volume
    # 4) Smooth the models
    # 5) Output each model into a separate file

    reader.SetFileName(file_name)

    histogram.SetInputConnection(reader.GetOutputPort())
    histogram.SetComponentExtent(0, end_label, 0, 0, 0, 0)
    histogram.SetComponentOrigin(0, 0, 0)
    histogram.SetComponentSpacing(1, 1, 1)
    histogram.Update()

    discrete_cubes.SetInputConnection(reader.GetOutputPort())
    discrete_cubes.GenerateValues(end_label - start_label + 1, start_label, end_label)

    smoother.SetInputConnection(discrete_cubes.GetOutputPort())
    smoother.SetNumberOfIterations(smoothing_iterations)
    smoother.BoundarySmoothingOff()
    smoother.FeatureEdgeSmoothingOff()
    smoother.SetFeatureAngle(feature_angle)
    smoother.SetPassBand(pass_band)
    smoother.NonManifoldSmoothingOn()
    smoother.NormalizeCoordinatesOn()
    smoother.Update()

    selector.SetInputConnection(smoother.GetOutputPort())
    if use_flying_edges:
        if using_marching_cubes:
            selector.SetInputArrayToProcess(0, 0, 0, vtkDataObject().FIELD_ASSOCIATION_CELLS,
                                            vtkDataSetAttributes().SCALARS)
        else:
            selector.SetInputArrayToProcess(0, 0, 0, vtkDataObject().FIELD_ASSOCIATION_POINTS,
                                            vtkDataSetAttributes().SCALARS)
    else:
        selector.SetInputArrayToProcess(0, 0, 0, vtkDataObject().FIELD_ASSOCIATION_CELLS,
                                        vtkDataSetAttributes().SCALARS)

    # Strip the scalars from the output
    scalars_off.SetInputConnection(selector.GetOutputPort())
    scalars_off.CopyAttributeOff(vtkMaskFields().POINT_DATA,
                                 vtkDataSetAttributes().SCALARS)
    scalars_off.CopyAttributeOff(vtkMaskFields().CELL_DATA,
                                 vtkDataSetAttributes().SCALARS)

    geometry.SetInputConnection(scalars_off.GetOutputPort())

    writer.SetInputConnection(geometry.GetOutputPort())

    for i in range(start_label, end_label + 1):
        # see if the label exists, if not skip it
        frequency = histogram.GetOutput().GetPointData().GetScalars().GetTuple1(i)
        if frequency == 0.0:
            continue

        # select the cells for a given label
        selector.SetLowerThreshold(i)
        selector.SetUpperThreshold(i)

        # output the polydata
        output_fn = '{:s}{:d}.vtp'.format(file_prefix, i)
        print('{:s} writing {:s}'.format(os.path.basename(sys.argv[0]), output_fn))

        writer.SetFileName(output_fn)
        writer.Write()


def get_program_parameters():
    import argparse
    description = 'Creates vtkPolyData models from a 3D volume that contains discrete labels.'
    epilogue = '''
These volumes are normally the output of a segmentation algorithm.
The polydata for each label will be output into a separate file.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='Input volume e.g. Frog/frogtissue.mhd.')
    parser.add_argument('startlabel', type=int, help='The starting label in the input volume, e,g, 1.')
    parser.add_argument('endlabel', type=int, help='The ending label in the input volume e.g. 29')
    args = parser.parse_args()
    return args.filename, args.startlabel, args.endlabel


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
        vtk_version_number = VTK_VERSION_NUMBER
    except AttributeError:  # as error:
        ver = vtkVersion()
        vtk_version_number = 10000000000 * ver.GetVTKMajorVersion() + 100000000 * ver.GetVTKMinorVersion() \
                             + ver.GetVTKBuildVersion()
    if vtk_version_number >= needed_version:
        return True
    else:
        return False


if __name__ == '__main__':
    main()
