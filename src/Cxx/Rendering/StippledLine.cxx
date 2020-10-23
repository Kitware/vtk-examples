#include <vtkActor.h>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include <vtkLineSource.h>
#include <vtkMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>

namespace {
void StippledLine(vtkActor* actor, int LineStipplePattern = 0xFFFF,
                  int LineStippleRepeat = 1);
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkLineSource> lines;
  // Create two points, P0 and P1
  double p0[3] = {1.0, 0.0, 0.0};
  double p1[3] = {5.0, 0.0, 0.0};

  lines->SetResolution(11);
  lines->SetPoint1(p0);
  lines->SetPoint2(p1);
  lines->Update();
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(lines->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetLineWidth(5);
  actor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());

  StippledLine(actor, 0xA1A1, 2);

  vtkNew<vtkRenderer> ren1;
  ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());
  vtkNew<vtkRenderWindow> renWin;
  renWin->SetSize(640, 480);
  renWin->SetWindowName("StippledLine");

  renWin->AddRenderer(ren1);
  vtkSmartPointer<vtkRenderWindowInteractor> iren =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renWin);
  ren1->AddActor(actor);

  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}

namespace {
void StippledLine(vtkActor* actor, int lineStipplePattern,
                  int lineStippleRepeat)
{
  vtkNew<vtkDoubleArray> tcoords;
  vtkNew<vtkImageData> image;
  vtkNew<vtkTexture> texture;

  // Create texture
  int dimension = 16 * lineStippleRepeat;

  image->SetDimensions(dimension, 1, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 4);
  image->SetExtent(0, dimension - 1, 0, 0, 0, 0);
  unsigned char* pixel;
  pixel = static_cast<unsigned char*>(image->GetScalarPointer());
  unsigned char on = 255;
  unsigned char off = 0;
  for (int i = 0; i < 16; ++i)
  {
    unsigned int mask = (1 << i);
    unsigned int bit = (lineStipplePattern & mask) >> i;
    unsigned char value = static_cast<unsigned char>(bit);
    if (value == 0)
    {
      for (int j = 0; j < lineStippleRepeat; ++j)
      {
        *pixel = on;
        *(pixel + 1) = on;
        *(pixel + 2) = on;
        *(pixel + 3) = off;
        pixel += 4;
      }
    }
    else
    {
      for (int j = 0; j < lineStippleRepeat; ++j)
      {
        *pixel = on;
        *(pixel + 1) = on;
        *(pixel + 2) = on;
        *(pixel + 3) = on;
        pixel += 4;
      }
    }
  }
  vtkPolyData* polyData =
      dynamic_cast<vtkPolyData*>(actor->GetMapper()->GetInput());

  // Create texture coordnates
  tcoords->SetNumberOfComponents(1);
  tcoords->SetNumberOfTuples(polyData->GetNumberOfPoints());
  for (int i = 0; i < polyData->GetNumberOfPoints(); ++i)
  {
    double value = static_cast<double>(i) * .5;
    tcoords->SetTypedTuple(i, &value);
  }

  polyData->GetPointData()->SetTCoords(tcoords);
  texture->SetInputData(image);
  texture->InterpolateOff();
  texture->RepeatOn();

  actor->SetTexture(texture);
}
} // namespace
