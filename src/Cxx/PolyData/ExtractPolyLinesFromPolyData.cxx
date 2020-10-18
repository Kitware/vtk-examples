#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkColor.h>
#include <vtkCutter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkStripper.h>

#ifdef VTK_CELL_ARRAY_V2
#include <vtkCellArrayIterator.h>
#endif // VTK_CELL_ARRAY_V2

int main(int, char*[])
{
  // Define colors for example
  vtkNew<vtkNamedColors> colors;
  vtkColor3d lineColor = colors->GetColor3d("peacock");
  vtkColor3d modelColor = colors->GetColor3d("silver");
  vtkColor3d backgroundColor = colors->GetColor3d("wheat");

  vtkNew<vtkSphereSource> modelSource;

  vtkNew<vtkPlane> plane;

  vtkNew<vtkCutter> cutter;
  cutter->SetInputConnection(modelSource->GetOutputPort());
  cutter->SetCutFunction(plane);
  cutter->GenerateValues(10, -0.5, 0.5);

  vtkNew<vtkPolyDataMapper> modelMapper;
  modelMapper->SetInputConnection(modelSource->GetOutputPort());

  vtkNew<vtkActor> model;
  model->SetMapper(modelMapper);
  model->GetProperty()->SetDiffuseColor(modelColor.GetData());
  model->GetProperty()->SetInterpolationToFlat();

  vtkNew<vtkStripper> stripper;
  stripper->SetInputConnection(cutter->GetOutputPort());
  stripper->JoinContiguousSegmentsOn();

  vtkNew<vtkPolyDataMapper> linesMapper;
  linesMapper->SetInputConnection(stripper->GetOutputPort());

  vtkNew<vtkActor> lines;
  lines->SetMapper(linesMapper);
  lines->GetProperty()->SetDiffuseColor(lineColor.GetData());
  lines->GetProperty()->SetLineWidth(3.0);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;

  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("ExtractPolyLinesFromPolyData");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer
  renderer->AddActor(model);
  renderer->AddActor(lines);
  renderer->SetBackground(backgroundColor.GetData());
  renderer->GetActiveCamera()->Azimuth(-45);
  renderer->GetActiveCamera()->Elevation(-22.5);
  renderer->ResetCamera();

  // This starts the event loop and as a side effect causes an initial
  // render.
  renderWindow->Render();

  interactor->Start();

  // Extract the lines from the polydata
  vtkIdType numberOfLines = cutter->GetOutput()->GetNumberOfLines();

  std::cout << "-----------Lines without using vtkStripper" << std::endl;
  std::cout << "There are " << numberOfLines << " lines in the polydata"
            << std::endl;

  numberOfLines = stripper->GetOutput()->GetNumberOfLines();
  vtkPoints* points = stripper->GetOutput()->GetPoints();
  vtkCellArray* cells = stripper->GetOutput()->GetLines();

  std::cout << "-----------Lines using vtkStripper" << std::endl;
  std::cout << "There are " << numberOfLines << " lines in the polydata"
            << std::endl;

#ifdef VTK_CELL_ARRAY_V2

  // Newer versions of vtkCellArray prefer local iterators:
  auto cellIter = vtk::TakeSmartPointer(cells->NewIterator());
  for (cellIter->GoToFirstCell(); !cellIter->IsDoneWithTraversal();
       cellIter->GoToNextCell())
  {
    std::cout << "Line " << cellIter->GetCurrentCellId() << ":\n";

    vtkIdList* cell = cellIter->GetCurrentCell();
    for (vtkIdType i = 0; i < cell->GetNumberOfIds(); ++i)
    {
      double point[3];
      points->GetPoint(cell->GetId(i), point);
      std::cout << "\t(" << point[0] << ", " << point[1] << ", " << point[2]
                << ")" << std::endl;
    }
  }

#else // VTK_CELL_ARRAY_V2

  // Older implementations of vtkCellArray use internal iterator APIs (not
  // thread safe):
  vtkIdType* indices;
  vtkIdType numberOfPoints;
  unsigned int lineCount = 0;
  for (cells->InitTraversal(); cells->GetNextCell(numberOfPoints, indices);
       lineCount++)
  {
    std::cout << "Line " << lineCount << ": " << std::endl;
    for (vtkIdType i = 0; i < numberOfPoints; i++)
    {
      double point[3];
      points->GetPoint(indices[i], point);
      std::cout << "\t(" << point[0] << ", " << point[1] << ", " << point[2]
                << ")" << std::endl;
    }
  }

#endif // VTK_CELL_ARRAY_V2

  return EXIT_SUCCESS;
}
