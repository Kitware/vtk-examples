#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDelaunay2D.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmoothPolyDataFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create parabola over a grid of points
  vtkNew<vtkPoints> points;

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(8775070);

  int GridSize = 20;
  double z;
  for (int x = -GridSize; x < GridSize; x++)
  {
    for (int y = -GridSize; y < GridSize; y++)
    {
      z = randomSequence->GetRangeValue(-1, 1) + 0.05 * x * x + 0.05 * y * y;
      randomSequence->Next();
      points->InsertNextPoint(x, y, z);
    }
  }

  // Add the grid points to a polydata object
  vtkNew<vtkPolyData> inputPolyData;
  inputPolyData->SetPoints(points);

  // Triangulate the grid points
  vtkNew<vtkDelaunay2D> delaunay;
  delaunay->SetInputData(inputPolyData);
  delaunay->Update();

  vtkNew<vtkSmoothPolyDataFilter> smoothFilter;
  smoothFilter->SetInputConnection(delaunay->GetOutputPort());
  smoothFilter->SetNumberOfIterations(15);
  smoothFilter->SetRelaxationFactor(0.1);
  smoothFilter->FeatureEdgeSmoothingOff();
  smoothFilter->BoundarySmoothingOn();
  smoothFilter->Update();

  // Update normals on newly smoothed polydata
  vtkNew<vtkPolyDataNormals> normalGenerator;
  normalGenerator->SetInputConnection(smoothFilter->GetOutputPort());
  normalGenerator->ComputePointNormalsOn();
  normalGenerator->ComputeCellNormalsOn();
  normalGenerator->Update();

  vtkNew<vtkPolyDataMapper> inputMapper;
  inputMapper->SetInputConnection(delaunay->GetOutputPort());
  vtkNew<vtkActor> inputActor;
  inputActor->SetMapper(inputMapper);
  inputActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  vtkNew<vtkPolyDataMapper> smoothedMapper;
  smoothedMapper->SetInputConnection(normalGenerator->GetOutputPort());
  vtkNew<vtkActor> smoothedActor;
  smoothedActor->SetMapper(smoothedMapper);
  smoothedActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("SmoothPolyDataFilter");

  // And one interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup both renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("BurlyWood").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("CadetBlue").GetData());

  // Add the input parabola to the left and the smoothed parabola to the right
  leftRenderer->AddActor(inputActor);
  rightRenderer->AddActor(smoothedActor);

  leftRenderer->ResetCamera();
  leftRenderer->GetActiveCamera()->Azimuth(130);
  leftRenderer->GetActiveCamera()->Elevation(-80);
  vtkCamera* camera;
  camera = leftRenderer->GetActiveCamera();

  rightRenderer->ResetCamera();
  rightRenderer->GetActiveCamera()->Azimuth(130);
  rightRenderer->GetActiveCamera()->Elevation(-80);
  rightRenderer->SetActiveCamera(camera);

  leftRenderer->ResetCamera();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
