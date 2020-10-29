#include <vtkFastSplatter.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>
#include <vtkImageViewer2.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {
constexpr int SPLAT_IMAGE_SIZE = 100;
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // For the purposes of this example we'll build the splat image by
  // hand.

  vtkNew<vtkImageData> splatImage;
  splatImage->SetDimensions(SPLAT_IMAGE_SIZE, SPLAT_IMAGE_SIZE, 1);
  splatImage->AllocateScalars(VTK_FLOAT, 1);

  for (int i = 0; i < SPLAT_IMAGE_SIZE; ++i)
  {
    for (int j = 0; j < SPLAT_IMAGE_SIZE; ++j)
    {
      double xCoord =
          1 - fabs((i - SPLAT_IMAGE_SIZE / 2) / (SPLAT_IMAGE_SIZE / 2.0));
      double yCoord =
          1 - fabs((j - SPLAT_IMAGE_SIZE / 2) / (SPLAT_IMAGE_SIZE / 2.0));

      splatImage->SetScalarComponentFromDouble(i, j, 0, 0, xCoord * yCoord);
    }
  }

  vtkNew<vtkPolyData> splatPoints;
  vtkNew<vtkPoints> points;

  points->SetNumberOfPoints(5);
  double point[3];

  point[0] = 0;
  point[1] = 0;
  point[2] = 0;
  points->SetPoint(0, point);

  point[0] = 1;
  point[1] = 1;
  point[2] = 0;
  points->SetPoint(1, point);

  point[0] = -1;
  point[1] = 1;
  point[2] = 0;
  points->SetPoint(2, point);

  point[0] = 1;
  point[1] = -1;
  point[2] = 0;
  points->SetPoint(3, point);

  point[0] = -1;
  point[1] = -1;
  point[2] = 0;
  points->SetPoint(4, point);

  splatPoints->SetPoints(points);

  vtkNew<vtkFastSplatter> splatter;
  splatter->SetInputData(splatPoints);
  splatter->SetInputData(1, splatImage);
  splatter->SetOutputDimensions(2 * SPLAT_IMAGE_SIZE, 2 * SPLAT_IMAGE_SIZE, 1);

  // The image viewers and writers are only happy with unsigned char
  // images.  This will convert the floats into that format.
  vtkNew<vtkImageShiftScale> resultScale;
  resultScale->SetOutputScalarTypeToUnsignedChar();
  resultScale->SetShift(0);
  resultScale->SetScale(255);
  resultScale->SetInputConnection(splatter->GetOutputPort());

  splatter->Update();
  resultScale->Update();

  // Set up a viewer for the image.  vtkImageViewer and
  // vtkImageViewer2 are convenient wrappers around vtkActor2D,
  // vtkImageMapper, vtkRenderer, and vtkRenderWindow.  All you need
  // to supply is the interactor and hooray, Bob's your uncle.
  vtkNew<vtkImageViewer2> imageViewer;
  imageViewer->SetInputConnection(resultScale->GetOutputPort());
  imageViewer->SetColorLevel(127);
  imageViewer->SetColorWindow(255);

  vtkNew<vtkRenderWindowInteractor> iren;
  imageViewer->SetupInteractor(iren);

  imageViewer->Render();
  imageViewer->GetRenderer()->SetBackground(
      colors->GetColor3d("SlateGray").GetData());
  imageViewer->GetRenderer()->ResetCamera();

  imageViewer->GetRenderWindow()->SetWindowName("FastSplatter");

  imageViewer->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
