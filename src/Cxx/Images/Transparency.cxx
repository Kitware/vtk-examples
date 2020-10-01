#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkImageMapper3D.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkInteractorStyleImage.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename(.jpg) e.g Gourds2.jpg"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkImageData* image = reader->GetOutput();

  // Create a mask - half of the image should be transparent and the other half
  // opaque
  vtkNew<vtkImageData> maskImage;
  int extent[6];
  image->GetExtent(extent);
  maskImage->SetExtent(extent);
  maskImage->AllocateScalars(VTK_DOUBLE, 1);

  for (int y = extent[2]; y < extent[3]; y++)
  {
    for (int x = extent[0]; x < extent[1]; x++)
    {
      double* pixel =
          static_cast<double*>(maskImage->GetScalarPointer(x, y, 0));
      if (y > (extent[3] - extent[2]) / 2.0)
      {
        pixel[0] = 0.0;
      }
      else
      {
        pixel[0] = 1.0;
      }
    }
  }

  vtkNew<vtkLookupTable> lookupTable;
  lookupTable->SetNumberOfTableValues(2);
  lookupTable->SetRange(0.0, 1.0);
  lookupTable->SetTableValue(0, 0.0, 0.0, 0.0, 0.0); // label 0 is transparent
  lookupTable->SetTableValue(
      1,
      colors->GetColor4d("Khaki").GetData()); // label 1 is opaque and colored
  lookupTable->Build();

  vtkNew<vtkImageMapToColors> mapTransparency;
  mapTransparency->SetLookupTable(lookupTable);
  mapTransparency->PassAlphaToOutputOn();
  mapTransparency->SetInputData(maskImage);

  // Create actors
  vtkNew<vtkImageActor> imageActor;
  imageActor->GetMapper()->SetInputData(image);

  vtkNew<vtkImageActor> maskActor;
  maskActor->GetMapper()->SetInputConnection(mapTransparency->GetOutputPort());

  // Visualize
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(imageActor);
  renderer->AddActor(maskActor);
  renderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());
  renderer->ResetCamera();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Transparency");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
