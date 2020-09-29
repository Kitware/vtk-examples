#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkImageProperty.h>
#include <vtkImageShiftScale.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGWriter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {

void CreateImage(vtkImageData*);

}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create an image
  vtkNew<vtkImageData> image;
  CreateImage(image);

  vtkNew<vtkImageShiftScale> shiftScaleFilter;
  shiftScaleFilter->SetOutputScalarTypeToUnsignedChar();
  shiftScaleFilter->SetInputData(image);
  shiftScaleFilter->SetShift(100);
  shiftScaleFilter->SetScale(1);
  shiftScaleFilter->Update();

  // Create actors
  vtkNew<vtkImageSliceMapper> originalSliceMapper;
  originalSliceMapper->SetInputData(image);

  vtkNew<vtkImageSlice> originalSlice;
  originalSlice->SetMapper(originalSliceMapper);
  originalSlice->GetProperty()->SetInterpolationTypeToNearest();

  vtkNew<vtkImageSliceMapper> shiftScaleMapper;
  shiftScaleMapper->SetInputConnection(shiftScaleFilter->GetOutputPort());

  vtkNew<vtkImageSlice> shiftScaleSlice;
  shiftScaleSlice->SetMapper(shiftScaleMapper);
  shiftScaleSlice->GetProperty()->SetInterpolationTypeToNearest();

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double shiftScaleViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddViewProp(originalSlice);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(
      colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> shiftScaleRenderer;
  shiftScaleRenderer->SetViewport(shiftScaleViewport);
  shiftScaleRenderer->AddViewProp(shiftScaleSlice);
  shiftScaleRenderer->ResetCamera();
  shiftScaleRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(shiftScaleRenderer);
  renderWindow->SetWindowName("ImageShiftScale");

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

void CreateImage(vtkImageData* image)
{
  unsigned int dim = 20;

  image->SetDimensions(dim, dim, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  for (unsigned int x = 0; x < dim; x++)
  {
    for (unsigned int y = 0; y < dim; y++)
    {
      unsigned char* pixel =
          static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
      if (x < dim / 2)
      {
        pixel[0] = 50;
      }
      else
      {
        pixel[0] = 150;
      }
    }
  }

  image->Modified();
}

} // namespace
