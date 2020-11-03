#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredPointsReader.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " ironProt.vtk" << std::endl;
    return EXIT_SUCCESS;
  }

  vtkNew<vtkNamedColors> colors;

  // Create the RenderWindow, Renderer and both Actors
  //

  vtkNew<vtkRenderer> ren1;

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // create pipeline
  //
  vtkNew<vtkStructuredPointsReader> reader;
  reader->SetFileName(argv[1]);

  vtkNew<vtkContourFilter> iso;
  iso->SetInputConnection(reader->GetOutputPort());
  iso->SetValue(0, 128);

  vtkNew<vtkPolyDataMapper> isoMapper;
  isoMapper->SetInputConnection(iso->GetOutputPort());
  isoMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> isoActor;
  isoActor->SetMapper(isoMapper);
  isoActor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());

  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(outlineActor);
  ren1->AddActor(isoActor);
  ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renWin->SetSize(640, 640);
  renWin->SetWindowName("IronIsoSurface");

  // render the image
  //
  ren1->ResetCamera();
  ren1->GetActiveCamera()->Azimuth(30);
  ren1->GetActiveCamera()->Elevation(30);
  renWin->Render();
  iren->Start();
  return EXIT_SUCCESS;
}
