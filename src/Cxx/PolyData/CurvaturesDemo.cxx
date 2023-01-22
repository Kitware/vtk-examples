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
#include <vtkFeatureEdges.h>
#include <vtkIdFilter.h>
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
#include <vtkVersion.h>

#include <algorithm>
#include <array>
#include <iterator>
#include <map>
#include <numeric>
#include <set>
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

vtkSmartPointer<vtkLookupTable> GetDivergingLut();
vtkSmartPointer<vtkLookupTable> GetDivergingLut1();
} // namespace

int main(int, char*[])
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

  // The next source will be a parametric function.
  vtkNew<vtkParametricRandomHills> rh;
  vtkNew<vtkParametricFunctionSource> rhFnSrc;
  rhFnSrc->SetParametricFunction(rh);
  rhFnSrc->Update();

  std::map<int, vtkSmartPointer<vtkPolyData>> sources;
  for (auto i = 0; i < 4; ++i)
  {
    std::string curvatureName;
    if (i < 2)
    {
      vtkNew<vtkCurvatures> cc;
      cc->SetInputConnection(cleaner->GetOutputPort());
      if (i % 2 == 0)
      {
        cc->SetCurvatureTypeToGaussian();
        curvatureName = "Gauss_Curvature";
      }
      else
      {
        cc->SetCurvatureTypeToMean();
        curvatureName = "Mean_Curvature";
      }
      cc->Update();
      AdjustEdgeCurvatures(cc->GetOutput(), curvatureName.c_str());
      sources[i] = cc->GetOutput();
    }
    else
    {
      vtkNew<vtkCurvatures> cc;
      cc->SetInputConnection(rhFnSrc->GetOutputPort());
      if (i % 2 == 0)
      {
        cc->SetCurvatureTypeToGaussian();
        curvatureName = "Gauss_Curvature";
      }
      else
      {
        cc->SetCurvatureTypeToMean();
        curvatureName = "Mean_Curvature";
      }
      cc->Update();
      AdjustEdgeCurvatures(cc->GetOutput(), curvatureName.c_str());
      sources[i] = cc->GetOutput();
    }
  }

  std::map<int, std::string> curvatures{
      {0, {"Gauss_Curvature"}},
      {1, {"Mean_Curvature"}},
      {2, {"Gauss_Curvature"}},
      {3, {"Mean_Curvature"}},
  };

  // auto lut = GetDivergingLut();
  auto lut = GetDivergingLut1();

  std::vector<vtkSmartPointer<vtkRenderer>> renderers;
  std::vector<vtkSmartPointer<vtkActor>> actors;
  std::vector<vtkSmartPointer<vtkPolyDataMapper>> mappers;
  std::vector<vtkSmartPointer<vtkTextMapper>> textmappers;
  std::vector<vtkSmartPointer<vtkActor2D>> textactors;
  std::vector<vtkSmartPointer<vtkScalarBarActor>> scalarBars;
  for (size_t idx = 0; idx < sources.size(); ++idx)
  {
    mappers.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
    actors.push_back(vtkSmartPointer<vtkActor>::New());
    textmappers.push_back(vtkSmartPointer<vtkTextMapper>::New());
    textactors.push_back(vtkSmartPointer<vtkActor2D>::New());
    scalarBars.push_back(vtkSmartPointer<vtkScalarBarActor>::New());
    renderers.push_back(vtkSmartPointer<vtkRenderer>::New());
  }

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

  for (auto idx = 0; idx < static_cast<int>(sources.size()); ++idx)
  {
    std::string curvatureName = curvatures[idx];
    std::replace(curvatureName.begin(), curvatureName.end(), '_', '\n');

    sources[idx]->GetPointData()->SetActiveScalars(curvatures[idx].c_str());
    auto scalarRange = sources[idx]
                           ->GetPointData()
                           ->GetScalars(curvatures[idx].c_str())
                           ->GetRange();

    mappers[idx]->SetInputData(sources[idx]);
    mappers[idx]->SetScalarModeToUsePointFieldData();
    mappers[idx]->SelectColorArray(curvatures[idx].c_str());
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
      // countInvalid += 1;
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
