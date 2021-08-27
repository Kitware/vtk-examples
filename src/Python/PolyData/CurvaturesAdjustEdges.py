#!/usr/bin/env python

import numpy as np
import vtk
from vtk.util import numpy_support


def main():
    # source = get_bour()
    # source = get_cube()
    # source = get_enneper()
    # source = get_mobius()
    source = get_random_hills()
    # source = get_sphere()
    # source = get_torus()

    cc = ComputeCurvatures(source)

    # After running update() the source will contain the Gaussian and Mean Curvatures.
    cc.update()

    # Uncomment the following lines if you want to write out the polydata.
    # writer = vtk.vtkXMLPolyDataWriter()
    # writer.SetFileName('Source.vtp')
    # writer.SetInputData(source)
    # writer.SetDataModeToAscii()
    # writer.Write()

    # Let's visualise what we have done.

    colors = vtk.vtkNamedColors()
    colors.SetColor("ParaViewBkg", [82, 87, 110, 255])

    window_width = 1024
    window_height = 512

    ren_win = vtk.vtkRenderWindow()
    ren_win.SetSize(window_width, window_height)
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)
    style = vtk.vtkInteractorStyleTrackballCamera()
    iren.SetInteractorStyle(style)

    # Create a common text property.
    text_property = vtk.vtkTextProperty()
    text_property.SetFontSize(24)
    text_property.SetJustificationToCentered()

    lut = get_lut()

    # Define viewport ranges
    xmins = [0, 0.5]
    xmaxs = [0.5, 1]
    ymins = [0, 0]
    ymaxs = [1.0, 1.0]

    camera = None
    cam_orient_manipulator = vtk.vtkCameraOrientationWidget()
    for idx, curvature_name in enumerate(cc.curvature_type):

        curvature_title = curvature_name.replace('_', '\n')

        scalar_range = source.GetPointData().GetScalars(curvature_name).GetRange()

        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInputData(source)
        mapper.SetScalarModeToUsePointFieldData()
        mapper.SelectColorArray(curvature_name)
        mapper.SetScalarRange(scalar_range)
        mapper.SetLookupTable(lut)

        actor = vtk.vtkActor()
        actor.SetMapper(mapper)

        # Create a scalar bar
        scalar_bar = vtk.vtkScalarBarActor()
        scalar_bar.SetLookupTable(mapper.GetLookupTable())
        scalar_bar.SetTitle(curvature_title)
        scalar_bar.UnconstrainedFontSizeOn()
        scalar_bar.SetNumberOfLabels(5)
        scalar_bar.SetMaximumWidthInPixels(window_width // 8)
        scalar_bar.SetMaximumHeightInPixels(window_height // 3)
        scalar_bar.SetBarRatio(scalar_bar.GetBarRatio() * 0.5)
        scalar_bar.SetPosition(0.85, 0.1)

        text_mapper = vtk.vtkTextMapper()
        text_mapper.SetInput(curvature_name)
        text_mapper.SetTextProperty(text_property)

        text_actor = vtk.vtkActor2D()
        text_actor.SetMapper(text_mapper)
        text_actor.SetPosition(250, 16)

        renderer = vtk.vtkRenderer()
        renderer.SetBackground(colors.GetColor3d('ParaViewBkg'))

        renderer.AddActor(actor)
        renderer.AddActor(text_actor)
        renderer.AddActor(scalar_bar)

        ren_win.AddRenderer(renderer)

        if idx == 0:
            cam_orient_manipulator.SetParentRenderer(renderer)
            camera = renderer.GetActiveCamera()
            camera.Elevation(60)
        else:
            renderer.SetActiveCamera(camera)
        renderer.SetViewport(xmins[idx], ymins[idx], xmaxs[idx], ymaxs[idx])
        renderer.ResetCamera()

    # Enable the widget.
    cam_orient_manipulator.On()

    ren_win.Render()
    ren_win.SetWindowName('CurvaturesAdjustEdges')
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


def get_lut():
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


def get_bour():
    u_resolution = 51
    v_resolution = 51
    surface = vtk.vtkParametricBour()

    source = vtk.vtkParametricFunctionSource()
    source.SetUResolution(u_resolution)
    source.SetVResolution(v_resolution)
    source.GenerateTextureCoordinatesOn()
    source.SetParametricFunction(surface)
    source.Update()

    # Build the tangents.
    tangents = vtk.vtkPolyDataTangents()
    tangents.SetInputConnection(source.GetOutputPort())
    tangents.Update()

    transform = vtk.vtkTransform()
    transform.RotateX(0.0)
    transform_filter = vtk.vtkTransformPolyDataFilter()
    transform_filter.SetInputConnection(tangents.GetOutputPort())
    transform_filter.SetTransform(transform)
    transform_filter.Update()

    return transform_filter.GetOutput()


def get_cube():
    surface = vtk.vtkCubeSource()

    # Triangulate.
    triangulation = vtk.vtkTriangleFilter()
    triangulation.SetInputConnection(surface.GetOutputPort())
    # Subdivide the triangles
    subdivide = vtk.vtkLinearSubdivisionFilter()
    subdivide.SetInputConnection(triangulation.GetOutputPort())
    subdivide.SetNumberOfSubdivisions(3)
    # Now the tangents.
    tangents = vtk.vtkPolyDataTangents()
    tangents.SetInputConnection(subdivide.GetOutputPort())
    tangents.Update()
    return tangents.GetOutput()


def get_enneper():
    u_resolution = 51
    v_resolution = 51
    surface = vtk.vtkParametricEnneper()

    source = vtk.vtkParametricFunctionSource()
    source.SetUResolution(u_resolution)
    source.SetVResolution(v_resolution)
    source.GenerateTextureCoordinatesOn()
    source.SetParametricFunction(surface)
    source.Update()

    # Build the tangents.
    tangents = vtk.vtkPolyDataTangents()
    tangents.SetInputConnection(source.GetOutputPort())
    tangents.Update()

    transform = vtk.vtkTransform()
    transform.RotateX(0.0)
    transform_filter = vtk.vtkTransformPolyDataFilter()
    transform_filter.SetInputConnection(tangents.GetOutputPort())
    transform_filter.SetTransform(transform)
    transform_filter.Update()

    return transform_filter.GetOutput()


def get_mobius():
    u_resolution = 51
    v_resolution = 51
    surface = vtk.vtkParametricMobius()
    surface.SetMinimumV(-0.25)
    surface.SetMaximumV(0.25)

    source = vtk.vtkParametricFunctionSource()
    source.SetUResolution(u_resolution)
    source.SetVResolution(v_resolution)
    source.GenerateTextureCoordinatesOn()
    source.SetParametricFunction(surface)
    source.Update()

    # Build the tangents.
    tangents = vtk.vtkPolyDataTangents()
    tangents.SetInputConnection(source.GetOutputPort())
    tangents.Update()

    transform = vtk.vtkTransform()
    transform.RotateX(-90.0)
    transform_filter = vtk.vtkTransformPolyDataFilter()
    transform_filter.SetInputConnection(tangents.GetOutputPort())
    transform_filter.SetTransform(transform)
    transform_filter.Update()

    return transform_filter.GetOutput()


def get_random_hills():
    u_resolution = 51
    v_resolution = 51
    surface = vtk.vtkParametricRandomHills()
    surface.SetRandomSeed(1)
    surface.SetNumberOfHills(30)
    # If you want a plane
    # surface.SetHillAmplitude(0)

    source = vtk.vtkParametricFunctionSource()
    source.SetUResolution(u_resolution)
    source.SetVResolution(v_resolution)
    source.GenerateTextureCoordinatesOn()
    source.SetParametricFunction(surface)
    source.Update()

    # Build the tangents.
    tangents = vtk.vtkPolyDataTangents()
    tangents.SetInputConnection(source.GetOutputPort())
    tangents.Update()

    transform = vtk.vtkTransform()
    transform.Translate(0.0, 5.0, 15.0)
    transform.RotateX(-90.0)
    transform_filter = vtk.vtkTransformPolyDataFilter()
    transform_filter.SetInputConnection(tangents.GetOutputPort())
    transform_filter.SetTransform(transform)
    transform_filter.Update()

    return transform_filter.GetOutput()


def get_sphere():
    theta_resolution = 32
    phi_resolution = 32
    surface = vtk.vtkTexturedSphereSource()
    surface.SetThetaResolution(theta_resolution)
    surface.SetPhiResolution(phi_resolution)

    # Now the tangents.
    tangents = vtk.vtkPolyDataTangents()
    tangents.SetInputConnection(surface.GetOutputPort())
    tangents.Update()
    return tangents.GetOutput()


def get_torus():
    u_resolution = 51
    v_resolution = 51
    surface = vtk.vtkParametricTorus()

    source = vtk.vtkParametricFunctionSource()
    source.SetUResolution(u_resolution)
    source.SetVResolution(v_resolution)
    source.GenerateTextureCoordinatesOn()
    source.SetParametricFunction(surface)
    source.Update()

    # Build the tangents.
    tangents = vtk.vtkPolyDataTangents()
    tangents.SetInputConnection(source.GetOutputPort())
    tangents.Update()

    transform = vtk.vtkTransform()
    transform.RotateX(-90.0)
    transform_filter = vtk.vtkTransformPolyDataFilter()
    transform_filter.SetInputConnection(tangents.GetOutputPort())
    transform_filter.SetTransform(transform)
    transform_filter.Update()

    return transform_filter.GetOutput()


if __name__ == '__main__':
    main()
