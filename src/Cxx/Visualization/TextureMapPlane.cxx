#include <vtkImageData.h>
#include <vtkJPEGReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>
#include <vtkTextureMapToPlane.h>

int main(int argc, char* argv[])
{
  // Parse command line arguments
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " Filename e.g. Yinyang.jpg"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  std::string inputFilename = argv[1];

  // Read the image which will be the texture
  vtkNew<vtkJPEGReader> jPEGReader;
  jPEGReader->SetFileName(inputFilename.c_str());

  // Create a plane
  vtkNew<vtkPlaneSource> plane;
  plane->SetCenter(0.0, 0.0, 0.0);
  plane->SetNormal(0.0, 0.0, 1.0);

  // Apply the texture
  vtkNew<vtkTexture> texture;
  texture->SetInputConnection(jPEGReader->GetOutputPort());

  vtkNew<vtkTextureMapToPlane> texturePlane;
  texturePlane->SetInputConnection(plane->GetOutputPort());

  vtkNew<vtkPolyDataMapper> planeMapper;
  planeMapper->SetInputConnection(texturePlane->GetOutputPort());

  vtkNew<vtkActor> texturedPlane;
  texturedPlane->SetMapper(planeMapper);
  texturedPlane->SetTexture(texture);
  texturedPlane->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  // Visualize the textured plane
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(texturedPlane);
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
  renderer->ResetCamera();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("TextureMapPlane");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
