#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkLineSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTubeFilter.h>

/*
 * This example creates a tube around a line.
 * This is helpful because when you zoom the camera,
 * the thickness of a line remains constant,
 * while the thickness of a tube varies.
 * */

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;
  // Create a line
  vtkNew<vtkLineSource> lineSource;
  lineSource->SetPoint1(1.0, 0.0, 0.0);
  lineSource->SetPoint2(0.0, 1.0, 0.0);

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> lineMapper;
  lineMapper->SetInputConnection(lineSource->GetOutputPort());
  vtkNew<vtkActor> lineActor;
  lineActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
  lineActor->SetMapper(lineMapper);

  // Create a tube (cylinder) around the line
  vtkNew<vtkTubeFilter> tubeFilter;
  tubeFilter->SetInputConnection(lineSource->GetOutputPort());
  tubeFilter->SetRadius(.025); // default is .5
  tubeFilter->SetNumberOfSides(50);
  tubeFilter->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> tubeMapper;
  tubeMapper->SetInputConnection(tubeFilter->GetOutputPort());
  vtkNew<vtkActor> tubeActor;
  tubeActor->GetProperty()->SetOpacity(
      0.5); // Make the tube have some transparency.
  tubeActor->SetMapper(tubeMapper);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("TubeFilter");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(tubeActor);
  renderer->AddActor(lineActor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
