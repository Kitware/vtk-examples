#include <vtkActor.h>
#include <vtkCutter.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkSmartPointer<vtkPolyData> inputPolyData;
  if (argc > 1)
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();
    inputPolyData = reader->GetOutput();
  }
  else
  {
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetThetaResolution(30);
    sphereSource->SetPhiResolution(15);
    sphereSource->Update();
    inputPolyData = sphereSource->GetOutput();
  }

  vtkNew<vtkPolyDataMapper> inputMapper;
  inputMapper->SetInputData(inputPolyData);

  // Create a plane to cut
  vtkNew<vtkPlane> plane;
  plane->SetOrigin(inputPolyData->GetCenter());
  plane->SetNormal(1, 1, 1);

  double minBound[3];
  minBound[0] = inputPolyData->GetBounds()[0];
  minBound[1] = inputPolyData->GetBounds()[2];
  minBound[2] = inputPolyData->GetBounds()[4];

  double maxBound[3];
  maxBound[0] = inputPolyData->GetBounds()[1];
  maxBound[1] = inputPolyData->GetBounds()[3];
  maxBound[2] = inputPolyData->GetBounds()[5];

  double center[3];
  center[0] = inputPolyData->GetCenter()[0];
  center[1] = inputPolyData->GetCenter()[1];
  center[2] = inputPolyData->GetCenter()[2];

  double distanceMin = sqrt(vtkMath::Distance2BetweenPoints(minBound, center));
  double distanceMax = sqrt(vtkMath::Distance2BetweenPoints(maxBound, center));

  // Create cutter
  vtkNew<vtkCutter> cutter;
  cutter->SetCutFunction(plane);
  cutter->SetInputData(inputPolyData);

  cutter->GenerateValues(20, -distanceMin, distanceMax);
  vtkNew<vtkPolyDataMapper> cutterMapper;
  cutterMapper->SetInputConnection(cutter->GetOutputPort());
  cutterMapper->ScalarVisibilityOff();

  // Create plane actor
  vtkNew<vtkActor> planeActor;
  planeActor->GetProperty()->SetColor(
      colors->GetColor3d("Deep_pink").GetData());
  planeActor->GetProperty()->SetLineWidth(5);
  planeActor->SetMapper(cutterMapper);

  // Create input actor
  vtkNew<vtkActor> inputActor;
  inputActor->GetProperty()->SetColor(colors->GetColor3d("Bisque").GetData());
  inputActor->SetMapper(inputMapper);

  // Create renderers and add actors of plane and cube
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(planeActor); // display the rectangle resulting from the
                                  // cut
  renderer->AddActor(inputActor); // display the cube

  // Add renderer to renderwindow and render
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ContoursFromPolyData");
  renderWindow->SetSize(600, 600);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);
  renderer->SetBackground(colors->GetColor3d("Slate_grey").GetData());
  renderWindow->Render();

  interactor->Start();

  return EXIT_SUCCESS;
}
