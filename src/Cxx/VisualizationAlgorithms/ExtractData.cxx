#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractGeometry.h>
#include <vtkImplicitBoolean.h>
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
#include <vtkShrinkFilter.h>
#include <vtkSphere.h>
#include <vtkTransform.h>

int main(int, char*[])
{
  // extract data
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> ren1;

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(0.5, 1, 0.2, 0, 0.1, 0, 0, 0.2, 0, 0);

  vtkNew<vtkSampleFunction> sample;
  sample->SetSampleDimensions(50, 50, 50);
  sample->SetImplicitFunction(quadric);
  sample->ComputeNormalsOff();

  vtkNew<vtkTransform> trans;
  trans->Scale(1, 0.5, 0.333);

  vtkNew<vtkSphere> sphere;
  sphere->SetRadius(0.25);
  sphere->SetTransform(trans);

  vtkNew<vtkTransform> trans2;
  trans2->Scale(0.25, 0.5, 1.0);

  vtkNew<vtkSphere> sphere2;
  sphere2->SetRadius(0.25);
  sphere2->SetTransform(trans2);

  vtkNew<vtkImplicitBoolean> booleanUnion;
  booleanUnion->AddFunction(sphere);
  booleanUnion->AddFunction(sphere2);
  booleanUnion->SetOperationType(0); // boolean Union

  vtkNew<vtkExtractGeometry> extract;
  extract->SetInputConnection(sample->GetOutputPort());
  extract->SetImplicitFunction(booleanUnion);

  vtkNew<vtkShrinkFilter> shrink;
  shrink->SetInputConnection(extract->GetOutputPort());
  shrink->SetShrinkFactor(0.5);

  vtkNew<vtkDataSetMapper> dataMapper;
  dataMapper->SetInputConnection(shrink->GetOutputPort());
  vtkNew<vtkActor> dataActor;
  dataActor->SetMapper(dataMapper);

  // outline
  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(sample->GetOutputPort());

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(0, 0, 0);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(outlineActor);
  ren1->AddActor(dataActor);
  ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renWin->SetSize(640, 480);
  renWin->SetWindowName("ExtractData");
  renWin->Render();

  ren1->GetActiveCamera()->Azimuth(30);
  ren1->GetActiveCamera()->Elevation(30);

  iren->Start();

  return EXIT_SUCCESS;
}
