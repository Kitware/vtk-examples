#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCollisionDetectionFilter.h>
#include <vtkMatrix4x4.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>

#include <chrono>
#include <sstream>
#include <string>
#include <thread>

int main(int argc, char* argv[])
{
  int contactMode = 0;
  if (argc > 1)
  {
    contactMode = std::stoi(std::string(argv[1]));
  }
  vtkNew<vtkSphereSource> sphere0;
  sphere0->SetRadius(0.29);
  sphere0->SetPhiResolution(31);
  sphere0->SetThetaResolution(31);
  sphere0->SetCenter(0.0, 0, 0);

  vtkNew<vtkSphereSource> sphere1;
  sphere1->SetPhiResolution(30);
  sphere1->SetThetaResolution(30);
  sphere1->SetRadius(0.3);

  vtkNew<vtkMatrix4x4> matrix1;
  vtkNew<vtkTransform> transform0;

  vtkNew<vtkCollisionDetectionFilter> collide;
  collide->SetInputConnection(0, sphere0->GetOutputPort());
  collide->SetTransform(0, transform0);
  collide->SetInputConnection(1, sphere1->GetOutputPort());
  collide->SetMatrix(1, matrix1);
  collide->SetBoxTolerance(0.0);
  collide->SetCellTolerance(0.0);
  collide->SetNumberOfCellsPerNode(2);
  if (contactMode == 0)
  {
    collide->SetCollisionModeToAllContacts();
  }
  else if (contactMode == 1)
  {
    collide->SetCollisionModeToFirstContact();
  }
  else
  {
    collide->SetCollisionModeToHalfContacts();
  }
  collide->GenerateScalarsOn();

  // Visualize
  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkPolyDataMapper> mapper1;
  mapper1->SetInputConnection(collide->GetOutputPort(0));
  mapper1->ScalarVisibilityOff();
  vtkNew<vtkActor> actor1;
  actor1->SetMapper(mapper1);
  actor1->GetProperty()->BackfaceCullingOn();
  actor1->SetUserTransform(transform0);
  actor1->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Tomato").GetData());
  actor1->GetProperty()->SetRepresentationToWireframe();

  vtkNew<vtkPolyDataMapper> mapper2;
  mapper2->SetInputConnection(collide->GetOutputPort(1));

  vtkNew<vtkActor> actor2;
  actor2->SetMapper(mapper2);
  actor2->GetProperty()->BackfaceCullingOn();
  actor2->SetUserMatrix(matrix1);

  vtkNew<vtkPolyDataMapper> mapper3;
  mapper3->SetInputConnection(collide->GetContactsOutputPort());
  mapper3->SetResolveCoincidentTopologyToPolygonOffset();

  vtkNew<vtkActor> actor3;
  actor3->SetMapper(mapper3);
  actor3->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());
  actor3->GetProperty()->SetLineWidth(3.0);

  vtkNew<vtkTextActor> txt;
  txt->GetTextProperty()->SetFontSize(18);

  vtkNew<vtkRenderer> renderer;
  renderer->UseHiddenLineRemovalOn();
  renderer->AddActor(actor1);
  renderer->AddActor(actor2);
  renderer->AddActor(actor3);
  renderer->AddActor(txt);
  renderer->SetBackground(colors->GetColor3d("Gray").GetData());
  renderer->UseHiddenLineRemovalOn();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(640, 480);
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Move the first object
  int numSteps = 100;
  double dx = 1.0 / static_cast<double>(numSteps) * 2.0;
  transform0->Translate(-numSteps * dx - .3, 0.0, 0.0);
  renderWindow->Render();
  renderer->GetActiveCamera()->Azimuth(-60);
  renderer->GetActiveCamera()->Elevation(45);
  renderer->GetActiveCamera()->Dolly(1.2);

  renderWindow->SetWindowName("CollisionDetection");
  renderWindow->Render();

  for (int i = 0; i < numSteps; ++i)
  {
    transform0->Translate(dx, 0.0, 0.0);
    renderer->ResetCameraClippingRange();
    std::ostringstream textStream;
    textStream << collide->GetCollisionModeAsString()
               << ": Number of contact cells is "
               << collide->GetNumberOfContacts();
    txt->SetInput(textStream.str().c_str());
    renderWindow->Render();
    if (collide->GetNumberOfContacts() > 0)
    {
      break;
    }
    // The total animation time is 3 seconds
    std::this_thread::sleep_for(std::chrono::milliseconds(3000 / numSteps));
  }
  renderer->ResetCamera();
  renderWindow->Render();
  interactor->Start();
  // In Field Data there will be an array named "ContactCells".
  // This array indexes contacting cells (e.g.) index 50 of array 0
  //  points to a cell (triangle) which contacts/intersects a cell
  //  at index 50 of array 1.
  // You can directly obtain these, see GetContactCells(int i)
  //  in the documentation.
  // collide->GetOutput(0)->Print(std::cout);
  // collide->GetOutput(1)->Print(std::cout);
  return EXIT_SUCCESS;
}
