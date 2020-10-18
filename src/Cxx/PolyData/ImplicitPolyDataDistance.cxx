#include <vtkActor.h>
#include <vtkFloatArray.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkVertexGlyphFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(1.0);
  sphereSource->Update();
  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  sphereMapper->ScalarVisibilityOff();
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetOpacity(0.3);
  sphereActor->GetProperty()->SetColor(1, 0, 0);

  vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance;
  implicitPolyDataDistance->SetInput(sphereSource->GetOutput());

  // Setup a grid
  vtkNew<vtkPoints> points;
  float step = 0.1;
  for (float x = -2.0; x <= 2.0; x += step)
  {
    for (float y = -2.0; y <= 2.0; y += step)
    {
      for (float z = -2.0; z <= 2.0; z += step)
      {
        points->InsertNextPoint(x, y, z);
      }
    }
  }

  // Add distances to each point
  vtkNew<vtkFloatArray> signedDistances;
  signedDistances->SetNumberOfComponents(1);
  signedDistances->SetName("SignedDistances");

  // Evaluate the signed distance function at all of the grid points
  for (vtkIdType pointId = 0; pointId < points->GetNumberOfPoints(); ++pointId)
  {
    double p[3];
    points->GetPoint(pointId, p);
    float signedDistance = implicitPolyDataDistance->EvaluateFunction(p);
    signedDistances->InsertNextValue(signedDistance);
  }

  vtkNew<vtkPolyData> polyData;
  polyData->SetPoints(points);
  polyData->GetPointData()->SetScalars(signedDistances);

  vtkNew<vtkVertexGlyphFilter> vertexGlyphFilter;
  vertexGlyphFilter->SetInputData(polyData);
  vertexGlyphFilter->Update();

  vtkNew<vtkPolyDataMapper> signedDistanceMapper;
  signedDistanceMapper->SetInputConnection(vertexGlyphFilter->GetOutputPort());
  signedDistanceMapper->ScalarVisibilityOn();

  vtkNew<vtkActor> signedDistanceActor;
  signedDistanceActor->SetMapper(signedDistanceMapper);

  vtkNew<vtkRenderer> renderer;
  renderer->AddViewProp(sphereActor);
  renderer->AddViewProp(signedDistanceActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ImplicitPolyDataDistance");

  vtkNew<vtkRenderWindowInteractor> renWinInteractor;
  renWinInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  renWinInteractor->Start();

  return EXIT_SUCCESS;
}
