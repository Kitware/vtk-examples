#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkDecimatePolylineFilter.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  const unsigned int numberofpoints = 100;

  vtkNew<vtkPolyData> circle;
  vtkNew<vtkPoints> points;
  vtkNew<vtkCellArray> lines;
  vtkIdType* lineIndices = new vtkIdType[numberofpoints + 1];

  for (unsigned int i = 0; i < numberofpoints; i++)
  {
    const double angle = 2.0 * vtkMath::Pi() * static_cast<double>(i) /
        static_cast<double>(numberofpoints);
    points->InsertPoint(static_cast<vtkIdType>(i), cos(angle), sin(angle), 0.);
    lineIndices[i] = static_cast<vtkIdType>(i);
  }
  lineIndices[numberofpoints] = 0;
  lines->InsertNextCell(numberofpoints + 1, lineIndices);
  delete[] lineIndices;

  circle->SetPoints(points);
  circle->SetLines(lines);

  vtkNew<vtkPolyDataMapper> c_mapper;
  c_mapper->SetInputData(circle);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkActor> c_actor;
  c_actor->SetMapper(c_mapper);
  c_actor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());
  c_actor->GetProperty()->SetLineWidth(3);

  vtkNew<vtkDecimatePolylineFilter> decimate;
  decimate->SetInputData(circle);
  decimate->SetTargetReduction(0.95);
  decimate->Update();

  vtkNew<vtkPolyDataMapper> d_mapper;
  d_mapper->SetInputConnection(decimate->GetOutputPort());

  vtkNew<vtkActor> d_actor;
  d_actor->SetMapper(d_mapper);
  d_actor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());
  d_actor->GetProperty()->SetLineWidth(3);

  vtkNew<vtkRenderer> ren;
  ren->AddActor(c_actor);
  ren->AddActor(d_actor);
  ren->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderWindow> renwin;
  renwin->AddRenderer(ren);
  renwin->SetSize(640, 480);
  renwin->SetWindowName("DecimatePolyline");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renwin);

  renwin->Render();

  iren->Start();

  return EXIT_SUCCESS;
}
