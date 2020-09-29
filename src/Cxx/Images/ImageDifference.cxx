#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageDifference.h>
#include <vtkImageMapper3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  std::array<double, 3> drawColor1{0, 0, 0};
  std::array<double, 3> drawColor2{0, 0, 0};
  auto color1 = colors->GetColor3ub("Black").GetData();
  auto color2 = colors->GetColor3ub("Wheat").GetData();
  for (auto i = 0; i < 3; ++i)
  {
    drawColor1[i] = color1[i];
    drawColor2[i] = color2[i];
  }

  // Create an image
  vtkNew<vtkImageCanvasSource2D> source1;
  source1->SetScalarTypeToUnsignedChar();
  source1->SetNumberOfScalarComponents(3);
  source1->SetExtent(0, 100, 0, 100, 0, 0);
  source1->SetDrawColor(drawColor1.data());
  source1->FillBox(0, 100, 0, 100);
  source1->SetDrawColor(drawColor2.data());
  source1->FillBox(10, 90, 10, 90);
  source1->Update();

  // Create another image
  vtkNew<vtkImageCanvasSource2D> source2;
  source2->SetScalarTypeToUnsignedChar();
  source2->SetNumberOfScalarComponents(3);
  source2->SetExtent(0, 100, 0, 100, 0, 0);
  source2->SetDrawColor(drawColor1.data());
  source2->FillBox(0, 100, 0, 100);
  source2->SetDrawColor(drawColor2.data());
  source2->FillBox(20, 80, 20, 80);
  source2->Update();

  vtkNew<vtkImageDifference> differenceFilter;

  differenceFilter->SetInputConnection(source1->GetOutputPort());
  differenceFilter->SetImageConnection(source2->GetOutputPort());
  differenceFilter->Update();

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.33, 1.0};
  double centerViewport[4] = {0.33, 0.0, 0.66, 1.0};
  double rightViewport[4] = {0.66, 0.0, 1.0, 1.0};

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;

  // Setup renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  vtkNew<vtkImageActor> leftActor;
  leftActor->GetMapper()->SetInputConnection(source1->GetOutputPort());

  leftRenderer->AddActor(leftActor);
  leftRenderer->SetBackground(colors->GetColor3d("Mint").GetData());

  vtkNew<vtkRenderer> centerRenderer;
  renderWindow->AddRenderer(centerRenderer);
  centerRenderer->SetViewport(centerViewport);
  vtkNew<vtkImageActor> centerActor;
  centerActor->GetMapper()->SetInputConnection(source2->GetOutputPort());

  centerRenderer->AddActor(centerActor);
  centerRenderer->SetBackground(colors->GetColor3d("Mint").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("Peacock").GetData());
  vtkNew<vtkImageActor> rightActor;
  rightActor->GetMapper()->SetInputConnection(
      differenceFilter->GetOutputPort());
  rightRenderer->AddActor(rightActor);

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  // Render and start interaction
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->SetSize(300, 100);
  renderWindow->SetWindowName("ImageDifference");
  renderWindow->Render();

  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
