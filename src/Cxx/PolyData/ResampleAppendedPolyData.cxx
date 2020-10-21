#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellLocator.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPlatonicSolidSource.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

//#include <random>

int main(int, char*[])
{
  // Define adjustable paramaters
  constexpr double xMin = -10.0;
  constexpr double xMax = 10.0;
  constexpr double yMin = -10.0;
  constexpr double yMax = 10.0;
  constexpr int xResolution = 100;
  constexpr int yResolution = 100;
  constexpr double scaleMin = 0.2;
  constexpr double scaleMax = 1.5;
  constexpr int numberOfObjects = 200;
  constexpr int probeResolution = 200.0;

  // Generate terrain
  vtkNew<vtkPlaneSource> terrain;
  terrain->SetOrigin(xMin, yMin, 0.0);
  terrain->SetPoint2(xMin, yMax, 0.0);
  terrain->SetPoint1(xMax, yMin, 0.0);
  terrain->SetXResolution(xResolution);
  terrain->SetYResolution(yResolution);
  terrain->Update();

  // Generate objects and append them to terrain
  vtkNew<vtkAppendPolyData> append;
  append->AddInputConnection(terrain->GetOutputPort());

  // Commented out since it gives different results for VS and gcc.
  // std::mt19937_64 mt(4355412); // Standard mersenne twister engine
  // std::uniform_int_distribution<> solid(0, 4);
  // std::uniform_real_distribution<> scale(scaleMin, scaleMax);
  // std::uniform_real_distribution<> position(xMin + 1.0, xMax - 1.0);

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(4355412);

  for (auto i = 0; i < numberOfObjects; ++i)
  {
    int solid = static_cast<int>(randomSequence->GetRangeValue(0, 4));
    randomSequence->Next();
    double s = randomSequence->GetRangeValue(scaleMin, scaleMax);
    randomSequence->Next();
    double x, y;
    x = randomSequence->GetRangeValue(xMin + 1.0, xMax - 1.0);
    randomSequence->Next();
    y = randomSequence->GetRangeValue(yMin + 1.0, yMax - 1.0);
    randomSequence->Next();

    // Generate an object
    vtkNew<vtkPlatonicSolidSource> platonic;
    // platonic->SetSolidType(solid(mt));
    platonic->SetSolidType(solid);

    // Translate and scale
    vtkNew<vtkTransform> transform;
    // double s = scale(mt);
    // transform->Translate(position(mt), position(mt), 0.0);
    transform->Translate(x, y, 0.0);
    transform->Scale(s, s, s);
    vtkNew<vtkTransformPolyDataFilter> transformPD;
    transformPD->SetTransform(transform);
    transformPD->SetInputConnection(platonic->GetOutputPort());
    transformPD->Update();

    vtkNew<vtkPolyData> pd;
    pd->DeepCopy(transformPD->GetOutput());
    append->AddInputData(pd);
  }
  append->Update();

  // Resample terrian
  vtkNew<vtkCellLocator> cellLocator;
  cellLocator->SetDataSet(append->GetOutput());
  cellLocator->BuildLocator();

  // Generate a probe plane
  vtkNew<vtkPlaneSource> probeTerrain;
  probeTerrain->SetOrigin(terrain->GetOrigin());
  probeTerrain->SetPoint2(terrain->GetPoint2());
  probeTerrain->SetPoint1(terrain->GetPoint1());
  probeTerrain->SetXResolution(probeResolution);
  probeTerrain->SetYResolution(probeResolution);
  probeTerrain->Update();

  vtkPoints* probePoints = probeTerrain->GetOutput()->GetPoints();
  for (auto i = 0; i < probeTerrain->GetOutput()->GetNumberOfPoints(); ++i)
  {
    int subId;
    double t, xyz[3], pcoords[3];
    double rayStart[3], rayEnd[3];
    probePoints->GetPoint(i, rayStart);
    rayStart[2] += 100000.0;
    probePoints->GetPoint(i, rayEnd);
    rayEnd[2] -= 100000.0;

    if (cellLocator->IntersectWithLine(rayStart, rayEnd, 0.0001, t, xyz,
                                       pcoords, subId))
    {
      // The new elevation is in xyz[2]
      double pt[3];
      probePoints->GetPoint(i, pt);
      probePoints->SetPoint(i, pt[0], pt[1], xyz[2]);
    }
  }
  probeTerrain->GetOutput()->GetPointData()->SetNormals(nullptr);

  // Visualize
  vtkNew<vtkNamedColors> colors;

  // Two viewports, left for original and right for resampled
  vtkNew<vtkPolyDataMapper> originalMapper;
  originalMapper->SetInputConnection(append->GetOutputPort());
  originalMapper->SetInputData(append->GetOutput());
  originalMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetRepresentationToWireframe();
  originalActor->GetProperty()->SetColor(
      colors->GetColor3d("Seashell").GetData());

  vtkNew<vtkPolyDataMapper> resampledMapper;
  resampledMapper->SetInputConnection(append->GetOutputPort());
  resampledMapper->SetInputData(probeTerrain->GetOutput());
  resampledMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> resampledActor;
  resampledActor->SetMapper(resampledMapper);
  resampledActor->GetProperty()->SetRepresentationToWireframe();
  resampledActor->GetProperty()->SetColor(
      colors->GetColor3d("Seashell").GetData());

  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};
  vtkNew<vtkRenderer> rightRenderer;
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->UseHiddenLineRemovalOn();
  rightRenderer->AddActor(resampledActor);
  rightRenderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  vtkNew<vtkRenderer> leftRenderer;
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->UseHiddenLineRemovalOn();
  leftRenderer->AddActor(originalActor);
  leftRenderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(rightRenderer);
  renderWindow->AddRenderer(leftRenderer);
  renderWindow->SetSize(1024, 512);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->SetWindowName("ResampleAppendedPolyData");

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  renderWindowInteractor->SetInteractorStyle(style);

  renderWindow->Render();
  vtkCamera* camera;
  camera = leftRenderer->GetActiveCamera();
  camera->SetFocalPoint(0.0, 0.0, 0.0);
  camera->SetPosition(1.0, 0.0, 0.0);
  camera->SetViewUp(0.0, 0.0, 1.0);
  camera->Azimuth(30.0);
  camera->Elevation(45.0);

  leftRenderer->ResetCamera();
  rightRenderer->SetActiveCamera(camera);
  renderWindow->Render();

  renderWindowInteractor->Start();
  return EXIT_SUCCESS;
}
