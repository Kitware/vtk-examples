#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkFloatArray.h>
#include <vtkHedgeHog.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>

namespace {

void CreateData(vtkStructuredGrid* sgrid);
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create the structured grid.
  vtkNew<vtkStructuredGrid> sgrid;
  CreateData(sgrid);

  // We create a simple pipeline to display the data.
  vtkNew<vtkHedgeHog> hedgehog;
  hedgehog->SetInputData(sgrid);
  hedgehog->SetScaleFactor(0.1);

  vtkNew<vtkPolyDataMapper> sgridMapper;
  sgridMapper->SetInputConnection(hedgehog->GetOutputPort());

  vtkNew<vtkActor> sgridActor;
  sgridActor->SetMapper(sgridMapper);
  sgridActor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  // Create the usual rendering stuff
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  renWin->SetWindowName("HedgeHog");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  renderer->AddActor(sgridActor);

  renderer->GradientBackgroundOn();
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
  renderer->SetBackground2(colors->GetColor3d("RoyalBlue").GetData());

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Elevation(60.0);
  renderer->GetActiveCamera()->Azimuth(30.0);
  renderer->GetActiveCamera()->Zoom(0.9);
  renWin->SetSize(300, 300);

  // interact with data
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}

namespace {

void CreateData(vtkStructuredGrid* sgrid)
{
  // int i, j, k, kOffset, jOffset, offset;
  float x[3], v[3], rMin = 0.5, rMax = 1.0;
  // float radius, theta;
  static int dims[3]{13, 11, 11};
  sgrid->SetDimensions(dims);

  // We also create the points and vectors. The points
  // form a hemi-cylinder of data.
  vtkNew<vtkFloatArray> vectors;
  vectors->SetNumberOfComponents(3);
  vectors->SetNumberOfTuples(dims[0] * dims[1] * dims[2]);

  vtkNew<vtkPoints> points;
  points->Allocate(dims[0] * dims[1] * dims[2]);

  auto deltaZ = 2.0 / (dims[2] - 1);
  auto deltaRad = (rMax - rMin) / (dims[1] - 1);
  v[2] = 0.0;
  for (auto k = 0; k < dims[2]; k++)
  {
    x[2] = -1.0 + k * deltaZ;
    auto kOffset = k * dims[0] * dims[1];
    for (auto j = 0; j < dims[1]; j++)
    {
      auto radius = rMin + j * deltaRad;
      auto jOffset = j * dims[0];
      for (auto i = 0; i < dims[0]; i++)
      {
        auto theta = i * vtkMath::RadiansFromDegrees(15.0);
        x[0] = radius * cos(theta);
        x[1] = radius * sin(theta);
        v[0] = -x[1];
        v[1] = x[0];
        auto offset = i + jOffset + kOffset;
        points->InsertPoint(offset, x);
        vectors->InsertTuple(offset, v);
      }
    }
  }
  sgrid->SetPoints(points);

  sgrid->GetPointData()->SetVectors(vectors);
}
} // namespace
