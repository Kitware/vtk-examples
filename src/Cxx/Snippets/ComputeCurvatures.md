### Description

A class that refines vtkCurvatures to adjust for edge effects. The namespace is anonymous, however feel free to rename it.

To use the snippet, click the *Copy to clipboard* at the upper right of the code blocks.

### Declaration Section

```c++

#include <array>
#include <map>
#include <set>
#include <string>

namespace {

/**
@class ComputeCurvatures

This class takes a vtkPolyData source and:
 - calculates Gaussian and Mean curvatures,
 - adjusts curvatures along the edges using a weighted average,
 - inserts the adjusted curvatures into the vtkPolyData source.

 Additional methods are provided for setting bounds and precision.
*/
class ComputeCurvatures
{
public:
  ComputeCurvatures() = default;
  ~ComputeCurvatures() = default;

  explicit ComputeCurvatures(vtkPolyData* source)
  {
    this->source = source;
  }

  ComputeCurvatures(vtkPolyData* source, double const& gaussEps,
                    double const& meanEps)
  {
    this->source = source;
    this->epsilons["Gauss_Curvature"] = gaussEps;
    this->epsilons["Mean_Curvature"] = meanEps;
  }

public:
  void Update();

  // Remember to run Update after these set and on/off methods.
  void SetGaussCurvatureBounds(double const& lower = 0.0,
                               double const& upper = 0.0);
  void GaussBoundsOn();
  void GaussBoundsOff();
  void SetMeanCurvatureBounds(double const& lower = 0.0,
                              double const& upper = 0.0);
  void MeanBoundsOn();
  void MeanBoundsOff();
  void SetEpsilons(double const& gauss_eps = 1.0e-8,
                   double const& mean_eps = 1.0e-8);

private:
  void ComputeCurvatureAndFixUpBoundary(std::string const& curvatureName);
  vtkSmartPointer<vtkPolyData>
  ComputeCurvature(std::string const& curvatureName);
  std::vector<double> ExtractData(vtkPolyData* curvatureData,
                                  std::string const& name);
  std::vector<vtkIdType> ExtractBoundaryIds();
  double ComputeDistance(vtkIdType const& ptIdA, vtkIdType const& ptIdB);
  std::set<vtkIdType> PointNeighborhood(vtkIdType const& pId);
  void UpdateCurvatures(std::string const& curvatureName);

public:
  vtkSmartPointer<vtkPolyData> source;
  std::array<std::string, 2> curvatureType{"Gauss_Curvature", "Mean_Curvature"};

private:
  std::map<std::string, std::vector<double>> adjustedCurvatures;
  std::map<std::string, std::vector<double>> bounds{
      {"Gauss_Curvature", {0.0, 0.0}}, {"Mean_Curvature", {0.0, 0.0}}};
  std::map<std::string, bool> boundsState{{"Gauss_Curvature", false},
                                          {"Mean_Curvature", false}};
  std::map<std::string, double> epsilons{{"Gauss_Curvature", 1.0e-8},
                                         {"Mean_Curvature", 1.0e-8}};
};

} // namespace

```

### Implementation Section

``` c++

#include <vtkCurvatures.h>
#include <vtkDoubleArray.h>
#include <vtkFeatureEdges.h>
#include <vtkIdFilter.h>
#include <vtkIdList.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

#include <algorithm>
#include <cctype>
#include <iterator>
#include <numeric>
#include <vector>

namespace {

void ComputeCurvatures::Update()
{
  for (auto curvatureName : this->curvatureType)
  {
    this->ComputeCurvatureAndFixUpBoundary(curvatureName);
    // Set small values to zero.
    if (this->epsilons[curvatureName] != 0.0)
    {
      auto eps = std::abs(this->epsilons[curvatureName]);
      for (size_t i = 0; i < this->adjustedCurvatures[curvatureName].size();
           ++i)
      {
        if (std::abs(this->adjustedCurvatures[curvatureName][i]) < eps)
        {
          this->adjustedCurvatures[curvatureName][i] = 0.0;
        }
      }
    }
    //  Set upper and lower bounds.
    if (this->boundsState[curvatureName])
    {
      auto lowerBound = this->bounds[curvatureName][0];
      for (size_t i = 0; i < this->adjustedCurvatures[curvatureName].size();
           ++i)
      {
        if (this->adjustedCurvatures[curvatureName][i] < lowerBound)
        {
          this->adjustedCurvatures[curvatureName][i] = lowerBound;
        }
      }
      auto upperBound = this->bounds[curvatureName][1];
      for (size_t i = 0; i < this->adjustedCurvatures[curvatureName].size();
           ++i)
      {
        if (this->adjustedCurvatures[curvatureName][i] > upperBound)
        {
          this->adjustedCurvatures[curvatureName][i] = upperBound;
        }
      }
    }
    this->UpdateCurvatures(curvatureName);
  }
}

void ComputeCurvatures::ComputeCurvatureAndFixUpBoundary(
    std::string const& curvatureName)
{
  // Curvature as vtkPolyData.
  auto curvatureData = this->ComputeCurvature(curvatureName);
  // Curvature as a vector.
  auto curvature = this->ExtractData(curvatureData, curvatureName);
  // Ids of the boundary points.
  auto pIds = this->ExtractBoundaryIds();
  // Remove duplicate Ids.
  std::set<vtkIdType> pIdsSet(pIds.begin(), pIds.end());
  for (auto pId : pIds)
  {
    auto pIdsNeighbors = this->PointNeighborhood(pId);
    std::set<vtkIdType> pIdsNeighborsInterior;
    std::set_difference(
        pIdsNeighbors.begin(), pIdsNeighbors.end(), pIdsSet.begin(),
        pIdsSet.end(),
        std::inserter(pIdsNeighborsInterior, pIdsNeighborsInterior.begin()));
    // Compute distances and extract curvature values.
    std::vector<double> curvs;
    std::vector<double> dists;
    for (auto pIdN : pIdsNeighborsInterior)
    {
      curvs.push_back(curvature[pIdN]);
      dists.push_back(this->ComputeDistance(pIdN, pId));
    }
    std::vector<vtkIdType> nonZeroDistIds;
    for (size_t i = 0; i < dists.size(); ++i)
    {
      if (dists[i] > 0)
      {
        nonZeroDistIds.push_back(i);
      }
    }
    std::vector<double> curvsNonZero;
    std::vector<double> distsNonZero;
    for (auto i : nonZeroDistIds)
    {
      curvsNonZero.push_back(curvs[i]);
      distsNonZero.push_back(dists[i]);
    }
    // Iterate over the edge points and compute the curvature as the weighted
    // average of the neighbors.
    auto countInvalid = 0;
    auto newCurv = 0.0;
    if (curvsNonZero.size() > 0)
    {
      std::vector<double> weights;
      double sum = 0.0;
      for (auto d : distsNonZero)
      {
        sum += 1.0 / d;
        weights.push_back(1.0 / d);
      }
      for (size_t i = 0; i < weights.size(); ++i)
      {
        weights[i] = weights[i] / sum;
      }
      newCurv = std::inner_product(curvsNonZero.begin(), curvsNonZero.end(),
                                   weights.begin(), 0.0);
    }
    else
    {
      // Corner case.
      countInvalid += 1;
      newCurv = 0;
    }
    // Set the new curvature value.
    curvature[pId] = newCurv;
  }
  this->adjustedCurvatures[curvatureName] = curvature;
}

vtkSmartPointer<vtkPolyData>
ComputeCurvatures::ComputeCurvature(std::string const& curvatureName)
{
  auto ct = curvatureName;
  std::transform(ct.begin(), ct.end(), ct.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  vtkNew<vtkCurvatures> curvatureFilter;
  curvatureFilter->SetInputData(this->source);
  std::size_t found = ct.find("gaus");
  if (found != std::string::npos)
  {
    curvatureFilter->SetCurvatureTypeToGaussian();
  }
  else
  {
    curvatureFilter->SetCurvatureTypeToMean();
  }
  curvatureFilter->Update();
  return curvatureFilter->GetOutput();
}

std::vector<double>
ComputeCurvatures::ExtractData(vtkPolyData* curvatureData,
                               std::string const& curvatureName)
{
  auto array =
      curvatureData->GetPointData()->GetAbstractArray(curvatureName.c_str());
  auto n = curvatureData->GetNumberOfPoints();
  std::vector<double> data;
  for (auto i = 0; i < n; ++i)
  {
    data.push_back(array->GetVariantValue(i).ToDouble());
  }
  return data;
}

std::vector<vtkIdType> ComputeCurvatures::ExtractBoundaryIds()
{
  std::string name = "Ids";
  vtkNew<vtkIdFilter> idFilter;
  idFilter->SetInputData(this->source);
  idFilter->SetPointIds(true);
  idFilter->SetCellIds(false);
  idFilter->SetPointIdsArrayName(name.c_str());
  idFilter->SetCellIdsArrayName(name.c_str());
  idFilter->Update();

  vtkNew<vtkFeatureEdges> edges;

  edges->SetInputConnection(idFilter->GetOutputPort());
  edges->BoundaryEdgesOn();
  edges->ManifoldEdgesOff();
  edges->NonManifoldEdgesOff();
  edges->FeatureEdgesOff();
  edges->Update();

  auto array =
      edges->GetOutput()->GetPointData()->GetAbstractArray(name.c_str());
  auto n = edges->GetOutput()->GetNumberOfPoints();
  std::vector<vtkIdType> boundaryIds;
  for (auto i = 0; i < n; ++i)
  {
    boundaryIds.push_back(array->GetVariantValue(i).ToInt());
  }
  return boundaryIds;
}

/**
 * Extract the topological neighbors for point pId. In two steps:
 * 1) self.source.GetPointCells(pId, cellIds)
 * 2) self.source.GetCellPoints(cellId, cellPointIds) for all cellId in cellIds
 */
std::set<vtkIdType> ComputeCurvatures::PointNeighborhood(vtkIdType const& pId)
{
  vtkNew<vtkIdList> cellIds;
  this->source->GetPointCells(pId, cellIds);
  std::set<vtkIdType> neighbors;
  auto n = cellIds->GetNumberOfIds();
  for (auto i = 0; i < n; ++i)
  {
    auto cellId = cellIds->GetId(i);
    vtkNew<vtkIdList> cellPointIds;
    this->source->GetCellPoints(cellId, cellPointIds);
    for (auto j = 0; j < cellPointIds->GetNumberOfIds(); ++j)
    {
      neighbors.insert(cellPointIds->GetId(j));
    }
  }
  return neighbors;
}

double ComputeCurvatures::ComputeDistance(vtkIdType const& ptIdA,
                                          vtkIdType const& ptIdB)
{
  double ptA[3]{0.0, 0.0, 0.0};
  double ptB[3]{0.0, 0.0, 0.0};
  double ptC[3]{0.0, 0.0, 0.0};
  this->source->GetPoint(ptIdA, ptA);
  this->source->GetPoint(ptIdB, ptB);
  vtkMath::Subtract(ptA, ptB, ptC);
  return vtkMath::Norm(ptC);
}

void ComputeCurvatures::UpdateCurvatures(std::string const& curvatureName)
{
  if (static_cast<size_t>(this->source->GetNumberOfPoints()) !=
      this->adjustedCurvatures[curvatureName].size())
  {
    std::string s = curvatureName;
    s += ":\nCannot add the adjusted curvatures to the source.\n";
    s += " The number of points in source does not equal the\n";
    s += " number of point ids in the adjusted curvature array.";
    std::cerr << s << std::endl;
    return;
  }
  vtkNew<vtkDoubleArray> curvatures;
  curvatures->SetName(curvatureName.c_str());
  for (auto curvature : this->adjustedCurvatures[curvatureName])
  {
    curvatures->InsertNextTuple1(curvature);
  }
  this->source->GetPointData()->AddArray(curvatures);
  this->source->GetPointData()->SetActiveScalars(curvatureName.c_str());
}

void ComputeCurvatures::SetGaussCurvatureBounds(double const& lower,
                                                double const& upper)
{
  this->bounds["Gauss_Curvature"][0] = lower;
  this->bounds["Gauss_Curvature"][1] = upper;
}

void ComputeCurvatures::GaussBoundsOn()
{
  boundsState["Gauss_Curvature"] = true;
}

void ComputeCurvatures::GaussBoundsOff()
{
  boundsState["Gauss_Curvature"] = false;
}

void ComputeCurvatures::SetMeanCurvatureBounds(double const& lower,
                                               double const& upper)
{
  this->bounds["Mean_Curvature"][0] = lower;
  this->bounds["Mean_Curvature"][1] = upper;
}

void ComputeCurvatures::MeanBoundsOn()
{
  boundsState["Mean_Curvature"] = true;
}

void ComputeCurvatures::MeanBoundsOff()
{
  boundsState["Mean_Curvature"] = false;
}

void ComputeCurvatures::SetEpsilons(double const& gauss_eps,
                                    double const& mean_eps)
{
  this->epsilons["Gauss_Curvature"] = gauss_eps;
  this->epsilons["Mean_Curvature"] = mean_eps;
}

} // namespace

```

### Usage

``` c++
  cc = ComputeCurvatures(source);

  // Add any refinements that you would like to make here e.g.:
  // cc = ComputeCurvatures(source);
  // cc.set_gauss_curvature_bounds(-0.5, 1.0);
  // cc.gauss_bounds_on();

  // After running update() the source will contain the Gaussian and Mean Curvatures.
  cc.update();

```
