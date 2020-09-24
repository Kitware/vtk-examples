#include <vtkActor.h>
#include <vtkImageData.h>
#include <vtkImageReslice.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkImageData> image;
  image->SetExtent(0, 9, 0, 9, 0, 0);
  image->AllocateScalars(VTK_INT, 1);

  int* pixel = static_cast<int*>(image->GetScalarPointer(0, 9, 0));
  std::cout << "pixel: " << *pixel << std::endl;

  vtkNew<vtkImageReslice> reslice;
  reslice->SetOutputExtent(0, 9, 0, 100, 0, 0);
  reslice->SetInputData(image);
  reslice->Update();

  int* pixel2 =
      static_cast<int*>(reslice->GetOutput()->GetScalarPointer(0, 11, 0));
  std::cout << "pixel2: " << *pixel2 << std::endl;

  return EXIT_SUCCESS;
}
