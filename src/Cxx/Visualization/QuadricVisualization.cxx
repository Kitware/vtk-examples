#include <vtkActor.h>
#include <vtkAppendFilter.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractVOI.h>
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
#include <vtkUnstructuredGrid.h>

namespace {
void CreateIsosurface(vtkSampleFunction* function, vtkActor* actor,
                      unsigned int numberOfContours = 5);

void CreatePlanes(vtkSampleFunction* function, vtkActor* actor,
                  unsigned numberOfPlanes);

void CreateContours(vtkSampleFunction* function, vtkActor* actor,
                    unsigned numberOfPlanes, unsigned numberOfContours);

void CreateOutline(vtkSampleFunction* function, vtkActor* actor);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);
  renderWindow->SetSize(640, 480);

  //
  // Create surface of implicit function
  //
  // double range[2];

  // Sample quadric function
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(1, 2, 3, 0, 1, 0, 0, 0, 0, 0);

  vtkNew<vtkSampleFunction> sample;
  sample->SetSampleDimensions(25, 25, 25);
  sample->SetImplicitFunction(quadric);

  vtkNew<vtkActor> isoActor;
  CreateIsosurface(sample, isoActor);
  vtkNew<vtkActor> outlineIsoActor;
  CreateOutline(sample, outlineIsoActor);

  vtkNew<vtkActor> planesActor;
  CreatePlanes(sample, planesActor, 3);
  vtkNew<vtkActor> outlinePlanesActor;
  CreateOutline(sample, outlinePlanesActor);
  planesActor->AddPosition(isoActor->GetBounds()[0] * 2.0, 0, 0);
  outlinePlanesActor->AddPosition(isoActor->GetBounds()[0] * 2.0, 0, 0);

  vtkNew<vtkActor> contourActor;
  CreateContours(sample, contourActor, 3, 15);
  vtkNew<vtkActor> outlineContourActor;
  CreateOutline(sample, outlineContourActor);
  contourActor->AddPosition(isoActor->GetBounds()[0] * 4.0, 0, 0);
  outlineContourActor->AddPosition(isoActor->GetBounds()[0] * 4.0, 0, 0);

  renderer->AddActor(planesActor);
  renderer->AddActor(outlinePlanesActor);
  renderer->AddActor(contourActor);
  renderer->AddActor(outlineContourActor);
  renderer->AddActor(isoActor);
  renderer->AddActor(outlineIsoActor);

  renderer->TwoSidedLightingOn();

  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Try to set camera to match figure on book
  renderer->GetActiveCamera()->SetPosition(0, -1, 0);
  renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
  renderer->GetActiveCamera()->SetViewUp(0, 0, -1);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Elevation(20);
  renderer->GetActiveCamera()->Azimuth(10);
  renderer->GetActiveCamera()->Dolly(1.2);
  renderer->ResetCameraClippingRange();

  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("QuadricVisualization");

  renderWindow->Render();

  // interact with data
  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {
void CreateIsosurface(vtkSampleFunction* function, vtkActor* actor,
                      unsigned int numberOfContours)
{
  double range[2];
  // Generate implicit surface
  vtkNew<vtkContourFilter> contour;
  contour->SetInputConnection(function->GetOutputPort());
  range[0] = 1.0;
  range[1] = 6.0;
  contour->GenerateValues(numberOfContours, range);

  // Map contour
  vtkNew<vtkPolyDataMapper> contourMapper;
  contourMapper->SetInputConnection(contour->GetOutputPort());
  contourMapper->SetScalarRange(0, 7);

  actor->SetMapper(contourMapper);
  return;
}

void CreatePlanes(vtkSampleFunction* function, vtkActor* actor,
                  unsigned int numberOfPlanes)
{
  //
  // Extract planes from implicit function
  //

  vtkNew<vtkAppendFilter> append;

  int dims[3];
  function->GetSampleDimensions(dims);
  int sliceIncr = (dims[2] - 1) / (numberOfPlanes + 1);

  int slice = -4;
  for (unsigned int i = 0; i < numberOfPlanes; ++i)
  {
    vtkNew<vtkExtractVOI> extract;
    extract->SetInputConnection(function->GetOutputPort());
    extract->SetVOI(0, dims[0] - 1, 0, dims[1] - 1, slice + sliceIncr,
                    slice + sliceIncr);
    append->AddInputConnection(extract->GetOutputPort());
    slice += sliceIncr;
  }
  append->Update();

  // Map planes
  vtkNew<vtkDataSetMapper> planesMapper;
  planesMapper->SetInputConnection(append->GetOutputPort());
  planesMapper->SetScalarRange(0, 7);

  actor->SetMapper(planesMapper);
  actor->GetProperty()->SetAmbient(1.);
  return;
}

void CreateContours(vtkSampleFunction* function, vtkActor* actor,
                    unsigned int numberOfPlanes, unsigned int numberOfContours)
{
  //
  // Extract planes from implicit function
  //

  vtkNew<vtkAppendFilter> append;

  int dims[3];
  function->GetSampleDimensions(dims);
  int sliceIncr = (dims[2] - 1) / (numberOfPlanes + 1);

  int slice = -4;
  for (unsigned int i = 0; i < numberOfPlanes; ++i)
  {
    vtkNew<vtkExtractVOI> extract;
    extract->SetInputConnection(function->GetOutputPort());
    extract->SetVOI(0, dims[0] - 1, 0, dims[1] - 1, slice + sliceIncr,
                    slice + sliceIncr);
    double range[2];
    range[0] = 1.0;
    range[1] = 6.0;
    vtkNew<vtkContourFilter> contour;
    contour->SetInputConnection(extract->GetOutputPort());
    contour->GenerateValues(numberOfContours, range);
    append->AddInputConnection(contour->GetOutputPort());
    slice += sliceIncr;
  }
  append->Update();

  // Map planes
  vtkNew<vtkDataSetMapper> planesMapper;
  planesMapper->SetInputConnection(append->GetOutputPort());
  planesMapper->SetScalarRange(0, 7);

  actor->SetMapper(planesMapper);
  actor->GetProperty()->SetAmbient(1.);
  return;
}

void CreateOutline(vtkSampleFunction* source, vtkActor* actor)
{
  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(source->GetOutputPort());
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(outline->GetOutputPort());
  actor->SetMapper(mapper);
  return;
}
} // namespace
