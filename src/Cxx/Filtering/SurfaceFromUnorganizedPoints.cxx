#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProgrammableSource.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkReverseSense.h>
#include <vtkSphereSource.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkSmartPointer.h>

int main(int argc, char* argv[])
{

  vtkNew<vtkNamedColors> colors;

  auto input = vtkSmartPointer<vtkPolyData>::New();
  if (argc > 1)
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();
    input = reader->GetOutput();
  }
  else
  {
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();
    input = sphereSource->GetOutput();
  }

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(input->GetPoints());

  // Construct the surface and create isosurface.
  vtkNew<vtkSurfaceReconstructionFilter> surf;
  surf->SetInputData(polydata);

  vtkNew<vtkContourFilter> cf;
  cf->SetInputConnection(surf->GetOutputPort());
  cf->SetValue(0, 0.0);

  // Sometimes the contouring algorithm can create a volume whose gradient
  // vector and ordering of polygon (using the right hand rule) are
  // inconsistent. vtkReverseSense cures this problem.
  vtkNew<vtkReverseSense> reverse;
  reverse->SetInputConnection(cf->GetOutputPort());
  reverse->ReverseCellsOn();
  reverse->ReverseNormalsOn();

  vtkNew<vtkPolyDataMapper> map;
  map->SetInputConnection(reverse->GetOutputPort());
  map->ScalarVisibilityOff();

  vtkNew<vtkActor> surfaceActor;
  surfaceActor->SetMapper(map);
  surfaceActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Tomato").GetData());
  surfaceActor->GetProperty()->SetSpecularColor(
      colors->GetColor3d("Seashell").GetData());
  surfaceActor->GetProperty()->SetSpecular(.4);
  surfaceActor->GetProperty()->SetSpecularPower(50);

  // Create the RenderWindow, Renderer and both Actors
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer, set the background and size
  renderer->AddActor(surfaceActor);
  renderer->SetBackground(colors->GetColor3d("Burlywood").GetData());

  renderWindow->SetWindowName("SurfaceFromUnorganizedPoints");
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
