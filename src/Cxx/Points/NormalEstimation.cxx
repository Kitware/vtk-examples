#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkGlyph3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPCANormalEstimation.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {
void MakeGlyphs(vtkPolyData* src, double size, vtkGlyph3D* glyph);
}

int main(int, char*[])
{
  double radius = 1.0;
  vtkNew<vtkPointSource> points;
  points->SetNumberOfPoints(1000);
  points->SetRadius(radius);
  points->SetCenter(0.0, 0.0, 0.0);
  points->SetDistributionToShell();

  int sampleSize = 10;
  vtkNew<vtkPCANormalEstimation> normals;
  normals->SetInputConnection(points->GetOutputPort());
  normals->SetSampleSize(sampleSize);
  normals->SetNormalOrientationToGraphTraversal();
  normals->Update();

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkGlyph3D> glyph3D;
  MakeGlyphs(normals->GetOutput(), radius * 0.2, glyph3D.GetPointer());

  vtkNew<vtkPolyDataMapper> glyph3DMapper;
  glyph3DMapper->SetInputConnection(glyph3D->GetOutputPort());

  vtkNew<vtkActor> glyph3DActor;
  glyph3DActor->SetMapper(glyph3DMapper);
  glyph3DActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Banana").GetData());

  vtkNew<vtkSphereSource> sphere;
  sphere->SetRadius(1.0);
  sphere->SetThetaResolution(41);
  sphere->SetPhiResolution(21);

  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphere->GetOutputPort());

  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Tomato").GetData());

  // Create graphics stuff
  //
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("NormalEstimation");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer, set the background and size
  //
  renderer->AddActor(glyph3DActor);
  renderer->AddActor(sphereActor);

  // Generate an interesting view
  //
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(120);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.0);
  renderer->ResetCameraClippingRange();

  renderWindow->Render();
  interactor->Initialize();
  interactor->Start();

  return EXIT_SUCCESS;
}
namespace {
void MakeGlyphs(vtkPolyData* src, double size, vtkGlyph3D* glyph)
{
  // Source for the glyph filter
  vtkNew<vtkArrowSource> arrow;
  arrow->SetTipResolution(16);
  arrow->SetTipLength(0.3);
  arrow->SetTipRadius(0.1);

  glyph->SetSourceConnection(arrow->GetOutputPort());
  glyph->SetInputData(src);
  glyph->SetVectorModeToUseNormal();
  glyph->SetScaleModeToScaleByVector();
  glyph->SetScaleFactor(size);
  glyph->OrientOn();
  glyph->Update();
}
} // namespace
