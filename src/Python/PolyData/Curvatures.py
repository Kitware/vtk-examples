#!/usr/bin/env python

from pathlib import Path

import numpy as np
import vtk
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
    parser.add_argument('-l', default=0, type=float, help='Lower bound e.g. -20.')
    parser.add_argument('-u', default=0, type=float, help='Upper bound e.g. 20.')
    parser.add_argument('-i', default=16, type=int, help='The color map index e.g. 16.')

    group1 = parser.add_mutually_exclusive_group()
    group1.add_argument('-m', help='Use Mean Curvature.', action='store_true')

    args = parser.parse_args()
    return args.file_name, args.l, args.u, args.i, args.m


def main(argv):
    file_name, lower_bound, upper_bound, color_map_idx, mean_curvature = get_program_parameters(argv)

    if not Path(file_name).is_file():
        print(f'The path: {file_name} does not exist.')
        return
    reader = vtk.vtkXMLPolyDataReader()
    reader.SetFileName(file_name)
    reader.Update()

    cc = ComputeCurvatures(reader.GetOutput())
    if mean_curvature:
        cc.set_curvature_type_to_mean()
        if lower_bound == upper_bound == 0:
            cc.mean_bounds_off()
        else:
            cc.set_mean_curvature_bounds(lower_bound, upper_bound)
            cc.mean_bounds_on()
    else:
        cc.set_curvature_type_to_gaussian()
        if lower_bound == upper_bound == 0:
            cc.gauss_bounds_off()
        else:
            # Try these bounds: -100.0, 200.0
            cc.set_gauss_curvature_bounds(lower_bound, upper_bound)
            cc.gauss_bounds_on()
    cc.update()
    scalar_range = cc.source.GetPointData().GetScalars(cc.get_curvature_type()).GetRange()

    # Uncomment the following lines if you want to write out the polydata.
    # writer = vtk.vtkXMLPolyDataWriter()
    # writer.SetFileName('Source.vtp')
    # writer.SetInputData(source)
    # writer.SetDataModeToAscii()
    # writer.Write()

    # Build a lookup table
    color_series = vtk.vtkColorSeries()
    color_series.SetColorScheme(color_map_idx)
    print(f'Using color scheme #: {color_series.GetColorScheme()}, {color_series.GetColorSchemeName()}')

    lut = vtk.vtkColorTransferFunction()
    lut.SetColorSpaceToHSV()

    # Use a color series to create a transfer function
    for i in range(0, color_series.GetNumberOfColors()):
        color = color_series.GetColor(i)
        double_color = list(map(lambda x: x / 255.0, color))
        t = scalar_range[0] + (scalar_range[1] - scalar_range[0]) / (color_series.GetNumberOfColors() - 1) * i
        lut.AddRGBPoint(t, double_color[0], double_color[1], double_color[2])

    colors = vtk.vtkNamedColors()

    # Create a mapper and actor.
    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputData(cc.source)
    mapper.SetScalarModeToUsePointFieldData()
    mapper.SelectColorArray(cc.get_curvature_type())
    mapper.SetScalarRange(scalar_range)
    mapper.SetLookupTable(lut)

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)

    windowWidth = 800
    windowHeight = 800

    # Create a scalar bar
    scalar_bar = vtk.vtkScalarBarActor()
    scalar_bar.SetLookupTable(mapper.GetLookupTable())
    scalar_bar.SetTitle(cc.get_curvature_type().replace('_', '\n'))
    scalar_bar.UnconstrainedFontSizeOn()
    scalar_bar.SetNumberOfLabels(5)
    scalar_bar.SetMaximumWidthInPixels(windowWidth // 8)
    scalar_bar.SetMaximumHeightInPixels(windowHeight // 3)

    # Create a renderer, render window, and interactor
    renderer = vtk.vtkRenderer()
    ren_win = vtk.vtkRenderWindow()
    ren_win.AddRenderer(renderer)
    ren_win.SetSize(windowWidth, windowHeight)
    ren_win.SetWindowName('Curvatures')

    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)
    # Important: The interactor must be set prior to enabling the widget.
    iren.SetRenderWindow(ren_win)

    cam_orient_manipulator = vtk.vtkCameraOrientationWidget()
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


class ComputeCurvatures:
    """
    This class takes a vtkPolyData source and:
     - calculates Gaussian and Mean curvatures,
     - adjusts curvatures along the edges using a weighted average,
     - inserts the adjusted curvatures into the vtkPolyData source.

     Additional methods are provided for setting bounds and precision.
    """

    def __init__(self, polydata_source, gauss_eps=1.0e-08, mean_eps=1.0e-08):
        """

        :param polydata_source: The polydata source.
        :param gauss_eps: Gaussian curvatures less than this will be set to zero.
        :param mean_eps: Mean curvatures less than this will be set to zero.
        """
        self.source = polydata_source
        self.curvature_type = 'Gauss_Curvature'
        self.adjusted_curvatures = dict()
        self.bounds = {'Gauss_Curvature': [0.0, 0.0], 'Mean_Curvature': [0.0, 0.0]}
        self.bounds_state = {'Gauss_Curvature': False, 'Mean_Curvature': False}
        self.epsilons = {'Gauss_Curvature': gauss_eps, 'Mean_Curvature': mean_eps}

    # Remember to run Update after these set and on/off methods.
    def get_curvature_type(self):
        return self.curvature_type

    def set_curvature_type_to_gaussian(self):
        self.curvature_type = 'Gauss_Curvature'

    def set_gauss_epsilon(self, eps=1.0e-08):
        self.bounds['Gauss_Curvature'] = abs(eps)

    def set_gauss_curvature_bounds(self, lower=0.0, upper=0.0):
        if lower <= upper:
            self.bounds['Gauss_Curvature'] = [lower, upper]
        else:
            self.bounds['Gauss_Curvature'] = [upper, lower]
            print('set_gauss_curvature_bounds: bounds swapped since lower > upper')

    def gauss_bounds_on(self):
        self.bounds_state['Gauss_Curvature'] = True

    def gauss_bounds_off(self):
        self.bounds_state['Gauss_Curvature'] = False

    def set_curvature_type_to_mean(self):
        self.curvature_type = 'Mean_Curvature'

    def set_mean_epsilon(self, eps=1.0e-08):
        self.bounds['Gauss_Curvature'] = abs(eps)

    def set_mean_curvature_bounds(self, lower=0.0, upper=0.0):
        if lower <= upper:
            self.bounds['Mean_Curvature'] = [lower, upper]
        else:
            self.bounds['Mean_Curvature'] = [upper, lower]
            print('set_mean_curvature_bounds: bounds swapped since lower > upper')

    def mean_bounds_on(self):
        self.bounds_state['Mean_Curvature'] = True

    def mean_bounds_off(self):
        self.bounds_state['Mean_Curvature'] = False

    def update(self):
        self.compute_curvature_and_fix_up_boundary()
        #  Set small values to zero.
        if self.epsilons[self.curvature_type] != 0.0:
            eps = abs(self.epsilons[self.curvature_type])
            self.adjusted_curvatures[self.curvature_type] = np.where(
                abs(self.adjusted_curvatures[self.curvature_type]) < eps, 0,
                self.adjusted_curvatures[self.curvature_type])
        # Set upper and lower bounds.
        if self.bounds_state[self.curvature_type]:
            lower_bound = self.bounds[self.curvature_type][0]
            self.adjusted_curvatures[self.curvature_type] = np.where(
                self.adjusted_curvatures[self.curvature_type] < lower_bound, lower_bound,
                self.adjusted_curvatures[self.curvature_type])
            upper_bound = self.bounds[self.curvature_type][1]
            self.adjusted_curvatures[self.curvature_type] = np.where(
                self.adjusted_curvatures[self.curvature_type] > upper_bound, upper_bound,
                self.adjusted_curvatures[self.curvature_type])
        self.update_curvature()

    def compute_curvature_and_fix_up_boundary(self):
        # Curvature as vtkPolyData.
        curvature_data = self.compute_curvature()
        # Curvature as python list.
        curvature = self.extract_data(curvature_data)
        # Ids of the boundary points.
        p_ids = self.extract_boundary_ids()
        # Remove duplicate Ids.
        p_ids_set = set(p_ids)

        # Iterate over the edge points and compute the curvature as the weighted
        # average of the neighbors.
        count_invalid = 0
        for p_id in p_ids:
            p_ids_neighbors = self.point_neighborhood(p_id)
            # Keep only interior points.
            p_ids_neighbors -= p_ids_set
            # Compute distances and extract curvature values.
            curvs = [curvature[p_id_n] for p_id_n in p_ids_neighbors]
            dists = [self.compute_distance(p_id_n, p_id) for p_id_n in p_ids_neighbors]
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
                new_curv = 0
            # Set the new curvature value.
            curvature[p_id] = new_curv
        self.adjusted_curvatures[self.curvature_type] = np.array(curvature)

    def compute_curvature(self):
        curvature_filter = vtk.vtkCurvatures()
        curvature_filter.SetInputData(self.source)
        if 'Gauss_Curvature' == self.curvature_type:
            curvature_filter.SetCurvatureTypeToGaussian()
        elif 'Mean_Curvature' == self.curvature_type:
            curvature_filter.SetCurvatureTypeToMean()
        else:
            print('Curvature type must be either Gaussian or Mean.')
            return None
        curvature_filter.Update()
        return curvature_filter.GetOutput()

    def extract_data(self, curvature_data):
        array = curvature_data.GetPointData().GetAbstractArray(self.curvature_type)
        n = curvature_data.GetNumberOfPoints()
        data = []
        for i in range(n):
            data.append(array.GetVariantValue(i).ToDouble())
        return data

    def extract_boundary_ids(self):
        """
        See here: https://discourse.vtk.org/t/2530/3
        """
        array_name = 'ids'
        id_filter = vtk.vtkIdFilter()
        id_filter.SetInputData(self.source)
        id_filter.SetPointIds(True)
        id_filter.SetCellIds(False)
        id_filter.SetPointIdsArrayName(array_name)
        id_filter.SetCellIdsArrayName(array_name)
        id_filter.Update()

        edges = vtk.vtkFeatureEdges()
        edges.SetInputConnection(id_filter.GetOutputPort())
        edges.BoundaryEdgesOn()
        edges.ManifoldEdgesOff()
        edges.NonManifoldEdgesOff()
        edges.FeatureEdgesOff()
        edges.Update()

        array = edges.GetOutput().GetPointData().GetArray(array_name)
        n = edges.GetOutput().GetNumberOfPoints()
        boundary_ids = []
        for i in range(n):
            boundary_ids.append(array.GetValue(i))
        return boundary_ids

    def point_neighborhood(self, p_id):
        """
        Extract the topological neighbors for point pId. In two steps:
        1) self.source.GetPointCells(pId, cell_ids)
        2) self.source.GetCellPoints(c_id, point_ids) for all c_id in cell_ids
        """
        cell_ids = vtk.vtkIdList()
        self.source.GetPointCells(p_id, cell_ids)
        neighbors = set()
        for i in range(0, cell_ids.GetNumberOfIds()):
            cell_id = cell_ids.GetId(i)
            cell_point_ids = vtk.vtkIdList()
            self.source.GetCellPoints(cell_id, cell_point_ids)
            for j in range(0, cell_point_ids.GetNumberOfIds()):
                neighbors.add(cell_point_ids.GetId(j))
        return neighbors

    def compute_distance(self, pt_id_a, pt_id_b):
        pt_a = np.array(self.source.GetPoint(pt_id_a))
        pt_b = np.array(self.source.GetPoint(pt_id_b))
        return np.linalg.norm(pt_a - pt_b)

    def update_curvature(self):
        """
        Add the adjusted curvatures into the self.source.
         :return:
        """
        if self.source.GetNumberOfPoints() != len(self.adjusted_curvatures[self.curvature_type]):
            s = f'{self.curvature_type:15s}:\nCannot add the adjusted curvatures to the source.\n'
            s += ' The number of points in source does not equal the\n'
            s += ' number of point ids in the adjusted curvature array.'
            print(s)
            return
        curvatures = numpy_support.numpy_to_vtk(num_array=self.adjusted_curvatures[self.curvature_type].ravel(),
                                                deep=True,
                                                array_type=vtk.VTK_DOUBLE)
        curvatures.SetName(self.curvature_type)
        self.source.GetPointData().AddArray(curvatures)
        self.source.GetPointData().SetActiveScalars(self.curvature_type)


if __name__ == '__main__':
    import sys

    main(sys.argv)
