#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkDataArray.h>
#include <vtkIntArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkVertexGlyphFilter.h>

int main(int, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // cube centered in origin, 1cm side.
  vtkNew<vtkCubeSource> cubeSource;
  cubeSource->Update();

  vtkPolyData* cube = cubeSource->GetOutput();

  double testInside[3] = {0.0, 0.0, 0.0};
  double testOutside[3] = {0.7, 0.0, 0.0};
  double testBorderOutside[3] = {0.5, 0.0, 0.0};
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(testInside);
  points->InsertNextPoint(testOutside);
  points->InsertNextPoint(testBorderOutside);

  vtkNew<vtkPolyData> pointsPolydata;
  pointsPolydata->SetPoints(points);

  // Points inside test
  vtkNew<vtkSelectEnclosedPoints> selectEnclosedPoints;
  selectEnclosedPoints->SetInputData(pointsPolydata);
  selectEnclosedPoints->SetSurfaceData(cube);
  selectEnclosedPoints->Update();

  for (unsigned int i = 0; i < 2; i++)
  {
    std::cout << "Point " << i << ": ";
    if (selectEnclosedPoints->IsInside(i) == 1)
    {
      std::cout << "inside" << std::endl;
    }
    else
    {
      std::cout << "outside" << std::endl;
    }
  }

  auto insideArray = dynamic_cast<vtkDataArray*>(
      selectEnclosedPoints->GetOutput()->GetPointData()->GetArray(
          "SelectedPoints"));

  for (vtkIdType i = 0; i < insideArray->GetNumberOfTuples(); i++)
  {
    std::cout << "Tuple " << i << ": ";
    if (insideArray->GetComponent(i, 0) == 1)
    {
      std::cout << "inside" << std::endl;
    }
    else
    {
      std::cout << "outside" << std::endl;
    }
  }

  // RENDERING PART

  // Cube mapper, actor
  vtkNew<vtkPolyDataMapper> cubeMapper;
  cubeMapper->SetInputConnection(cubeSource->GetOutputPort());

  vtkNew<vtkActor> cubeActor;
  cubeActor->SetMapper(cubeMapper);
  cubeActor->GetProperty()->SetOpacity(0.5);
  cubeActor->GetProperty()->SetColor(colors->GetColor3d("SandyBrown").GetData());

  // Points mapper, actor
  // First, apply vtkVertexGlyphFilter to make cells around points, vtk only
  // render cells.
  vtkNew<vtkVertexGlyphFilter> vertexGlyphFilter;
  vertexGlyphFilter->AddInputData(pointsPolydata);
  vertexGlyphFilter->Update();

  vtkNew<vtkPolyDataMapper> pointsMapper;
  pointsMapper->SetInputConnection(vertexGlyphFilter->GetOutputPort());

  vtkNew<vtkActor> pointsActor;
  pointsActor->SetMapper(pointsMapper);
  pointsActor->GetProperty()->SetPointSize(5);
  pointsActor->GetProperty()->SetColor(colors->GetColor3d("GreenYellow").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("PointCellIds");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(cubeActor);
  renderer->AddActor(pointsActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
