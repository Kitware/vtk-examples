#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkDataSet.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkVRMLImporter.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  if (argc != 3)
  {
    std::cout << "Required arguments: Filename Actorname e.g. teapot.wrl teapot"
              << std::endl;
    return EXIT_FAILURE;
  }

  std::string filename = argv[1];
  std::cout << "Showing " << argv[2] << " from " << filename << std::endl;

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("IndividualVRML");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // VRML Import
  vtkNew<vtkVRMLImporter> importer;
  importer->SetFileName(filename.c_str());
  importer->Read();
  importer->SetRenderWindow(renderWindow);
  importer->Update();

  // ----------------------------------------------------------
  vtkObject* defActor = importer->GetVRMLDEFObject(argv[2]);
  if (defActor == NULL)
  {
    std::cout << "Cannot locate actor " << argv[2] << " in " << filename
              << std::endl;
    importer->Print(std::cout);
    return EXIT_FAILURE;
  }

  vtkActor* actor = static_cast<vtkActor*>(defActor);
  double color[3] = {0.89, 0.81, 0.34};
  actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());
  actor->GetProperty()->SetRepresentationToWireframe();

  vtkNew<vtkTransform> transform;

  transform->Translate(actor->GetCenter()[0], actor->GetCenter()[1],
                       actor->GetCenter()[2]);
  // axes
  vtkNew<vtkAxesActor> axes;

  double l[3];
  l[0] = (actor->GetBounds()[1] - actor->GetBounds()[0]) * 1.5;
  l[1] = (actor->GetBounds()[3] - actor->GetBounds()[2]) * 1.5;
  l[2] = (actor->GetBounds()[5] - actor->GetBounds()[4]) * 1.5;

  axes->SetTotalLength(l);
  axes->SetUserTransform(transform);
  renderer->AddActor(axes);
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
  renderWindow->Render();

  renderer->GetActiveCamera()->SetPosition(-14.8296, 18.1304, 12.3352);
  renderer->GetActiveCamera()->SetFocalPoint(2.09905, 0.0832915, 2.47961);
  renderer->GetActiveCamera()->SetViewUp(0.262918, -0.260671, 0.928937);
  renderer->GetActiveCamera()->SetDistance(26.6348);
  renderer->ResetCameraClippingRange();

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
