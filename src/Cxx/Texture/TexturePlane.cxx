#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>
//
// This simple example shows how to do basic texture mapping.
//
//
// Load in the texture map. A texture is any unsigned char image. If it
// is not of this type, you will have to map it through a lookup table
// or by using vtkImageShiftScale.
//
int main(int argc, char* argv[])
{
  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> textureFile;
  textureFile.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  textureFile->SetFileName(argv[1]);
  textureFile->Update();

  vtkNew<vtkTexture> atext;
  atext->SetInputConnection(textureFile->GetOutputPort());
  atext->InterpolateOn();

  // Create a plane source and actor. The vtkPlanesSource generates
  // texture coordinates.
  //
  vtkNew<vtkPlaneSource> plane;

  vtkNew<vtkPolyDataMapper> planeMapper;
  planeMapper->SetInputConnection(plane->GetOutputPort());

  vtkNew<vtkActor> planeActor;
  planeActor->SetMapper(planeMapper);
  planeActor->SetTexture(atext);

  // Create the RenderWindow, Renderer and both Actors
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size
  renderer->AddActor(planeActor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
  renWin->SetSize(640, 480);
  renWin->SetWindowName("TexturePlane");

  // render the image
  renWin->Render();

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Elevation(-30);
  renderer->GetActiveCamera()->Roll(-20);
  renderer->ResetCameraClippingRange();
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
