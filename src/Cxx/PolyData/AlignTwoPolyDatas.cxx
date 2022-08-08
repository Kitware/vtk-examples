#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkFieldData.h>
#include <vtkHausdorffDistancePointSetFilter.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkLandmarkTransform.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBBTree.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkVersion.h>

// Readers
#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>

#include <vtkPolyData.h>
#include <vtkSphereSource.h>

#include <algorithm> // For transform()
#include <cctype>    // For to_lower
#include <cmath>     // for std::isnan and std::isinf
#include <string>    // For find_last_of()

#include <array>
#include <random>
#include <sstream>

#if VTK_VERSION_NUMBER >= 90020210809ULL
#define VTK_HAS_COW 1
#endif

#if VTK_HAS_COW
#include <vtkCameraOrientationWidget.h>
#endif

namespace {
/**
 * Show the command lime parameters.
 *
 * @param fn: The program name.
 */
std::string ShowUsage(std::string fn);

vtkSmartPointer<vtkPolyData> ReadPolyData(std::string const& fileName);
void AlignBoundingBoxes(vtkPolyData*, vtkPolyData*);
void BestBoundingBox(std::string const& axis, vtkPolyData* target,
                     vtkPolyData* source, vtkPolyData* targetLandmarks,
                     vtkPolyData* sourceLandmarks, double& distance,
                     vtkPoints* bestPoints);
} // namespace

int main(int argc, char* argv[])
{
  if (argc != 3)
  {
    std::cout << ShowUsage(argv[0]) << std::endl;
    return EXIT_FAILURE;
  }

  // Vis Pipeline
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(640, 480);
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->SetBackground(colors->GetColor3d("sea_green_light").GetData());
  renderer->UseHiddenLineRemovalOn();

  std::cout << "Loading source: " << argv[1] << std::endl;
  auto sourcePolyData = ReadPolyData(argv[1]);

  // Save the source polydata in case the align does not improve
  // segmentation
  vtkNew<vtkPolyData> originalSourcePolyData;
  originalSourcePolyData->DeepCopy(sourcePolyData);

  std::cout << "Loading target: " << argv[2] << std::endl;
  auto targetPolyData = ReadPolyData(argv[2]);

  // If the target orientation is markedly different, you may need to apply a
  // transform to orient the target with the source.
  // For example, when using Grey_Nurse_Shark.stl as the source and
  // greatWhite.stl as the target, you need to transform the target.
  auto sourceFound =
      std::string(argv[1]).find("Grey_Nurse_Shark.stl") != std::string::npos;
  auto targetFound =
      std::string(argv[2]).find("greatWhite.stl") != std::string::npos;
  vtkNew<vtkTransform> trnf;
  if (sourceFound && targetFound)
  {
    trnf->RotateY(90);
  }
  vtkNew<vtkTransformPolyDataFilter> tpd;
  tpd->SetTransform(trnf);
  tpd->SetInputData(targetPolyData);
  tpd->Update();

  vtkNew<vtkHausdorffDistancePointSetFilter> distance;
  distance->SetInputData(0, tpd->GetOutput());
  distance->SetInputData(1, sourcePolyData);
  distance->Update();

  double distanceBeforeAlign = static_cast<vtkPointSet*>(distance->GetOutput(0))
                                   ->GetFieldData()
                                   ->GetArray("HausdorffDistance")
                                   ->GetComponent(0, 0);

  // Get initial alignment using oriented bounding boxes
  AlignBoundingBoxes(sourcePolyData, tpd->GetOutput());

  distance->SetInputData(0, tpd->GetOutput());
  distance->SetInputData(1, sourcePolyData);
  distance->Modified();
  distance->Update();
  double distanceAfterAlign = static_cast<vtkPointSet*>(distance->GetOutput(0))
                                  ->GetFieldData()
                                  ->GetArray("HausdorffDistance")
                                  ->GetComponent(0, 0);

  double bestDistance = std::min(distanceBeforeAlign, distanceAfterAlign);

  if (distanceAfterAlign > distanceBeforeAlign)
  {
    sourcePolyData->DeepCopy(originalSourcePolyData);
  }

  // Refine the alignment using IterativeClosestPoint
  vtkNew<vtkIterativeClosestPointTransform> icp;
  icp->SetSource(sourcePolyData);
  icp->SetTarget(tpd->GetOutput());
  icp->GetLandmarkTransform()->SetModeToRigidBody();
  icp->SetMaximumNumberOfLandmarks(100);
  icp->SetMaximumMeanDistance(.00001);
  icp->SetMaximumNumberOfIterations(500);
  icp->CheckMeanDistanceOn();
  icp->StartByMatchingCentroidsOn();
  icp->Update();
  auto icpMeanDistance = icp->GetMeanDistance();

  //  icp->Print(std::cout);

  auto lmTransform = icp->GetLandmarkTransform();
  vtkNew<vtkTransformPolyDataFilter> transform;
  transform->SetInputData(sourcePolyData);
  transform->SetTransform(lmTransform);
  transform->SetTransform(icp);
  transform->Update();

  distance->SetInputData(0, tpd->GetOutput());
  distance->SetInputData(1, transform->GetOutput());
  distance->Update();

  // Note: If there is an error extracting eigenfunctions, then this will be
  // zero.
  double distanceAfterICP = static_cast<vtkPointSet*>(distance->GetOutput(0))
                                ->GetFieldData()
                                ->GetArray("HausdorffDistance")
                                ->GetComponent(0, 0);
  if (!(std::isnan(icpMeanDistance) || std::isinf(icpMeanDistance)))
  {
    if (distanceAfterICP < bestDistance)
    {
      bestDistance = distanceAfterICP;
    }
  }

  std::cout << "Distances:" << std::endl;
  std::cout << "  Before aligning:                        "
            << distanceBeforeAlign << std::endl;
  std::cout << "  Aligning using oriented bounding boxes: "
            << distanceAfterAlign << std::endl;
  std::cout << "  Aligning using IterativeClosestPoint:   " << distanceAfterICP
            << std::endl;
  std::cout << "  Best distance:                          " << bestDistance
            << std::endl;

  // Select the source to use.
  vtkNew<vtkDataSetMapper> sourceMapper;
  if (bestDistance == distanceBeforeAlign)
  {
    sourceMapper->SetInputData(originalSourcePolyData);
    std::cout << "Using original alignment" << std::endl;
  }
  else if (bestDistance == distanceAfterAlign)
  {
    sourceMapper->SetInputData(sourcePolyData);
    std::cout << "Using alignment by OBB" << std::endl;
  }
  else
  {
    sourceMapper->SetInputConnection(transform->GetOutputPort());
    std::cout << "Using alignment by ICP" << std::endl;
  }
  sourceMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> sourceActor;
  sourceActor->SetMapper(sourceMapper);
  sourceActor->GetProperty()->SetOpacity(0.6);
  sourceActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("White").GetData());
  renderer->AddActor(sourceActor);

  vtkNew<vtkDataSetMapper> targetMapper;
  targetMapper->SetInputData(tpd->GetOutput());
  targetMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> targetActor;
  targetActor->SetMapper(targetMapper);
  targetActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Tomato").GetData());
  renderer->AddActor(targetActor);

  renderWindow->AddRenderer(renderer);

  renderWindow->Render();
  renderWindow->SetWindowName("AlignTwoPolyDatas");

#if VTK_HAS_COW
  vtkNew<vtkCameraOrientationWidget> camOrientManipulator;
  camOrientManipulator->SetParentRenderer(renderer);
  // Enable the widget.
  camOrientManipulator->On();
#else
  vtkNew<vtkAxesActor> axes;

  vtkNew<vtkOrientationMarkerWidget> widget;
  double rgba[4]{0.0, 0.0, 0.0, 0.0};
  colors->GetColor("Carrot", rgba);
  widget->SetOutlineColor(rgba[0], rgba[1], rgba[2]);
  widget->SetOrientationMarker(axes);
  widget->SetInteractor(interactor);
  widget->SetViewport(0.0, 0.0, 0.2, 0.2);
  widget->EnabledOn();
  widget->InteractiveOn();
#endif

  interactor->Start();

  return EXIT_SUCCESS;
}
namespace {

std::string ShowUsage(std::string fn)
{
  // Remove the folder (if present) then remove the extension in this order
  // since the folder name may contain periods.
  auto last_slash_idx = fn.find_last_of("\\/");
  if (std::string::npos != last_slash_idx)
  {
    fn.erase(0, last_slash_idx + 1);
  }
  auto period_idx = fn.rfind('.');
  if (std::string::npos != period_idx)
  {
    fn.erase(period_idx);
  }
  std::ostringstream os;
  os << "\nusage: " << fn << " src_fn tgt_fn\n\n"
     << "How to align two vtkPolyData's.\n\n"
     << "positional arguments:\n"
     << "  src_fn      The polydata source file name,e.g. "
        "Grey_Nurse_Shark.stl.\n"
     << "  tgt_fn      The polydata target file name, e.g. shark.ply.\n"
     << "\n"
     << std::endl;
  return os.str();
}

vtkSmartPointer<vtkPolyData> ReadPolyData(std::string const& fileName)
{
  vtkSmartPointer<vtkPolyData> polyData;
  std::string extension = "";
  if (fileName.find_last_of(".") != std::string::npos)
  {
    extension = fileName.substr(fileName.find_last_of("."));
  }
  // Make the extension lowercase
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 ::tolower);
  if (extension == ".ply")
  {
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtp")
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".obj")
  {
    vtkNew<vtkOBJReader> reader;
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".stl")
  {
    vtkNew<vtkSTLReader> reader;
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtk")
  {
    vtkNew<vtkPolyDataReader> reader;
    reader->SetFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".g")
  {
    vtkNew<vtkBYUReader> reader;
    reader->SetGeometryFileName(fileName.c_str());
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    // Return a polydata sphere if the extension is unknown.
    vtkNew<vtkSphereSource> source;
    source->SetThetaResolution(20);
    source->SetPhiResolution(11);
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}

void AlignBoundingBoxes(vtkPolyData* source, vtkPolyData* target)
{
  // Use OBBTree to create an oriented bounding box for target and source
  vtkNew<vtkOBBTree> sourceOBBTree;
  sourceOBBTree->SetDataSet(source);
  sourceOBBTree->SetMaxLevel(1);
  sourceOBBTree->BuildLocator();

  vtkNew<vtkOBBTree> targetOBBTree;
  targetOBBTree->SetDataSet(target);
  targetOBBTree->SetMaxLevel(1);
  targetOBBTree->BuildLocator();

  vtkNew<vtkPolyData> sourceLandmarks;
  sourceOBBTree->GenerateRepresentation(0, sourceLandmarks);

  vtkNew<vtkPolyData> targetLandmarks;
  targetOBBTree->GenerateRepresentation(0, targetLandmarks);

  vtkNew<vtkLandmarkTransform> lmTransform;
  lmTransform->SetModeToSimilarity();
  lmTransform->SetTargetLandmarks(targetLandmarks->GetPoints());
  // vtkNew<vtkTransformPolyDataFilter> lmTransformPD;
  double bestDistance = VTK_DOUBLE_MAX;
  vtkNew<vtkPoints> bestPoints;
  BestBoundingBox("X", target, source, targetLandmarks, sourceLandmarks,
                  bestDistance, bestPoints);
  BestBoundingBox("Y", target, source, targetLandmarks, sourceLandmarks,
                  bestDistance, bestPoints);
  BestBoundingBox("Z", target, source, targetLandmarks, sourceLandmarks,
                  bestDistance, bestPoints);

  lmTransform->SetSourceLandmarks(bestPoints);
  lmTransform->Modified();

  vtkNew<vtkTransformPolyDataFilter> transformPD;
  transformPD->SetInputData(source);
  transformPD->SetTransform(lmTransform);
  transformPD->Update();

  source->DeepCopy(transformPD->GetOutput());
}
void BestBoundingBox(std::string const& axis, vtkPolyData* target,
                     vtkPolyData* source, vtkPolyData* targetLandmarks,
                     vtkPolyData* sourceLandmarks, double& bestDistance,
                     vtkPoints* bestPoints)
{
  vtkNew<vtkHausdorffDistancePointSetFilter> distance;
  vtkNew<vtkTransform> testTransform;
  vtkNew<vtkTransformPolyDataFilter> testTransformPD;
  vtkNew<vtkLandmarkTransform> lmTransform;
  vtkNew<vtkTransformPolyDataFilter> lmTransformPD;

  lmTransform->SetModeToSimilarity();
  lmTransform->SetTargetLandmarks(targetLandmarks->GetPoints());

  double sourceCenter[3];
  sourceLandmarks->GetCenter(sourceCenter);

  auto delta = 90.0;
  for (auto i = 0; i < 4; ++i)
  {
    auto angle = delta * i;
    // Rotate about center
    testTransform->Identity();
    testTransform->Translate(sourceCenter[0], sourceCenter[1], sourceCenter[2]);
    if (axis == "X")
    {
      testTransform->RotateX(angle);
    }
    else if (axis == "Y")
    {
      testTransform->RotateY(angle);
    }
    else
    {
      testTransform->RotateZ(angle);
    }
    testTransform->Translate(-sourceCenter[0], -sourceCenter[1],
                             -sourceCenter[2]);

    testTransformPD->SetTransform(testTransform);
    testTransformPD->SetInputData(sourceLandmarks);
    testTransformPD->Update();

    lmTransform->SetSourceLandmarks(testTransformPD->GetOutput()->GetPoints());
    lmTransform->Modified();

    lmTransformPD->SetInputData(source);
    lmTransformPD->SetTransform(lmTransform);
    lmTransformPD->Update();

    distance->SetInputData(0, target);
    distance->SetInputData(1, lmTransformPD->GetOutput());
    distance->Update();

    double testDistance = static_cast<vtkPointSet*>(distance->GetOutput(0))
                              ->GetFieldData()
                              ->GetArray("HausdorffDistance")
                              ->GetComponent(0, 0);
    if (testDistance < bestDistance)
    {
      bestDistance = testDistance;
      bestPoints->DeepCopy(testTransformPD->GetOutput()->GetPoints());
    }
  }
  return;
}

} // namespace
