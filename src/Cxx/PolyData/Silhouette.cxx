#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataSilhouette.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCleanPolyData.h>
#include <vtkNamedColors.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  vtkSmartPointer<vtkPolyData> polyData;
  if (argc < 2)
  {
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    polyData = sphereSource->GetOutput();
  }
  else
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(argv[1]);

    vtkNew<vtkCleanPolyData> clean;
    clean->SetInputConnection(reader->GetOutputPort());
    clean->Update();

    polyData = clean->GetOutput();
  }

  vtkNew<vtkNamedColors> colors;

  // create mapper and actor for original model
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polyData);
  mapper->ScalarVisibilityOff();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetInterpolationToFlat();
  actor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());

  // create renderer and renderWindow
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  renderer->AddActor(actor); // view the original model

  // Compute the silhouette
  vtkNew<vtkPolyDataSilhouette> silhouette;
  silhouette->SetInputData(polyData);
  silhouette->SetCamera(renderer->GetActiveCamera());
  silhouette->SetEnableFeatureAngle(0);

  // create mapper and actor for silouette
  vtkNew<vtkPolyDataMapper> mapper2;
  mapper2->SetInputConnection(silhouette->GetOutputPort());

  vtkNew<vtkActor> actor2;
  actor2->SetMapper(mapper2);
  actor2->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());
  actor2->GetProperty()->SetLineWidth(5);

  renderer->AddActor(actor2);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.5);

  renderer->ResetCameraClippingRange();

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renderWindow);

  // render and interact
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("Silhouette");

  renderWindow->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
