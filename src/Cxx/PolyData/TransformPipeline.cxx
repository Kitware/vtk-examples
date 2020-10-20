// This example describes the transformation pipeline. Robotic arm was used to
// demonstrate an example.
#include <vtkActor.h>
#include <vtkCylinderSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> ren1;
  ren1->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->SetSize(600, 600);
  renWin->SetWindowName("Robotic Arm");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // arm
  vtkNew<vtkCylinderSource> arm;
  arm->SetRadius(8);
  arm->SetHeight(20);
  arm->SetResolution(20);

  vtkNew<vtkPolyDataMapper> armMapper;
  armMapper->SetInputConnection(arm->GetOutputPort());

  vtkNew<vtkTransform> armTransform;

  vtkNew<vtkActor> armActor;
  armActor->SetUserTransform(armTransform);
  armActor->SetMapper(armMapper);
  armActor->GetProperty()->SetColor(colors->GetColor3d("SandyBrown").GetData());

  // forearm
  vtkNew<vtkCylinderSource> forearm;
  forearm->SetRadius(6);
  forearm->SetHeight(15);
  forearm->SetResolution(20);
  forearm->SetCenter(*(arm->GetCenter()),
                     *(arm->GetCenter() + 1) + forearm->GetHeight(),
                     *(arm->GetCenter() + 2));

  vtkNew<vtkPolyDataMapper> forearmMapper;
  forearmMapper->SetInputConnection(forearm->GetOutputPort());

  vtkNew<vtkTransform> forearmTransform;
  forearmTransform->SetInput(armTransform);

  vtkNew<vtkActor> forearmActor;
  forearmActor->SetUserTransform(forearmTransform);
  forearmActor->SetMapper(forearmMapper);
  forearmActor->GetProperty()->SetColor(
      colors->GetColor3d("RoyalBLue").GetData());

  // hand
  vtkNew<vtkCylinderSource> hand;
  hand->SetRadius(4);
  hand->SetHeight(10);
  hand->SetResolution(20);
  hand->SetCenter(*(forearm->GetCenter()),
                  *(forearm->GetCenter() + 1) + hand->GetHeight(),
                  *(forearm->GetCenter() + 2));

  vtkNew<vtkPolyDataMapper> handMapper;
  handMapper->SetInputConnection(hand->GetOutputPort());

  vtkNew<vtkTransform> handTransform;
  handTransform->SetInput(forearmTransform);

  vtkNew<vtkActor> handActor;
  handActor->SetUserTransform(handTransform);
  handActor->SetMapper(handMapper);
  handActor->GetProperty()->SetColor(
      colors->GetColor3d("GreenYellow").GetData());

  ren1->AddActor(armActor);
  ren1->AddActor(forearmActor);
  ren1->AddActor(handActor);

  renWin->Render();

  // execution of robot arm motion
  for (int i = 0; i < 45; i++)
  {
    armTransform->Identity();
    armTransform->RotateZ(-i);
    renWin->Render();
  }
  // execution of robot forearm motion
  for (int i = 0; i < 45; i++)
  {
    forearmTransform->Identity();
    forearmTransform->RotateZ(i);
    renWin->Render();
  }

  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
