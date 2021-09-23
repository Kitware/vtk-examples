#!/usr/bin/env python

from pathlib import Path

import numpy as np
from vtkmodules.numpy_interface import dataset_adapter as dsa
from vtkmodules.vtkCommonColor import (
    vtkColorSeries,
    vtkNamedColors
)
from vtkmodules.vtkCommonCore import (
    VTK_DOUBLE,
    vtkIdList,
    vtkVersion
)
from vtkmodules.vtkFiltersCore import (
    vtkFeatureEdges,
    vtkIdFilter
)
from vtkmodules.vtkFiltersGeneral import vtkCurvatures
# noinspection PyUnresolvedReferences
from vtkmodules.vtkIOXML import (
    vtkXMLPolyDataReader,
    vtkXMLPolyDataWriter
)
from vtkmodules.vtkInteractionWidgets import vtkCameraOrientationWidget
from vtkmodules.vtkRenderingAnnotation import vtkScalarBarActor
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkColorTransferFunction,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)
from vtk.util import numpy_support


def get_program_parameters(argv):
    import argparse
    import textwrap

    description = 'Calculate Gauss or Mean Curvature.'
    epilogue = textwrap.dedent('''
    ''')
    parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter, description=description,
                                     epilog=epilogue)
    parser.add_argument('file_name', help=' e.g. cowHead.vtp.')
    parser.add_argument('-i', default=16, type=int, help='The color map index e.g. 16.')
    parser.add_argument('-g', help='Use Gaussian Curvature.', action='store_true')

    args = parser.parse_args()
    return args.file_name, args.i, args.g


def main(argv):
    file_name, color_map_idx, gaussian_curvature = get_program_parameters(argv)

    if not Path(file_name).is_file():
        print(f'The path: {file_name} does not exist.')
        return
    if gaussian_curvature:
        curvature = 'Gauss_Curvature'
    else:
        curvature = 'Mean_Curvature'

    reader = vtkXMLPolyDataReader()
    reader.SetFileName(file_name)
    reader.Update()

    source = reader.GetOutput()

    cc = vtkCurvatures()
    cc.SetInputData(source)
    if gaussian_curvature:
        cc.SetCurvatureTypeToGaussian()
        cc.Update()
    else:
        cc.SetCurvatureTypeToMean()
        cc.Update()
    adjust_edge_curvatures(cc.GetOutput(), curvature)
    source.GetPointData().AddArray(cc.GetOutput().GetPointData().GetAbstractArray(curvature))
    scalar_range = source.GetPointData().GetScalars(curvature).GetRange()

    # Uncomment the following lines if you want to write out the polydata.
    # writer = vtkXMLPolyDataWriter()
    # writer.SetFileName('Source.vtp')
    # writer.SetInputData(source)
    # writer.SetDataModeToAscii()
    # writer.Write()

    # Build a lookup table
    color_series = vtkColorSeries()
    color_series.SetColorScheme(color_map_idx)
    print(f'Using color scheme #: {color_series.GetColorScheme()}, {color_series.GetColorSchemeName()}')

    lut = vtkColorTransferFunction()
    lut.SetColorSpaceToHSV()

    # Use a color series to create a transfer function
    for i in range(0, color_series.GetNumberOfColors()):
        color = color_series.GetColor(i)
        double_color = list(map(lambda x: x / 255.0, color))
        t = scalar_range[0] + (scalar_range[1] - scalar_range[0]) / (color_series.GetNumberOfColors() - 1) * i
        lut.AddRGBPoint(t, double_color[0], double_color[1], double_color[2])

    colors = vtkNamedColors()

    # Create a mapper and actor.
    mapper = vtkPolyDataMapper()
    mapper.SetInputData(source)
    mapper.SetScalarModeToUsePointFieldData()
    mapper.SelectColorArray(curvature)
    mapper.SetScalarRange(scalar_range)
    mapper.SetLookupTable(lut)

    actor = vtkActor()
    actor.SetMapper(mapper)

    window_width = 800
    window_height = 800

    # Create a scalar bar
    scalar_bar = vtkScalarBarActor()
    scalar_bar.SetLookupTable(mapper.GetLookupTable())
    scalar_bar.SetTitle(curvature.replace('_', '\n'))
    scalar_bar.UnconstrainedFontSizeOn()
    scalar_bar.SetNumberOfLabels(5)
    scalar_bar.SetMaximumWidthInPixels(window_width // 8)
    scalar_bar.SetMaximumHeightInPixels(window_height // 3)

    # Create a renderer, render window, and interactor
    renderer = vtkRenderer()
    ren_win = vtkRenderWindow()
    ren_win.AddRenderer(renderer)
    ren_win.SetSize(window_width, window_height)
    ren_win.SetWindowName('Curvatures')

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)
    # Important: The interactor must be set prior to enabling the widget.
    iren.SetRenderWindow(ren_win)

    if vtk_version_ok(9, 0, 20210718):
        cam_orient_manipulator = vtkCameraOrientationWidget()
        cam_orient_manipulator.SetParentRenderer(renderer)
        # Enable the widget.
        cam_orient_manipulator.On()

    # Add the actors to the scene
    renderer.AddActor(actor)
    renderer.AddActor2D(scalar_bar)
    renderer.SetBackground(colors.GetColor3d('DarkSlateGray'))

    # Render and interact
    ren_win.Render()
    iren.Start()


def vtk_version_ok(major, minor, build):
    """
    Check the VTK version.

    :param major: Requested major version.
    :param minor: Requested minor version.
    :param build: Requested build version.
    :return: True if the requested VTK version is >= the actual VTK version.
    """
    requested_version = (100 * int(major) + int(minor)) * 100000000 + int(build)
    ver = vtkVersion()
    actual_version = (100 * ver.GetVTKMajorVersion() + ver.GetVTKMinorVersion()) \
                     * 100000000 + ver.GetVTKBuildVersion()
    if actual_version >= requested_version:
        return True
    else:
        return False


def adjust_edge_curvatures(source, curvature_name, epsilon=1.0e-08):
    """
    This function adjusts curvatures along the edges of the surface by replacing
     the value with the average value of the curvatures of points in the neighborhood.

    Remember to update the vtkCurvatures object before calling this.

    :param source: A vtkPolyData object corresponding to the vtkCurvatures object.
    :param curvature_name: The name of the curvature, 'Gauss_Curvature' or 'Mean_Curvature'.
    :param epsilon: Absolute curvature values less than this will be set to zero.
    :return:
    """

    def point_neighbourhood(pt_id):
        """
        Find the ids of the neighbours of pt_id.

        :param pt_id: The point id.
        :return: The neighbour ids.
        """
        """
        Extract the topological neighbors for point pId. In two steps:
        1) source.GetPointCells(pt_id, cell_ids)
        2) source.GetCellPoints(cell_id, cell_point_ids) for all cell_id in cell_ids
        """
        cell_ids = vtkIdList()
        source.GetPointCells(pt_id, cell_ids)
        neighbour = set()
        for cell_idx in range(0, cell_ids.GetNumberOfIds()):
            cell_id = cell_ids.GetId(cell_idx)
            cell_point_ids = vtkIdList()
            source.GetCellPoints(cell_id, cell_point_ids)
            for cell_pt_idx in range(0, cell_point_ids.GetNumberOfIds()):
                neighbour.add(cell_point_ids.GetId(cell_pt_idx))
        return neighbour

    def compute_distance(pt_id_a, pt_id_b):
        """
        Compute the distance between two points given their ids.

        :param pt_id_a:
        :param pt_id_b:
        :return:
        """
        pt_a = np.array(source.GetPoint(pt_id_a))
        pt_b = np.array(source.GetPoint(pt_id_b))
        return np.linalg.norm(pt_a - pt_b)

    # Get the active scalars
    source.GetPointData().SetActiveScalars(curvature_name)
    np_source = dsa.WrapDataObject(source)
    curvatures = np_source.PointData[curvature_name]

    #  Get the boundary point IDs.
    array_name = 'ids'
    id_filter = vtkIdFilter()
    id_filter.SetInputData(source)
    id_filter.SetPointIds(True)
    id_filter.SetCellIds(False)
    id_filter.SetPointIdsArrayName(array_name)
    id_filter.SetCellIdsArrayName(array_name)
    id_filter.Update()

    edges = vtkFeatureEdges()
    edges.SetInputConnection(id_filter.GetOutputPort())
    edges.BoundaryEdgesOn()
    edges.ManifoldEdgesOff()
    edges.NonManifoldEdgesOff()
    edges.FeatureEdgesOff()
    edges.Update()

    edge_array = edges.GetOutput().GetPointData().GetArray(array_name)
    boundary_ids = []
    for i in range(edges.GetOutput().GetNumberOfPoints()):
        boundary_ids.append(edge_array.GetValue(i))
    # Remove duplicate Ids.
    p_ids_set = set(boundary_ids)

    # Iterate over the edge points and compute the curvature as the weighted
    # average of the neighbours.
    count_invalid = 0
    for p_id in boundary_ids:
        p_ids_neighbors = point_neighbourhood(p_id)
        # Keep only interior points.
        p_ids_neighbors -= p_ids_set
        # Compute distances and extract curvature values.
        curvs = [curvatures[p_id_n] for p_id_n in p_ids_neighbors]
        dists = [compute_distance(p_id_n, p_id) for p_id_n in p_ids_neighbors]
        curvs = np.array(curvs)
        dists = np.array(dists)
        curvs = curvs[dists > 0]
        dists = dists[dists > 0]
        if len(curvs) > 0:
            weights = 1 / np.array(dists)
            weights /= weights.sum()
            new_curv = np.dot(curvs, weights)
        else:
            # Corner case.
            count_invalid += 1
            # Assuming the curvature of the point is planar.
            new_curv = 0.0
        # Set the new curvature value.
        curvatures[p_id] = new_curv

    #  Set small values to zero.
    if epsilon != 0.0:
        curvatures = np.where(abs(curvatures) < epsilon, 0, curvatures)
        # Curvatures is now an ndarray
        curv = numpy_support.numpy_to_vtk(num_array=curvatures.ravel(),
                                          deep=True,
                                          array_type=VTK_DOUBLE)
        curv.SetName(curvature_name)
        source.GetPointData().RemoveArray(curvature_name)
        source.GetPointData().AddArray(curv)
        source.GetPointData().SetActiveScalars(curvature_name)


if __name__ == '__main__':
    import sys

    main(sys.argv)
