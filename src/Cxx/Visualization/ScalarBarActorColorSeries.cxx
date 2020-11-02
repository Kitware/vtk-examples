#include <vtkActor.h>
#include <vtkColorSeries.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere fora some geometry
  vtkNew<vtkSphereSource> sphere;
  sphere->SetCenter(0, 0, 0);
  sphere->SetRadius(1);
  sphere->SetPhiResolution(30);
  sphere->SetThetaResolution(60);
  sphere->Update();

  // Create scalar data to associate with the vertices of the sphere
  int numPts = sphere->GetOutput()->GetPoints()->GetNumberOfPoints();
  vtkNew<vtkFloatArray> scalars;
  scalars->SetNumberOfValues(numPts);
  for (int i = 0; i < numPts; ++i)
  {
    scalars->SetValue(i, static_cast<float>(i) / numPts);
  }
  vtkNew<vtkPolyData> poly;
  poly->DeepCopy(sphere->GetOutput());
  poly->GetPointData()->SetScalars(scalars);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(poly);
  mapper->ScalarVisibilityOn();
  mapper->SetScalarModeToUsePointData();
  mapper->SetColorModeToMapScalars();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetInterpolationToFlat();

  vtkNew<vtkScalarBarActor> scalarBar;
  scalarBar->SetLookupTable(mapper->GetLookupTable());
  scalarBar->SetTitle("Beachball");
  scalarBar->SetNumberOfLabels(4);

  // Create a lookup table to share between the mapper and the scalarbar
  vtkNew<vtkLookupTable> seriesLut;

  vtkNew<vtkColorSeries> series;
  int seriesEnum = series->BREWER_QUALITATIVE_SET3;
  series->SetColorScheme(seriesEnum);
  series->BuildLookupTable(seriesLut, series->ORDINAL);

  mapper->SetLookupTable(seriesLut);
  scalarBar->SetLookupTable(seriesLut);

  // Create a renderer and render window
  vtkNew<vtkRenderer> renderer;

  renderer->GradientBackgroundOn();
  renderer->SetBackground2(colors->GetColor3d("NavajoWhite").GetData());
  renderer->SetBackground(colors->GetColor3d("DarkSlateBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ScalarBarActorColorSeries");

  // Create an interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->AddActor2D(scalarBar);

  // Render the scene (lights and cameras are created automatically)
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
