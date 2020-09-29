#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageToStructuredPoints.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredPoints.h>

#include <array>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  std::array<double, 3> drawColor1{0, 0, 0};
  std::array<double, 3> drawColor2{0, 0, 0};
  auto color1 = colors->GetColor3ub("Gray").GetData();
  auto color2 = colors->GetColor3ub("Gray").GetData();
  for (auto i = 0; i < 3; ++i)
  {
    drawColor1[i] = color1[i];
    drawColor2[i] = color2[i];
  }
  drawColor2[0] = 255;

  // Create an image
  vtkNew<vtkImageCanvasSource2D> source;
  source->SetExtent(0, 20, 0, 20, 0, 0);
  source->SetScalarTypeToUnsignedChar();
  source->SetNumberOfScalarComponents(3);
  source->SetDrawColor(drawColor1.data());
  source->FillBox(0, 20, 0, 20);
  source->SetDrawColor(drawColor2.data());
  source->FillBox(0, 15, 0, 20);
  source->Update();

  vtkNew<vtkImageToStructuredPoints> convertFilter;
  convertFilter->SetInputConnection(source->GetOutputPort());
  convertFilter->Update();

  //  vtkStructuredPoints* structuredPoints =
  //  convertFilter->GetStructuredPointsOutput();

  // Create a mapper and actor
  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(convertFilter->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("LimeGreen").GetData());

  renderWindow->SetWindowName("ImageToStructuredPoints");

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
