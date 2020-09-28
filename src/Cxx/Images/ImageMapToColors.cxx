//
// Displays a "grayscale" image as a full color image via the
// vtkImageMapToColors filter, which uses a lookup table to
// map scalar values to colors
//
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkImageMapper3D.h>
#include <vtkImageProperty.h>
#include <vtkInteractorStyleImage.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a "grayscale" 16x16 image, 1-component pixels of type "double"
  vtkNew<vtkImageData> image;
  int imageExtent[6] = {0, 15, 0, 15, 0, 0};
  image->SetExtent(imageExtent);
  image->AllocateScalars(VTK_DOUBLE, 1);

  double scalarvalue = 0.0;

  for (int y = imageExtent[2]; y <= imageExtent[3]; y++)
  {
    for (int x = imageExtent[0]; x <= imageExtent[1]; x++)
    {
      double* pixel = static_cast<double*>(image->GetScalarPointer(x, y, 0));
      pixel[0] = scalarvalue;
      scalarvalue += 1.0;
    }
  }

  // Map the scalar values in the image to colors with a lookup table:
  vtkNew<vtkLookupTable> lookupTable;
  lookupTable->SetNumberOfTableValues(256);
  lookupTable->SetRange(0.0, 255.0);
  lookupTable->Build();

  // Pass the original image and the lookup table to a filter to create
  // a color image:
  vtkNew<vtkImageMapToColors> scalarValuesToColors;
  scalarValuesToColors->SetLookupTable(lookupTable);
  scalarValuesToColors->PassAlphaToOutputOn();
  scalarValuesToColors->SetInputData(image);

  // Create an image actor
  vtkNew<vtkImageActor> imageActor;
  imageActor->GetMapper()->SetInputConnection(
      scalarValuesToColors->GetOutputPort());
  imageActor->GetProperty()->SetInterpolationTypeToNearest();

  // Visualize
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(imageActor);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ImageMapToColors");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
