#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNew.h>
#include <vtkUnsignedCharArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkCamera.h>

// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

int main(int, char*[])
{
  vtkNew<vtkNamedColors> nc;

  // Create points
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(5, 0, 0);
  points->InsertNextPoint(10, 0, 0);

  // Setup scales. This can also be an Int array
  // char is used since it takes the least memory
  vtkNew<vtkUnsignedCharArray> colors;
  colors->SetName("colors");
  colors->SetNumberOfComponents(3);
  unsigned char r[3] = {255, 0, 0};
  unsigned char g[3] = {0, 255, 0};
  unsigned char b[3] = {0, 0, 255};
  colors->InsertNextTupleValue(r);
  colors->InsertNextTupleValue(g);
  colors->InsertNextTupleValue(b);

  // Combine into a polydata
  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);
  polydata->GetPointData()->SetScalars(colors);

  // Create anything you want here, we will use a cube for the demo.
  vtkNew<vtkCubeSource> cubeSource;

  vtkNew<vtkGlyph3D> glyph3D;
  glyph3D->SetColorModeToColorByScalar();
  glyph3D->SetSourceConnection(cubeSource->GetOutputPort());
  glyph3D->SetInputData(polydata);
  glyph3D->ScalingOff();
  glyph3D->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(glyph3D->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ColorGlyphs");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(
      nc->GetColor3d("SlateGray").GetData());

  renderWindow->Render();

  renderer->GetActiveCamera()->Elevation(20);
  renderer->GetActiveCamera()->Azimuth(10);
  renderer->GetActiveCamera()->Zoom(0.9);

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
