#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkGlyph3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> namedColors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(10);
  sphereSource->SetPhiResolution(50);
  sphereSource->SetThetaResolution(50);

  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(60);
  pointSource->SetRadius(2);

  vtkNew<vtkSmoothPolyDataFilter> smoothFilter;
  smoothFilter->SetInputConnection(0, sphereSource->GetOutputPort());
  smoothFilter->SetInputConnection(1, pointSource->GetOutputPort());

  vtkNew<vtkSphereSource> glyphSource;
  glyphSource->SetRadius(.04);

  vtkNew<vtkGlyph3D> glyph3D;
  glyph3D->SetInputConnection(pointSource->GetOutputPort());
  glyph3D->SetSourceConnection(glyphSource->GetOutputPort());
  glyph3D->ScalingOff();

  vtkNew<vtkDataSetMapper> glyph3DMapper;
  glyph3DMapper->SetInputConnection(glyph3D->GetOutputPort());
  glyph3DMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> glyph3DActor;
  glyph3DActor->SetMapper(glyph3DMapper);
  glyph3DActor->GetProperty()->SetColor(
      namedColors->GetColor3d("Banana").GetData());

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(smoothFilter->GetOutputPort());
  mapper->ScalarVisibilityOff();

  // Create an actor for the surface
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetDiffuseColor(
      namedColors->GetColor3d("Tomato").GetData());
  actor->GetProperty()->SetEdgeColor(
      namedColors->GetColor3d("IvoryBlack").GetData());
  actor->GetProperty()->SetOpacity(1.0);
  actor->GetProperty()->EdgeVisibilityOff();

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ConvexHullShrinkWrap");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->AddActor(glyph3DActor);
  renderer->SetBackground(namedColors->GetColor3d("SlateGray").GetData());

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
