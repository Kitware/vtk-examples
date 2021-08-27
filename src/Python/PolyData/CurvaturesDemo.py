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


def main():
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

    cc = list()
    for i in range(table_size):
        cc.append(ctf.GetColor(float(i) / table_size))

    # Lookup table.
    lut = vtk.vtkLookupTable()
    lut.SetNumberOfColors(table_size)
    for i, item in enumerate(cc):
        lut.SetTableValue(i, item[0], item[1], item[2], 1.0)
    lut.Build()

    renderers = list()
    mappers = list()
    actors = list()
    text_mappers = list()
    text_actors = list()
    scalar_bars = list()

    names = {
        0: ['Gauss_Curvature', 'Torus - Gaussian Curvature'],
        1: ['Mean_Curvature', 'Torus - Mean Curvature'],
        2: ['Gauss_Curvature', 'Random Hills - Gaussian Curvature'],
        3: ['Mean_Curvature', 'Random Hills - Mean Curvature'],
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
        item.update()

        curvature_title = names[idx][0].replace('_', '\n')

        item.source.GetPointData().SetActiveScalars(names[idx][0])
        scalar_range = item.source.GetPointData().GetScalars(names[idx][0]).GetRange()

        mappers.append(vtk.vtkPolyDataMapper())
        mappers[idx].SetInputData(item.source)
        mappers[idx].SetScalarModeToUsePointFieldData()
        mappers[idx].SelectColorArray(names[idx][0])
        mappers[idx].SetScalarRange(scalar_range)
        mappers[idx].SetLookupTable(lut)

        actors.append(vtk.vtkActor())
        actors[idx].SetMapper(mappers[idx])

        text_mappers.append(vtk.vtkTextMapper())
        text_mappers[idx].SetInput(names[idx][1])
        text_mappers[idx].SetTextProperty(text_property)

        text_actors.append(vtk.vtkActor2D())
        text_actors[idx].SetMapper(text_mappers[idx])
        text_actors[idx].SetPosition(250, 16)

        # Create a scalar bar
        scalar_bars.append(vtk.vtkScalarBarActor())
        scalar_bars[idx].SetLookupTable(mappers[idx].GetLookupTable())
        scalar_bars[idx].SetTitle(curvature_title)
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
        self.curvature_type = ['Gauss_Curvature', 'Mean_Curvature']
        self.adjusted_curvatures = dict()
        self.bounds = {'Gauss_Curvature': [0.0, 0.0], 'Mean_Curvature': [0.0, 0.0]}
        self.bounds_state = {'Gauss_Curvature': False, 'Mean_Curvature': False}
        self.epsilons = {'Gauss_Curvature': gauss_eps, 'Mean_Curvature': mean_eps}

    def update(self):
        for curvature_name in self.curvature_type:
            self.compute_curvature_and_fix_up_boundary(curvature_name)
            #  Set small values to zero.
            if self.epsilons[curvature_name] != 0.0:
                eps = abs(self.epsilons[curvature_name])
                self.adjusted_curvatures[curvature_name] = np.where(
                    abs(self.adjusted_curvatures[curvature_name]) < eps, 0,
                    self.adjusted_curvatures[curvature_name])
            # Set upper and lower bounds.
            if self.bounds_state[curvature_name]:
                lower_bound = self.bounds[curvature_name][0]
                self.adjusted_curvatures[curvature_name] = np.where(
                    self.adjusted_curvatures[curvature_name] < lower_bound, lower_bound,
                    self.adjusted_curvatures[curvature_name])
                upper_bound = self.bounds[curvature_name][1]
                self.adjusted_curvatures[curvature_name] = np.where(
                    self.adjusted_curvatures[curvature_name] > upper_bound, upper_bound,
                    self.adjusted_curvatures[curvature_name])
            self.update_curvatures(curvature_name)

    def compute_curvature_and_fix_up_boundary(self, curvature_name):
        # Curvature as vtkPolyData.
        curvature_data = self.compute_curvature(curvature_name)
        # Curvature as python list.
        curvature = self.extract_data(curvature_data, curvature_name)
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
        self.adjusted_curvatures[curvature_name] = np.array(curvature)

    def compute_curvature(self, curvature_name):
        curvature_filter = vtk.vtkCurvatures()
        curvature_filter.SetInputData(self.source)
        if 'gaus' in curvature_name.lower():
            curvature_filter.SetCurvatureTypeToGaussian()
        else:
            curvature_filter.SetCurvatureTypeToMean()
        curvature_filter.Update()
        return curvature_filter.GetOutput()

    @staticmethod
    def extract_data(curvature_data, curvature_name):
        array = curvature_data.GetPointData().GetAbstractArray(curvature_name)
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

    def update_curvatures(self, curvature_name):
        """
        Add the adjusted curvatures into the self.source.
         :return:
        """
        if self.source.GetNumberOfPoints() != len(self.adjusted_curvatures[curvature_name]):
            print(curvature_name, ':\nCannot add the adjusted curvatures to the source.\n'
                                  ' The number of points in source does not equal the\n'
                                  ' number of point ids in the adjusted curvature array.')
            return
        curvatures = numpy_support.numpy_to_vtk(num_array=self.adjusted_curvatures[curvature_name].ravel(), deep=True,
                                                array_type=vtk.VTK_DOUBLE)
        curvatures.SetName(curvature_name)
        self.source.GetPointData().AddArray(curvatures)
        self.source.GetPointData().SetActiveScalars(curvature_name)

    # Remember to run Update after these set and on/off methods.
    def set_gauss_curvature_bounds(self, lower=0.0, upper=0.0):
        self.bounds['Gauss_Curvature'] = [lower, upper]

    def gauss_bounds_on(self):
        self.bounds_state['Gauss_Curvature'] = True

    def gauss_bounds_off(self):
        self.bounds_state['Gauss_Curvature'] = False

    def set_mean_curvature_bounds(self, lower=0.0, upper=0.0):
        self.bounds['Mean_Curvature'] = [lower, upper]

    def mean_bounds_on(self):
        self.bounds_state['Mean_Curvature'] = True

    def mean_bounds_off(self):
        self.bounds_state['Mean_Curvature'] = False

    def set_epsilons(self, gauss_eps=1.0e-08, mean_eps=1.0e-08):
        self.epsilons = {'Gauss_Curvature': gauss_eps, 'Mean_Curvature': mean_eps}


if __name__ == '__main__':
    main()
