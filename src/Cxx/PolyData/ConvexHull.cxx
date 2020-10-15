#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkHull.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  // Parse command line arguments
  if (argc != 2)
  {
    std::cout << "Required arguments: Filename e.g. cowHead.vtp" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> namedColors;

  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(argv[1]);

  vtkNew<vtkHull> hullFilter;
  hullFilter->SetInputConnection(reader->GetOutputPort());
  hullFilter->AddCubeFacePlanes();
  hullFilter->AddRecursiveSpherePlanes(5);

  vtkNew<vtkDataSetMapper> originalMapper;
  originalMapper->SetInputConnection(reader->GetOutputPort());
  originalMapper->ScalarVisibilityOff();

  // Create an actor for the surface
  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetDiffuseColor(
      namedColors->GetColor3d("Banana").GetData());

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(hullFilter->GetOutputPort());
  mapper->ScalarVisibilityOff();

  // Create an actor for the surface
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetDiffuseColor(
      namedColors->GetColor3d("Tomato").GetData());
  actor->GetProperty()->SetEdgeColor(
      namedColors->GetColor3d("IvoryBlack").GetData());
  actor->GetProperty()->SetOpacity(0.5);
  actor->GetProperty()->EdgeVisibilityOff();

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ConvexHull");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->AddActor(originalActor);
  renderer->AddActor(actor);
  renderer->SetBackground(namedColors->GetColor3d("SlateGray").GetData());

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
