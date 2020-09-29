#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageStencil.h>
#include <vtkImageStencilData.h>
#include <vtkImageToImageStencil.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {
void CreateColorImage(vtkImageData*, unsigned int channel);
void CreateMask(vtkImageData*);
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkImageData> image1;
  CreateColorImage(image1, 0); // Create a red image

  vtkNew<vtkImageData> image2;
  CreateColorImage(image2, 1); // Create a green image

  vtkNew<vtkImageData> mask;
  CreateMask(mask);

  // vtkNew<vtkImageStencilData> stencilData;
  vtkNew<vtkImageToImageStencil> imageToImageStencil;
  imageToImageStencil->SetInputData(mask);
  imageToImageStencil->ThresholdByUpper(122);

  vtkNew<vtkImageStencil> stencil;
  stencil->SetInputConnection(2, imageToImageStencil->GetOutputPort());
  stencil->ReverseStencilOn();
  stencil->SetBackgroundInputData(image2);
  stencil->SetInputData(image1);
  stencil->Update();

  // Create an actor
  vtkNew<vtkImageActor> actor;
  actor->GetMapper()->SetInputConnection(stencil->GetOutputPort());

  // Setup renderer
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ImageStencil");

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  // Render and start interaction
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  ;
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
void CreateColorImage(vtkImageData* image, const unsigned int channel)
{
  unsigned int dim = 20;

  image->SetDimensions(dim, dim, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

  for (unsigned int x = 0; x < dim; x++)
  {
    for (unsigned int y = 0; y < dim; y++)
    {
      unsigned char* pixel =
          static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
      pixel[0] = 0;
      pixel[1] = 0;
      pixel[2] = 0;

      pixel[channel] = 255;
    }
  }

  image->Modified();
}

void CreateMask(vtkImageData* image)
{
  unsigned int dim = 20;

  image->SetDimensions(dim, dim, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  for (unsigned int x = 0; x < dim; x++)
  {
    for (unsigned int y = 0; y < dim; y++)
    {
      unsigned char* pixel =
          static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
      if (x < dim / 2)
      {
        pixel[0] = 0;
      }
      else
      {
        pixel[0] = 255;
      }
    }
  }

  image->Modified();
}
} // namespace
