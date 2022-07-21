#include <vtkAnnotatedCubeActor.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Set up the renderer, window, and interactor.
  //
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("Wheat").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("AnnotatedCubeActor");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  vtkNew<vtkAnnotatedCubeActor> cube;
  cube->SetFaceTextScale(2.0 / 3.0);

  // Anatomic labelling.
  //
  cube->SetXPlusFaceText("A");
  cube->SetXMinusFaceText("P");
  cube->SetYPlusFaceText("L");
  cube->SetYMinusFaceText("R");
  cube->SetZPlusFaceText("S");
  cube->SetZMinusFaceText("I");

  // Change the vector text colors.
  //
  cube->GetTextEdgesProperty()->SetColor(colors->GetColor3d("Black").GetData());
  cube->GetTextEdgesProperty()->SetLineWidth(4);

  // clang-format off
  cube->GetXPlusFaceProperty()->SetColor(
      colors->GetColor3d("Turquoise").GetData());
  cube->GetXMinusFaceProperty()->SetColor(
      colors->GetColor3d("Turquoise").GetData());
  cube->GetYPlusFaceProperty()->SetColor(
      colors->GetColor3d("Mint").GetData());
  cube->GetYMinusFaceProperty()->SetColor(
      colors->GetColor3d("Mint").GetData());
  cube->GetZPlusFaceProperty()->SetColor(
      colors->GetColor3d("Tomato").GetData());
  cube->GetZMinusFaceProperty()->SetColor(
      colors->GetColor3d("Tomato").GetData());
  // clang-format on

  renderer->AddActor(cube);

  // Set up an interesting view.
  //
  vtkCamera* camera = renderer->GetActiveCamera();
  camera->SetViewUp(0, 0, 1);
  camera->SetFocalPoint(0, 0, 0);
  camera->SetPosition(4.5, 4.5, 2.5);
  renderer->ResetCamera();
  camera->Dolly(1.0);
  renderer->ResetCameraClippingRange();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
