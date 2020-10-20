#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShrinkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  vtkSmartPointer<vtkPolyData> inputPolyData;
  if (argc > 1)
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(argv[1]); // e.g. cowHead.vtp
    reader->Update();
    inputPolyData = reader->GetOutput();
  }
  else
  {
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetPhiResolution(15);
    sphereSource->SetThetaResolution(15);
    sphereSource->Update();
    inputPolyData = sphereSource->GetOutput();
  }

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkShrinkPolyData> shrink;
  shrink->SetShrinkFactor(.8);
  shrink->SetInputData(inputPolyData);

  // Visualize
  vtkNew<vtkRenderer> renderer;

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(shrink->GetOutputPort());
  mapper->ScalarVisibilityOff();

  vtkNew<vtkProperty> back;
  back->SetColor(colors->GetColor3d("Peacock").GetData());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Salmon").GetData());
  actor->SetBackfaceProperty(back);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Burlywood").GetData());
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.5);

  renderer->ResetCameraClippingRange();

  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("ShrinkPolyData");

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
