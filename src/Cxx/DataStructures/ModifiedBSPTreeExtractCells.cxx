#include <vtkExtractCells.h>
#include <vtkIdList.h>
#include <vtkLine.h>
#include <vtkLineSource.h>
#include <vtkModifiedBSPTree.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>

#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetPhiResolution(7);
  sphereSource->SetThetaResolution(15);
  sphereSource->Update();

  // Create the locator
  vtkNew<vtkModifiedBSPTree> tree;
  tree->SetDataSet(sphereSource->GetOutput());

  // Intersect the locator with the line
  double lineP0[3] = {-0.6, -0.6, -0.6};
  double lineP1[3] = {0.6, 0.6, 0.6};
  vtkNew<vtkPoints> intersectPoints;

  vtkNew<vtkIdList> intersectCells;

  double tol = 1.0e-8;
  tree->IntersectWithLine(lineP0, lineP1, tol, intersectPoints, intersectCells);

  std::cout << "NumPoints: " << intersectPoints->GetNumberOfPoints()
            << std::endl;

  // Display list of intersections
  double intersection[3];
  for (int i = 0; i < intersectPoints->GetNumberOfPoints(); i++)
  {
    intersectPoints->GetPoint(i, intersection);
    std::cout << "\tPoint Intersection " << i << ": " << intersection[0] << ", "
              << intersection[1] << ", " << intersection[2] << std::endl;
  }

  std::cout << "NumCells: " << intersectCells->GetNumberOfIds() << std::endl;

  vtkIdType cellId;
  for (int i = 0; i < intersectCells->GetNumberOfIds(); i++)
  {
    cellId = intersectCells->GetId(i);
    std::cout << "\tCellId " << i << ": " << cellId << std::endl;
  }

  // Render the line, sphere and intersected cells
  vtkNew<vtkLineSource> lineSource;
  lineSource->SetPoint1(lineP0);
  lineSource->SetPoint2(lineP1);

  vtkNew<vtkPolyDataMapper> lineMapper;
  lineMapper->SetInputConnection(lineSource->GetOutputPort());
  vtkNew<vtkActor> lineActor;
  lineActor->SetMapper(lineMapper);

  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetRepresentationToWireframe();
  sphereActor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  vtkNew<vtkExtractCells> cellSource;
  cellSource->SetInputConnection(sphereSource->GetOutputPort());
  cellSource->SetCellList(intersectCells);

  vtkNew<vtkDataSetMapper> cellMapper;
  cellMapper->SetInputConnection(cellSource->GetOutputPort());
  vtkNew<vtkActor> cellActor;
  cellActor->SetMapper(cellMapper);
  cellActor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(lineActor);
  renderer->AddActor(sphereActor);
  renderer->AddActor(cellActor);
  renderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  renderWindow->SetWindowName("ModifiedBSPTreeExtractCells");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
