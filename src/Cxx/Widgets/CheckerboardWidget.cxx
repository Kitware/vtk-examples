#include <vtkCheckerboardRepresentation.h>
#include <vtkCheckerboardWidget.h>
#include <vtkCommand.h>
#include <vtkImageActor.h>
#include <vtkImageCheckerboard.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0]
              << " Input1Filename Input2Filename e.g. Gourds2.jpg, Ox.jpg"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the images
  vtkNew<vtkJPEGReader> reader1;
  reader1->SetFileName(argv[1]);

  vtkNew<vtkJPEGReader> reader2;
  reader2->SetFileName(argv[2]);

  // Create a checker pipeline
  vtkNew<vtkImageCheckerboard> checker;
  checker->SetInputConnection(0, reader1->GetOutputPort());
  checker->SetInputConnection(1, reader2->GetOutputPort());
  checker->SetNumberOfDivisions(3, 3, 1);

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkRenderer> ren1;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->SetWindowName("CheckerboardWidget");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkImageActor> checkerActor;
  checkerActor->GetMapper()->SetInputConnection(checker->GetOutputPort());

  // VTK widgets consist of two parts: the widget part that handles
  // event processing; and the widget representation that defines how
  // the widget appears in the scene
  // (i.e., matters pertaining to geometry).
  vtkNew<vtkCheckerboardWidget> checkerWidget;
  checkerWidget->SetInteractor(iren);

  vtkCheckerboardRepresentation* checkerWidgetRep =
      static_cast<vtkCheckerboardRepresentation*>(
          checkerWidget->GetRepresentation());

  checkerWidgetRep->SetImageActor(checkerActor);
  checkerWidgetRep->SetCheckerboard(checker);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(checkerActor);
  ren1->SetBackground(colors->GetColor3d("Wheat").GetData());
  renWin->SetSize(300, 300);

  // render the image
  //
  iren->Initialize();
  renWin->Render();
  checkerWidget->On();
  iren->Start();

  return EXIT_SUCCESS;
}
