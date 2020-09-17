#include <vtkActor.h>
#include <vtkAppendFilter.h>
#include <vtkCamera.h>
#include <vtkConnectivityFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkDelaunay3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource1;
  sphereSource1->Update();

  vtkNew<vtkDelaunay3D> delaunay1;
  delaunay1->SetInputConnection(sphereSource1->GetOutputPort());
  delaunay1->Update();

  vtkNew<vtkSphereSource> sphereSource2;
  sphereSource2->SetCenter(5, 0, 0);
  sphereSource2->Update();

  vtkNew<vtkDelaunay3D> delaunay2;
  delaunay2->SetInputConnection(sphereSource2->GetOutputPort());
  delaunay2->Update();

  vtkNew<vtkAppendFilter> appendFilter;
  appendFilter->AddInputConnection(delaunay1->GetOutputPort());
  appendFilter->AddInputConnection(delaunay2->GetOutputPort());
  appendFilter->Update();

  vtkNew<vtkConnectivityFilter> connectivityFilter;
  connectivityFilter->SetInputConnection(appendFilter->GetOutputPort());
  connectivityFilter->SetExtractionModeToAllRegions();
  connectivityFilter->ColorRegionsOn();
  connectivityFilter->Update();

  // Visualize
  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(connectivityFilter->GetOutputPort());
  mapper->Update();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);

  vtkNew<vtkRenderWindow> renwin;
  renwin->AddRenderer(renderer);
  renwin->Render();

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renwin);

  iren->Initialize();
  renwin->Render();
  renderer->SetBackground(colors->GetColor3d("deep_ochre").GetData());
  renderer->GetActiveCamera()->Zoom(0.9);
  renwin->SetWindowName("ConnectivityFilter");
  renwin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
