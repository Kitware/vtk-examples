#include <vtkImageData.h>
#include <vtkImageExport.h>
#include <vtkNew.h>

#include <memory>

int main(int, char*[])
{
  // Create an image data
  vtkNew<vtkImageData> imageData;

  // Specify the size of the image data
  constexpr int dims[3] = {2, 3, 1};

  imageData->SetDimensions(dims[0], dims[1], dims[2]);
  imageData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  unsigned char value = 0;
  for (int row = 0; row < dims[0]; ++row)
  {
    for (int col = 0; col < dims[1]; ++col)
    {
      unsigned char* pixel =
          static_cast<unsigned char*>(imageData->GetScalarPointer(row, col, 0));
      pixel[0] = value;
      value += 10;
    }
  }

  // Create the c-style image to convert the VTK image to
  std::unique_ptr<unsigned char> cImage(
      new unsigned char[dims[0] * dims[1] * dims[2]]);

  vtkNew<vtkImageExport> exporter;
  exporter->SetInputData(imageData);
  exporter->ImageLowerLeftOn();
  exporter->Export(cImage.get());
  exporter->Update();

  // Output the raw c-style image
  for (int row = 0; row < dims[0]; ++row)
  {
    for (int col = 0; col < dims[1]; ++col)
    {
      std::cout << static_cast<int>(cImage.get()[col * dims[0] + row]) << " ";
    }
    std::cout << std::endl;
  }

  return EXIT_SUCCESS;
}
