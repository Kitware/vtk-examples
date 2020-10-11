#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkCallbackCommand.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DCollection.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkXMLPolyDataWriter.h>

namespace {
void PickCallbackFunction(vtkObject* caller, long unsigned int eventId,
                          void* clientData, void* callData);
}

int main(int, char*[])
{
  // Create a set of points
  vtkNew<vtkPoints> points;
  vtkNew<vtkCellArray> vertices;
  vtkIdType pid[1];
  pid[0] = points->InsertNextPoint(1.0, 0.0, 0.0);
  vertices->InsertNextCell(1, pid);
  pid[0] = points->InsertNextPoint(0.0, 0.0, 0.0);
  vertices->InsertNextCell(1, pid);
  pid[0] = points->InsertNextPoint(0.0, 1.0, 0.0);
  vertices->InsertNextCell(1, pid);

  // Create a polydata
  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);
  polydata->SetVerts(vertices);

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polydata);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetPointSize(8);
  actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("AreaPicking");

  vtkNew<vtkAreaPicker> areaPicker;

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindowInteractor->SetPicker(areaPicker);

  renderer->AddActor(actor);

  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  renderWindow->Render();

  // For vtkInteractorStyleRubberBandPick - use 'r' and left-mouse to draw a
  // selection box used to pick
  vtkNew<vtkInteractorStyleRubberBandPick> style;

  // For vtkInteractorStyleTrackballCamera - use 'p' to pick at the current
  // mouse position
  //  vtkNew<vtkInteractorStyleTrackballCamera> style;
  //    paraview
  style->SetCurrentRenderer(renderer);
  renderWindowInteractor->SetInteractorStyle(style);

  vtkNew<vtkCallbackCommand> pickCallback;
  pickCallback->SetCallback(PickCallbackFunction);

  areaPicker->AddObserver(vtkCommand::EndPickEvent, pickCallback);

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
void PickCallbackFunction(vtkObject* caller,
                          long unsigned int vtkNotUsed(eventId),
                          void* vtkNotUsed(clientData),
                          void* vtkNotUsed(callData))
{
  std::cout << "Pick." << std::endl;
  vtkAreaPicker* areaPicker = static_cast<vtkAreaPicker*>(caller);
  vtkProp3DCollection* props = areaPicker->GetProp3Ds();
  props->InitTraversal();

  for (vtkIdType i = 0; i < props->GetNumberOfItems(); i++)
  {
    vtkProp3D* prop = props->GetNextProp3D();
    std::cout << "Picked prop: " << prop << std::endl;
  }
}
} // namespace
