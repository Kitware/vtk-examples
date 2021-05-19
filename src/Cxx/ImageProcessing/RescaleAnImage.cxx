#include <vtkImageData.h>
#include <vtkImageShiftScale.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNew.h>

namespace {
void CreateImage(vtkImageData* const image,
                 vtkMinimalStandardRandomSequence* rng);
}

int main(int, char*[])
{
  vtkNew<vtkMinimalStandardRandomSequence> rng;
  rng->SetSeed(8775070);
  // rng->SetSeed(0);

  vtkNew<vtkImageData> image;
  CreateImage(image, rng);

  vtkNew<vtkImageShiftScale> shiftScaleFilter;
  shiftScaleFilter->SetOutputScalarTypeToUnsignedChar();
  shiftScaleFilter->SetInputData(image);
  shiftScaleFilter->SetShift(
      -1.0 * image->GetScalarRange()[0]); // brings the lower bound to 0
  double oldRange = image->GetScalarRange()[1] - image->GetScalarRange()[0];
  std::cout << "Old range: [" << image->GetScalarRange()[0] << ", "
            << image->GetScalarRange()[1] << "]" << std::endl;
  std::cout << "Old range magnitude: " << oldRange << std::endl;
  double newRange = 255; // We want the output [0,255]
  shiftScaleFilter->SetScale(newRange / oldRange);
  shiftScaleFilter->Update();

  std::cout << "New range: ["
            << shiftScaleFilter->GetOutput()->GetScalarRange()[0] << ", "
            << shiftScaleFilter->GetOutput()->GetScalarRange()[1] << "]"
            << std::endl;

  return EXIT_SUCCESS;
}

namespace {

void CreateImage(vtkImageData* const image,
                 vtkMinimalStandardRandomSequence* rng)
{
  // Specify the size of the image data
  image->SetDimensions(2, 3, 1);
  image->AllocateScalars(VTK_DOUBLE, 1);

  int* dims = image->GetDimensions();

  std::cout << "Dims: "
            << " x: " << dims[0] << " y: " << dims[1] << " z: " << dims[2]
            << std::endl;

  for (int z = 0; z < dims[2]; z++)
  {
    for (int y = 0; y < dims[1]; y++)
    {
      for (int x = 0; x < dims[0]; x++)
      {
        double* pixel = static_cast<double*>(image->GetScalarPointer(x, y, z));
        pixel[0] = rng->GetRangeValue(0.0, 2000.0);
        rng->Next();
      }
    }
  }
}

} // namespace
