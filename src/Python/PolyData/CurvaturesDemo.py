#!/usr/bin/env python

import numpy as np
from vtk.util import numpy_support

"""
The purpose of this is to demonstrate how to get the Gaussian and Mean curvatures of a surface.

Two different surfaces are used in this demonstration with each surface coloured according
 to its Gaussian and Mean curvatures.

The first surface is a superquadric surface, this demonstrates the use of extra filters
 that are needed to get a nice smooth surface.

The second surface is a parametric surface, in this case the surface has already been triangulated
so no extra processing is necessary.

In order to get a nice coloured image, a vtkColorTransferFunction has been used to generate
 a set of colours for the vtkLookUp tables. We have used a diverging colour space.
Because of the symmetry of the ranges selected for the lookup tables, the white colouration
 represents a midpoint value whilst the blue represents values less than the midopoint value
 and orange represents colours greater than the midpoint value.

In the case of the Random Hills Gaussian Curvature surface, this colouration shows the nature
 of the surface quite nicely. The blue areas are saddle points (negative Gaussian curvature)
 and the orange areas have a positive Gaussian curvature.
In the case of the mean curvature the blue colouration is representing negative curvature
 perpendicular to one of the principal axes.

This example also demonstrates the use of lists and the linking of the elements of the
 lists together to form a pipeline.

"""

import vtk


def main(argv):
    colors = vtk.vtkNamedColors()

    # We are going to handle two different sources.
    # The first source is a superquadric source.
    torus = vtk.vtkSuperquadricSource()
    torus.SetCenter(0.0, 0.0, 0.0)
    torus.SetScale(1.0, 1.0, 1.0)
    torus.SetPhiResolution(64)
    torus.SetThetaResolution(64)
    torus.SetThetaRoundness(1)
    torus.SetThickness(0.5)
    torus.SetSize(0.5)
    torus.SetToroidal(1)

    # Rotate the torus towards the observer (around the x-axis)
    toroid_transform = vtk.vtkTransform()
    toroid_transform.RotateX(55)

    toroid_transform_filter = vtk.vtkTransformFilter()
    toroid_transform_filter.SetInputConnection(torus.GetOutputPort())
    toroid_transform_filter.SetTransform(toroid_transform)

    # The quadric is made of strips, so pass it through a triangle filter as
    # the curvature filter only operates on polys
    tri = vtk.vtkTriangleFilter()
    tri.SetInputConnection(toroid_transform_filter.GetOutputPort())

    # The quadric has nasty discontinuities from the way the edges are generated
    # so let's pass it though a CleanPolyDataFilter and merge any points which
    # are coincident, or very close

    cleaner = vtk.vtkCleanPolyData()
    cleaner.SetInputConnection(tri.GetOutputPort())
    cleaner.SetTolerance(0.005)
    cleaner.Update()

    toroid_poly = ComputeCurvatures(cleaner.GetOutput())

    # The next source will be a parametric function
    rh = vtk.vtkParametricRandomHills()
    rh_fn_src = vtk.vtkParametricFunctionSource()
    rh_fn_src.SetParametricFunction(rh)
    rh_fn_src.Update()

    random_hills_poly = ComputeCurvatures(rh_fn_src.GetOutput())

    # Now we have the sources, lets put them into a list.
    sources = list()
    sources.append(toroid_poly)
    sources.append(toroid_poly)
    sources.append(random_hills_poly)
    sources.append(random_hills_poly)

    lut = get_diverging_lut1()
    # lut = get_diverging_lut()

    renderers = list()
    mappers = list()
    actors = list()
    text_mappers = list()
    text_actors = list()
    scalar_bars = list()

    curvatures = {
        0: 'Gauss_Curvature',
        1: 'Mean_Curvature',
        2: 'Gauss_Curvature',
        3: 'Mean_Curvature',
    }

    # Create a common text property.
    text_property = vtk.vtkTextProperty()
    text_property.SetFontSize(24)
    text_property.SetJustificationToCentered()

    # RenderWindow Dimensions
    #
    renderer_size = 512
    grid_dimensions = 2
    window_width = renderer_size * grid_dimensions
    window_height = renderer_size * grid_dimensions

    # Link the pipeline together.
    for idx, item in enumerate(sources):
        if 'Gauss_Curvature' == curvatures[idx]:
            item.set_curvature_type_to_gaussian()
        if 'Mean_Curvature' == curvatures[idx]:
            item.set_curvature_type_to_mean()
        item.update()
        curvature_type = item.get_curvature_type()
        curvature_name = item.get_curvature_type().replace('_', '\n')

        item.source.GetPointData().SetActiveScalars(curvature_type)
        scalar_range = item.source.GetPointData().GetScalars(curvature_type).GetRange()

        mappers.append(vtk.vtkPolyDataMapper())
        mappers[idx].SetInputData(item.source)
        mappers[idx].SetScalarModeToUsePointFieldData()
        mappers[idx].SelectColorArray(curvature_type)
        mappers[idx].SetScalarRange(scalar_range)
        mappers[idx].SetLookupTable(lut)

        actors.append(vtk.vtkActor())
        actors[idx].SetMapper(mappers[idx])

        text_mappers.append(vtk.vtkTextMapper())
        text_mappers[idx].SetInput(curvature_name)
        text_mappers[idx].SetTextProperty(text_property)

        text_actors.append(vtk.vtkActor2D())
        text_actors[idx].SetMapper(text_mappers[idx])
        text_actors[idx].SetPosition(250, 16)

        # Create a scalar bar
        scalar_bars.append(vtk.vtkScalarBarActor())
        scalar_bars[idx].SetLookupTable(mappers[idx].GetLookupTable())
        scalar_bars[idx].SetTitle(curvature_name)
        scalar_bars[idx].UnconstrainedFontSizeOn()
        scalar_bars[idx].SetNumberOfLabels(5)
        scalar_bars[idx].SetMaximumWidthInPixels(window_width // 8)
        scalar_bars[idx].SetMaximumHeightInPixels(window_height // 3)
        scalar_bars[idx].SetBarRatio(scalar_bars[idx].GetBarRatio() * 0.5)
        scalar_bars[idx].SetPosition(0.85, 0.1)

        renderers.append(vtk.vtkRenderer())

    for idx in range(len(sources)):
        if idx < grid_dimensions * grid_dimensions:
            renderers.append(vtk.vtkRenderer)

    # Create the RenderWindow
    #
    render_window = vtk.vtkRenderWindow()
    render_window.SetSize(renderer_size * grid_dimensions, renderer_size * grid_dimensions)
    render_window.SetWindowName('CurvaturesDemo')

    # Add and position the renders to the render window.
    viewport = list()
    for row in range(grid_dimensions):
        for col in range(grid_dimensions):
            idx = row * grid_dimensions + col

            viewport[:] = []
            viewport.append(float(col) / grid_dimensions)
            viewport.append(float(grid_dimensions - (row + 1)) / grid_dimensions)
            viewport.append(float(col + 1) / grid_dimensions)
            viewport.append(float(grid_dimensions - row) / grid_dimensions)

            if idx > (len(sources) - 1):
                continue

            renderers[idx].SetViewport(viewport)
            render_window.AddRenderer(renderers[idx])

            renderers[idx].AddActor(actors[idx])
            renderers[idx].AddActor(text_actors[idx])
            renderers[idx].AddActor(scalar_bars[idx])
            renderers[idx].SetBackground(colors.GetColor3d('SlateGray'))

    interactor = vtk.vtkRenderWindowInteractor()
    interactor.SetRenderWindow(render_window)
    style = vtk.vtkInteractorStyleTrackballCamera()
    interactor.SetInteractorStyle(style)

    render_window.Render()

    interactor.Start()


def get_diverging_lut1():
    colors = vtk.vtkNamedColors()
    # Colour transfer function.
    ctf = vtk.vtkColorTransferFunction()
    ctf.SetColorSpaceToDiverging()
    p1 = [0.0] + list(colors.GetColor3d('MidnightBlue'))
    p2 = [0.5] + list(colors.GetColor3d('Gainsboro'))
    p3 = [1.0] + list(colors.GetColor3d('DarkOrange'))
    ctf.AddRGBPoint(*p1)
    ctf.AddRGBPoint(*p2)
    ctf.AddRGBPoint(*p3)

    table_size = 256
    lut = vtk.vtkLookupTable()
    lut.SetNumberOfTableValues(table_size)
    lut.Build()

    for i in range(0, table_size):
        rgba = list(ctf.GetColor(float(i) / table_size))
        rgba.append(1)
        lut.SetTableValue(i, rgba)

    return lut


def get_diverging_lut():
    """
    See: [Diverging Color Maps for Scientific Visualization](https://www.kennethmoreland.com/color-maps/)
                       start point         midPoint            end point
     cool to warm:     0.230, 0.299, 0.754 0.865, 0.865, 0.865 0.706, 0.016, 0.150
     purple to orange: 0.436, 0.308, 0.631 0.865, 0.865, 0.865 0.759, 0.334, 0.046
     green to purple:  0.085, 0.532, 0.201 0.865, 0.865, 0.865 0.436, 0.308, 0.631
     blue to brown:    0.217, 0.525, 0.910 0.865, 0.865, 0.865 0.677, 0.492, 0.093
     green to red:     0.085, 0.532, 0.201 0.865, 0.865, 0.865 0.758, 0.214, 0.233

    :return:
    """
    ctf = vtk.vtkColorTransferFunction()
    ctf.SetColorSpaceToDiverging()
    # Cool to warm.
    ctf.AddRGBPoint(0.0, 0.230, 0.299, 0.754)
    ctf.AddRGBPoint(0.5, 0.865, 0.865, 0.865)
    ctf.AddRGBPoint(1.0, 0.706, 0.016, 0.150)

    table_size = 256
    lut = vtk.vtkLookupTable()
    lut.SetNumberOfTableValues(table_size)
    lut.Build()

    for i in range(0, table_size):
        rgba = list(ctf.GetColor(float(i) / table_size))
        rgba.append(1)
        lut.SetTableValue(i, rgba)

    return lut


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
