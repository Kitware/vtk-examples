#include <vtkImageActor.h>
#include <vtkImageAppendComponents.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  std::array<double, 4> drawColor1{0, 0, 0, 0};
  std::array<double, 4> drawColor2{0, 0, 0, 0};
  auto color1 = colors->GetColor4ub("Black").GetData();
  auto color2 = colors->GetColor4ub("Red").GetData();
  for (auto i = 0; i < 4; ++i)
  {
    drawColor1[i] = color1[i];
    drawColor2[i] = color2[i];
  }
  // Set the alpha to 0
  drawColor1[3] = 0;
  drawColor2[3] = 0;

  // Create an image (will be used as the R channel)
  vtkNew<vtkImageCanvasSource2D> red;
  red->SetScalarTypeToUnsignedChar();
  red->SetNumberOfScalarComponents(1);
  red->SetExtent(0, 100, 0, 100, 0, 0);
  red->SetDrawColor(drawColor1.data());
  red->FillBox(0, 100, 0, 100);
  red->SetDrawColor(drawColor2.data());
  red->FillBox(20, 40, 20, 40);
  red->Update();

  // Create an image (will be used as the G channel)
  vtkNew<vtkImageCanvasSource2D> green;
  green->SetScalarTypeToUnsignedChar();
  green->SetNumberOfScalarComponents(1);
  green->SetExtent(0, 100, 0, 100, 0, 0);
  green->SetDrawColor(drawColor1.data());
  green->FillBox(0, 100, 0, 100);
  green->SetDrawColor(drawColor2.data());
  green->FillBox(30, 50, 30, 50);
  green->Update();

  // Create an image (will be used as the B channel)
  vtkNew<vtkImageCanvasSource2D> blue;
  blue->SetScalarTypeToUnsignedChar();
  blue->SetNumberOfScalarComponents(1);
  blue->SetExtent(0, 100, 0, 100, 0, 0);
  blue->SetDrawColor(drawColor1.data());
  blue->FillBox(0, 100, 0, 100);
  blue->SetDrawColor(drawColor2.data());
  blue->FillBox(40, 60, 40, 60);
  blue->Update();

  vtkNew<vtkImageAppendComponents> appendFilter;
  appendFilter->SetInputConnection(0, red->GetOutputPort());
  appendFilter->AddInputConnection(0, green->GetOutputPort());
  appendFilter->AddInputConnection(0, blue->GetOutputPort());
  appendFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> redActor;
  redActor->GetMapper()->SetInputConnection(red->GetOutputPort());

  vtkNew<vtkImageActor> greenActor;
  greenActor->GetMapper()->SetInputConnection(green->GetOutputPort());

  vtkNew<vtkImageActor> blueActor;
  blueActor->GetMapper()->SetInputConnection(blue->GetOutputPort());

  vtkNew<vtkImageActor> combinedActor;
  combinedActor->GetMapper()->SetInputConnection(appendFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double redViewport[4] = {0.0, 0.0, 0.25, 1.0};
  double greenViewport[4] = {0.25, 0.0, 0.5, 1.0};
  double blueViewport[4] = {0.5, 0.0, 0.75, 1.0};
  double combinedViewport[4] = {0.75, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> redRenderer;
  redRenderer->SetViewport(redViewport);
  redRenderer->AddActor(redActor);
  redRenderer->ResetCamera();
  redRenderer->SetBackground(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkRenderer> greenRenderer;
  greenRenderer->SetViewport(greenViewport);
  greenRenderer->AddActor(greenActor);
  greenRenderer->ResetCamera();
  greenRenderer->SetBackground(colors->GetColor3d("Mint").GetData());

  vtkNew<vtkRenderer> blueRenderer;
  blueRenderer->SetViewport(blueViewport);
  blueRenderer->AddActor(blueActor);
  blueRenderer->ResetCamera();
  blueRenderer->SetBackground(colors->GetColor3d("Peacock").GetData());

  vtkNew<vtkRenderer> combinedRenderer;
  combinedRenderer->SetViewport(combinedViewport);
  combinedRenderer->AddActor(combinedActor);
  combinedRenderer->ResetCamera();
  combinedRenderer->SetBackground(colors->GetColor3d("Snow").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(1000, 250);
  renderWindow->AddRenderer(redRenderer);
  renderWindow->AddRenderer(greenRenderer);
  renderWindow->AddRenderer(blueRenderer);
  renderWindow->AddRenderer(combinedRenderer);
  renderWindow->SetWindowName("CombiningRGBChannels");

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  // Render and start interaction
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
