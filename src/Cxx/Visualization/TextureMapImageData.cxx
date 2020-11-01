#include <vtkImageCanvasSource2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>

#include <array>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  std::array<double, 3> drawColor1{0, 0, 0};
  std::array<double, 3> drawColor2{0, 0, 0};
  auto color1 = colors->GetColor3ub("ForestGreen").GetData();
  auto color2 = colors->GetColor3ub("DarkMagenta").GetData();
  for (auto i = 0; i < 3; ++i)
  {
    drawColor1[i] = color1[i];
    drawColor2[i] = color2[i];
  }

  // Create an image
  vtkNew<vtkImageCanvasSource2D> imageSource;
  imageSource->SetScalarTypeToUnsignedChar();
  imageSource->SetExtent(0, 20, 0, 20, 0, 0);
  imageSource->SetNumberOfScalarComponents(3);
  imageSource->SetDrawColor(drawColor1.data());
  imageSource->FillBox(0, 20, 0, 20);
  imageSource->SetDrawColor(drawColor2.data());
  imageSource->DrawSegment(0, 0, 19, 19);
  imageSource->DrawSegment(19, 0, 0, 19);
  imageSource->Update();

  // Create a plane
  vtkNew<vtkPlaneSource> plane;
  plane->SetCenter(0.0, 0.0, 0.0);
  plane->SetNormal(0.0, 0.0, 1.0);

  // Apply the texture
  vtkNew<vtkTexture> texture;
  texture->SetInputConnection(imageSource->GetOutputPort());

  vtkNew<vtkPolyDataMapper> planeMapper;
  planeMapper->SetInputConnection(plane->GetOutputPort());

  vtkNew<vtkActor> texturedPlane;
  texturedPlane->SetMapper(planeMapper);
  texturedPlane->SetTexture(texture);

  // Visualize the textured plane
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(texturedPlane);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());
  renderer->ResetCamera();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("TextureMapImageData");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
