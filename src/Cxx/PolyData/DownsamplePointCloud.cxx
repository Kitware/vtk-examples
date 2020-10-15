#include <vtkActor.h>
#include <vtkCleanPolyData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(1000);
  pointSource->SetRadius(1.0);
  pointSource->Update();
  auto pts = pointSource->GetNumberOfPoints();

  vtkNew<vtkCleanPolyData> cleanPolyData;
  cleanPolyData->SetInputConnection(pointSource->GetOutputPort());
  cleanPolyData->SetTolerance(0.1);
  cleanPolyData->Update();
  auto cleanPts = cleanPolyData->GetOutput()->GetNumberOfPoints();

  std::cout << "Original points" << pts << std::endl;
  std::cout << "Cleaned points " << cleanPts << std::endl;
  std::cout << "Reduction      "
            << (1.0 - static_cast<double>(cleanPts) / pts) * 100.0 << "%"
            << std::endl;

  vtkNew<vtkPolyDataMapper> inputMapper;
  inputMapper->SetInputConnection(pointSource->GetOutputPort());
  vtkNew<vtkActor> inputActor;
  inputActor->SetMapper(inputMapper);
  inputActor->GetProperty()->SetColor(colors->GetColor3d("Lime").GetData());
  inputActor->GetProperty()->SetPointSize(3);

  vtkNew<vtkPolyDataMapper> cleanedMapper;
  cleanedMapper->SetInputConnection(cleanPolyData->GetOutputPort());
  vtkNew<vtkActor> cleanedActor;
  cleanedActor->SetMapper(cleanedMapper);
  cleanedActor->GetProperty()->SetColor(colors->GetColor3d("Lime").GetData());
  cleanedActor->GetProperty()->SetPointSize(3);

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("DownsamplePointCloud");

  // And one interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup both renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("van_dyke_brown").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("ultramarine").GetData());

  leftRenderer->AddActor(inputActor);
  rightRenderer->AddActor(cleanedActor);

  leftRenderer->ResetCamera();
  rightRenderer->ResetCamera();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
