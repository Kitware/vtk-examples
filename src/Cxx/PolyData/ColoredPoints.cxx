#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVertexGlyphFilter.h>

#include <vtkNamedColors.h>

// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

int main(int, char*[])
{
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(1.0, 0.0, 0.0);
  points->InsertNextPoint(0.0, 1.0, 0.0);

  vtkNew<vtkPolyData> pointsPolydata;

  pointsPolydata->SetPoints(points);

  vtkNew<vtkVertexGlyphFilter> vertexFilter;
  vertexFilter->SetInputData(pointsPolydata);
  vertexFilter->Update();

  vtkNew<vtkPolyData> polydata;
  polydata->ShallowCopy(vertexFilter->GetOutput());

  // Setup colors
  vtkNew<vtkNamedColors> namedColors;

  vtkNew<vtkUnsignedCharArray> colors;
  colors->SetNumberOfComponents(3);
  colors->SetName("Colors");
  colors->InsertNextTupleValue(namedColors->GetColor3ub("Tomato").GetData());
  colors->InsertNextTupleValue(namedColors->GetColor3ub("Mint").GetData());
  colors->InsertNextTupleValue(namedColors->GetColor3ub("Peacock").GetData());

  polydata->GetPointData()->SetScalars(colors);

  // Visualization
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polydata);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetPointSize(10);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ColoredPoints");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(namedColors->GetColor3d("Burlywood").GetData());
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
