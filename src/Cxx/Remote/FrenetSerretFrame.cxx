#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkFrenetSerretFrame.h>
#include <vtkGlyph3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
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
  vtkNew<vtkNamedColors> colors;

  // Generate  some random points
  int numberOfPoints = 8;
  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(numberOfPoints);
  pointSource->Update();

  vtkPoints* points = pointSource->GetOutput()->GetPoints();

  vtkNew<vtkParametricSpline> spline;
  spline->SetPoints(points);

  vtkNew<vtkParametricFunctionSource> functionSource;
  functionSource->SetParametricFunction(spline);
  functionSource->SetUResolution(10 * numberOfPoints);
  functionSource->SetVResolution(10 * numberOfPoints);
  functionSource->SetWResolution(10 * numberOfPoints);

  // Create the frame
  vtkNew<vtkFrenetSerretFrame> frame;
  frame->SetInputConnection(functionSource->GetOutputPort());
  frame->ConsistentNormalsOn();
  frame->Update();

  vtkNew<vtkGlyph3D> glyph3DNormals;
  vtkNew<vtkGlyph3D> glyph3DTangents;
  vtkNew<vtkGlyph3D> glyph3DBinormals;

  // for each vector, create a Glyph3D and DeepCopy the output
  double radius = 0.05;
  frame->GetOutput()->GetPointData()->SetActiveVectors("FSNormals");
  MakeGlyphs(frame->GetOutput(), radius, glyph3DNormals.GetPointer());
  vtkNew<vtkPolyData> normalsPolyData;
  normalsPolyData->DeepCopy(glyph3DNormals->GetOutput());

  frame->GetOutput()->GetPointData()->SetActiveVectors("FSTangents");
  MakeGlyphs(frame->GetOutput(), radius, glyph3DTangents.GetPointer());
  vtkNew<vtkPolyData> tangentsPolyData;
  tangentsPolyData->DeepCopy(glyph3DTangents->GetOutput());

  frame->GetOutput()->GetPointData()->SetActiveVectors("FSBinormals");
  MakeGlyphs(frame->GetOutput(), radius, glyph3DBinormals.GetPointer());
  vtkNew<vtkPolyData> binormalsPolyData;
  binormalsPolyData->DeepCopy(glyph3DBinormals->GetOutput());

  // Setup actors and mappers
  vtkNew<vtkPolyDataMapper> glyph3DNormalsMapper;
  glyph3DNormalsMapper->SetInputData(normalsPolyData);

  vtkNew<vtkActor> glyph3DNormalsActor;
  glyph3DNormalsActor->SetMapper(glyph3DNormalsMapper);
  glyph3DNormalsActor->GetProperty()->SetColor(
      colors->GetColor3d("Banana").GetData());

  vtkNew<vtkPolyDataMapper> glyph3DTangentsMapper;
  glyph3DTangentsMapper->SetInputData(tangentsPolyData);

  vtkNew<vtkActor> glyph3DTangentsActor;
  glyph3DTangentsActor->SetMapper(glyph3DTangentsMapper);
  glyph3DTangentsActor->GetProperty()->SetColor(
      colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkPolyDataMapper> glyph3DBinormalsMapper;
  glyph3DBinormalsMapper->SetInputData(binormalsPolyData);

  vtkNew<vtkActor> glyph3DBinormalsActor;
  glyph3DBinormalsActor->SetMapper(glyph3DBinormalsMapper);
  glyph3DBinormalsActor->GetProperty()->SetColor(
      colors->GetColor3d("ForestGreen").GetData());

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(functionSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Setup render window, renderer, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("FrenetSerretFrame");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(glyph3DNormalsActor);
  renderer->AddActor(glyph3DTangentsActor);
  renderer->AddActor(glyph3DBinormalsActor);
  renderer->AddActor(actor);

  renderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  // Pick a good view
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(120);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.8);
  renderer->ResetCameraClippingRange();

  renderWindow->SetSize(640, 480);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
void MakeGlyphs(vtkPolyData* src, double size, vtkGlyph3D* glyph)
{
  // Source for the glyph filter
  vtkNew<vtkArrowSource> arrow;
  arrow->SetTipResolution(16);
  arrow->SetTipLength(.3);
  arrow->SetTipRadius(.1);

  glyph->SetSourceConnection(arrow->GetOutputPort());
  glyph->SetInputData(src);
  glyph->SetVectorModeToUseVector();
  glyph->SetScaleModeToScaleByVector();
  glyph->SetScaleFactor(size);
  glyph->OrientOn();
  glyph->Update();
}
} // namespace
