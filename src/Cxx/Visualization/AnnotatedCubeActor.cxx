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
  vtkNew<vtkAnnotatedCubeActor> cube;
  cube->SetFaceTextScale(0.666667);

  // set up the renderer, window, and interactor
  //
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("Wheat").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("AnnotatedCubeActor");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // anatomic labelling
  //
  cube->SetXPlusFaceText("A");
  cube->SetXMinusFaceText("P");
  cube->SetYPlusFaceText("L");
  cube->SetYMinusFaceText("R");
  cube->SetZPlusFaceText("S");
  cube->SetZMinusFaceText("I");

  // change the vector text colors
  //
  cube->GetTextEdgesProperty()->SetColor(colors->GetColor3d("Black").GetData());
  cube->GetTextEdgesProperty()->SetLineWidth(4);

  vtkProperty* property;
  property = cube->GetXPlusFaceProperty();
  property->SetColor(colors->GetColor3d("Turquoise").GetData());
  property = cube->GetXMinusFaceProperty();
  property->SetColor(colors->GetColor3d("Turquoise").GetData());
  property = cube->GetYPlusFaceProperty();
  property->SetColor(colors->GetColor3d("Mint").GetData());
  property = cube->GetYMinusFaceProperty();
  property->SetColor(colors->GetColor3d("Mint").GetData());
  property = cube->GetZPlusFaceProperty();
  property->SetColor(colors->GetColor3d("Tomato").GetData());
  property = cube->GetZMinusFaceProperty();
  property->SetColor(colors->GetColor3d("Tomato").GetData());

  renderer->AddActor(cube);

  // set up an interesting view
  //
  vtkCamera* camera = renderer->GetActiveCamera();
  camera->SetViewUp(0, 0, 1);
  camera->SetFocalPoint(0, 0, 0);
  camera->SetPosition(4.5, 4.5, 2.5);
  renderer->ResetCamera();
  camera->Dolly(1.3);
  renderer->ResetCameraClippingRange();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
