#include <vtkActor.h>
#include <vtkCameraOrientationWidget.h>
#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>
#include <vtkCurvatures.h>
#include <vtkFeatureEdges.h>
#include <vtkIdFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkTextProperty.h>
#include <vtkVersion.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

#if VTK_VERSION_NUMBER >= 90020210809ULL
#define HAS_COW
#include <vtkCameraOrientationWidget.h>
#endif

#include <array>
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

} // namespace

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Parse command line arguments
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " Filename(.vtp) e.g. cowHead.vtp m 16"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Defaults
  auto scheme = 16;
  std::string curvature{"Mean_Curvature"};
  if (argc > 2)
  {
    std::string tmp = argv[2];
    if (tmp[0] == 'g' || tmp[0] == 'G')
    {
      curvature = "Gauss_Curvature";
    }
    else
    {
      if (tmp[0] == 'm' || tmp[0] == 'm')
      {
        curvature = "Mean_Curvature";
      }
      else
      {
        std::cerr << "Curvature must be either gaussian or mean." << std::endl;
        return EXIT_FAILURE;
      }
    }
  }

  if (argc > 3)
  {
    scheme = atoi(argv[3]);
  }

  // Create a polydata
  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  auto source = reader->GetOutput();

  vtkNew<vtkCurvatures> cc;
  cc->SetInputData(source);
  if (curvature == "Gauss_Curvature")
  {
    cc->SetCurvatureTypeToGaussian();
    cc->Update();
  }
  else
  {
    if (curvature == "Mean_Curvature")
    {
      cc->SetCurvatureTypeToMean();
      cc->Update();
    }
    else
    {
      std::cerr << "Unknown curvature" << std::endl;
      return EXIT_FAILURE;
    }
  }
  AdjustEdgeCurvatures(cc->GetOutput(), curvature);
  source->GetPointData()->AddArray(
      cc->GetOutput()->GetPointData()->GetAbstractArray(curvature.c_str()));

  auto scalarRange =
      source->GetPointData()->GetScalars(curvature.c_str())->GetRange();

  auto curvatureTitle = curvature;
  std::replace(curvatureTitle.begin(), curvatureTitle.end(), '_', '\n');

  // Build a lookup table
  vtkNew<vtkColorSeries> colorSeries;
  colorSeries->SetColorScheme(scheme);
  std::cout << "Using color scheme #: " << colorSeries->GetColorScheme() << ", "
            << colorSeries->GetColorSchemeName() << std::endl;

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

  // Create a mapper and actor.
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(source);
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

#ifdef HAS_COW
  vtkNew<vtkCameraOrientationWidget> camOrientManipulator;
  camOrientManipulator->SetParentRenderer(renderer);
  // Enable the widget.
  camOrientManipulator->On();
#endif

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->AddActor2D(scalarBar);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  // Render and interact
  renWin->Render();
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

} // namespace
