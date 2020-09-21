#include <vtkDataSetAttributes.h>
#include <vtkGeoAssignCoordinates.h>
#include <vtkDoubleArray.h>
#include <vtkGraphMapper.h>
#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkMutableDirectedGraph.h>

int main(int, char* [])
{

  vtkNew<vtkMutableDirectedGraph> g;

  vtkNew<vtkDoubleArray> latitude;
  latitude->SetName("latitude");

  vtkNew<vtkDoubleArray> longitude;
  longitude->SetName("longitude");

  for(int lat = -90; lat <= 90; lat += 10)
  {
    for(int lon = -180; lon <= 180; lon += 20)
    {
      g->AddVertex();
      latitude->InsertNextValue(lat);
      longitude->InsertNextValue(lon);
    }
  }

  g->GetVertexData()->AddArray(latitude);
  g->GetVertexData()->AddArray(longitude);

  vtkNew<vtkGeoAssignCoordinates> assign;
  assign->SetInputData(g);
  assign->SetLatitudeArrayName("latitude");
  assign->SetLongitudeArrayName("longitude");
  assign->SetGlobeRadius(1.0);
  assign->Update();

  vtkNew<vtkGraphMapper> mapper;
  mapper->SetInputConnection(assign->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> ren;
  ren->AddActor(actor);

  vtkNew<vtkRenderWindowInteractor> iren;

  vtkNew<vtkRenderWindow> win;
  win->AddRenderer(ren);
  win->SetInteractor(iren);
  ren->ResetCamera();

  win->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
