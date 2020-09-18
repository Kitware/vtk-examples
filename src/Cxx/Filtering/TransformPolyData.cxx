#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create the polydata geometry

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  // Set up the actor to display the untransformed polydata

  vtkNew<vtkPolyDataMapper> originalMapper;
  originalMapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetColor(colors->GetColor3d("Blue").GetData());

  // Set up the transform filter

  vtkNew<vtkTransform> translation;
  translation->Translate(1.0, 2.0, 3.0);

  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetInputConnection(sphereSource->GetOutputPort());
  transformFilter->SetTransform(translation);
  transformFilter->Update();

  // Set up the actor to display the transformed polydata

  vtkNew<vtkPolyDataMapper> transformedMapper;
  transformedMapper->SetInputConnection(transformFilter->GetOutputPort());

  vtkNew<vtkActor> transformedActor;
  transformedActor->SetMapper(transformedMapper);
  transformedActor->GetProperty()->SetColor(
      colors->GetColor3d("Red").GetData());

  // Set up the rest of the visualization pipeline

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(originalActor);
  renderer->AddActor(transformedActor);
  renderer->SetBackground(colors->GetColor3d("Green").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->SetWindowName("TransformPolyData");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
