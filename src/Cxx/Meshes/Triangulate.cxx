#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTriangleFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRegularPolygonSource> polygonSource;
  polygonSource->Update();

  vtkNew<vtkTriangleFilter> triangleFilter;
  triangleFilter->SetInputConnection(polygonSource->GetOutputPort());
  triangleFilter->Update();

  vtkNew<vtkPolyDataMapper> inputMapper;
  inputMapper->SetInputConnection(polygonSource->GetOutputPort());
  vtkNew<vtkActor> inputActor;
  inputActor->SetMapper(inputMapper);
  inputActor->GetProperty()->SetRepresentationToWireframe();
  inputActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  vtkNew<vtkPolyDataMapper> triangleMapper;
  triangleMapper->SetInputConnection(triangleFilter->GetOutputPort());
  vtkNew<vtkActor> triangleActor;
  triangleActor->SetMapper(triangleMapper);
  triangleActor->GetProperty()->SetRepresentationToWireframe();
  triangleActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("Triangulate");

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
  leftRenderer->SetBackground(colors->GetColor3d("SaddleBrown").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  leftRenderer->AddActor(inputActor);
  rightRenderer->AddActor(triangleActor);

  leftRenderer->ResetCamera();

  rightRenderer->ResetCamera();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
