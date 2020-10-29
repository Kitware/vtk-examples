#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkFloatArray.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnsignedCharArray.h>

int main(int, char*[])
{
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0, 0, 0);
  points->InsertNextPoint(1, 1, 1);
  points->InsertNextPoint(2, 2, 2);

  vtkNew<vtkFloatArray> scaleFactors;
  scaleFactors->SetNumberOfComponents(3);
  scaleFactors->SetName("Scale Factors");
  scaleFactors->InsertNextTuple3(0.7, 1.0, 1.0);
  scaleFactors->InsertNextTuple3(1.0, 0.7, 1.0);
  scaleFactors->InsertNextTuple3(1.0, 1.0, 0.7);

  vtkNew<vtkNamedColors> namedColors;

  vtkNew<vtkUnsignedCharArray> colors;
  colors->SetName("Colors");
  colors->SetNumberOfComponents(3);
  colors->InsertNextTypedTuple(namedColors->GetColor3ub("Red").GetData());
  colors->InsertNextTypedTuple(namedColors->GetColor3ub("Green").GetData());
  colors->InsertNextTypedTuple(namedColors->GetColor3ub("Blue").GetData());

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);
  polydata->GetPointData()->AddArray(colors);
  polydata->GetPointData()->AddArray(scaleFactors);

  // Create anything you want here, we will use a cube for the demo.
  vtkNew<vtkCubeSource> cubeSource;

  vtkNew<vtkGlyph3DMapper> glyph3Dmapper;
  glyph3Dmapper->SetSourceConnection(cubeSource->GetOutputPort());
  glyph3Dmapper->SetInputData(polydata);
  glyph3Dmapper->SetScalarModeToUsePointFieldData();
  glyph3Dmapper->SetScaleArray("Scale Factors");
  glyph3Dmapper->SetScaleModeToScaleByVectorComponents();
  glyph3Dmapper->SelectColorArray("Colors");
  glyph3Dmapper->Update();

  vtkNew<vtkActor> actor;
  actor->SetMapper(glyph3Dmapper);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Glyph3DMapper");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(namedColors->GetColor3d("SlateGray").GetData());

  // Position the camera
  renderer->GetActiveCamera()->SetPosition(-10, 5, 0);
  renderer->GetActiveCamera()->SetFocalPoint(1, 1, 1);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
