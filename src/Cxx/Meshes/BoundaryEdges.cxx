#include <vtkActor.h>
#include <vtkDiskSource.h>
#include <vtkFeatureEdges.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>


int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkDiskSource> diskSource;
  diskSource->Update();

  vtkNew<vtkFeatureEdges> featureEdges;
  featureEdges->SetInputConnection(diskSource->GetOutputPort());
  featureEdges->BoundaryEdgesOn();
  featureEdges->FeatureEdgesOff();
  featureEdges->ManifoldEdgesOff();
  featureEdges->NonManifoldEdgesOff();
  featureEdges->ColoringOn();
  featureEdges->Update();

  // Visualize
  vtkNew<vtkPolyDataMapper> edgeMapper;
  edgeMapper->SetInputConnection(featureEdges->GetOutputPort());
  edgeMapper->SetScalarModeToUseCellData();
  vtkNew<vtkActor> edgeActor;
  edgeActor->SetMapper(edgeMapper);

  vtkNew<vtkPolyDataMapper> diskMapper;
  diskMapper->SetInputConnection(diskSource->GetOutputPort());
  vtkNew<vtkActor> diskActor;
  diskActor->SetMapper(diskMapper);
  diskActor->GetProperty()->SetColor(colors->GetColor3d("Gray").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("BoundaryEdges");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(edgeActor);
  renderer->AddActor(diskActor);
  renderer->SetBackground(colors->GetColor3d("DimGray").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
