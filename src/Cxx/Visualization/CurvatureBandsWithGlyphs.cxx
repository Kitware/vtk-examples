#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkCamera.h>
#include <vtkCameraOrientationWidget.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkColorSeries.h>
#include <vtkCurvatures.h>
#include <vtkElevationFilter.h>
#include <vtkGlyph3D.h>
#include <vtkImplicitBoolean.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkMaskPoints.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricRandomHills.h>
#include <vtkParametricTorus.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkReverseSense.h>
#include <vtkScalarBarActor.h>
#include <vtkSphereSource.h>
#include <vtkSuperquadricSource.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkVariantArray.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
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

namespace {

//! Divide a range into bands
/*!
@param dR - [min, max] the range that is to be covered by the bands.
@param numberOfBands - the number of bands, a positive integer.
@param nearestInteger - if True then [floor(min), ceil(max)] is used.
@return A vector consisting of [min, midpoint, max] for each band.
*/
std::vector<std::vector<double>> MakeBands(double const dR[2],
                                           int const& numberOfBands,
                                           bool const& nearestInteger);

//! Divide a range into custom bands
/*!
You need to specify each band as an array [r1, r2] where r1 < r2 and
append these to a vector.
The vector should ultimately look
like this: [[r1, r2], [r2, r3], [r3, r4]...]

@param dR - [min, max] the range that is to be covered by the bands.
@param numberOfBands - the number of bands, a positive integer.
@param myBands - the bands.
@return A List consisting of [min, midpoint, max] for each band.
*/
std::vector<std::vector<double>>
MakeCustomBands(double const dR[2], int const& numberOfBands,
                std::vector<std::array<double, 2>> const& myBands);

//! Divide a range into integral bands
/*!
Divide a range into bands
@param dR - [min, max] the range that is to be covered by the bands.
@return A List consisting of [min, midpoint, max] for each band.
*/
std::vector<std::vector<double>> MakeIntegralBands(double const dR[2]);

void PrintBands(std::vector<std::vector<double>> const& bands);

//! Generate elevations over the surface.
/*!
@param src - the vtkPolyData source.
@return elev - the elevations.
*/
vtkSmartPointer<vtkPolyData> MakeElevations(vtkPolyData* src);

//! Make a torus as the source.
/*!
@return - vtkPolyData.
*/
vtkSmartPointer<vtkPolyData> MakeTorus();

//! Make a parametric torus as the source.
/*!
@return - vtkPolyData.
*/
vtkSmartPointer<vtkPolyData> MakeParametricTorus();

//! Make a parametric hills surface as the source.
/*!
@return - vtkPolyData.
*/
vtkSmartPointer<vtkPolyData> MakeParametricHills();

//! Clip a vtkPolyData source.
/*!
A cube is made whose size corresponds the the bounds of the source.
Then each side is shrunk by the appropriate dx, dy or dz. After
this operation the source is clipped by the cube.
@param src - the vtkPolyData source
@param dx - the amount to clip in the x-direction
@param dy - the amount to clip in the y-direction
@param dz - the amount to clip in the z-direction
@return - clipped vtkPolyData.
*/
vtkSmartPointer<vtkPolyData> Clipper(vtkPolyData* src, double const& dx,
                                     double const& dy, double const& dz);

vtkSmartPointer<vtkLookupTable> MakeLUT();

vtkSmartPointer<vtkLookupTable> ReverseLUT(vtkLookupTable* lut);

//! Count the number of scalars in each band.
/*!
@param bands - the bands.
@param src - the vtkPolyData source.
@return The frequencies of the scalars in each band.
*/
std::vector<int> Frequencies(std::vector<std::vector<double>> const& bands,
                             vtkPolyData* src);

//! Print the frequency table.
/*!
@param freq - the frequencies.
*/
void PrintFrequencies(std::vector<int> const& freq);

void PrintBandsFrequencies(std::vector<std::vector<double>> const& bands,
                           std::vector<int>& freq);

//!  Glyph the normals on the surface.
/*!
@param src - the vtkPolyData source.
@param reverseNormals - if True the normals on the surface are reversed.
@return The glyphs.
*/
vtkNew<vtkGlyph3D> MakeGlyphs(vtkPolyData* src, bool const& reverseNormals);

} // namespace

int main(int, char*[])
{
  // Get the surface
  // std::string desiredSurface = "Torus";
  // std::string desiredSurface = "ParametricTorus";
  std::string desiredSurface = "RandomHills";
  auto lcSurface = desiredSurface;
  std::transform(lcSurface.begin(), lcSurface.end(), lcSurface.begin(),
                 [](char c) { return std::tolower(c); });
  std::map<std::string, int> availableSurfaces = {
      {"torus", 0}, {"parametrictorus", 1}, {"randomhills", 2}};
  vtkSmartPointer<vtkPolyData> src;
  if (availableSurfaces.find(lcSurface) == availableSurfaces.end())
  {
    std::cout << "No surface specified." << std::endl;
    return EXIT_FAILURE;
  }
  switch (availableSurfaces[lcSurface])
  {
  case 0: {
    src = MakeTorus();
    src = MakeElevations(src);
    break;
  }
  case 1: {
    src = MakeParametricTorus();
    break;
  }
  case 2: {
    src = MakeParametricHills();
    break;
  }
  default: {
    std::cout << "No surface specified." << std::endl;
    return EXIT_FAILURE;
  }
  }
  std::cout << desiredSurface << std::endl;
  auto curvatures = ComputeCurvatures(src);
  curvatures.Update();

  src->GetPointData()->SetActiveScalars("Gauss_Curvature");
  auto scalarRangeCurvatures =
      src->GetPointData()->GetScalars("Gauss_Curvature")->GetRange();
  auto scalarRangeElevation =
      src->GetPointData()->GetScalars("Elevation")->GetRange();

  double scalarRange[2];
  scalarRange[0] = scalarRangeCurvatures[0];
  scalarRange[1] = scalarRangeCurvatures[1];

  auto lut = MakeLUT();
  vtkIdType numberOfBands = lut->GetNumberOfTableValues();
  std::vector<std::vector<double>> bands;
  if (desiredSurface == "RandomHills")
  {
    // These are my custom bands.
    // Generated by first running:
    // bands = MakeBands(scalarRange, numberOfBands, false);
    // then:
    //  std::vector<int> freq = Frequencies(bands, src);
    //  PrintBandsFrequencies(bands, freq);
    // Finally using the output to create this table:
    std::vector<std::array<double, 2>> myBands = {
        {-0.630, -0.190},  {-0.190, -0.043}, {-0.043, -0.0136},
        {-0.0136, 0.0158}, {0.0158, 0.0452}, {0.0452, 0.0746},
        {0.0746, 0.104},   {0.104, 0.251},   {0.251, 1.131}};
    // Comment this out if you want to see how allocating
    // equally spaced bands works.
    bands = MakeCustomBands(scalarRange, numberOfBands, myBands);
    // bands = MakeBands(scalarRange, numberOfBands, false);
    // Adjust the number of table values
    numberOfBands = static_cast<vtkIdType>(bands.size());
    lut->SetNumberOfTableValues(numberOfBands);
  }
  else
  {
    bands = MakeBands(scalarRange, numberOfBands, false);
  }
  lut->SetTableRange(scalarRange);

  // PrintBands(bands);

  // Let's do a frequency table.
  // The number of scalars in each band.
  std::vector<int> freq = Frequencies(bands, src);
  // PrintFrequencies(freq);
  PrintBandsFrequencies(bands, freq);

  // We will use the midpoint of the band as the label.
  std::vector<std::string> labels;
  for (std::vector<std::vector<double>>::const_iterator p = bands.begin();
       p != bands.end(); ++p)
  {
    std::ostringstream os;
    os << std::fixed << std::setw(6) << std::setprecision(2) << (*p)[1];
    labels.push_back(os.str());
  }

  // Annotate
  vtkNew<vtkVariantArray> values;
  for (size_t i = 0; i < labels.size(); ++i)
  {
    values->InsertNextValue(vtkVariant(labels[i]));
  }
  for (vtkIdType i = 0; i < values->GetNumberOfTuples(); ++i)
  {
    lut->SetAnnotation(i, values->GetValue(i).ToString());
  }

  // Create a lookup table with the colors reversed.
  auto lutr = ReverseLUT(lut);

  // Create the contour bands.
  vtkNew<vtkBandedPolyDataContourFilter> bcf;
  bcf->SetInputData(src);
  // Use either the minimum or maximum value for each band.
  int i = 0;
  for (std::vector<std::vector<double>>::const_iterator p = bands.begin();
       p != bands.end(); ++p)
  {
    bcf->SetValue(i, (*p)[2]);
    ++i;
  }
  // We will use an indexed lookup table.
  bcf->SetScalarModeToIndex();
  bcf->GenerateContourEdgesOn();

  // Generate the glyphs on the original surface.

  auto glyph = MakeGlyphs(src, false);

  // ------------------------------------------------------------
  // Create the mappers and actors
  // ------------------------------------------------------------
  vtkNew<vtkNamedColors> colors;

  // Set the background color.
  std::array<unsigned char, 4> bkg{{179, 204, 255, 255}};
  colors->SetColor("BkgColor", bkg.data());

  vtkNew<vtkPolyDataMapper> srcMapper;
  srcMapper->SetInputConnection(bcf->GetOutputPort());
  srcMapper->SetScalarRange(scalarRange);
  srcMapper->SetLookupTable(lut);
  srcMapper->SetScalarModeToUseCellData();

  vtkNew<vtkActor> srcActor;
  srcActor->SetMapper(srcMapper);

  // Create contour edges
  vtkNew<vtkPolyDataMapper> edgeMapper;
  edgeMapper->SetInputData(bcf->GetContourEdgesOutput());
  edgeMapper->SetResolveCoincidentTopologyToPolygonOffset();

  vtkNew<vtkActor> edgeActor;
  edgeActor->SetMapper(edgeMapper);
  edgeActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  vtkNew<vtkPolyDataMapper> glyphMapper;
  glyphMapper->SetInputConnection(glyph->GetOutputPort());
  glyphMapper->SetScalarModeToUsePointFieldData();
  glyphMapper->SetColorModeToMapScalars();
  glyphMapper->ScalarVisibilityOn();
  glyphMapper->SelectColorArray("Elevation");
  // Colour by scalars.
  // The default lookup table is used but you can
  // use whatever lookup table you like.
  glyphMapper->SetScalarRange(scalarRangeElevation);

  vtkNew<vtkActor> glyphActor;
  glyphActor->SetMapper(glyphMapper);

  auto windowWidth = 800;
  auto windowHeight = 800;
  // Add a scalar bar->
  vtkNew<vtkScalarBarActor> scalarBar;
  // This LUT puts the lowest value at the top of the scalar bar.
  // scalarBar->SetLookupTable(lut);
  // Use this LUT if you want the highest value at the top.
  scalarBar->SetLookupTable(lutr);
  scalarBar->SetTitle("Gaussian\nCurvature");
  scalarBar->GetTitleTextProperty()->SetColor(
      colors->GetColor3d("Black").GetData());
  scalarBar->GetLabelTextProperty()->SetColor(
      colors->GetColor3d("Black").GetData());
  scalarBar->GetAnnotationTextProperty()->SetColor(
      colors->GetColor3d("Black").GetData());
  scalarBar->SetMaximumWidthInPixels(windowWidth / 8);
  scalarBar->SetMaximumHeightInPixels(windowHeight / 3);
  scalarBar->UnconstrainedFontSizeOn();

  // ------------------------------------------------------------
  // Create the RenderWindow, Renderer and Interactor
  // ------------------------------------------------------------
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderWindowInteractor> iren;
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  iren->SetInteractorStyle(style);

  renWin->AddRenderer(ren);
  // Important: The interactor must be set prior to enabling the widget.
  iren->SetRenderWindow(renWin);
  vtkNew<vtkCameraOrientationWidget> camOrientManipulator;
  camOrientManipulator->SetParentRenderer(ren);
  // Enable the widget.
  camOrientManipulator->On();

  // add actors
  ren->AddViewProp(srcActor);
  ren->AddViewProp(edgeActor);
  ren->AddViewProp(glyphActor);
  ren->AddActor2D(scalarBar);

  ren->SetBackground(colors->GetColor3d("BkgColor").GetData());
  renWin->SetSize(windowWidth, windowHeight);
  renWin->SetWindowName("CurvatureBandsWithGlyphs");

  if (desiredSurface == "RandomHills")
  {
    auto camera = ren->GetActiveCamera();
    camera->SetPosition(10.9299, 59.1505, 24.9823);
    camera->SetFocalPoint(2.21692, 7.97545, 7.75135);
    camera->SetViewUp(-0.230136, 0.345504, -0.909761);
    camera->SetDistance(54.6966);
    camera->SetClippingRange(36.3006, 77.9852);
    renWin->Render();
  }

  iren->Start();

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

namespace {

std::vector<std::vector<double>> MakeBands(double const dR[2],
                                           int const& numberOfBands,
                                           bool const& nearestInteger)
{
  std::vector<std::vector<double>> bands;
  if ((dR[1] < dR[0]) || (numberOfBands <= 0))
  {
    return bands;
  }
  double x[2];
  for (int i = 0; i < 2; ++i)
  {
    x[i] = dR[i];
  }
  if (nearestInteger)
  {
    x[0] = std::floor(x[0]);
    x[1] = std::ceil(x[1]);
  }
  double dx = (x[1] - x[0]) / static_cast<double>(numberOfBands);
  std::vector<double> b;
  b.push_back(x[0]);
  b.push_back(x[0] + dx / 2.0);
  b.push_back(x[0] + dx);
  for (int i = 0; i < numberOfBands; ++i)
  {
    bands.push_back(b);
    for (std::vector<double>::iterator p = b.begin(); p != b.end(); ++p)
    {
      *p = *p + dx;
    }
  }
  return bands;
}

std::vector<std::vector<double>>
MakeCustomBands(double const dR[2], int const& numberOfBands,
                std::vector<std::array<double, 2>> const& myBands)
{
  std::vector<std::vector<double>> bands;
  if ((dR[1] < dR[0]) || (numberOfBands <= 0))
  {
    return bands;
  }
  // std::vector<std::array<double, 2>> myBands{{-0.7, -0.05}, {-0.05, 0},
  //                                           {0, 0.13},     {0.13, 1.07},
  //                                           {1.07, 35.4},  {35.4, 37.1}};
  std::vector<std::array<double, 2>> x;
  for (auto b : myBands)
  {
    x.push_back(b);
  }
  // Determine the index of the range minimum and range maximum.
  size_t idxMin = 0;
  for (auto idx = 0; idx < myBands.size(); ++idx)
  {
    if (dR[0] < myBands[idx][1] && dR[0] >= myBands[idx][0])
    {
      idxMin = idx;
      break;
    }
  }
  size_t idxMax = myBands.size() - 1;
  for (int idx = static_cast<int>(myBands.size()) - 1; idx >= 0; --idx)
  {
    if (dR[1] < myBands[idx][1] && dR[1] >= myBands[idx][0])
    {
      idxMax = idx;
      break;
    }
  }

  // Set the minimum to match the range minimum.
  x[idxMin][0] = dR[0];
  x[idxMax][1] = dR[1];
  for (size_t i = idxMin; i < idxMax + 1; ++i)
  {
    std::vector<double> b(3);
    b[0] = x[i][0];
    b[1] = x[i][0] + (x[i][1] - x[i][0]) / 2.0;
    b[2] = x[i][1];
    bands.push_back(b);
  }
  return bands;
}

std::vector<std::vector<double>> MakeIntegralBands(double const dR[2])
{
  std::vector<std::vector<double>> bands;
  if (dR[1] < dR[0])
  {
    return bands;
  }
  double x[2];
  for (int i = 0; i < 2; ++i)
  {
    x[i] = dR[i];
  }
  x[0] = std::floor(x[0]);
  x[1] = std::ceil(x[1]);
  int numberOfBands = static_cast<int>(std::abs(x[1]) + std::abs(x[0]));
  return MakeBands(x, numberOfBands, false);
}

void PrintBands(std::vector<std::vector<double>> const& bands)
{
  std::ostringstream os;
  size_t idx = 0;
  for (std::vector<std::vector<double>>::const_iterator p = bands.begin();
       p != bands.end(); ++p)
  {
    if (p == bands.begin())
    {
      os << "Bands:\n";
    }
    for (std::vector<double>::const_iterator q = p->begin(); q != p->end(); ++q)
    {
      if (q == p->begin())
      {
        os << std::setw(4) << idx << " [";
      }
      if (q == std::prev(p->end()))
      {
        os << std::fixed << std::setw(6) << std::setprecision(3) << *q << "]\n";
      }
      else
      {
        os << std::fixed << std::setw(6) << std::setprecision(3) << *q << ", ";
      }
    }
    ++idx;
  }
  std::cout << os.str() << std::endl;
}

vtkSmartPointer<vtkPolyData> MakeElevations(vtkPolyData* src)
{
  double bounds[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  src->GetBounds(bounds);
  vtkNew<vtkElevationFilter> elevFilter;
  elevFilter->SetInputData(src);
  elevFilter->SetLowPoint(0, bounds[2], 0);
  elevFilter->SetHighPoint(0, bounds[3], 0);
  elevFilter->SetScalarRange(bounds[2], bounds[3]);
  elevFilter->Update();

  return elevFilter->GetPolyDataOutput();
}

vtkSmartPointer<vtkPolyData> MakeTorus()
{
  vtkNew<vtkSuperquadricSource> source;
  source->SetCenter(0.0, 0.0, 0.0);
  source->SetCenter(1.0, 1.0, 1.0);
  source->SetPhiResolution(64);
  source->SetThetaResolution(64);
  source->SetThetaRoundness(1);
  source->SetThickness(0.5);
  source->SetSize(10);
  source->SetToroidal(1);
  source->Update();

  // The quadric is made of strips, so pass it through a triangle filter as
  // the curvature filter only operates on polys
  vtkNew<vtkTriangleFilter> tri;
  tri->SetInputConnection(source->GetOutputPort());

  // The quadric has nasty discontinuities from the way the edges are generated
  // so let's pass it though a CleanPolyDataFilter and merge any points which
  // are coincident, or very close
  vtkNew<vtkCleanPolyData> cleaner;
  cleaner->SetInputConnection(tri->GetOutputPort());
  cleaner->SetTolerance(0.005);
  cleaner->Update();

  return cleaner->GetOutput();
}

vtkSmartPointer<vtkPolyData> MakeParametricTorus()
{
  vtkNew<vtkParametricTorus> fn;
  fn->SetRingRadius(5);
  fn->SetCrossSectionRadius(2);

  vtkNew<vtkParametricFunctionSource> source;
  source->SetParametricFunction(fn);
  source->SetUResolution(50);
  source->SetVResolution(50);
  source->SetScalarModeToZ();
  source->Update();

  // Name the arrays (not needed in VTK 6.2+ for vtkParametricFunctionSource)
  source->GetOutput()->GetPointData()->GetNormals()->SetName("Normals");
  // We have calculated the elevation, just rename the scalars.
  source->GetOutput()->GetPointData()->GetScalars()->SetName("Elevation");

  vtkNew<vtkTransform> transform;
  transform->RotateX(-90.0);
  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetInputConnection(source->GetOutputPort());
  transformFilter->SetTransform(transform);
  transformFilter->Update();

  return transformFilter->GetOutput();
}

vtkSmartPointer<vtkPolyData> MakeParametricHills()
{
  vtkNew<vtkParametricRandomHills> fn;
  fn->AllowRandomGenerationOn();
  fn->SetRandomSeed(1);
  fn->SetNumberOfHills(30);

  vtkNew<vtkParametricFunctionSource> source;
  source->SetParametricFunction(fn);
  source->SetUResolution(50);
  source->SetVResolution(50);
  source->SetScalarModeToZ();
  source->Update();

  // Name the arrays (not needed in VTK 6.2+ for vtkParametricFunctionSource)
  source->GetOutput()->GetPointData()->GetNormals()->SetName("Normals");
  // We have calculated the elevation, just rename the scalars.
  source->GetOutput()->GetPointData()->GetScalars()->SetName("Elevation");

  vtkNew<vtkTransform> transform;
  transform->Translate(0.0, 5.0, 15.0);
  transform->RotateX(-90.0);
  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetInputConnection(source->GetOutputPort());
  transformFilter->SetTransform(transform);
  transformFilter->Update();

  return transformFilter->GetOutput();
}

vtkSmartPointer<vtkPolyData> Clipper(vtkPolyData* src, double const& dx,
                                     double const& dy, double const& dz)
{
  double bounds[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  src->GetBounds(bounds);

  vtkNew<vtkPlane> plane1;
  plane1->SetOrigin(bounds[0] + dx, 0, 0);
  plane1->SetNormal(1, 0, 0);

  vtkNew<vtkPlane> plane2;
  plane2->SetOrigin(bounds[1] - dx, 0, 0);
  plane2->SetNormal(-1, 0, 0);

  vtkNew<vtkPlane> plane3;
  plane3->SetOrigin(0, bounds[2] + dy, 0);
  plane3->SetNormal(0, 1, 0);

  vtkNew<vtkPlane> plane4;
  plane4->SetOrigin(0, bounds[3] - dy, 0);
  plane4->SetNormal(0, -1, 0);

  vtkNew<vtkPlane> plane5;
  plane5->SetOrigin(0, 0, bounds[4] + dz);
  plane5->SetNormal(0, 0, 1);

  vtkNew<vtkPlane> plane6;
  plane6->SetOrigin(0, 0, bounds[5] - dz);
  plane6->SetNormal(0, 0, -1);

  vtkNew<vtkImplicitBoolean> clipFunction;
  clipFunction->SetOperationTypeToUnion();
  clipFunction->AddFunction(plane1);
  clipFunction->AddFunction(plane2);
  clipFunction->AddFunction(plane3);
  clipFunction->AddFunction(plane4);
  clipFunction->AddFunction(plane5);
  clipFunction->AddFunction(plane6);

  // Create clipper for the random hills
  vtkNew<vtkClipPolyData> clipper;
  clipper->SetClipFunction(clipFunction);
  clipper->SetInputData(src);
  clipper->GenerateClipScalarsOff();
  clipper->GenerateClippedOutputOff();
  // clipper->GenerateClippedOutputOn();
  clipper->Update();

  return clipper->GetOutput();
}

vtkSmartPointer<vtkLookupTable> MakeLUT()
{
  // Make the lookup table.
  vtkNew<vtkColorSeries> colorSeries;
  // Select a color scheme.
  int colorSeriesEnum;
  // colorSeriesEnum = colorSeries->BREWER_DIVERGING_BROWN_BLUE_GREEN_9;
  // colorSeriesEnum = colorSeries->BREWER_DIVERGING_SPECTRAL_10;
  // colorSeriesEnum = colorSeries->BREWER_DIVERGING_SPECTRAL_3;
  // colorSeriesEnum = colorSeries->BREWER_DIVERGING_PURPLE_ORANGE_9;
  // colorSeriesEnum = colorSeries->BREWER_SEQUENTIAL_BLUE_PURPLE_9;
  // colorSeriesEnum = colorSeries->BREWER_SEQUENTIAL_BLUE_GREEN_9;
  colorSeriesEnum = colorSeries->BREWER_QUALITATIVE_SET3;
  // colorSeriesEnum = colorSeries->CITRUS;
  colorSeries->SetColorScheme(colorSeriesEnum);
  vtkNew<vtkLookupTable> lut;
  colorSeries->BuildLookupTable(lut);
  lut->SetNanColor(0, 0, 0, 1);

  return lut;
}

vtkSmartPointer<vtkLookupTable> ReverseLUT(vtkLookupTable* lut)
{
  // First do a deep copy just to get the whole structure
  // and then reverse the colors and annotations.
  vtkNew<vtkLookupTable> lutr;
  lutr->DeepCopy(lut);
  vtkIdType t = lut->GetNumberOfTableValues() - 1;
  for (vtkIdType i = t; i >= 0; --i)
  {
    std::array<double, 3> rgb{0.0, 0.0, 0.0};
    std::array<double, 4> rgba{0.0, 0.0, 0.0, 1.0};
    lut->GetColor(i, rgb.data());
    std::copy(std::begin(rgb), std::end(rgb), std::begin(rgba));
    rgba[3] = lut->GetOpacity(i);
    lutr->SetTableValue(t - i, rgba.data());
  }
  t = lut->GetNumberOfAnnotatedValues() - 1;
  for (vtkIdType i = t; i >= 0; --i)
  {
    lutr->SetAnnotation(t - i, lut->GetAnnotation(i));
  }

  return lutr;
}

std::vector<int> Frequencies(std::vector<std::vector<double>> const& bands,
                             vtkPolyData* src)
{
  std::vector<int> freq(bands.size(), 0);
  vtkIdType tuples = src->GetPointData()->GetScalars()->GetNumberOfTuples();
  for (int i = 0; i < tuples; ++i)
  {
    double* x = src->GetPointData()->GetScalars()->GetTuple(i);
    for (size_t j = 0; j < bands.size(); ++j)
    {
      if (*x <= bands[j][2])
      {
        freq[j] = freq[j] + 1;
        break;
      }
    }
  }
  return freq;
}

void PrintFrequencies(std::vector<int> const& freq)
{
  std::ostringstream os;
  int i = 0;
  for (std::vector<int>::const_iterator p = freq.begin(); p != freq.end(); ++p)
  {
    if (p == freq.begin())
    {
      os << "Frequencies: [";
    }
    if (p == std::prev(freq.end()))
    {
      os << i << ": " << *p << "]\n";
    }
    else
    {
      os << i << ": " << *p << ", ";
    }
    ++i;
  }
  std::cout << os.str() << endl;
}

void PrintBandsFrequencies(std::vector<std::vector<double>> const& bands,
                           std::vector<int>& freq)
{
  if (bands.size() != freq.size())
  {
    std::cout << "Bands and frequencies must be the same size." << std::endl;
    return;
  }
  std::ostringstream os;
  size_t idx = 0;
  for (std::vector<std::vector<double>>::const_iterator p = bands.begin();
       p != bands.end(); ++p)
  {
    if (p == bands.begin())
    {
      os << "Bands & frequencies:\n";
    }
    for (std::vector<double>::const_iterator q = p->begin(); q != p->end(); ++q)
    {
      if (q == p->begin())
      {
        os << std::setw(4) << idx << " [";
      }
      if (q == std::prev(p->end()))
      {
        os << std::fixed << std::setw(6) << std::setprecision(3) << *q
           << "]: " << std::setw(6) << freq[idx] << "\n";
      }
      else
      {
        os << std::fixed << std::setw(6) << std::setprecision(3) << *q << ", ";
      }
    }
    ++idx;
  }
  std::cout << os.str() << endl;
}

vtkNew<vtkGlyph3D> MakeGlyphs(vtkPolyData* src, bool const& reverseNormals)
{
  // Sometimes the contouring algorithm can create a volume whose gradient
  // vector and ordering of polygon(using the right hand rule) are
  // inconsistent. vtkReverseSense cures this problem.
  vtkNew<vtkReverseSense> reverse;
  vtkNew<vtkMaskPoints> maskPts;
  maskPts->SetOnRatio(5);
  maskPts->RandomModeOn();
  if (reverseNormals)
  {
    reverse->SetInputData(src);
    reverse->ReverseCellsOn();
    reverse->ReverseNormalsOn();
    maskPts->SetInputConnection(reverse->GetOutputPort());
  }
  else
  {
    maskPts->SetInputData(src);
  }

  // Source for the glyph filter
  vtkNew<vtkArrowSource> arrow;
  arrow->SetTipResolution(16);
  arrow->SetTipLength(0.3);
  arrow->SetTipRadius(0.1);

  vtkNew<vtkGlyph3D> glyph;
  glyph->SetSourceConnection(arrow->GetOutputPort());
  glyph->SetInputConnection(maskPts->GetOutputPort());
  glyph->SetVectorModeToUseNormal();
  glyph->SetScaleFactor(1.0);
  glyph->SetColorModeToColorByVector();
  glyph->SetScaleModeToScaleByVector();
  glyph->OrientOn();
  glyph->Update();

  return glyph;
}

} // namespace
