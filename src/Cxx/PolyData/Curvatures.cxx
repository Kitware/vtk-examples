#include <vtkActor.h>
#include <vtkCameraOrientationWidget.h>
#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>
#include <vtkCurvatures.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkTextProperty.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

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

  ComputeCurvatures(vtkPolyData* source)
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

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Parse command line arguments
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0]
              << " Filename(.vtp) e.g. cowHead.vtp -20 20" << std::endl;
    return EXIT_FAILURE;
  }

  // Create a polydata
  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  ComputeCurvatures cc(reader->GetOutput());
  cc.SetGaussCurvatureBounds(-20.0, 20.0);
  cc.GaussBoundsOn();
  cc.SetMeanCurvatureBounds(-20.0, 20.0);
  cc.MeanBoundsOn();

  // After running update() the source will contain the Gaussian and Mean
  // Curvatures.
  cc.Update();

  // Choose a curvature
  //std::string curvature = "Gauss_Curvature";
  std::string curvature = "Mean_Curvature";
  auto curvatureTitle = curvature;
  std::replace(curvatureTitle.begin(), curvatureTitle.end(), '_', '\n');

  // Uncomment the following lines if you want to write out the polydata.
  // vtkNew<vtkXMLPolyDataWriter> writer;
  // writer->SetFileName("Source.vtp");
  // writer->SetInputData(cc.source);
  // writer->SetDataModeToAscii();
  // writer->Write();

  // Get scalar range from command line if present, otherwise use
  // range of computed curvature
  double scalarRange[2];
  if (argc >= 4)
  {
    scalarRange[0] = atof(argv[2]);
    scalarRange[1] = atof(argv[3]);
  }
  else
  {
    cc.source->GetPointData()
        ->GetScalars(curvature.c_str())
        ->GetRange(scalarRange);
  }

  int scheme = 16;
  if (argc >= 5)
  {
    scheme = atoi(argv[4]);
  }

  // Build a lookup table
  vtkNew<vtkColorSeries> colorSeries;
  colorSeries->SetColorScheme(scheme);
  std::cout << "Using color scheme #: " << colorSeries->GetColorScheme()
            << " is " << colorSeries->GetColorSchemeName() << std::endl;

  vtkNew<vtkColorTransferFunction> lut;
  lut->SetColorSpaceToHSV();

  // Use a color series to create a transfer function
  auto numColors = colorSeries->GetNumberOfColors();
  for (int i = 0; i < numColors; i++)
  {
    vtkColor3ub color = colorSeries->GetColor(i);
    double dColor[3];
    dColor[0] = static_cast<double>(color[0]) / 255.0;
    dColor[1] = static_cast<double>(color[1]) / 255.0;
    dColor[2] = static_cast<double>(color[2]) / 255.0;
    double t = scalarRange[0] +
        (scalarRange[1] - scalarRange[0]) /
            (static_cast<double>(numColors) - 1) * i;
    lut->AddRGBPoint(t, dColor[0], dColor[1], dColor[2]);
  }

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(cc.source);
  mapper->SetScalarModeToUsePointFieldData();
  mapper->SelectColorArray(curvature.c_str());
  mapper->SetScalarRange(scalarRange);
  mapper->SetLookupTable(lut);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  auto windowWidth = 800;
  auto windowHeight = 800;

  // Create a scalar bar
  vtkNew<vtkScalarBarActor> scalarBar;
  scalarBar->SetLookupTable(mapper->GetLookupTable());
  scalarBar->SetTitle(curvatureTitle.c_str());
  scalarBar->UnconstrainedFontSizeOn();
  scalarBar->SetNumberOfLabels(5);
  scalarBar->SetMaximumWidthInPixels(windowWidth / 8);
  scalarBar->SetMaximumHeightInPixels(windowHeight / 3);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  renWin->SetSize(windowWidth, windowHeight);
  renWin->SetWindowName("Curvatures");

  vtkNew<vtkRenderWindowInteractor> iRen;
  iRen->SetRenderWindow(renWin);
  // Important: The interactor must be set prior to enabling the widget.
  iRen->SetRenderWindow(renWin);

  vtkNew<vtkCameraOrientationWidget> camOrientManipulator;
  camOrientManipulator->SetParentRenderer(renderer);
  // Enable the widget.
  camOrientManipulator->On();

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->AddActor2D(scalarBar);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  // Render and interact
  renWin->Render();
  iRen->Start();

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
