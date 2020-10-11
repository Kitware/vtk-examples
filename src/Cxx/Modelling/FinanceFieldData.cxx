#include <vtkActor.h>
#include <vtkAxes.h>
#include <vtkCamera.h>
#include <vtkDataObjectReader.h>
#include <vtkDataObjectToDataSetFilter.h>
#include <vtkFieldDataToAttributeDataFilter.h>
#include <vtkFollower.h>
#include <vtkGaussianSplatter.h>
#include <vtkImageData.h>
#include <vtkMarchingContourFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTubeFilter.h>
#include <vtkVectorText.h>
#include <vtkNamedColors.h>

int main (int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " financial.vtk" << std::endl;
    return EXIT_FAILURE;
  }
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkDataObjectReader> reader;
  reader->SetFileName(argv[1]);

  int size = 3187; // maximum number possible

  std::string xAxis = "INTEREST_RATE";
  std::string yAxis = "MONTHLY_PAYMENT";
  std::string zAxis = "MONTHLY_INCOME";
  std::string scalar = "TIME_LATE";

  // extract data from field as a polydata (just points), then extract scalars
  vtkNew<vtkDataObjectToDataSetFilter> do2ds;
  do2ds->SetInputConnection(reader->GetOutputPort());
  do2ds->SetDataSetTypeToPolyData();
  // format: component//, arrayname, arraycomp, minArrayId, maxArrayId,
  // normalize
  do2ds->DefaultNormalizeOn();
  do2ds->SetPointComponent(0, const_cast<char*>(xAxis.c_str()), 0);
  do2ds->SetPointComponent(1, const_cast<char*>(yAxis.c_str()), 0, 0, size, 1);
  do2ds->SetPointComponent(2, const_cast<char*>(zAxis.c_str()), 0);
  do2ds->Update();
  vtkNew<vtkFieldDataToAttributeDataFilter> fd2ad;
  fd2ad->SetInputConnection(do2ds->GetOutputPort());
  fd2ad->SetInputFieldToDataObjectField();
  fd2ad->SetOutputAttributeDataToPointData();
  fd2ad->DefaultNormalizeOn();
  fd2ad->SetScalarComponent(0, scalar.c_str(), 0);

  // construct pipeline for original population
  vtkNew<vtkGaussianSplatter> popSplatter;
  popSplatter->SetInputConnection(fd2ad->GetOutputPort());
  popSplatter->SetSampleDimensions(150, 150, 150);
  popSplatter->SetRadius(0.05);
  popSplatter->ScalarWarpingOff();

  vtkNew<vtkMarchingContourFilter> popSurface;
  popSurface->SetInputConnection(popSplatter->GetOutputPort());
  popSurface->SetValue(0, 0.01);
  vtkNew<vtkPolyDataMapper> popMapper;
  popMapper->SetInputConnection(popSurface->GetOutputPort());
  popMapper->ScalarVisibilityOff();
  vtkNew<vtkActor> popActor;
  popActor->SetMapper(popMapper);
  popActor->GetProperty()->SetOpacity(0.3);
  popActor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  // construct pipeline for delinquent population
  vtkNew<vtkGaussianSplatter> lateSplatter;
  lateSplatter->SetInputConnection(fd2ad->GetOutputPort());
  lateSplatter->SetSampleDimensions(150, 150, 150);
  lateSplatter->SetRadius(0.05);
  lateSplatter->SetScaleFactor(0.05);

  vtkNew<vtkMarchingContourFilter> lateSurface;
  lateSurface->SetInputConnection(lateSplatter->GetOutputPort());
  lateSurface->SetValue(0, 0.01);
  vtkNew<vtkPolyDataMapper> lateMapper;
  lateMapper->SetInputConnection(lateSurface->GetOutputPort());
  lateMapper->ScalarVisibilityOff();
  vtkNew<vtkActor> lateActor;
  lateActor->SetMapper(lateMapper);
  lateActor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

  // create axes
  popSplatter->Update();
  double* bounds;
  bounds = popSplatter->GetOutput()->GetBounds();
  vtkNew<vtkAxes> axes;
  axes->SetOrigin(bounds[0], bounds[2], bounds[4]);
  axes->SetScaleFactor(popSplatter->GetOutput()->GetLength() / 5.0);
  vtkNew<vtkTubeFilter> axesTubes;
  axesTubes->SetInputConnection(axes->GetOutputPort());
  axesTubes->SetRadius(axes->GetScaleFactor() / 25.0);
  axesTubes->SetNumberOfSides(6);
  vtkNew<vtkPolyDataMapper> axesMapper;
  axesMapper->SetInputConnection(axesTubes->GetOutputPort());
  vtkNew<vtkActor> axesActor;
  axesActor->SetMapper(axesMapper);

  // label the axes
  vtkNew<vtkVectorText> XText;
  XText->SetText(const_cast<char*>(xAxis.c_str()));
  vtkNew<vtkPolyDataMapper> XTextMapper;
  XTextMapper->SetInputConnection(XText->GetOutputPort());

  vtkNew<vtkFollower> XActor;
  XActor->SetMapper(XTextMapper);
  XActor->SetScale(0.02, .02, .02);
  XActor->SetPosition(0.35, -0.05, -0.05);
  XActor->GetProperty()->SetColor(0, 0, 0);

  vtkNew<vtkVectorText> YText;
  YText->SetText(const_cast<char*>(yAxis.c_str()));

  vtkNew<vtkPolyDataMapper> YTextMapper;
  YTextMapper->SetInputConnection(YText->GetOutputPort());
  vtkNew<vtkFollower> YActor;
  YActor->SetMapper(YTextMapper);
  YActor->SetScale(0.02, .02, .02);
  YActor->SetPosition(-0.05, 0.35, -0.05);
  YActor->GetProperty()->SetColor(0, 0, 0);

  vtkNew<vtkVectorText> ZText;
  ZText->SetText(const_cast<char*>(zAxis.c_str()));
  vtkNew<vtkPolyDataMapper> ZTextMapper;
  ZTextMapper->SetInputConnection(ZText->GetOutputPort());
  vtkNew<vtkFollower> ZActor;
  ZActor->SetMapper(ZTextMapper);
  ZActor->SetScale(0.02, .02, .02);
  ZActor->SetPosition(-0.05, -0.05, 0.35);
  ZActor->GetProperty()->SetColor(0, 0, 0);

  // Graphics stuff
  //
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("FinanceFieldData");

  // Add the actors to the renderer, set the background and size
  //
  renderer->AddActor(axesActor);
  renderer->AddActor(lateActor);
  renderer->AddActor(XActor);
  renderer->AddActor(YActor);
  renderer->AddActor(ZActor);
  renderer->AddActor(popActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderWindow->SetSize(650, 480);

  vtkNew<vtkCamera> camera;
  camera->SetClippingRange(.274, 13.72);
  camera->SetFocalPoint(0.433816, 0.333131, 0.449);
  camera->SetPosition(-1.96987, 1.15145, 1.49053);
  camera->SetViewUp(0.378927, 0.911821, 0.158107);
  renderer->SetActiveCamera(camera);
  XActor->SetCamera(camera);
  YActor->SetCamera(camera);
  ZActor->SetCamera(camera);

  // render and interact with data

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
