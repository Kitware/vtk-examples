#include <vtkActor.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNew.h>
#include <vtkSphereSource.h>
#include <vtkWarpScalar.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  // Create Scalars
  vtkNew<vtkDoubleArray> scalars;
  int numberOfPoints = sphereSource->GetOutput()->GetNumberOfPoints();
  scalars->SetNumberOfTuples(numberOfPoints);

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(8775070);
  for (vtkIdType i = 0; i < numberOfPoints; ++i)
  {
    scalars->SetTuple1(i, randomSequence->GetRangeValue(0.0, 1.0 / 7.0));
    randomSequence->Next();
  }

  sphereSource->GetOutput()->GetPointData()->SetScalars(scalars);

  vtkNew<vtkWarpScalar> warpScalar;
  warpScalar->SetInputConnection(sphereSource->GetOutputPort());
  warpScalar->SetScaleFactor(1); // use the scalars themselves

  // Required for SetNormal to have an effect
  warpScalar->UseNormalOn();
  warpScalar->SetNormal(0, 0, 1);

  warpScalar->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(warpScalar->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("WarpScalar");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Cornsilk").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
