#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkGlyph3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataWriter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkPolyData> input;
  input->ShallowCopy(sphereSource->GetOutput());

  vtkNew<vtkArrowSource> arrowSource;

  vtkNew<vtkGlyph3D> glyph3D;
  glyph3D->SetSourceConnection(arrowSource->GetOutputPort());
  glyph3D->SetVectorModeToUseNormal();
  glyph3D->SetInputData(input);
  glyph3D->SetScaleFactor(.2);
  glyph3D->Update();

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(glyph3D->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("OrientedGlyphs");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DarkGreen").GetData());

  renderWindow->Render();

  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(-0.399941, -1.070475, 2.931458);
  camera->SetFocalPoint(-0.000000, -0.000000, 0.000000);
  camera->SetViewUp(-0.028450, 0.940195, 0.339448);
  camera->SetDistance(3.146318);
  camera->SetClippingRange(1.182293, 5.626211);

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
