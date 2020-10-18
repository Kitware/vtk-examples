#include <vtkClipPolyData.h>
#include <vtkImplicitSelectionLoop.h>
#include <vtkLODActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetPhiResolution(100);
  sphereSource->SetThetaResolution(100);
  sphereSource->Update();

  vtkNew<vtkPoints> selectionPoints;

  selectionPoints->InsertPoint(0, -0.16553, 0.135971, 0.451972);
  selectionPoints->InsertPoint(1, -0.0880123, -0.134952, 0.4747);
  selectionPoints->InsertPoint(2, 0.00292618, -0.134604, 0.482459);
  selectionPoints->InsertPoint(3, 0.0641941, 0.067112, 0.490947);
  selectionPoints->InsertPoint(4, 0.15577, 0.0734765, 0.469245);
  selectionPoints->InsertPoint(5, 0.166667, -0.129217, 0.454622);
  selectionPoints->InsertPoint(6, 0.241259, -0.123363, 0.420581);
  selectionPoints->InsertPoint(7, 0.240334, 0.0727106, 0.432555);
  selectionPoints->InsertPoint(8, 0.308529, 0.0844311, 0.384357);
  selectionPoints->InsertPoint(9, 0.32672, -0.121674, 0.359187);
  selectionPoints->InsertPoint(10, 0.380721, -0.117342, 0.302527);
  selectionPoints->InsertPoint(11, 0.387804, 0.0455074, 0.312375);
  selectionPoints->InsertPoint(12, 0.43943, -0.111673, 0.211707);
  selectionPoints->InsertPoint(13, 0.470984, -0.0801913, 0.147919);
  selectionPoints->InsertPoint(14, 0.436777, 0.0688872, 0.233021);
  selectionPoints->InsertPoint(15, 0.44874, 0.188852, 0.109882);
  selectionPoints->InsertPoint(16, 0.391352, 0.254285, 0.176943);
  selectionPoints->InsertPoint(17, 0.373274, 0.154162, 0.294296);
  selectionPoints->InsertPoint(18, 0.274659, 0.311654, 0.276609);
  selectionPoints->InsertPoint(19, 0.206068, 0.31396, 0.329702);
  selectionPoints->InsertPoint(20, 0.263789, 0.174982, 0.387308);
  selectionPoints->InsertPoint(21, 0.213034, 0.175485, 0.417142);
  selectionPoints->InsertPoint(22, 0.169113, 0.261974, 0.390286);
  selectionPoints->InsertPoint(23, 0.102552, 0.25997, 0.414814);
  selectionPoints->InsertPoint(24, 0.131512, 0.161254, 0.454705);
  selectionPoints->InsertPoint(25, 0.000192443, 0.156264, 0.475307);
  selectionPoints->InsertPoint(26, -0.0392091, 0.000251724, 0.499943);
  selectionPoints->InsertPoint(27, -0.096161, 0.159646, 0.46438);

  vtkNew<vtkImplicitSelectionLoop> loop;
  loop->SetLoop(selectionPoints);

  vtkNew<vtkClipPolyData> clip;
  clip->SetInputConnection(sphereSource->GetOutputPort());
  clip->SetClipFunction(loop);
  clip->SetValue(0.0);

  vtkNew<vtkPolyDataMapper> clipMapper;
  clipMapper->SetInputConnection(clip->GetOutputPort());
  clipMapper->ScalarVisibilityOff();

  vtkNew<vtkProperty> backProp;
  backProp->SetColor(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkLODActor> clipActor;
  clipActor->SetMapper(clipMapper);
  clipActor->SetBackfaceProperty(backProp);
  clipActor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());

  vtkNew<vtkRenderer> renderer;

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer, set the background and size
  renderer->AddActor(clipActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renderWindow->SetSize(500, 500);
  renderWindow->SetWindowName("ImplicitSelectionLoop");

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
