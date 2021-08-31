#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkCamera.h>
#include <vtkCameraOrientationWidget.h>
#include <vtkCleanPolyData.h>
#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>
#include <vtkElevationFilter.h>
#include <vtkGlyph3D.h>
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

enum SURFACE_TYPE
{
  PLANE = 0,
  SPHERE,
  PARAMETRIC_SURFACE
};

namespace {

//! Divide a range into bands
/*!
@param dR - [min, max] the range that is to be covered by the bands.
@param numberOfBands - the number of bands, a positive integer.
@param nearestInteger - if True then [floor(min), ceil(max)] is used.
@return A map consisting of the band inxex and [min, midpoint, max] for each
band.
*/
std::map<int, std::vector<double>> MakeBands(double const dR[2],
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
@return  A map consisting of the band inxex and [min, midpoint, max] for each
band.
*/
std::map<int, std::vector<double>>
MakeCustomBands(double const dR[2], int const& numberOfBands,
                std::vector<std::array<double, 2>> const& myBands);

//! Divide a range into integral bands
/*!
Divide a range into bands
@param dR - [min, max] the range that is to be covered by the bands.
@returnA map consisting of the band inxex and [min, midpoint, max] for each
band.
*/
std::map<int, std::vector<double>> MakeIntegralBands(double const dR[2]);

//! Generate elevations over the surface.
/*!
@param src - the vtkPolyData source.
@return elev - the elevations.
*/
vtkSmartPointer<vtkPolyData> MakeElevations(vtkPolyData* src);

//! Make a parametric hills surface as the source.
/*!
@return - vtkPolyData.
*/
vtkSmartPointer<vtkPolyData> MakeParametricHills();

//! Make a parametric torus as the source.
/*!
@return - vtkPolyData.
*/
vtkSmartPointer<vtkPolyData> MakeParametricTorus();

//! Make a Plane as the source.
/*!
@return - vtkPolyData.
*/
vtkSmartPointer<vtkPolyData> MakePlane();

//! Make a MakeSphere as the source.
/*!
@return - vtkPolyData.
*/
vtkSmartPointer<vtkPolyData> MakeSphere();

//! Make a torus as the source.
/*!
@return - vtkPolyData.
*/
vtkSmartPointer<vtkPolyData> MakeTorus();

vtkSmartPointer<vtkColorSeries> GetColorSeries();

vtkSmartPointer<vtkLookupTable> MakeCategoricalLUT();

vtkSmartPointer<vtkLookupTable> MakeOrdinaLUT();

vtkSmartPointer<vtkLookupTable> MakeDivergingLUT();

vtkSmartPointer<vtkLookupTable> ReverseLUT(vtkLookupTable* lut);

//! Count the number of scalars in each band.
/*!
@param bands - the bands.
@param src - the vtkPolyData source.
@return The frequencies of the scalars in each band.
*/
std::map<int, int> Frequencies(std::map<int, std::vector<double>>& bands,
                               vtkPolyData* src);

std::pair<int, int> AdjustFrequencyRanges(std::map<int, int>& freq);

void PrintBands(std::map<int, std::vector<double>> const& bands);

void PrintFrequencies(std::map<int, int> const& freq);

void PrintBandsFrequencies(std::map<int, std::vector<double>> const& bands,
                           std::map<int, int>& freq);

//!  Glyph the normals on the surface.
/*!
@param src - the vtkPolyData source.
@param reverseNormals - if True the normals on the surface are reversed.
@return The glyphs.
*/
vtkNew<vtkGlyph3D> MakeGlyphs(vtkPolyData* src, bool const& reverseNormals);

} // namespace

//! Make and display the surface.
int main(int, char*[])
{
  // Get the surface
  // std::string desiredSurface = "ParametricTorus";
  // std::string desiredSurface = "Plane";
  std::string desiredSurface = "RandomHills";
  // std::string desiredSurface = "Sphere";
  // std::string desiredSurface = "Torus";
  auto lcSurface = desiredSurface;
  std::transform(lcSurface.begin(), lcSurface.end(), lcSurface.begin(),
                 [](char c) { return std::tolower(c); });
  std::map<std::string, int> availableSurfaces = {{"parametrictorus", 0},
                                                  {"plane", 1},
                                                  {"randomhills", 2},
                                                  {"sphere", 3},
                                                  {"torus", 4}};
  vtkSmartPointer<vtkPolyData> src;
  if (availableSurfaces.find(lcSurface) == availableSurfaces.end())
  {
    std::cout << "No surface specified." << std::endl;
    return EXIT_FAILURE;
  }
  switch (availableSurfaces[lcSurface])
  {
  case 0: {
    src = MakeParametricTorus();
    break;
  }
  case 1: {
    src = MakePlane();
    src = MakeElevations(src);
    break;
  }
  case 2: {
    src = MakeParametricHills();
    break;
  }
  case 3: {
    src = MakeSphere();
    src = MakeElevations(src);
    break;
  }
  case 4: {
    src = MakeTorus();
    src = MakeElevations(src);
    break;
  }
  default: {
    std::cout << "No surface specified." << std::endl;
    return EXIT_FAILURE;
  }
  }
  std::cout << desiredSurface << std::endl;

  src->GetPointData()->SetActiveScalars("Elevation");
  auto scalarRange = src->GetPointData()->GetScalars("Elevation")->GetRange();

  auto lut = MakeCategoricalLUT();
  auto lut1 = MakeOrdinaLUT();
  lut->SetTableRange(scalarRange);
  lut1->SetTableRange(scalarRange);
  vtkIdType numberOfBands = lut->GetNumberOfTableValues();
  std::map<int, std::vector<double>> bands;

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
        {0, 1.0},   {1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0},
        {4.0, 5.0}, {5.0, 6.0}, {6.0, 7.0}, {7.0, 8.0}};
    // Comment this out if you want to see how allocating
    // equally spaced bands works.
    bands = MakeCustomBands(scalarRange, numberOfBands, myBands);
    // bands = MakeBands(scalarRange, numberOfBands, false);
    // Adjust the number of table values
    lut->SetNumberOfTableValues(static_cast<vtkIdType>(bands.size()));
    lut1->SetNumberOfTableValues(static_cast<vtkIdType>(bands.size()));
  }
  else
  {
    bands = MakeBands(scalarRange, numberOfBands, false);
  }

  // Let's do a frequency table.
  // The number of scalars in each band.
  std::map<int, int> freq = Frequencies(bands, src);

  std::vector<int> keys;
  for (int k = 0; k < freq.size(); ++k)
  {
    keys.push_back(k);
  }
  auto minKey = std::min_element(keys.begin(), keys.end());
  auto maxKey = std::max_element(keys.begin(), keys.end());
  if (lcSurface == "sphere")
  {
    freq[0] = 0;
  }
  auto freqRange = AdjustFrequencyRanges(freq);
  std::map<int, int>::iterator freqItr;
  freqItr = freq.find(freqRange.first);
  freq.erase(freq.begin(), freqItr);
  freqItr = ++freq.find(freqRange.second);
  freq.erase(freqItr, freq.end());
  std::map<int, std::vector<double>>::iterator bandItr;
  bandItr = bands.find(freqRange.first);
  bands.erase(bands.begin(), bandItr);
  bandItr = ++bands.find(freqRange.second);
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
  // PrintBandsFrequencies(bands, freq);
  PrintBandsFrequencies(adjBands, adjFreq);
  freq.erase(freq.begin(), freq.end());
  bands.erase(bands.begin(), bands.end());

  scalarRange[0] = adjBands.begin()->second[0];
  scalarRange[1] = std::prev(adjBands.end())->second[2];
  lut->SetTableRange(scalarRange);
  lut->SetNumberOfTableValues(adjBands.size());

  // We will use the midpoint of the band as the label.
  // std::vector<std::string> labels;
  // for (std::vector<std::vector<double>>::const_iterator p = bands.begin();
  //     p != bands.end(); ++p)
  //{
  //  std::ostringstream os;
  //  os << std::fixed << std::setw(6) << std::setprecision(2) << (*p)[1];
  //  labels.push_back(os.str());
  //}

  // We will use the midpoint of the band as the label.
  std::vector<std::string> labels;
  for (std::map<int, std::vector<double>>::const_iterator p = adjBands.begin();
       p != adjBands.end(); ++p)
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

  //// Create the contour bands.
  // vtkNew<vtkBandedPolyDataContourFilter> bcf;
  // bcf->SetInputData(src);
  //// Use either the minimum or maximum value for each band.
  // int i = 0;
  // for (std::vector<std::vector<double>>::const_iterator p = bands.begin();
  //     p != bands.end(); ++p)
  //{
  //  bcf->SetValue(i, (*p)[2]);
  //  ++i;
  //}

  // Create the contour bands.
  vtkNew<vtkBandedPolyDataContourFilter> bcf;
  bcf->SetInputData(src);
  // Use either the minimum or maximum value for each band.
  int i = 0;
  for (std::map<int, std::vector<double>>::const_iterator p = adjBands.begin();
       p != adjBands.end(); ++p)
  {
    bcf->SetValue(i, p->second[2]);
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
  colors->SetColor("BkgColor",
                   std::array<unsigned char, 4>{179, 204, 255, 255}.data());
  colors->SetColor("ParaViewBkg",
                   std::array<unsigned char, 4>{82, 87, 110, 255}.data());

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
  glyphMapper->SetLookupTable(lut1);
  glyphMapper->SetScalarRange(scalarRange);

  vtkNew<vtkActor> glyphActor;
  glyphActor->SetMapper(glyphMapper);

  auto windowWidth = 800;
  auto windowHeight = 800;

  // Add scalar bars.
  vtkNew<vtkScalarBarActor> scalarBar;
  // This LUT puts the lowest value at the top of the scalar bar.
  // scalarBar->SetLookupTable(lut);
  // Use this LUT if you want the highest value at the top.
  scalarBar->SetLookupTable(lutr);
  scalarBar->SetTitle("Elevation");
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

  ren->SetBackground(colors->GetColor3d("ParaViewBkg").GetData());
  renWin->SetSize(windowWidth, windowHeight);
  renWin->SetWindowName("ElevationBandsWithGlyphs");

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

std::map<int, std::vector<double>> MakeBands(double const dR[2],
                                             int const& numberOfBands,
                                             bool const& nearestInteger)
{
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
    bands[i] = b;
    for (std::vector<double>::iterator p = b.begin(); p != b.end(); ++p)
    {
      *p = *p + dx;
    }
  }
  return bands;
}

std::map<int, std::vector<double>>
MakeCustomBands(double const dR[2], int const& numberOfBands,
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
  for (auto idx = 0; idx < myBands.size(); ++idx)
  {
    if (dR[0] < myBands[idx][1] && dR[0] >= myBands[idx][0])
    {
      idxMin = idx;
      break;
    }
  }
  int idxMax = static_cast<int>(myBands.size()) - 1;
  for (auto idx = myBands.size() - 1; idx >= 0; --idx)
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

std::map<int, std::vector<double>> MakeIntegralBands(double const dR[2])
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
  return MakeBands(x, numberOfBands, false);
}

vtkSmartPointer<vtkPolyData> MakeElevations(vtkPolyData* src)
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

vtkSmartPointer<vtkPolyData> MakePlane()
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

vtkSmartPointer<vtkPolyData> MakeSphere()
{
  vtkNew<vtkSphereSource> source;
  source->SetCenter(0.0, 0.0, 0.0);
  source->SetRadius(10.0);
  source->SetThetaResolution(32);
  source->SetPhiResolution(32);
  source->Update();

  return source->GetOutput();
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

vtkSmartPointer<vtkLookupTable> MakeCategoricalLUT()
{
  vtkSmartPointer<vtkColorSeries> colorSeries = GetColorSeries();
  // Make the lookup table.
  vtkNew<vtkLookupTable> lut;
  colorSeries->BuildLookupTable(lut, vtkColorSeries::CATEGORICAL);
  lut->SetNanColor(0, 0, 0, 1);

  return lut;
}

vtkSmartPointer<vtkLookupTable> MakeOrdinaLUT()
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
vtkSmartPointer<vtkLookupTable> MakeDivergingLUT()
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

std::map<int, int> Frequencies(std::map<int, std::vector<double>>& bands,
                               vtkPolyData* src)
{
  std::map<int, int> freq;
  for (int i = 0; i < bands.size(); ++i)
  {
    freq[i] = 0;
  }
  vtkIdType tuples = src->GetPointData()->GetScalars()->GetNumberOfTuples();
  for (int i = 0; i < tuples; ++i)
  {
    double* x = src->GetPointData()->GetScalars()->GetTuple(i);
    for (int j = 0; j < bands.size(); ++j)
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

std::pair<int, int> AdjustFrequencyRanges(std::map<int, int>& freq)
{
  auto first = 0;
  for (auto i = 0; i < freq.size(); ++i)
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
  for (auto i = 0; i < keys.size(); ++i)
  {
    if (freq[keys[i]] != 0)
    {
      last = keys[i];
      break;
    }
  }
  return std::pair<int, int>(first, last);
}

void PrintBands(std::map<int, std::vector<double>> const& bands)
{
  std::ostringstream os;
  os << "Bands:\n";
  size_t idx = 0;
  for (std::map<int, std::vector<double>>::const_iterator p = bands.begin();
       p != bands.end(); ++p)
  {
    for (std::vector<double>::const_iterator q = p->second.begin();
         q != p->second.end(); ++q)
    {
      if (q == p->second.begin())
      {
        os << std::setw(4) << idx << " [";
      }
      if (q == std::prev(p->second.end()))
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

void PrintFrequencies(std::map<int, int> const& freq)
{
  std::ostringstream os;
  int i = 0;
  for (std::map<int, int>::const_iterator p = freq.begin(); p != freq.end();
       ++p)
  {
    if (i == 0)
    {
      os << "Frequencies: [";
    }
    if (p == std::prev(freq.end()))
    {
      os << i << ": " << p->second << "]\n";
    }
    else
    {
      os << i << ": " << p->second << ", ";
    }
    ++i;
  }
  std::cout << os.str() << endl;
}

void PrintBandsFrequencies(std::map<int, std::vector<double>> const& bands,
                           std::map<int, int>& freq)
{
  if (bands.size() != freq.size())
  {
    std::cout << "Bands and frequencies must be the same size." << std::endl;
    return;
  }
  std::ostringstream os;
  os << "Bands & frequencies:\n";
  size_t idx = 0;
  for (std::map<int, std::vector<double>>::const_iterator p = bands.begin();
       p != bands.end(); ++p)
  {
    for (std::vector<double>::const_iterator q = p->second.begin();
         q != p->second.end(); ++q)
    {
      if (q == p->second.begin())
      {
        os << std::setw(4) << idx << " [";
      }
      if (q == std::prev(p->second.end()))
      {
        os << std::fixed << std::setw(6) << std::setprecision(3) << *q
           << "]: " << std::setw(6) << freq[p->first] << "\n";
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
