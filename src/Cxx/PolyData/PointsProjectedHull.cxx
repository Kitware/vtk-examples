#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPointsProjectedHull.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTubeFilter.h>
#include <vtkXMLPolyDataWriter.h>

#include <memory>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphere;
  sphere->SetPhiResolution(21);
  sphere->SetThetaResolution(21);
  sphere->SetRadius(.01);

  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(40);
  pointSource->Update();

  // Setup actor and mapper
  vtkNew<vtkGlyph3DMapper> pointMapper;
  pointMapper->SetInputConnection(pointSource->GetOutputPort());
  pointMapper->SetSourceConnection(sphere->GetOutputPort());

  vtkNew<vtkActor> pointActor;
  pointActor->SetMapper(pointMapper);
  pointActor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

  vtkNew<vtkPointsProjectedHull> points;
  points->DeepCopy(pointSource->GetOutput()->GetPoints());

  int xSize = points->GetSizeCCWHullX();
  std::cout << "xSize: " << xSize << std::endl;

  std::unique_ptr<double[]> pts{new double[xSize * 2]};

  points->GetCCWHullX(pts.get(), xSize);

  vtkNew<vtkPoints> xHullPoints;
  for (int i = 0; i < xSize; i++)
  {
    double yval = pts[2 * i];
    double zval = pts[2 * i + 1];
    std::cout << "(y,z) value of point " << i << " : (" << yval << " , " << zval
              << ")" << std::endl;
    xHullPoints->InsertNextPoint(0.0, yval, zval);
  }
  // Insert the first point again to close the loop
  xHullPoints->InsertNextPoint(0.0, pts[0], pts[1]);

  // Display the x hull
  vtkNew<vtkPolyLine> xPolyLine;
  xPolyLine->GetPointIds()->SetNumberOfIds(xHullPoints->GetNumberOfPoints());

  for (vtkIdType i = 0; i < xHullPoints->GetNumberOfPoints(); i++)
  {
    xPolyLine->GetPointIds()->SetId(i, i);
  }

  // Create a cell array to store the lines in and add the lines to it
  vtkNew<vtkCellArray> cells;
  cells->InsertNextCell(xPolyLine);

  // Create a polydata to store everything in
  vtkNew<vtkPolyData> polyData;

  // Add the points to the dataset
  polyData->SetPoints(xHullPoints);

  // Add the lines to the dataset
  polyData->SetLines(cells);

  // Create tubes around the lines
  vtkNew<vtkTubeFilter> tubes;
  tubes->SetInputData(polyData);
  tubes->SetRadius(0.005);
  tubes->SetNumberOfSides(21);

  // Setup actor and mapper
  vtkNew<vtkPolyDataMapper> xHullMapper;
  xHullMapper->SetInputConnection(tubes->GetOutputPort());

  vtkNew<vtkActor> xHullActor;
  xHullActor->SetMapper(xHullMapper);
  xHullActor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

  // Setup render window, renderer, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("PointsProjectedHull");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
  renderer->AddActor(xHullActor);
  renderer->AddActor(pointActor);

  // Rotate camera
  renderer->GetActiveCamera()->Azimuth(90);
  renderer->ResetCamera();

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
