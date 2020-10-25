#include <vtkActor.h>
#include <vtkImageReader.h>
#include <vtkImageReader2Factory.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>
#include <vtkTextureMapToSphere.h>
#include <vtkTexturedSphereSource.h>
#include <vtkTransformTextureCoords.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " texture(.png/.ppm) e.g. earth.ppm"
              << " [translate]" << std::endl;
    return EXIT_FAILURE;
  }
  double translate[3];
  if (argc > 2)
  {
    translate[0] = atof(argv[2]);
  }
  else
  {
    translate[0] = 0.0;
  }
  translate[1] = 0.0;
  translate[2] = 0.0;
  std::cout << translate[0] << ", " << translate[1] << ", " << translate[2]
            << "\n";
  // Create a sphere with texture coordinates
  vtkNew<vtkTexturedSphereSource> source;
  source->SetThetaResolution(100);
  source->SetPhiResolution(100);

  // Read texture file
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> imageReader;
  imageReader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  imageReader->SetFileName(argv[1]);

  // Create texture
  vtkNew<vtkTexture> texture;
  texture->SetInputConnection(imageReader->GetOutputPort());

  vtkNew<vtkTransformTextureCoords> transformTexture;
  transformTexture->SetInputConnection(source->GetOutputPort());
  transformTexture->SetPosition(translate);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(transformTexture->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->SetTexture(texture);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Black").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("TexturedSphere");

  vtkNew<vtkRenderWindowInteractor> renWinInteractor;
  renWinInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  renWinInteractor->Start();

  return EXIT_SUCCESS;
}
