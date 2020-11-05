#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkDoubleArray.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTensorGlyph.h>
#include <vtkVertexGlyphFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Setup points
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(5.0, 0.0, 0.0);

  vtkNew<vtkPolyData> polyData;
  polyData->SetPoints(points);

  vtkNew<vtkDoubleArray> tensors;
  tensors->SetNumberOfTuples(2);
  tensors->SetNumberOfComponents(9);

  tensors->InsertTuple9(0, 1, 0, 0, 0, 1, 0, 0, 0, 1);
  tensors->InsertTuple9(1, 1, 0, 0, 0, 0.7, 0.7, 0, -0.7, 0.7); // column major

  polyData->GetPointData()->SetTensors(tensors);

  vtkNew<vtkCubeSource> cubeSource;
  cubeSource->Update();

  vtkNew<vtkTensorGlyph> tensorGlyph;
  tensorGlyph->SetInputData(polyData);
  tensorGlyph->SetSourceConnection(cubeSource->GetOutputPort());
  tensorGlyph->ColorGlyphsOff();
  tensorGlyph->ThreeGlyphsOff();
  tensorGlyph->ExtractEigenvaluesOff();
  tensorGlyph->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(tensorGlyph->GetOutput());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("LightSalmon").GetData());

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("TensorGlyph");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
