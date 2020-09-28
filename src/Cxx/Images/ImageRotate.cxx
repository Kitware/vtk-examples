#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReslice.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify command line arguments
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " InputFilename e.g. Gourds.png"
              << std::endl;
    return EXIT_FAILURE;
  }

  double angle = 45;
  if (argc > 2)
  {
    angle = atof(argv[2]);
  }

  // Read file
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);
  reader->Update();
  double bounds[6];
  reader->GetOutput()->GetBounds(bounds);

  // Rotate about the center of the image
  vtkNew<vtkTransform> transform;

  // Compute the center of the image
  double center[3];
  center[0] = (bounds[1] + bounds[0]) / 2.0;
  center[1] = (bounds[3] + bounds[2]) / 2.0;
  center[2] = (bounds[5] + bounds[4]) / 2.0;

  // Rotate about the center
  transform->Translate(center[0], center[1], center[2]);
  transform->RotateWXYZ(angle, 0, 0, 1);
  transform->Translate(-center[0], -center[1], -center[2]);

  // Reslice does all of the work
  vtkNew<vtkImageReslice> reslice;
  reslice->SetInputConnection(reader->GetOutputPort());
  reslice->SetResliceTransform(transform);
  reslice->SetInterpolationModeToCubic();
  reslice->SetOutputSpacing(reader->GetOutput()->GetSpacing()[0],
                            reader->GetOutput()->GetSpacing()[1],
                            reader->GetOutput()->GetSpacing()[2]);
  reslice->SetOutputOrigin(reader->GetOutput()->GetOrigin()[0],
                           reader->GetOutput()->GetOrigin()[1],
                           reader->GetOutput()->GetOrigin()[2]);
  reslice->SetOutputExtent(
      reader->GetOutput()->GetExtent()); // Use a larger extent than the
                                         // original image's to prevent clipping

  // Visualize
  vtkNew<vtkImageViewer2> imageViewer;
  imageViewer->SetInputConnection(reslice->GetOutputPort());
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  imageViewer->SetupInteractor(renderWindowInteractor);
  imageViewer->GetRenderer()->SetBackground(
      colors->GetColor3d("RoyalBlue").GetData());
  imageViewer->GetRenderWindow()->SetWindowName("ImageRotate");
  imageViewer->Render();
  imageViewer->GetRenderer()->ResetCamera();
  imageViewer->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
