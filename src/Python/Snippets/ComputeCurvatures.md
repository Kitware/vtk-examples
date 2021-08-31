### Description

A class that refines vtkCurvatures to adjust for edge effects.

To use the snippet, click the *Copy to clipboard* at the upper right of the code blocks.

### Implementation

``` Python

import numpy as np
import vtk
from vtk.util import numpy_support

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

```

### Usage

```python
    cc = ComputeCurvatures(source)

    # Add any refinements that you would like to make here e.g.:
    # cc = ComputeCurvatures(source)
    # cc.set_gauss_curvature_bounds(-0.5, 1.0)
    # cc.gauss_bounds_on()

    # After running update() the source will contain the Gaussian and Mean Curvatures.
    cc.update()
```
