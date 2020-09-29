#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkImageProperty.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkInteractorStyleImage.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {

void CreateRandomImage(vtkImageData* image, const unsigned int dimension);

}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Big image
  vtkNew<vtkImageData> image;
  CreateRandomImage(image, 50);

  vtkNew<vtkImageSliceMapper> imageSliceMapper;
  imageSliceMapper->SetInputData(image);
  imageSliceMapper->BorderOn(); // This line tells the mapper to draw the full
                                // border pixels.
  vtkNew<vtkImageSlice> imageSlice;
  imageSlice->SetMapper(imageSliceMapper);
  imageSlice->GetProperty()->SetInterpolationTypeToNearest();

  vtkNew<vtkRenderer> renderer;
  renderer->AddViewProp(imageSlice);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
  renderer->ResetCamera();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("BorderPixelSize");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {

void CreateRandomImage(vtkImageData* image, const unsigned int dimension)
{
  image->SetDimensions(dimension, dimension, 1);
  image->SetOrigin(.5, .5, 0);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

  unsigned int seed = 0;
  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->Initialize(seed);

  for (unsigned int x = 0; x < dimension; x++)
  {
    for (unsigned int y = 0; y < dimension; y++)
    {
      unsigned char* pixel =
          static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
      pixel[0] = static_cast<unsigned char>(randomSequence->GetValue() * 255);
      randomSequence->Next();
      pixel[1] = static_cast<unsigned char>(randomSequence->GetValue() * 255);
      randomSequence->Next();
      pixel[2] = static_cast<unsigned char>(randomSequence->GetValue() * 255);
      randomSequence->Next();
    }
  }
  image->Modified();
}

} // namespace
