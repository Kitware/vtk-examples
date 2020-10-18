#include <vtkActor.h>
#include <vtkIntersectionPolyDataFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNew.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource1;
  sphereSource1->SetCenter(0.0, 0.0, 0.0);
  sphereSource1->SetRadius(2.0);
  sphereSource1->Update();
  vtkNew<vtkPolyDataMapper> sphere1Mapper;
  sphere1Mapper->SetInputConnection(sphereSource1->GetOutputPort());
  sphere1Mapper->ScalarVisibilityOff();
  vtkNew<vtkActor> sphere1Actor;
  sphere1Actor->SetMapper(sphere1Mapper);
  sphere1Actor->GetProperty()->SetOpacity(.3);
  sphere1Actor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

  vtkNew<vtkSphereSource> sphereSource2;
  sphereSource2->SetCenter(1.0, 0.0, 0.0);
  sphereSource2->SetRadius(2.0);
  vtkNew<vtkPolyDataMapper> sphere2Mapper;
  sphere2Mapper->SetInputConnection(sphereSource2->GetOutputPort());
  sphere2Mapper->ScalarVisibilityOff();
  vtkNew<vtkActor> sphere2Actor;
  sphere2Actor->SetMapper(sphere2Mapper);
  sphere2Actor->GetProperty()->SetOpacity(.3);
  sphere2Actor->GetProperty()->SetColor(colors->GetColor3d("Lime").GetData());

  vtkNew<vtkIntersectionPolyDataFilter> intersectionPolyDataFilter;
  intersectionPolyDataFilter->SetInputConnection(
      0, sphereSource1->GetOutputPort());
  intersectionPolyDataFilter->SetInputConnection(
      1, sphereSource2->GetOutputPort());
  intersectionPolyDataFilter->Update();

  vtkNew<vtkPolyDataMapper> intersectionMapper;
  intersectionMapper->SetInputConnection(
      intersectionPolyDataFilter->GetOutputPort());
  intersectionMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> intersectionActor;
  intersectionActor->SetMapper(intersectionMapper);

  vtkNew<vtkRenderer> renderer;
  renderer->AddViewProp(sphere1Actor);
  renderer->AddViewProp(sphere2Actor);
  renderer->AddViewProp(intersectionActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("IntersectionPolyDataFilter");

  vtkNew<vtkRenderWindowInteractor> renWinInteractor;
  renWinInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  renWinInteractor->Start();

  return EXIT_SUCCESS;
}
