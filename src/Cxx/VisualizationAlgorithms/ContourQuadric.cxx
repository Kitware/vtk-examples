#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkQuadric.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> aren;

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(aren);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  //
  // Create surfaces F(x,y,z) = constant
  //
  // Sample quadric function
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(.5, 1, .2, 0, .1, 0, 0, .2, 0, 0);
  vtkNew<vtkSampleFunction> sample;
  sample->SetSampleDimensions(50, 50, 50);
  sample->SetImplicitFunction(quadric);

  vtkNew<vtkContourFilter> contour;
  contour->SetInputConnection(sample->GetOutputPort());
  contour->GenerateValues(5, 0, 1.2);

  vtkNew<vtkPolyDataMapper> contourMapper;
  contourMapper->SetInputConnection(contour->GetOutputPort());
  contourMapper->SetScalarRange(0, 1.2);

  vtkNew<vtkActor> contourActor;
  contourActor->SetMapper(contourMapper);

  // Create outline
  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(sample->GetOutputPort());

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(colors->GetColor3d("Brown").GetData());
  outlineActor->GetProperty()->SetLineWidth(3.0);

  //
  // Rendering stuff
  //
  aren->SetBackground(colors->GetColor3d("SlateGray").GetData());
  aren->AddActor(contourActor);
  aren->AddActor(outlineActor);

  aren->ResetCamera();
  aren->GetActiveCamera()->Azimuth(30);
  aren->GetActiveCamera()->Elevation(30);

  renWin->SetSize(640, 512);
  renWin->Render();
  renWin->SetWindowName("ContourQuadric");

  // interact with data
  iren->Start();

  return EXIT_SUCCESS;
}
