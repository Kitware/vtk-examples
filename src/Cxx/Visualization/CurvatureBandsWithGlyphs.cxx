#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkCamera.h>
#include <vtkCleanPolyData.h>
#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>
#include <vtkCurvatures.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkElevationFilter.h>
#include <vtkFeatureEdges.h>
#include <vtkFloatArray.h>
#include <vtkGlyph3D.h>
#include <vtkIdFilter.h>
#include <vtkIdList.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkMaskPoints.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricRandomHills.h>
#include <vtkParametricTorus.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkReverseSense.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkSuperquadricSource.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkVariant.h>
#include <vtkVariantArray.h>
#include <vtkVersion.h>

#if VTK_VERSION_NUMBER >= 90020210809ULL
#define HAS_COW
#include <vtkCameraOrientationWidget.h>
#endif

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
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace {

//! Adjust curvatures along the edges of the surface.
/*!
 * This function adjusts curvatures along the edges of the surface by replacing
 *  the value with the average value of the curvatures of points in the
 *  neighborhood.
 *
 * Remember to update the vtkCurvatures object before calling this.
 *
 * @param source - A vtkPolyData object corresponding to the vtkCurvatures
 * object.
 * @param curvatureName: The name of the curvature, "Gauss_Curvature" or
 * "Mean_Curvature".
 * @param epsilon: Curvature values less than this will be set to zero.
 * @return
 */
void AdjustEdgeCurvatures(vtkPolyData* source, std::string const& curvatureName,
                          double const& epsilon = 1.0e-08);

//! Constrain curvatures to the range [lower_bound ... upper_bound].
/*!
 * Remember to update the vtkCurvatures object before calling this.
 *
 * @param source - A vtkPolyData object corresponding to the vtkCurvatures
 * object.
 * @param curvatureName: The name of the curvature, "Gauss_Curvature" or
 * "Mean_Curvature".
 * @param lowerBound: The lower bound.
 * @param upperBound: The upper bound.
 * @return
 */
void ConstrainCurvatures(vtkPolyData* source, std::string const& curvatureName,
                         double const& lowerBound = 0.0,
                         double const& upperBound = 0.0);

//! Generate elevations over the surface.
/*!
@param src - the vtkPolyData source.
@return elev - the elevations.
*/
vtkSmartPointer<vtkPolyData> GetElevations(vtkPolyData* src);

vtkSmartPointer<vtkPolyData> GetHills();
vtkSmartPointer<vtkPolyData> GetParametricHills();
vtkSmartPointer<vtkPolyData> GetParametricTorus();
vtkSmartPointer<vtkPolyData> GetPlane();
vtkSmartPointer<vtkPolyData> GetSphere();
vtkSmartPointer<vtkPolyData> GetTorus();
vtkSmartPointer<vtkPolyData> GetSource(std::string const& source);

vtkSmartPointer<vtkColorSeries> GetColorSeries();

vtkSmartPointer<vtkLookupTable> GetCategoricalLUT();

vtkSmartPointer<vtkLookupTable> GetOrdinaLUT();

vtkSmartPointer<vtkLookupTable> GetDivergingLUT();

vtkSmartPointer<vtkLookupTable> ReverseLUT(vtkLookupTable* lut);

//!  Glyph the normals on the surface.
/*!
@param src - the vtkPolyData source.
#param scaleFactor - the scale factor for the glyphs.
@param reverseNormals - if True the normals on the surface are reversed.
@return The glyphs.
*/
vtkNew<vtkGlyph3D> GetGlyphs(vtkPolyData* src, double const& scaleFactor = 1.0,
                             bool const& reverseNormals = false);

std::map<int, std::vector<double>> GetBands(double const dR[2],
                                            int const& numberOfBands,
                                            int const& precision = 2,
                                            bool const& nearestInteger = false);

//! Divide a range into custom bands
/*!
You need to specify each band as an array [r1, r2] where r1 < r2 and
append these to a vector.
The vector should ultimately look
like this: [[r1, r2], [r2, r3], [r3, r4]...]

@param dR - [min, max] the range that is to be covered by the bands.
@param numberOfBands - the number of bands, a positive integer.
@param myBands - the bands.
@return  A map consisting of the band inxex and [min, midpoint, max] for each
band.
*/
std::map<int, std::vector<double>>
GetCustomBands(double const dR[2], int const& numberOfBands,
               std::vector<std::array<double, 2>> const& myBands);

//! Divide a range into integral bands
/*!
Divide a range into bands
@param dR - [min, max] the range that is to be covered by the bands.
@returnA map consisting of the band inxex and [min, midpoint, max] for each
band.
*/
std::map<int, std::vector<double>> GetIntegralBands(double const dR[2]);

//! Count the number of scalars in each band.
/*
 * The scalars used are the active scalars in the polydata.
 *
 * @param bands - the bands.
 * @param src - the vtkPolyData source.
 * @return The frequencies of the scalars in each band.
 */
std::map<int, int> GetFrequencies(std::map<int, std::vector<double>>& bands,
                                  vtkPolyData* src);
//!
/*
 * The bands and frequencies are adjusted so that the first and last
 *  frequencies in the range are non-zero.
 * @param bands: The bands.
 * @param freq: The frequencies.
 */
void AdjustFrequencyRanges(std::map<int, std::vector<double>>& bands,
                           std::map<int, int>& freq);

void PrintBandsFrequencies(std::map<int, std::vector<double>> const& bands,
                           std::map<int, int>& freq, int const& precision = 2);

} // namespace

int main(int, char*[])
{
  // Get the surface
  std::string desiredSurface{"RandomHills"};
  // desiredSurface = "Hills";
  // desiredSurface = "ParametricTorus";
  // desiredSurface = "Plane";
  // desiredSurface = "RandomHills";
  // desiredSurface = "Sphere";
  // desiredSurface = "Torus";
  auto source = GetSource(desiredSurface);

  // The length of the normal arrow glyphs.
  auto scaleFactor = 1.0;
  if (desiredSurface == "Hills")
  {
     scaleFactor = 0.5;
 }
  if (desiredSurface == "Sphere")
 {
    scaleFactor = 2.0;
 }
  std::cout << desiredSurface << std::endl;

  auto gaussianCurvature = true;
  std::string curvature =
      (gaussianCurvature) ? "Gauss_Curvature" : "Mean_Curvature";

  vtkNew<vtkCurvatures> cc;
  cc->SetInputData(source);
  std::vector<std::string> needsAdjusting{"Hills", "ParametricTorus", "Plane",
                                          "RandomHills", "Torus"};
  if (gaussianCurvature)
  {
    cc->SetCurvatureTypeToGaussian();
    cc->Update();
    if (std::find(needsAdjusting.begin(), needsAdjusting.end(),
                  desiredSurface) != needsAdjusting.end())
    {
      AdjustEdgeCurvatures(cc->GetOutput(), curvature);
    }
    if (desiredSurface == "Plane")
    {
      ConstrainCurvatures(cc->GetOutput(), curvature, 0.0, 0.0);
    }
    if (desiredSurface == "Sphere")
    {

      // Gaussian curvature is 1/r^2
      ConstrainCurvatures(cc->GetOutput(), curvature, 4.0, 4.0);
    }
  }
  else
  {

    cc->SetCurvatureTypeToMean();
    cc->Update();
    if (std::find(needsAdjusting.begin(), needsAdjusting.end(),
                  desiredSurface) != needsAdjusting.end())
    {
      AdjustEdgeCurvatures(cc->GetOutput(), curvature);
    }
    if (desiredSurface == "Plane")
    {
      ConstrainCurvatures(cc->GetOutput(), curvature, 0.0, 0.0);
    }
    if (desiredSurface == "Sphere")
    {

      // Mean curvature is 1/r
      ConstrainCurvatures(cc->GetOutput(), curvature, 2.0, 2.0);
    }
  }

  cc->GetOutput()->GetPointData()->SetActiveScalars(curvature.c_str());
  auto scalarRangeCurvatures = cc->GetOutput()
                                   ->GetPointData()
                                   ->GetScalars(curvature.c_str())
                                   ->GetRange();
  auto scalarRangeElevation =
      cc->GetOutput()->GetPointData()->GetScalars("Elevation")->GetRange();

  auto lut = GetCategoricalLUT();
  auto lut1 = GetDivergingLUT();
  lut->SetTableRange(scalarRangeCurvatures);
  lut1->SetTableRange(scalarRangeElevation);
  auto numberOfBands = lut->GetNumberOfTableValues();
  auto precision = 10;
  auto bands = GetBands(scalarRangeCurvatures, numberOfBands, precision, false);

  if (desiredSurface == "RandomHills")
  {
    // These are my custom bands.
    // Generated by first running:
    // bands = GetBands(scalarRangeCurvatures, numberOfBands, precision, false);
    // then:
    //  std::vector<int> freq = Frequencies(bands, src);
    //  PrintBandsFrequencies(bands, freq);
    // Finally using the output to create this table:
    // std::vector<std::array<double, 2>> myBands = {
    //    {-0.630, -0.190},  {-0.190, -0.043}, {-0.043, -0.0136},
    //    {-0.0136, 0.0158}, {0.0158, 0.0452}, {0.0452, 0.0746},12
    //    {0.0746, 0.104},   {0.104, 0.251},   {0.251, 1.131}};
    //  This demonstrates that the gaussian curvature of the surface
    //   is mostly planar with some hyperbolic regions (saddle points)
    //   and some spherical regions.
    std::vector<std::array<double, 2>> myBands = {
        {-0.630, -0.190}, {-0.190, -0.043}, {-0.043, 0.0452}, {0.0452, 0.0746},
        {0.0746, 0.104},  {0.104, 0.251},   {0.251, 1.131}};

    // Comment this out if you want to see how allocating
    // equally spaced bands works.
    bands = GetCustomBands(scalarRangeCurvatures, numberOfBands, myBands);
    // Adjust the number of table values
    lut->SetNumberOfTableValues(static_cast<vtkIdType>(bands.size()));
  }
  else if (desiredSurface == "Hills")
  {
    std::vector<std::array<double, 2>> myBands = {
        {-2.104, -0.15},   {-0.15, -0.1},   {-0.1, -0.05},
        {-0.05, -0.02},    {-0.02, -0.005}, {-0.005, -0.0005},
        {-0.0005, 0.0005}, {0.0005, 0.09},  {0.09, 4.972},
    };

    // Comment this out if you want to see how allocating
    // equally spaced bands works.
    bands = GetCustomBands(scalarRangeCurvatures, numberOfBands, myBands);
    // Adjust the number of table values
    lut->SetNumberOfTableValues(static_cast<vtkIdType>(bands.size()));
  }

  // Let's do a frequency table.
  auto freq = GetFrequencies(bands, cc->GetOutput());
  AdjustFrequencyRanges(bands, freq);
  PrintBandsFrequencies(bands, freq);

  scalarRangeCurvatures[0] = bands.begin()->second[0];
  scalarRangeCurvatures[1] = std::prev(bands.end())->second[2];
  lut->SetTableRange(scalarRangeCurvatures);
  lut->SetNumberOfTableValues(bands.size());

  // We will use the midpoint of the band as the label.
  std::vector<std::string> labels;
  for (std::map<int, std::vector<double>>::const_iterator p = bands.begin();
       p != bands.end(); ++p)
  {
    std::ostringstream os;
    os << std::fixed << std::setw(6) << std::setprecision(2) << p->second[1];
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
  bcf->SetInputData(cc->GetOutput());
  // Use either the minimum or maximum value for each band.
  int i = 0;
  for (std::map<int, std::vector<double>>::const_iterator p = bands.begin();
       p != bands.end(); ++p)
  {
    bcf->SetValue(i, p->second[2]);
    ++i;
  }
  // We will use an indexed lookup table.
  bcf->SetScalarModeToIndex();
  bcf->GenerateContourEdgesOn();

  // Generate the glyphs on the original surface.

  auto glyph = GetGlyphs(cc->GetOutput(), scaleFactor, false);

  // ------------------------------------------------------------
  // Create the mappers and actors
  // ------------------------------------------------------------
  vtkNew<vtkNamedColors> colors;

  // Set the background color.
  colors->SetColor("BkgColor",
                   std::array<unsigned char, 4>{179, 204, 255, 255}.data());
  colors->SetColor("ParaViewBkg",
                   std::array<unsigned char, 4>{82, 87, 110, 255}.data());

  vtkNew<vtkPolyDataMapper> srcMapper;
  srcMapper->SetInputConnection(bcf->GetOutputPort());
  srcMapper->SetScalarRange(scalarRangeCurvatures);
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
  glyphMapper->SetLookupTable(lut1);
  glyphMapper->SetScalarRange(scalarRangeElevation);

  vtkNew<vtkActor> glyphActor;
  glyphActor->SetMapper(glyphMapper);

  auto windowWidth = 800;
  auto windowHeight = 800;

  // Add scalar bars.
  auto curvatureType = curvature;
  std::replace(curvatureType.begin(), curvatureType.end(), '_', '\n');

  vtkNew<vtkScalarBarActor> scalarBar;
  // This LUT puts the lowest value at the top of the scalar bar.
  // scalarBar->SetLookupTable(lut);
  // Use this LUT if you want the highest value at the top.
  scalarBar->SetLookupTable(lutr);
  scalarBar->SetTitle(curvatureType.c_str());
  scalarBar->GetTitleTextProperty()->SetColor(
      colors->GetColor3d("AliceBlue").GetData());
  scalarBar->GetLabelTextProperty()->SetColor(
      colors->GetColor3d("AliceBlue").GetData());
  scalarBar->GetAnnotationTextProperty()->SetColor(
      colors->GetColor3d("AliceBlue").GetData());
  scalarBar->UnconstrainedFontSizeOn();
  scalarBar->SetMaximumWidthInPixels(windowWidth / 8);
  scalarBar->SetMaximumHeightInPixels(windowHeight / 3);
  scalarBar->SetPosition(0.85, 0.05);

  vtkNew<vtkScalarBarActor> scalarBarElev;
  scalarBarElev->SetLookupTable(lut1);
  scalarBarElev->SetTitle("Elevation");
  scalarBarElev->GetTitleTextProperty()->SetColor(
      colors->GetColor3d("AliceBlue").GetData());
  scalarBarElev->GetLabelTextProperty()->SetColor(
      colors->GetColor3d("AliceBlue").GetData());
  scalarBarElev->GetAnnotationTextProperty()->SetColor(
      colors->GetColor3d("AliceBlue").GetData());
  scalarBarElev->UnconstrainedFontSizeOn();
  if (desiredSurface == "Plane")
  {
    scalarBarElev->SetNumberOfLabels(1);
  }
  else
  {
    scalarBarElev->SetNumberOfLabels(5);
  }
  scalarBarElev->SetMaximumWidthInPixels(windowWidth / 8);
  scalarBarElev->SetMaximumHeightInPixels(windowHeight / 3);
  // scalarBarElev->SetBarRatio(scalarBarElev->GetBarRatio() * 0.5);
  scalarBarElev->SetPosition(0.85, 0.4);

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
#ifdef HAS_COW
  vtkNew<vtkCameraOrientationWidget> camOrientManipulator;
  camOrientManipulator->SetParentRenderer(ren);
  // Enable the widget.
  camOrientManipulator->On();
#endif

  // add actors
  ren->AddViewProp(srcActor);
  ren->AddViewProp(edgeActor);
  ren->AddViewProp(glyphActor);
  ren->AddActor2D(scalarBar);
  ren->AddActor2D(scalarBarElev);

  ren->SetBackground(colors->GetColor3d("ParaViewBkg").GetData());
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

namespace {

void AdjustEdgeCurvatures(vtkPolyData* source, std::string const& curvatureName,
                          double const& epsilon)
{
  auto PointNeighbourhood =
      [&source](vtkIdType const& pId) -> std::set<vtkIdType> {
    // Extract the topological neighbors for point pId. In two steps:
    //  1) source->GetPointCells(pId, cellIds)
    //  2) source->GetCellPoints(cellId, cellPointIds) for all cellId in cellIds
    vtkNew<vtkIdList> cellIds;
    source->GetPointCells(pId, cellIds);
    std::set<vtkIdType> neighbours;
    for (vtkIdType i = 0; i < cellIds->GetNumberOfIds(); ++i)
    {
      auto cellId = cellIds->GetId(i);
      vtkNew<vtkIdList> cellPointIds;
      source->GetCellPoints(cellId, cellPointIds);
      for (vtkIdType j = 0; j < cellPointIds->GetNumberOfIds(); ++j)
      {
        neighbours.insert(cellPointIds->GetId(j));
      }
    }
    return neighbours;
  };

  auto ComputeDistance = [&source](vtkIdType const& ptIdA,
                                   vtkIdType const& ptIdB) {
    std::array<double, 3> ptA{0.0, 0.0, 0.0};
    std::array<double, 3> ptB{0.0, 0.0, 0.0};
    std::array<double, 3> ptC{0.0, 0.0, 0.0};
    source->GetPoint(ptIdA, ptA.data());
    source->GetPoint(ptIdB, ptB.data());
    std::transform(std::begin(ptA), std::end(ptA), std::begin(ptB),
                   std::begin(ptC), std::minus<double>());
    // Calculate the norm.
    auto result = std::sqrt(std::inner_product(std::begin(ptC), std::end(ptC),
                                               std::begin(ptC), 0.0));
    return result;
  };

  source->GetPointData()->SetActiveScalars(curvatureName.c_str());
  // Curvature as a vector.
  auto array = source->GetPointData()->GetAbstractArray(curvatureName.c_str());
  std::vector<double> curvatures;
  for (vtkIdType i = 0; i < source->GetNumberOfPoints(); ++i)
  {
    curvatures.push_back(array->GetVariantValue(i).ToDouble());
  }

  // Get the boundary point IDs.
  std::string name = "Ids";
  vtkNew<vtkIdFilter> idFilter;
  idFilter->SetInputData(source);
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

  auto edgeAarray =
      edges->GetOutput()->GetPointData()->GetAbstractArray(name.c_str());
  std::vector<vtkIdType> boundaryIds;
  for (vtkIdType i = 0; i < edges->GetOutput()->GetNumberOfPoints(); ++i)
  {
    boundaryIds.push_back(edgeAarray->GetVariantValue(i).ToInt());
  }
  // Remove duplicate Ids.
  std::set<vtkIdType> pIdsSet(boundaryIds.begin(), boundaryIds.end());
  for (auto const pId : boundaryIds)
  {
    auto pIdsNeighbors = PointNeighbourhood(pId);
    std::set<vtkIdType> pIdsNeighborsInterior;
    std::set_difference(
        pIdsNeighbors.begin(), pIdsNeighbors.end(), pIdsSet.begin(),
        pIdsSet.end(),
        std::inserter(pIdsNeighborsInterior, pIdsNeighborsInterior.begin()));
    // Compute distances and extract curvature values.
    std::vector<double> curvs;
    std::vector<double> dists;
    for (auto const pIdN : pIdsNeighborsInterior)
    {
      curvs.push_back(curvatures[pIdN]);
      dists.push_back(ComputeDistance(pIdN, pId));
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
    for (auto const i : nonZeroDistIds)
    {
      curvsNonZero.push_back(curvs[i]);
      distsNonZero.push_back(dists[i]);
    }
    // Iterate over the edge points and compute the curvature as the weighted
    // average of the neighbours.
    auto countInvalid = 0;
    auto newCurv = 0.0;
    if (curvsNonZero.size() > 0)
    {
      std::vector<double> weights;
      double sum = 0.0;
      for (auto const d : distsNonZero)
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
      // Assuming the curvature of the point is planar.
      newCurv = 0.0;
    }
    // Set the new curvature value.
    curvatures[pId] = newCurv;
  }

  // Set small values to zero.
  if (epsilon != 0.0)
  {
    auto eps = std::abs(epsilon);
    for (size_t i = 0; i < curvatures.size(); ++i)
    {
      if (std::abs(curvatures[i]) < eps)
      {
        curvatures[i] = 0.0;
      }
    }
  }

  if (static_cast<size_t>(source->GetNumberOfPoints()) != curvatures.size())
  {
    std::string s = curvatureName;
    s += ":\nCannot add the adjusted curvatures to the source.\n";
    s += " The number of points in source does not equal the\n";
    s += " number of point ids in the adjusted curvature array.";
    std::cerr << s << std::endl;
    return;
  }
  vtkNew<vtkDoubleArray> adjustedCurvatures;
  adjustedCurvatures->SetName(curvatureName.c_str());
  for (auto curvature : curvatures)
  {
    adjustedCurvatures->InsertNextTuple1(curvature);
  }
  source->GetPointData()->AddArray(adjustedCurvatures);
  source->GetPointData()->SetActiveScalars(curvatureName.c_str());
}

void ConstrainCurvatures(vtkPolyData* source, std::string const& curvatureName,
                         double const& lowerBound, double const& upperBound)
{
  std::array<double, 2> bounds{0.0, 0.0};
  if (lowerBound < upperBound)
  {
    bounds[0] = lowerBound;
    bounds[1] = upperBound;
  }
  else
  {
    bounds[0] = upperBound;
    bounds[1] = lowerBound;
  }

  source->GetPointData()->SetActiveScalars(curvatureName.c_str());
  // Curvature as a vector.
  auto array = source->GetPointData()->GetAbstractArray(curvatureName.c_str());
  std::vector<double> curvatures;
  for (vtkIdType i = 0; i < source->GetNumberOfPoints(); ++i)
  {
    curvatures.push_back(array->GetVariantValue(i).ToDouble());
  }
  //  Set upper and lower bounds.
  for (size_t i = 0; i < curvatures.size(); ++i)
  {
    if (curvatures[i] < bounds[0])
    {
      curvatures[i] = bounds[0];
    }
    else
    {
      if (curvatures[i] > bounds[1])
      {
        curvatures[i] = bounds[1];
      }
    }
  }
  vtkNew<vtkDoubleArray> adjustedCurvatures;
  for (auto curvature : curvatures)
  {
    adjustedCurvatures->InsertNextTuple1(curvature);
  }
  adjustedCurvatures->SetName(curvatureName.c_str());
  source->GetPointData()->RemoveArray(curvatureName.c_str());
  source->GetPointData()->AddArray(adjustedCurvatures);
  source->GetPointData()->SetActiveScalars(curvatureName.c_str());
}

vtkSmartPointer<vtkPolyData> GetElevations(vtkPolyData* src)
{
  double bounds[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  src->GetBounds(bounds);
  if (std::abs(bounds[2]) < 1.0e-8 && std::abs(bounds[3]) < 1.0e-8)
  {
    bounds[3] = bounds[2] + 1;
  }
  vtkNew<vtkElevationFilter> elevFilter;
  elevFilter->SetInputData(src);
  elevFilter->SetLowPoint(0, bounds[2], 0);
  elevFilter->SetHighPoint(0, bounds[3], 0);
  elevFilter->SetScalarRange(bounds[2], bounds[3]);
  elevFilter->Update();

  return elevFilter->GetPolyDataOutput();
}

vtkSmartPointer<vtkPolyData> GetHills()
{
  // Create four hills on a plane.
  // This will have regions of negative, zero and positive Gsaussian curvatures.

  auto xRes = 50;
  auto yRes = 50;
  auto xMin = -5.0;
  auto xMax = 5.0;
  auto dx = (xMax - xMin) / (xRes - 1.0);
  auto yMin = -5.0;
  auto yMax = 5.0;
  auto dy = (yMax - yMin) / (xRes - 1.0);

  // Make a grid.
  vtkNew<vtkPoints> points;
  for (auto i = 0; i < xRes; ++i)
  {
    auto x = xMin + i * dx;
    for (auto j = 0; j < yRes; ++j)
    {
      auto y = yMin + j * dy;
      points->InsertNextPoint(x, y, 0);
    }
  }

  // Add the grid points to a polydata object.
  vtkNew<vtkPolyData> plane;
  plane->SetPoints(points);

  // Triangulate the grid.
  vtkNew<vtkDelaunay2D> delaunay;
  delaunay->SetInputData(plane);
  delaunay->Update();

  auto polydata = delaunay->GetOutput();

  vtkNew<vtkDoubleArray> elevation;
  elevation->SetNumberOfTuples(points->GetNumberOfPoints());

  //  We define the parameters for the hills here.
  // [[0: x0, 1: y0, 2: x variance, 3: y variance, 4: amplitude]...]
  std::vector<std::array<double, 5>> hd{{-2.5, -2.5, 2.5, 6.5, 3.5},
                                        {2.5, 2.5, 2.5, 2.5, 2},
                                        {5.0, -2.5, 1.5, 1.5, 2.5},
                                        {-5.0, 5, 2.5, 3.0, 3}};
  std::array<double, 2> xx{0.0, 0.0};
  for (auto i = 0; i < points->GetNumberOfPoints(); ++i)
  {
    auto x = polydata->GetPoint(i);
    for (size_t j = 0; j < hd.size(); ++j)
    {
      xx[0] = std::pow(x[0] - hd[j][0] / hd[j][2], 2.0);
      xx[1] = std::pow(x[1] - hd[j][1] / hd[j][3], 2.0);
      x[2] += hd[j][4] * std::exp(-(xx[0] + xx[1]) / 2.0);
    }
    polydata->GetPoints()->SetPoint(i, x);
    elevation->SetValue(i, x[2]);
  }

  vtkNew<vtkFloatArray> textures;
  textures->SetNumberOfComponents(2);
  textures->SetNumberOfTuples(2 * polydata->GetNumberOfPoints());
  textures->SetName("Textures");

  for (auto i = 0; i < xRes; ++i)
  {
    float tc[2];
    tc[0] = i / (xRes - 1.0);
    for (auto j = 0; j < yRes; ++j)
    {
      // tc[1] = 1.0 - j / (yRes - 1.0);
      tc[1] = j / (yRes - 1.0);
      textures->SetTuple(static_cast<vtkIdType>(i) * yRes + j, tc);
    }
  }

  polydata->GetPointData()->SetScalars(elevation);
  polydata->GetPointData()->GetScalars()->SetName("Elevation");
  polydata->GetPointData()->SetTCoords(textures);

  vtkNew<vtkPolyDataNormals> normals;
  normals->SetInputData(polydata);
  normals->SetInputData(polydata);
  normals->SetFeatureAngle(30);
  normals->SplittingOff();

  vtkNew<vtkTransform> tr1;
  tr1->RotateX(-90);

  vtkNew<vtkTransformPolyDataFilter> tf1;
  tf1->SetInputConnection(normals->GetOutputPort());
  tf1->SetTransform(tr1);
  tf1->Update();

  return tf1->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetParametricHills()
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

  // Name the arrays (not needed in VTK 6.2+ for vtkParametricFunctionSource).
  // source->GetOutput()->GetPointData()->GetNormals()->SetName("Normals");
  // source->GetOutput()->GetPointData()->GetScalars()->SetName("Scalars");
  // Rename the scalars to "Elevation" since we are using the Z-scalars as
  // elevations.
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

vtkSmartPointer<vtkPolyData> GetParametricTorus()
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

  // Name the arrays (not needed in VTK 6.2+ for vtkParametricFunctionSource).
  // source->GetOutput()->GetPointData()->GetNormals()->SetName("Normals");
  // source->GetOutput()->GetPointData()->GetScalars()->SetName("Scalars");
  // Rename the scalars to "Elevation" since we are using the Z-scalars as
  // elevations.
  source->GetOutput()->GetPointData()->GetScalars()->SetName("Elevation");

  vtkNew<vtkTransform> transform;
  transform->RotateX(-90.0);
  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetInputConnection(source->GetOutputPort());
  transformFilter->SetTransform(transform);
  transformFilter->Update();

  return transformFilter->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetPlane()
{
  vtkNew<vtkPlaneSource> source;
  source->SetOrigin(-10.0, -10.0, 0.0);
  source->SetPoint2(-10.0, 10.0, 0.0);
  source->SetPoint1(10.0, -10.0, 0.0);
  source->SetXResolution(20);
  source->SetYResolution(20);
  source->Update();

  vtkNew<vtkTransform> transform;
  transform->Translate(0.0, 0.0, 0.0);
  transform->RotateX(-90.0);
  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetInputConnection(source->GetOutputPort());
  transformFilter->SetTransform(transform);
  transformFilter->Update();

  // We have a m x n array of quadrilaterals arranged as a regular tiling in a
  // plane. So pass it through a triangle filter since the curvature filter only
  // operates on polys.
  vtkNew<vtkTriangleFilter> tri;
  tri->SetInputConnection(transformFilter->GetOutputPort());

  // Pass it though a CleanPolyDataFilter and merge any points which
  // are coincident, or very close
  vtkNew<vtkCleanPolyData> cleaner;
  cleaner->SetInputConnection(tri->GetOutputPort());
  cleaner->SetTolerance(0.005);
  cleaner->Update();

  return cleaner->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetSphere()
{
  vtkNew<vtkSphereSource> source;
  source->SetCenter(0.0, 0.0, 0.0);
  source->SetRadius(10.0);
  source->SetThetaResolution(32);
  source->SetPhiResolution(32);
  source->Update();

  return source->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetTorus()
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

vtkSmartPointer<vtkPolyData> GetSource(std::string const& source)
{
  std::string surface = source;
  std::transform(surface.begin(), surface.end(), surface.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  std::map<std::string, int> available_surfaces = {
      {"hills", 0},       {"parametrictorus", 1}, {"plane", 2},
      {"randomhills", 3}, {"sphere", 4},          {"torus", 5}};
  if (available_surfaces.find(surface) == available_surfaces.end())
  {
    std::cout << "The surface is not available." << std::endl;
    std::cout << "Using RandomHills instead." << std::endl;
    surface = "randomhills";
  }
  switch (available_surfaces[surface])
  {
  case 0:
    return GetHills();
    break;
  case 1:
    return GetParametricTorus();
    break;
  case 2:
    return GetElevations(GetPlane());
    break;
  case 3:
    return GetParametricHills();
    break;
  case 4:
    return GetElevations(GetSphere());
    break;
  case 5:
    return GetElevations(GetTorus());
    break;
  }
  return GetParametricHills();
}

vtkSmartPointer<vtkColorSeries> GetColorSeries()
{

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
  return colorSeries;
}

vtkSmartPointer<vtkLookupTable> GetCategoricalLUT()
{
  vtkSmartPointer<vtkColorSeries> colorSeries = GetColorSeries();
  // Make the lookup table.
  vtkNew<vtkLookupTable> lut;
  colorSeries->BuildLookupTable(lut, vtkColorSeries::CATEGORICAL);
  lut->SetNanColor(0, 0, 0, 1);

  return lut;
}

vtkSmartPointer<vtkLookupTable> GetOrdinaLUT()
{
  vtkSmartPointer<vtkColorSeries> colorSeries = GetColorSeries();
  // Make the lookup table.
  vtkNew<vtkLookupTable> lut;
  colorSeries->BuildLookupTable(lut, vtkColorSeries::ORDINAL);
  lut->SetNanColor(0, 0, 0, 1);

  return lut;
}

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
vtkSmartPointer<vtkLookupTable> GetDivergingLUT()
{

  vtkNew<vtkColorTransferFunction> ctf;
  ctf->SetColorSpaceToDiverging();
  ctf->AddRGBPoint(0.0, 0.085, 0.532, 0.201);
  ctf->AddRGBPoint(0.5, 0.865, 0.865, 0.865);
  ctf->AddRGBPoint(1.0, 0.758, 0.214, 0.233);

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

vtkNew<vtkGlyph3D> GetGlyphs(vtkPolyData* src, double const& scaleFactor,
                             bool const& reverseNormals)
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
  glyph->SetScaleFactor(scaleFactor);
  glyph->SetColorModeToColorByVector();
  glyph->SetScaleModeToScaleByVector();
  glyph->OrientOn();
  glyph->Update();

  return glyph;
}

std::map<int, std::vector<double>> GetBands(double const dR[2],
                                            int const& numberOfBands,
                                            int const& precision,
                                            bool const& nearestInteger)
{
  auto prec = abs(precision);
  prec = (prec > 14) ? 14 : prec;

  auto RoundOff = [&prec](const double& x) {
    auto pow_10 = std::pow(10.0, prec);
    return std::round(x * pow_10) / pow_10;
  };

  std::map<int, std::vector<double>> bands;
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
    if (i == 0)
    {
      for (std::vector<double>::iterator p = b.begin(); p != b.end(); ++p)
      {
        *p = RoundOff(*p);
      }
      b[0] = x[0];
    }
    bands[i] = b;
    for (std::vector<double>::iterator p = b.begin(); p != b.end(); ++p)
    {
      *p = RoundOff(*p + dx);
    }
  }
  return bands;
}

std::map<int, std::vector<double>>
GetCustomBands(double const dR[2], int const& numberOfBands,
               std::vector<std::array<double, 2>> const& myBands)
{
  std::map<int, std::vector<double>> bands;
  if ((dR[1] < dR[0]) || (numberOfBands <= 0))
  {
    return bands;
  }

  std::vector<std::array<double, 2>> x;
  std::copy(myBands.begin(), myBands.end(), std::back_inserter(x));

  // Determine the index of the range minimum and range maximum.
  int idxMin = 0;
  for (auto idx = 0; idx < static_cast<int>(myBands.size()); ++idx)
  {
    if (dR[0] < myBands[idx][1] && dR[0] >= myBands[idx][0])
    {
      idxMin = idx;
      break;
    }
  }
  int idxMax = static_cast<int>(myBands.size()) - 1;
  for (int idx = static_cast<int>(myBands.size()) - 1; idx >= 0; --idx)
  {
    if (dR[1] < myBands[idx][1] && dR[1] >= myBands[idx][0])
    {
      idxMax = static_cast<int>(idx);
      break;
    }
  }

  // Set the minimum to match the range minimum.
  x[idxMin][0] = dR[0];
  x[idxMax][1] = dR[1];
  for (int i = idxMin; i < idxMax + 1; ++i)
  {
    std::vector<double> b(3);
    b[0] = x[i][0];
    b[1] = x[i][0] + (x[i][1] - x[i][0]) / 2.0;
    b[2] = x[i][1];
    bands[i] = b;
  }
  return bands;
}

std::map<int, std::vector<double>> GetIntegralBands(double const dR[2])
{
  std::map<int, std::vector<double>> bands;
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
  return GetBands(x, numberOfBands, false);
}

std::map<int, int> GetFrequencies(std::map<int, std::vector<double>>& bands,
                                  vtkPolyData* src)
{
  std::map<int, int> freq;
  for (auto i = 0; i < static_cast<int>(bands.size()); ++i)
  {
    freq[i] = 0;
  }
  vtkIdType tuples = src->GetPointData()->GetScalars()->GetNumberOfTuples();
  for (int i = 0; i < tuples; ++i)
  {
    double* x = src->GetPointData()->GetScalars()->GetTuple(i);
    for (auto j = 0; j < static_cast<int>(bands.size()); ++j)
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

void AdjustFrequencyRanges(std::map<int, std::vector<double>>& bands,
                           std::map<int, int>& freq)
{
  // Get the indices of the first and last non-zero elements.
  auto first = 0;
  for (auto i = 0; i < static_cast<int>(freq.size()); ++i)
  {
    if (freq[i] != 0)
    {
      first = i;
      break;
    }
  }
  std::vector<int> keys;
  for (std::map<int, int>::iterator it = freq.begin(); it != freq.end(); ++it)
  {
    keys.push_back(it->first);
  }
  std::reverse(keys.begin(), keys.end());
  auto last = keys[0];
  for (size_t i = 0; i < keys.size(); ++i)
  {
    if (freq[keys[i]] != 0)
    {
      last = keys[i];
      break;
    }
  }
  // Now adjust the ranges.
  std::map<int, int>::iterator freqItr;
  freqItr = freq.find(first);
  freq.erase(freq.begin(), freqItr);
  freqItr = ++freq.find(last);
  freq.erase(freqItr, freq.end());
  std::map<int, std::vector<double>>::iterator bandItr;
  bandItr = bands.find(first);
  bands.erase(bands.begin(), bandItr);
  bandItr = ++bands.find(last);
  bands.erase(bandItr, bands.end());
  // Reindex freq and bands.
  std::map<int, int> adjFreq;
  int idx = 0;
  for (auto p : freq)
  {
    adjFreq[idx] = p.second;
    ++idx;
  }
  std::map<int, std::vector<double>> adjBands;
  idx = 0;
  for (auto p : bands)
  {
    adjBands[idx] = p.second;
    ++idx;
  }
  bands = adjBands;
  freq = adjFreq;
}

void PrintBandsFrequencies(std::map<int, std::vector<double>> const& bands,
                           std::map<int, int>& freq, int const& precision)
{
  auto prec = abs(precision);
  prec = (prec > 14) ? 14 : prec;

  if (bands.size() != freq.size())
  {
    std::cout << "Bands and frequencies must be the same size." << std::endl;
    return;
  }
  std::ostringstream os;
  os << "Bands & Frequencies:\n";
  size_t idx = 0;
  auto total = 0;
  auto width = prec + 6;
  for (std::map<int, std::vector<double>>::const_iterator p = bands.begin();
       p != bands.end(); ++p)
  {
    total += freq[p->first];
    for (std::vector<double>::const_iterator q = p->second.begin();
         q != p->second.end(); ++q)
    {
      if (q == p->second.begin())
      {
        os << std::setw(4) << idx << " [";
      }
      if (q == std::prev(p->second.end()))
      {
        os << std::fixed << std::setw(width) << std::setprecision(prec) << *q
           << "]: " << std::setw(8) << freq[p->first] << "\n";
      }
      else
      {
        os << std::fixed << std::setw(width) << std::setprecision(prec) << *q
           << ", ";
      }
    }
    ++idx;
  }
  width = 3 * width + 13;
  os << std::left << std::setw(width) << "Total" << std::right << std::setw(8)
     << total << std::endl;
  std::cout << os.str() << endl;
}

} // namespace
