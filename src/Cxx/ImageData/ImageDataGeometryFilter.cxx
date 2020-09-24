#include <vtkActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <array>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Convert our unsigned char colors to doubles
  // Set draw color needs doubles.
  auto color1 = colors->GetColor4ub("SteelBlue").GetData();
  auto color2 = colors->GetColor4ub("PaleGoldenrod").GetData();
  std::array<double, 4> sourceColor1{0, 0, 0, 0};
  std::array<double, 4> sourceColor2{0, 0, 0, 0};
  for (auto i = 0; i < 4; ++i)
  {
    sourceColor1[i] = color1[i];
    sourceColor2[i] = color2[i];
  }

  // Create an image
  vtkNew<vtkImageCanvasSource2D> source1;
  source1->SetScalarTypeToUnsignedChar();
  source1->SetNumberOfScalarComponents(3);
  source1->SetExtent(0, 100, 0, 100, 0, 0);
  source1->SetDrawColor(sourceColor1.data());
  source1->FillBox(0, 100, 0, 100);
  source1->SetDrawColor(sourceColor2.data());
  source1->FillBox(10, 20, 10, 20);
  source1->FillBox(40, 50, 20, 30);
  source1->Update();

  // Convert the image to a polydata
  vtkNew<vtkImageDataGeometryFilter> imageDataGeometryFilter;
  imageDataGeometryFilter->SetInputConnection(source1->GetOutputPort());
  imageDataGeometryFilter->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(imageDataGeometryFilter->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Visualization
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("RosyBrown").GetData());

  renderWindow->SetWindowName("ImageDataGeometryFilter");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
