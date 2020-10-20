#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCleanPolyData.h>
#include <vtkDataSetAttributes.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkWarpVector.h>
#include <vtkXMLPolyDataReader.h>
int main(int argc, char* argv[])
{
  double scale = 1.0;
  vtkSmartPointer<vtkPolyData> inputPolyData;
  // If a file is provided, use it, otherwise generate a sphere
  if (argc > 1)
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(argv[1]); // e.g. cowHead.vtp
    reader->Update();
    inputPolyData = reader->GetOutput();
    if (argc > 2)
    {
      scale = atof(argv[2]);
    }
  }
  else
  {
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetPhiResolution(15);
    sphereSource->SetThetaResolution(15);
    sphereSource->Update();
    inputPolyData = sphereSource->GetOutput();
  }

  vtkNew<vtkCleanPolyData> clean;
  clean->SetInputData(inputPolyData);

  // Generate normals
  vtkNew<vtkPolyDataNormals> normals;
  normals->SetInputConnection(clean->GetOutputPort());
  normals->SplittingOff();

  // Warp using the normals
  vtkNew<vtkWarpVector> warp;
  warp->SetInputConnection(normals->GetOutputPort());
  warp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS,
                               vtkDataSetAttributes::NORMALS);
  warp->SetScaleFactor(scale);

  // Visualize the original and warped models
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(warp->GetOutputPort());
  mapper->ScalarVisibilityOff();

  vtkNew<vtkActor> warpedActor;
  warpedActor->SetMapper(mapper);
  warpedActor->GetProperty()->SetColor(colors->GetColor4d("Flesh").GetData());

  vtkNew<vtkPolyDataMapper> originalMapper;
  originalMapper->SetInputConnection(normals->GetOutputPort());
  originalMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetInterpolationToFlat();
  originalActor->GetProperty()->SetColor(colors->GetColor4d("Flesh").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("WarpSurface");

  // Create a camera for all renderers
  vtkNew<vtkCamera> camera;

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup both renderers
  vtkNew<vtkRenderer> leftRenderer;
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("Burlywood").GetData());
  leftRenderer->SetActiveCamera(camera);

  vtkNew<vtkRenderer> rightRenderer;
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("CornFlower").GetData());
  rightRenderer->SetActiveCamera(camera);

  leftRenderer->AddActor(originalActor);
  rightRenderer->AddActor(warpedActor);

  rightRenderer->ResetCamera();

  renderWindow->AddRenderer(rightRenderer);
  renderWindow->AddRenderer(leftRenderer);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
