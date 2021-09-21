#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkCubeSource.h>
#include <vtkCurvatures.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkFeatureEdges.h>
#include <vtkFloatArray.h>
#include <vtkIdFilter.h>
#include <vtkIdList.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricBour.h>
#include <vtkParametricEnneper.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricMobius.h>
#include <vtkParametricRandomHills.h>
#include <vtkParametricTorus.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataTangents.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkTexturedSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkVersion.h>
#include <vtkXMLPolyDataWriter.h>

#if VTK_VERSION_NUMBER >= 90020210809ULL
#define HAS_COW
#include <vtkCameraOrientationWidget.h>
#endif

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <set>

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

// Some sample surfaces to try.
vtkSmartPointer<vtkPolyData> GetBour();
vtkSmartPointer<vtkPolyData> GetCube();
vtkSmartPointer<vtkPolyData> GetEnneper();
vtkSmartPointer<vtkPolyData> GetHills();
vtkSmartPointer<vtkPolyData> GetMobius();
vtkSmartPointer<vtkPolyData> GetRandomHills();
vtkSmartPointer<vtkPolyData> GetSphere();
vtkSmartPointer<vtkPolyData> GetTorus();

vtkSmartPointer<vtkPolyData> GetSource(std::string const& soource);

vtkSmartPointer<vtkLookupTable> GetDivergingLut();
vtkSmartPointer<vtkLookupTable> GetDivergingLut1();

std::map<int, std::vector<double>> GetBands(double const dR[2],
                                            int const& numberOfBands,
                                            int const& precision = 2,
                                            bool const& nearestInteger = false);
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

  std::string desiredSurface = "RandomHills";
  // desiredSurface = "Bour";
  // desiredSurface = "Cube";
  // desiredSurface = "Hills";
  // desiredSurface = "Enneper";
  // desiredSurface = "Mobius";
  // desiredSurface = "RandomHills";
  // desiredSurface = "Sphere";
  // desiredSurface = "Torus";

  auto source = GetSource(desiredSurface);

  vtkNew<vtkCurvatures> gc;
  gc->SetInputData(source);
  gc->SetCurvatureTypeToGaussian();
  gc->Update();
  std::vector<std::string> adjSurfaces{"Bour", "Enneper", "Hills",
                                       "RandomHills", "Torus"};
  if (std::find(adjSurfaces.begin(), adjSurfaces.end(), desiredSurface) !=
      adjSurfaces.end())
  {
    AdjustEdgeCurvatures(gc->GetOutput(), "Gauss_Curvature");
  }
  if (desiredSurface == "Bour")
  {
    // Gaussian curvature is -1/(r(r+1)^4))
    ConstrainCurvatures(gc->GetOutput(), "Gauss_Curvature", -0.0625, -0.0625);
  }
  if (desiredSurface == "Enneper")
  {
    // Gaussian curvature is -4/(1 + r^2)^4
    ConstrainCurvatures(gc->GetOutput(), "Gauss_Curvature", -0.25, -0.25);
  }
  if (desiredSurface == "Cube")
  {
    ConstrainCurvatures(gc->GetOutput(), "Gauss_Curvature", 0.0, 0.0);
  }
  if (desiredSurface == "Mobius")
  {
    ConstrainCurvatures(gc->GetOutput(), "Gauss_Curvature", 0.0, 0.0);
  }
  if (desiredSurface == "Sphere")
  {
    // Gaussian curvature is 1/r^2
    ConstrainCurvatures(gc->GetOutput(), "Gauss_Curvature", 4.0, 4.0);
  }
  source->GetPointData()->AddArray(
      gc->GetOutput()->GetPointData()->GetAbstractArray("Gauss_Curvature"));

  vtkNew<vtkCurvatures> mc;
  mc->SetInputData(source);
  mc->SetCurvatureTypeToMean();
  mc->Update();
  if (std::find(adjSurfaces.begin(), adjSurfaces.end(), desiredSurface) !=
      adjSurfaces.end())
  {
    AdjustEdgeCurvatures(mc->GetOutput(), "Mean_Curvature");
  }
  if (desiredSurface == "Bour")
  {
    // Mean curvature is 0
    ConstrainCurvatures(mc->GetOutput(), "Mean_Curvature", 0.0, 0.0);
  }
  if (desiredSurface == "Enneper")
  {
    // Mean curvature is 0
    ConstrainCurvatures(mc->GetOutput(), "Mean_Curvature", 0.0, 0.0);
  }
  if (desiredSurface == "Sphere")
  {
    // Mean curvature is 1/r
    ConstrainCurvatures(mc->GetOutput(), "Mean_Curvature", 2.0, 2.0);
  }
  source->GetPointData()->AddArray(
      mc->GetOutput()->GetPointData()->GetAbstractArray("Mean_Curvature"));

  // Uncomment the following lines if you want to write out the polydata.
  // vtkNew<vtkXMLPolyDataWriter> writer;
  // writer->SetFileName("Source.vtp");
  // writer->SetInputData(source);
  // writer->SetDataModeToAscii();
  // writer->Write();

  // Let's visualise what we have done.

  vtkNew<vtkNamedColors> colors;

  colors->SetColor("ParaViewBkg",
                   std::array<unsigned char, 4>{82, 87, 110, 255}.data());

  auto windowWidth = 1024;
  auto windowHeight = 512;

  vtkNew<vtkRenderWindow> renWin;
  renWin->SetSize(windowWidth, windowHeight);
  vtkNew<vtkRenderWindowInteractor> iRen;
  iRen->SetRenderWindow(renWin);
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  iRen->SetInteractorStyle(style);

  // Create a common text property.
  vtkNew<vtkTextProperty> textProperty;
  textProperty->SetFontSize(24);
  textProperty->SetJustificationToCentered();

  auto lut = GetDivergingLut();
  // auto lut = GetDivergingLut1();

  // Define viewport ranges
  std::array<double, 2> xmins{0, 0.5};
  std::array<double, 2> xmaxs{0.5, 1};
  std::array<double, 2> ymins{0, 0};
  std::array<double, 2> ymaxs{1.0, 1.0};

  vtkCamera* camera = nullptr;

#ifdef HAS_COW
  vtkNew<vtkCameraOrientationWidget> camOrientManipulator;
#endif

  std::array<std::string, 2> curvatureTypes{"Gauss_Curvature",
                                            "Mean_Curvature"};
  for (size_t idx = 0; idx < curvatureTypes.size(); ++idx)
  {
    auto curvatureTitle = curvatureTypes[idx];
    std::replace(curvatureTitle.begin(), curvatureTitle.end(), '_', '\n');

    source->GetPointData()->SetActiveScalars(curvatureTypes[idx].c_str());
    auto scalarRange = source->GetPointData()
                           ->GetScalars(curvatureTypes[idx].c_str())
                           ->GetRange();

    auto bands = GetBands(scalarRange, 10);
    auto freq = GetFrequencies(bands, source);
    AdjustFrequencyRanges(bands, freq);
    std::cout << curvatureTypes[idx] << std::endl;
    PrintBandsFrequencies(bands, freq);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source);
    mapper->SetScalarModeToUsePointFieldData();
    mapper->SelectColorArray(curvatureTypes[idx].c_str());
    mapper->SetScalarRange(scalarRange);
    mapper->SetLookupTable(lut);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    // Create a scalar bar
    vtkNew<vtkScalarBarActor> scalarBar;
    scalarBar->SetLookupTable(mapper->GetLookupTable());
    scalarBar->SetTitle(curvatureTitle.c_str());
    scalarBar->UnconstrainedFontSizeOn();
    scalarBar->SetNumberOfLabels(
        std::min<int>(5, static_cast<int>(freq.size())));
    scalarBar->SetMaximumWidthInPixels(windowWidth / 8);
    scalarBar->SetMaximumHeightInPixels(windowHeight / 3);
    scalarBar->SetBarRatio(scalarBar->GetBarRatio() * 0.5);
    scalarBar->SetPosition(0.85, 0.1);

    vtkNew<vtkTextMapper> textMapper;
    textMapper->SetInput(curvatureTitle.c_str());
    textMapper->SetTextProperty(textProperty);

    vtkNew<vtkActor2D> textActor;
    textActor->SetMapper(textMapper);
    textActor->SetPosition(250, 16);

    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("ParaViewBkg").GetData());

    renderer->AddActor(actor);
    renderer->AddActor(textActor);
    renderer->AddActor(scalarBar);

    renWin->AddRenderer(renderer);

    if (idx == 0)
    {
#ifdef HAS_COW
      camOrientManipulator->SetParentRenderer(renderer);
#endif
      camera = renderer->GetActiveCamera();
      camera->Elevation(60);
    }
    else
    {
      renderer->SetActiveCamera(camera);
    }
    renderer->SetViewport(xmins[idx], ymins[idx], xmaxs[idx], ymaxs[idx]);
    renderer->ResetCamera();
  }
#ifdef HAS_COW
  // Enable the widget.
  camOrientManipulator->On();
#endif

  renWin->Render();
  renWin->SetWindowName("CurvaturesAdjustEdges");
  iRen->Start();

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

vtkSmartPointer<vtkPolyData> GetBour()
{
  auto uResolution = 51;
  auto vResolution = 51;
  vtkNew<vtkParametricBour> surface;

  vtkNew<vtkParametricFunctionSource> source;
  source->SetUResolution(uResolution);
  source->SetVResolution(vResolution);
  source->GenerateTextureCoordinatesOn();
  source->SetParametricFunction(surface);
  source->Update();

  // Build the tangents
  vtkNew<vtkPolyDataTangents> tangents;
  tangents->SetInputConnection(source->GetOutputPort());
  tangents->Update();

  return tangents->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetCube()
{
  vtkNew<vtkCubeSource> surface;

  // Triangulate
  vtkNew<vtkTriangleFilter> triangulation;
  triangulation->SetInputConnection(surface->GetOutputPort());
  // Subdivide the triangles
  vtkNew<vtkLinearSubdivisionFilter> subdivide;
  subdivide->SetInputConnection(triangulation->GetOutputPort());
  subdivide->SetNumberOfSubdivisions(3);
  // Now the tangents
  vtkNew<vtkPolyDataTangents> tangents;
  tangents->SetInputConnection(subdivide->GetOutputPort());
  tangents->Update();
  return tangents->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetEnneper()
{
  auto uResolution = 51;
  auto vResolution = 51;
  vtkNew<vtkParametricEnneper> surface;

  vtkNew<vtkParametricFunctionSource> source;
  source->SetUResolution(uResolution);
  source->SetVResolution(vResolution);
  source->GenerateTextureCoordinatesOn();
  source->SetParametricFunction(surface);
  source->Update();

  // Build the tangents
  vtkNew<vtkPolyDataTangents> tangents;
  tangents->SetInputConnection(source->GetOutputPort());
  tangents->Update();

  return tangents->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetHills()
{
  // Create four hills on a plane.
  // This will have regions of negative, zero and positive Gsaussian
  // curvatures.

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

vtkSmartPointer<vtkPolyData> GetMobius()
{
  auto uResolution = 51;
  auto vResolution = 51;
  vtkNew<vtkParametricMobius> surface;
  surface->SetMinimumV(-0.25);
  surface->SetMaximumV(0.25);

  vtkNew<vtkParametricFunctionSource> source;
  source->SetUResolution(uResolution);
  source->SetVResolution(vResolution);
  source->GenerateTextureCoordinatesOn();
  source->SetParametricFunction(surface);
  source->Update();

  // Build the tangents
  vtkNew<vtkPolyDataTangents> tangents;
  tangents->SetInputConnection(source->GetOutputPort());
  tangents->Update();

  vtkNew<vtkTransform> transform;
  transform->RotateX(-90.0);
  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetInputConnection(tangents->GetOutputPort());
  transformFilter->SetTransform(transform);
  transformFilter->Update();

  return transformFilter->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetRandomHills()
{
  auto uResolution = 51;
  auto vResolution = 51;
  vtkNew<vtkParametricRandomHills> surface;
  surface->SetRandomSeed(1);
  surface->SetNumberOfHills(30);
  // If you want a plane
  // surface->SetHillAmplitude(0);

  vtkNew<vtkParametricFunctionSource> source;
  source->SetUResolution(uResolution);
  source->SetVResolution(vResolution);
  source->GenerateTextureCoordinatesOn();
  source->SetParametricFunction(surface);
  source->Update();

  // Build the tangents
  vtkNew<vtkPolyDataTangents> tangents;
  tangents->SetInputConnection(source->GetOutputPort());
  tangents->Update();

  vtkNew<vtkTransform> transform;
  transform->Translate(0.0, 5.0, 15.0);
  transform->RotateX(-90.0);
  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetInputConnection(tangents->GetOutputPort());
  transformFilter->SetTransform(transform);
  transformFilter->Update();

  return transformFilter->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetSphere()
{
  auto thetaResolution = 32;
  auto phiResolution = 32;
  vtkNew<vtkTexturedSphereSource> surface;
  surface->SetThetaResolution(thetaResolution);
  surface->SetPhiResolution(phiResolution);

  // Now the tangents
  vtkNew<vtkPolyDataTangents> tangents;
  tangents->SetInputConnection(surface->GetOutputPort());
  tangents->Update();
  return tangents->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetTorus()
{
  auto uResolution = 51;
  auto vResolution = 51;
  vtkNew<vtkParametricTorus> surface;

  vtkNew<vtkParametricFunctionSource> source;
  source->SetUResolution(uResolution);
  source->SetVResolution(vResolution);
  source->GenerateTextureCoordinatesOn();
  source->SetParametricFunction(surface);
  source->Update();

  // Build the tangents
  vtkNew<vtkPolyDataTangents> tangents;
  tangents->SetInputConnection(source->GetOutputPort());
  tangents->Update();

  vtkNew<vtkTransform> transform;
  transform->RotateX(-90.0);
  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetInputConnection(tangents->GetOutputPort());
  transformFilter->SetTransform(transform);
  transformFilter->Update();

  return transformFilter->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetSource(std::string const& source)
{
  std::string surface = source;
  std::transform(surface.begin(), surface.end(), surface.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  std::map<std::string, int> available_surfaces = {
      {"bour", 0},   {"cube", 1},        {"enneper", 2}, {"hills", 3},
      {"mobius", 4}, {"randomhills", 5}, {"sphere", 6},  {"torus", 7}};
  if (available_surfaces.find(surface) == available_surfaces.end())
  {
    std::cout << "The surface is not available." << std::endl;
    std::cout << "Using RandomHills instead." << std::endl;
    surface = "randomhills";
  }
  switch (available_surfaces[surface])
  {
  case 0:
    return GetBour();
    break;
  case 1:
    return GetCube();
    break;
  case 2:
    return GetEnneper();
    break;
  case 3:
    return GetHills();
    break;
  case 4:
    return GetMobius();
    break;
  case 5:
    return GetRandomHills();
    break;
  case 6:
    return GetSphere();
    break;
  case 7:
    return GetTorus();
    break;
  }
  return GetRandomHills();
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
