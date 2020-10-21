#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkConeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransformPolyDataFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a rendering window and renderer
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren);
  renWin->SetSize(640, 480);
  renWin->SetWindowName("RotationAroundLine");

  // Create a renderwindowinteractor
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Create arrow
  vtkNew<vtkArrowSource> source;

  // Create a transform that rotates the arrow 45° around the z-axis
  vtkNew<vtkTransform> transform;
  transform->RotateWXYZ(45, 0, 0, 1);
  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetTransform(transform);
  transformFilter->SetInputConnection(source->GetOutputPort());
  transformFilter->Update();

  // Mapper for the original arrow
  vtkNew<vtkPolyDataMapper> coneMapper1;
  coneMapper1->SetInputConnection(source->GetOutputPort());

  // Another mapper for the rotated arrow
  vtkNew<vtkPolyDataMapper> coneMapper2;
  coneMapper2->SetInputConnection(transformFilter->GetOutputPort());

  // Actor for original arrow
  vtkNew<vtkActor> actor1;
  actor1->SetMapper(coneMapper1);

  // Actor for rotated arrow
  vtkNew<vtkActor> actor2;
  actor2->SetMapper(coneMapper2);

  // Color the original arrow
  actor1->GetProperty()->SetColor(colors->GetColor3d("LightCoral").GetData());
  // Color rotated arrow
  actor2->GetProperty()->SetColor(
      colors->GetColor3d("PaleTurquoise").GetData());

  // Assign actor to the renderer
  ren->AddActor(actor1);
  ren->AddActor(actor2);
  ren->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Enable the user interface interactor
  iren->Initialize();
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
