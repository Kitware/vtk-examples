#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkImageProperty.h>
#include <vtkImageResliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {

void CreateColorImage(vtkImageData*);

}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkImageData> colorImage;
  CreateColorImage(colorImage);

  vtkNew<vtkImageResliceMapper> imageResliceMapper;
  imageResliceMapper->SetInputData(colorImage);

  vtkNew<vtkImageSlice> imageSlice;
  imageSlice->SetMapper(imageResliceMapper);
  imageSlice->GetProperty()->SetInterpolationTypeToNearest();

  // Setup renderers
  vtkNew<vtkRenderer> renderer;
  renderer->AddViewProp(imageSlice);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("NavajoWhite").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(300, 300);
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Interpolation");

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

namespace {

void CreateColorImage(vtkImageData* image)
{
  image->SetDimensions(10, 10, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

  vtkNew<vtkNamedColors> colors;
  auto pixelColor = colors->GetColor3ub("Turquoise").GetData();

  for (unsigned int x = 0; x < 10; x++)
  {
    for (unsigned int y = 0; y < 10; y++)
    {
      unsigned char* pixel =
          static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
      for (auto j = 0; j < 3; ++j)
      {
        pixel[j] = pixelColor[j];
      }
    }
  }
}

} // namespace
