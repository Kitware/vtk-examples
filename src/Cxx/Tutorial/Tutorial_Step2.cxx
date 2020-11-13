/*=========================================================================

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// First include the required header files for the VTK classes we are using.
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include <iostream>

//
// We put the callback in an anonymous namespace, i.e. a namespace with
// no name. This anonymous namespace is only accessible within the file
// that you have created it in. So it is a good way of declaring unique
// identifiers and avoiding making global static variables.
//
namespace {
// Callback for the interaction.
class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback* New()
  {
    return new vtkMyCallback;
  }
  void Execute(vtkObject* caller, unsigned long, void*) override
  {
    // Note the use of reinterpret_cast to cast the caller to the expected type.
    auto renderer = reinterpret_cast<vtkRenderer*>(caller);
    std::cout << renderer->GetActiveCamera()->GetPosition()[0] << " "
              << renderer->GetActiveCamera()->GetPosition()[1] << " "
              << renderer->GetActiveCamera()->GetPosition()[2] << std::endl;
  }
  vtkMyCallback() = default;
};
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  //
  // The pipeline creation is documented in Tutorial_Step1.
  //
  vtkNew<vtkConeSource> cone;
  cone->SetHeight(3.0);
  cone->SetRadius(1.0);
  cone->SetResolution(10);

  vtkNew<vtkPolyDataMapper> coneMapper;
  coneMapper->SetInputConnection(cone->GetOutputPort());
  vtkNew<vtkActor> coneActor;
  coneActor->SetMapper(coneMapper);
  coneActor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  vtkNew<vtkRenderer> ren1;
  ren1->AddActor(coneActor);
  ren1->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
  ren1->ResetCamera();

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->SetSize(300, 300);
  renWin->SetWindowName("Tutorial_Step2");

  // Here is where we setup the observer.
  vtkNew<vtkMyCallback> mo1;
  ren1->AddObserver(vtkCommand::StartEvent, mo1);

  //
  // Now we loop over 360 degrees and render the cone each time.
  //
  for (int i = 0; i < 360; ++i)
  {
    // Render the image.
    renWin->Render();
    // Rotate the active camera by one degree.
    ren1->GetActiveCamera()->Azimuth(1);
  }

  return EXIT_SUCCESS;
}
