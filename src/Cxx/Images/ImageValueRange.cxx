#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkPointData.h>

int main(int, char*[])
{
  // Create an image data
  vtkNew<vtkImageData> imageData;

  // Specify the size of the image data
  imageData->SetDimensions(5, 1, 1);
  imageData->AllocateScalars(VTK_DOUBLE, 1);

  int* dims = imageData->GetDimensions();

  for (int x = 0; x < dims[0]; x++)
  {
    double* pixel = static_cast<double*>(imageData->GetScalarPointer(x, 0, 0));
    pixel[0] = x * 10;
  }

  double valuesRange[2];
  dynamic_cast<vtkDoubleArray*>(
      imageData->GetPointData()->GetArray("ImageScalars"))
      ->GetValueRange(valuesRange);
  std::cout << "valuesRange = " << valuesRange[0] << " " << valuesRange[1]
            << std::endl;

  // Alternatively
  auto min = imageData->GetScalarRange()[0];
  auto max = imageData->GetScalarRange()[1];
  std::cout << "valuesRange = " << min << " " << max << std::endl;

  return EXIT_SUCCESS;
}
