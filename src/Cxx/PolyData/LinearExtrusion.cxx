#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTriangleFilter.h>
#include <vtkVectorText.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create vector text
  vtkNew<vtkVectorText> vecText;
  vecText->SetText("Text!");

  // Apply linear extrusion
  vtkNew<vtkLinearExtrusionFilter> extrude;
  extrude->SetInputConnection(vecText->GetOutputPort());
  extrude->SetExtrusionTypeToNormalExtrusion();
  extrude->SetVector(0, 0, 1);
  extrude->SetScaleFactor(0.5);

  vtkNew<vtkTriangleFilter> triangleFilter;
  triangleFilter->SetInputConnection(extrude->GetOutputPort());

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(triangleFilter->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("PaleGoldenrod").GetData());

  vtkNew<vtkRenderWindow> renderWindow;

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("LinearExtrusion");

  renderer->AddActor(actor);

  renderer->ResetCamera();
  // Generate an interesting view
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.0);
  renderer->ResetCameraClippingRange();

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
