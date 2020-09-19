#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a rendering window and renderer.
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->SetWindowName("TextActor");
  renWin->AddRenderer(ren);

  // Create a render window interactor.
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Create a text actor.
  vtkNew<vtkTextActor> txt;
  txt->SetInput("Hello World!");
  vtkTextProperty* txtprop = txt->GetTextProperty();
  txtprop->SetFontFamilyToArial();
  txtprop->BoldOn();
  txtprop->SetFontSize(36);
  txtprop->ShadowOn();
  txtprop->SetShadowOffset(4, 4);
  txtprop->SetColor(colors->GetColor3d("Cornsilk").GetData());
  txt->SetDisplayPosition(20, 30);

  // Assign actor to the renderer.
  ren->AddActor(txt);
  ren->SetBackground(colors->GetColor3d("DarkGreen").GetData());

  // Enable user interface interactor.
  iren->Initialize();
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
