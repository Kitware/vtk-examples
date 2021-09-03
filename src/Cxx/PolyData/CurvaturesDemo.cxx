/*
The purpose of this is to demonstrate how to get the Gaussian and Mean
curvatures of a surface.

Two different surfaces are used in this demonstration with each
surface coloured according to its Gaussian and Mean curvatures.

The first surface is a superquadric surface, this demonstrates the use
of extra filters that are needed to get a nice smooth surface.

The second surface is a parametric surface, in this case the surface
has already been triangulated so no extra processing is necessary.

In order to get a nice coloured image, a vtkColorTransferFunction has
been used to generate a set of colours for the vtkLookUp tables. We
have used a diverging colour space.  Because of the symmetry of the
ranges selected for the lookup tables, the white colouration
represents a midpoint value whilst the blue represents values less
than the midopoint value and orange represents colours greater than the
midpoint value.

In the case of the Random Hills Gaussian Curvature surface, this
colouration shows the nature of the surface quite nicely. The blue
areas are saddle points (negative Gaussian curvature) and the orange
areas have a positive Gaussian curvature.
In the case of the mean curvature the blue colouration is representing
negative curvature perpendicular to one of the principal axes.

This example also demonstrates the use of std::vector and the linking
of the elements of the vector together to form a pipeline.
*/

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCleanPolyData.h>
#include <vtkColorTransferFunction.h>
#include <vtkCurvatures.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricRandomHills.h>
#include <vtkPointData.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkSuperquadricSource.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkTriangleFilter.h>

#include <algorithm>
#include <array>
#include <iterator>
#include <map>
#include <vector>

// Here is the ComputeCurvatures definition.
//
// Includes needed by the ComputeCurvatures definition.
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

  std::string GetCurvatureType();

  void SetCurvatureTypeToGaussian();

  void SetGaussEpsilon(double const& gauss_eps = 1.0e-8);

  void SetGaussCurvatureBounds(double const& lower = 0.0,
                               double const& upper = 0.0);
  void GaussBoundsOn();

  void GaussBoundsOff();

  void SetCurvatureTypeToMean();

  void SetMeanEpsilon(double const& gauss_eps = 1.0e-8);

  void SetMeanCurvatureBounds(double const& lower = 0.0,
                              double const& upper = 0.0);
  void MeanBoundsOn();

  void MeanBoundsOff();

private:
  void ComputeCurvatureAndFixUpBoundary();
  vtkSmartPointer<vtkPolyData> ComputeCurvature();
  std::vector<double> ExtractData(vtkPolyData* curvatureData);
  std::vector<vtkIdType> ExtractBoundaryIds();
  double ComputeDistance(vtkIdType const& ptIdA, vtkIdType const& ptIdB);
  std::set<vtkIdType> PointNeighborhood(vtkIdType const& pId);
  void UpdateCurvature();

public:
  vtkSmartPointer<vtkPolyData> source;

private:
  std::string curvatureType{"Gauss_Curvature"};
  std::map<std::string, std::vector<double>> adjustedCurvatures;
  std::map<std::string, std::vector<double>> bounds{
      {"Gauss_Curvature", {0.0, 0.0}}, {"Mean_Curvature", {0.0, 0.0}}};
  std::map<std::string, bool> boundsState{{"Gauss_Curvature", false},
                                          {"Mean_Curvature", false}};
  std::map<std::string, double> epsilons{{"Gauss_Curvature", 1.0e-8},
                                         {"Mean_Curvature", 1.0e-8}};
};

} // namespace

namespace {
vtkSmartPointer<vtkLookupTable> GetDivergingLut();
vtkSmartPointer<vtkLookupTable> GetDivergingLut1();
} // namespace

int main(int, char* argv[])
{
  vtkNew<vtkNamedColors> colors;
  // We are going to handle two different sources.
  // The first source is a superquadric source.
  vtkNew<vtkSuperquadricSource> torus;
  torus->SetCenter(0.0, 0.0, 0.0);
  torus->SetScale(1.0, 1.0, 1.0);
  torus->SetPhiResolution(64);
  torus->SetThetaResolution(64);
  torus->SetThetaRoundness(1);
  torus->SetThickness(0.5);
  torus->SetSize(0.5);
  torus->SetToroidal(1);

  // Rotate the torus towards the observer (around the x-axis).
  vtkNew<vtkTransform> torusT;
  torusT->RotateX(55);

  vtkNew<vtkTransformFilter> torusTF;
  torusTF->SetInputConnection(torus->GetOutputPort());
  torusTF->SetTransform(torusT);

  // The quadric is made of strips, so pass it through a triangle filter as
  // the curvature filter only operates on polys.
  vtkNew<vtkTriangleFilter> tri;
  tri->SetInputConnection(torusTF->GetOutputPort());

  // The quadric has nasty discontinuities from the way the edges are generated
  // so let's pass it though a CleanPolyDataFilter and merge any points which
  // are coincident, or very close.
  vtkNew<vtkCleanPolyData> cleaner;
  cleaner->SetInputConnection(tri->GetOutputPort());
  cleaner->SetTolerance(0.005);
  cleaner->Update();

  auto toroidPoly = ComputeCurvatures(cleaner->GetOutput());

  // The next source will be a parametric function.
  vtkNew<vtkParametricRandomHills> rh;
  vtkNew<vtkParametricFunctionSource> rhFnSrc;
  rhFnSrc->SetParametricFunction(rh);
  rhFnSrc->Update();

  auto randomHillsPoly = ComputeCurvatures(rhFnSrc->GetOutput());

  // Now we have the sources, lets put them into a vector.
  // Remember to Update before use.
  std::vector<ComputeCurvatures> sources;
  sources.push_back(toroidPoly);
  sources.push_back(toroidPoly);
  sources.push_back(randomHillsPoly);
  sources.push_back(randomHillsPoly);

  auto lut = GetDivergingLut1();

  std::vector<vtkSmartPointer<vtkRenderer>> renderers;
  std::vector<vtkSmartPointer<vtkActor>> actors;
  std::vector<vtkSmartPointer<vtkPolyDataMapper>> mappers;
  std::vector<vtkSmartPointer<vtkTextMapper>> textmappers;
  std::vector<vtkSmartPointer<vtkActor2D>> textactors;
  std::vector<vtkSmartPointer<vtkScalarBarActor>> scalarBars;
  for (auto idx = 0; idx < sources.size(); ++idx)
  {
    mappers.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
    actors.push_back(vtkSmartPointer<vtkActor>::New());
    textmappers.push_back(vtkSmartPointer<vtkTextMapper>::New());
    textactors.push_back(vtkSmartPointer<vtkActor2D>::New());
    scalarBars.push_back(vtkSmartPointer<vtkScalarBarActor>::New());
    renderers.push_back(vtkSmartPointer<vtkRenderer>::New());
  }

  std::map<int, std::string> curvatures{
      {0, {"Gauss_Curvature"}},
      {1, {"Mean_Curvature"}},
      {2, {"Gauss_Curvature"}},
      {3, {"Mean_Curvature"}},
  };

  // Create a common text property.
  vtkNew<vtkTextProperty> textProperty;
  textProperty->SetFontSize(24);
  textProperty->SetJustificationToCentered();

  // RenderWindow Dimensions
  //
  auto rendererSize = 512;
  auto gridDimensions = 2;
  auto windowWidth = rendererSize * gridDimensions;
  auto windowHeight = rendererSize * gridDimensions;

  for (auto idx = 0; idx < sources.size(); ++idx)
  {
    if ("Gauss_Curvature" == curvatures[idx])
    {
      sources[idx].SetCurvatureTypeToGaussian();
    }
    if ("Mean_Curvature" == curvatures[idx])
    {
      sources[idx].SetCurvatureTypeToMean();
    }
    sources[idx].Update();

    auto curvatureType = sources[idx].GetCurvatureType();
    auto curvatureName = sources[idx].GetCurvatureType();
    std::replace(curvatureName.begin(), curvatureName.end(), '_', '\n');

    sources[idx].source->GetPointData()->SetActiveScalars(
        curvatureType.c_str());
    auto scalarRange = sources[idx]
                           .source->GetPointData()
                           ->GetScalars(curvatureType.c_str())
                           ->GetRange();

    mappers[idx]->SetInputData(sources[idx].source);
    mappers[idx]->SetScalarModeToUsePointFieldData();
    mappers[idx]->SelectColorArray(curvatureType.c_str());
    mappers[idx]->SetScalarRange(scalarRange);
    mappers[idx]->SetLookupTable(lut);

    actors[idx]->SetMapper(mappers[idx]);

    textmappers[idx]->SetInput(curvatureName.c_str());
    textmappers[idx]->SetTextProperty(textProperty);

    textactors[idx]->SetMapper(textmappers[idx]);
    textactors[idx]->SetPosition(250, 16);

    // Create a scalar bar
    scalarBars[idx]->SetLookupTable(mappers[idx]->GetLookupTable());
    scalarBars[idx]->SetTitle(curvatureName.c_str());
    scalarBars[idx]->UnconstrainedFontSizeOn();
    scalarBars[idx]->SetNumberOfLabels(5);
    scalarBars[idx]->SetMaximumWidthInPixels(windowWidth / 8);
    scalarBars[idx]->SetMaximumHeightInPixels(windowHeight / 3);
    scalarBars[idx]->SetBarRatio(scalarBars[idx]->GetBarRatio() * 0.5);
    scalarBars[idx]->SetPosition(0.85, 0.1);
  }

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(rendererSize * gridDimensions,
                        rendererSize * gridDimensions);
  renderWindow->SetWindowName("CurvaturesDemo");

  // Add and position the renders to the render window.
  for (auto row = 0; row < gridDimensions; ++row)
  {
    for (auto col = 0; col < gridDimensions; ++col)
    {
      auto idx = row * gridDimensions + col;
      renderers[idx]->SetViewport(
          static_cast<double>(col) / gridDimensions,
          (static_cast<double>(gridDimensions) - (row + 1.0)) / gridDimensions,
          (static_cast<double>(col) + 1) / gridDimensions,
          (static_cast<double>(gridDimensions) - row) / gridDimensions);
      renderWindow->AddRenderer(renderers[idx]);

      renderers[idx]->AddActor(actors[idx]);
      renderers[idx]->AddActor(textactors[idx]);
      renderers[idx]->AddActor(scalarBars[idx]);
      renderers[idx]->SetBackground(colors->GetColor3d("SlateGray").GetData());
    }
  }

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderWindow->Render();

  interactor->Start();

  return EXIT_SUCCESS;
}

// Here is the implementation for ComputeCurvatures
//
// Includes needed by the ComputeCurvatures implementation.

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

std::string ComputeCurvatures::GetCurvatureType()
{
  return this->curvatureType;
}

void ComputeCurvatures::SetCurvatureTypeToGaussian()
{
  this->curvatureType = "Gauss_Curvature";
}

void ComputeCurvatures::SetGaussEpsilon(double const& gauss_eps)
{
  this->epsilons["Gauss_Curvature"] = std::abs(gauss_eps);
}

void ComputeCurvatures::SetGaussCurvatureBounds(double const& lower,
                                                double const& upper)
{
  if (lower <= upper)
  {
    this->bounds["Gauss_Curvature"][0] = lower;
    this->bounds["Gauss_Curvature"][1] = upper;
  }
  else
  {
    this->bounds["Gauss_Curvature"][0] = upper;
    this->bounds["Gauss_Curvature"][1] = lower;
    std::cout << "SetGaussCurvatureBounds: bounds swapped since lower > upper"
              << std::endl;
  }
}

void ComputeCurvatures::GaussBoundsOn()
{
  boundsState["Gauss_Curvature"] = true;
}

void ComputeCurvatures::GaussBoundsOff()
{
  boundsState["Gauss_Curvature"] = false;
}

void ComputeCurvatures::SetCurvatureTypeToMean()
{
  this->curvatureType = "Mean_Curvature";
}

void ComputeCurvatures::SetMeanEpsilon(double const& gauss_eps)
{
  this->epsilons["Mean_Curvature"] = std::abs(gauss_eps);
}

void ComputeCurvatures::SetMeanCurvatureBounds(double const& lower,
                                               double const& upper)
{
  if (lower <= upper)
  {
    this->bounds["Mean_Curvature"][0] = lower;
    this->bounds["Mean_Curvature"][1] = upper;
  }
  else
  {
    this->bounds["Mean_Curvature"][0] = upper;
    this->bounds["Mean_Curvature"][1] = lower;
    std::cout << "SetMeanCurvatureBounds: bounds swapped since lower > upper"
              << std::endl;
  }
}

void ComputeCurvatures::MeanBoundsOn()
{
  boundsState["Mean_Curvature"] = true;
}

void ComputeCurvatures::MeanBoundsOff()
{
  boundsState["Mean_Curvature"] = false;
}

void ComputeCurvatures::Update()
{
  this->ComputeCurvatureAndFixUpBoundary();
  // Set small values to zero.
  if (this->epsilons[this->curvatureType] != 0.0)
  {
    auto eps = std::abs(this->epsilons[this->curvatureType]);
    for (size_t i = 0; i < this->adjustedCurvatures[this->curvatureType].size();
         ++i)
    {
      if (std::abs(this->adjustedCurvatures[this->curvatureType][i]) < eps)
      {
        this->adjustedCurvatures[this->curvatureType][i] = 0.0;
      }
    }
  }
  //  Set upper and lower bounds.
  if (this->boundsState[this->curvatureType])
  {
    auto lowerBound = this->bounds[this->curvatureType][0];
    for (size_t i = 0; i < this->adjustedCurvatures[this->curvatureType].size();
         ++i)
    {
      if (this->adjustedCurvatures[this->curvatureType][i] < lowerBound)
      {
        this->adjustedCurvatures[this->curvatureType][i] = lowerBound;
      }
    }
    auto upperBound = this->bounds[this->curvatureType][1];
    for (size_t i = 0; i < this->adjustedCurvatures[this->curvatureType].size();
         ++i)
    {
      if (this->adjustedCurvatures[this->curvatureType][i] > upperBound)
      {
        this->adjustedCurvatures[this->curvatureType][i] = upperBound;
      }
    }
  }
  this->UpdateCurvature();
}

void ComputeCurvatures::ComputeCurvatureAndFixUpBoundary()
{
  // Curvature as vtkPolyData.
  auto curvatureData = this->ComputeCurvature();
  // Curvature as a vector.
  auto curvature = this->ExtractData(curvatureData);
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
  this->adjustedCurvatures[this->curvatureType] = curvature;
}

vtkSmartPointer<vtkPolyData> ComputeCurvatures::ComputeCurvature()
{
  vtkNew<vtkCurvatures> curvatureFilter;
  curvatureFilter->SetInputData(this->source);
  if ("Gauss_Curvature" == this->curvatureType)
  {
    curvatureFilter->SetCurvatureTypeToGaussian();
  }
  else if ("Mean_Curvature" == this->curvatureType)
  {
    curvatureFilter->SetCurvatureTypeToMean();
  }
  else
  {
    std::cerr << "Curvature type must be either Gaussian or Mean." << std::endl;
    vtkSmartPointer<vtkPolyData> ret;
    return ret;
  }
  curvatureFilter->Update();
  return curvatureFilter->GetOutput();
}

std::vector<double> ComputeCurvatures::ExtractData(vtkPolyData* curvatureData)
{
  auto array = curvatureData->GetPointData()->GetAbstractArray(
      this->curvatureType.c_str());
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

void ComputeCurvatures::UpdateCurvature()
{
  if (static_cast<size_t>(this->source->GetNumberOfPoints()) !=
      this->adjustedCurvatures[this->curvatureType].size())
  {
    std::string s = this->curvatureType;
    s += ":\nCannot add the adjusted curvatures to the source.\n";
    s += " The number of points in source does not equal the\n";
    s += " number of point ids in the adjusted curvature array.";
    std::cerr << s << std::endl;
    return;
  }
  vtkNew<vtkDoubleArray> curvatures;
  curvatures->SetName(this->curvatureType.c_str());
  for (auto curvature : this->adjustedCurvatures[this->curvatureType])
  {
    curvatures->InsertNextTuple1(curvature);
  }
  this->source->GetPointData()->AddArray(curvatures);
  this->source->GetPointData()->SetActiveScalars(this->curvatureType.c_str());
}

} // namespace

namespace {
// clang-format off
/**
 * See: [Diverging Color Maps for Scientific Visualization](https://www.kennethmoreland.com/color-maps/)
 *
 *                   start point         midPoint            end point
 * cool to warm:     0.230, 0.299, 0.754 0.865, 0.865, 0.865 0.706, 0.016, 0.150
 * purple to orange: 0.436, 0.308, 0.631 0.865, 0.865, 0.865 0.759, 0.334, 0.046
 * green to purple:  0.085, 0.532, 0.201 0.865, 0.865, 0.865 0.436, 0.308, 0.631
 * blue to brown:    0.217, 0.525, 0.910 0.865, 0.865, 0.865 0.677, 0.492, 0.093
 * green to red:     0.085, 0.532, 0.201 0.865, 0.865, 0.865 0.758, 0.214, 0.233
 *
 */
// clang-format on
vtkSmartPointer<vtkLookupTable> GetDivergingLut()
{

  vtkNew<vtkColorTransferFunction> ctf;
  ctf->SetColorSpaceToDiverging();
  ctf->AddRGBPoint(0.0, 0.230, 0.299, 0.754);
  ctf->AddRGBPoint(0.5, 0.865, 0.865, 0.865);
  ctf->AddRGBPoint(1.0, 0.706, 0.016, 0.150);

  auto tableSize = 256;
  vtkNew<vtkLookupTable> lut;
  lut->SetNumberOfTableValues(tableSize);
  lut->Build();

  for (auto i = 0; i < lut->GetNumberOfColors(); ++i)
  {
    std::array<double, 3> rgb;
    ctf->GetColor(static_cast<double>(i) / lut->GetNumberOfColors(),
                  rgb.data());
    std::array<double, 4> rgba{0.0, 0.0, 0.0, 1.0};
    std::copy(std::begin(rgb), std::end(rgb), std::begin(rgba));
    lut->SetTableValue(static_cast<vtkIdType>(i), rgba.data());
  }

  return lut;
}

vtkSmartPointer<vtkLookupTable> GetDivergingLut1()
{
  vtkNew<vtkNamedColors> colors;
  // Colour transfer function.
  vtkNew<vtkColorTransferFunction> ctf;
  ctf->SetColorSpaceToDiverging();
  ctf->AddRGBPoint(0.0, colors->GetColor3d("MidnightBlue").GetRed(),
                   colors->GetColor3d("MidnightBlue").GetGreen(),
                   colors->GetColor3d("MidnightBlue").GetBlue());
  ctf->AddRGBPoint(0.5, colors->GetColor3d("Gainsboro").GetRed(),
                   colors->GetColor3d("Gainsboro").GetGreen(),
                   colors->GetColor3d("Gainsboro").GetBlue());
  ctf->AddRGBPoint(1.0, colors->GetColor3d("DarkOrange").GetRed(),
                   colors->GetColor3d("DarkOrange").GetGreen(),
                   colors->GetColor3d("DarkOrange").GetBlue());

  // Lookup table.
  vtkNew<vtkLookupTable> lut;
  lut->SetNumberOfColors(256);
  for (auto i = 0; i < lut->GetNumberOfColors(); ++i)
  {
    std::array<double, 3> rgb;
    ctf->GetColor(double(i) / lut->GetNumberOfColors(), rgb.data());
    std::array<double, 4> rgba{0.0, 0.0, 0.0, 1.0};
    std::copy(std::begin(rgb), std::end(rgb), std::begin(rgba));
    lut->SetTableValue(i, rgba.data());
  }

  return lut;
}

} // namespace
