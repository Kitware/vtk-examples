#include <vtkImageData.h>
#include <vtkPoints.h>
#include <vtkImageDataToPointSet.h>
#include <vtkNew.h>
#include <vtkXMLStructuredGridWriter.h>
#include <vtkStructuredGrid.h>

namespace {

void CreateImage(vtkImageData* image);
}

int main(int, char *[])
{
  vtkNew<vtkImageData> image;
  CreateImage(image);

  vtkNew<vtkImageDataToPointSet> imageDataToPointSet;
  imageDataToPointSet->SetInputData(image);
  imageDataToPointSet->Update();

  vtkNew<vtkXMLStructuredGridWriter> writer;
  writer->SetInputConnection(imageDataToPointSet->GetOutputPort());
  writer->SetFileName("Test.vts");
  writer->Write();

  return EXIT_SUCCESS;
}

namespace {

void CreateImage(vtkImageData* image)
{
  // Create an image
  image->SetDimensions(3,3,1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR,1);

  int dims[3];
  image->GetDimensions(dims);
  for(int i = 0; i < dims[0]; i++)
  {
    for(int j = 0; j < dims[1]; j++)
    {
      unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(i,j,0));
      pixel[0] = 1;
    }
  }
}

}
