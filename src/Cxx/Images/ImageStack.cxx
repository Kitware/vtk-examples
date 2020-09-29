#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkImageProperty.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageStack.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

static void CreateColorImage(vtkImageData*, const int corner,
                             const unsigned int channel);

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Image 1
  vtkNew<vtkImageData> image1;
  CreateColorImage(image1, 1, 0);

  vtkNew<vtkImageSliceMapper> imageSliceMapper1;
  imageSliceMapper1->SetInputData(image1);

  vtkNew<vtkImageSlice> imageSlice1;
  imageSlice1->SetMapper(imageSliceMapper1);
  imageSlice1->GetProperty()->SetOpacity(.5);

  // Image 2
  vtkNew<vtkImageData> image2;
  CreateColorImage(image2, 4, 1);

  vtkNew<vtkImageSliceMapper> imageSliceMapper2;
  imageSliceMapper2->SetInputData(image2);

  vtkNew<vtkImageSlice> imageSlice2;
  imageSlice2->SetMapper(imageSliceMapper2);
  imageSlice2->GetProperty()->SetOpacity(.5);

  // Stack
  vtkNew<vtkImageStack> imageStack;
  imageStack->AddImage(imageSlice1);
  imageStack->AddImage(imageSlice2);
  // imageStack->SetActiveLayer(1);

  // Setup renderers
  vtkNew<vtkRenderer> renderer;
  renderer->AddViewProp(imageStack);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ImageStack");

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

void CreateColorImage(vtkImageData* image, const int corner,
                      const unsigned int channel)
{
  image->SetDimensions(10, 10, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

  for (unsigned int x = 0; x < 10; x++)
  {
    for (unsigned int y = 0; y < 10; y++)
    {
      unsigned char* pixel =
          static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
      pixel[0] = 0;
      pixel[1] = 0;
      pixel[2] = 0;
    }
  }

  for (int x = corner; x < corner + 3; x++)
  {
    for (int y = corner; y < corner + 3; y++)
    {
      unsigned char* pixel =
          static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
      pixel[channel] = 255;
    }
  }
}
