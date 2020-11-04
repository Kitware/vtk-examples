#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkConeSource.h>
#include <vtkContourFilter.h>
#include <vtkDoubleArray.h>
#include <vtkGaussianSplatter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  double x[3], n[3];

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> aren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(aren);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Create single splat point
  vtkNew<vtkPoints> pts;
  vtkNew<vtkCellArray> verts;
  vtkNew<vtkDoubleArray> norms;
  vtkNew<vtkDoubleArray> scalars;

  x[0] = x[1] = x[2] = 0.0;
  pts->InsertNextPoint(x);
  norms->SetNumberOfTuples(1);
  norms->SetNumberOfComponents(3);
  n[0] = 0.707;
  n[1] = 0.707;
  n[2] = 0.0;
  norms->InsertTuple(0, n);
  scalars->SetNumberOfTuples(1);
  scalars->SetNumberOfComponents(1);
  scalars->InsertTuple1(0, 1.0);

  verts->InsertNextCell(1);
  verts->InsertCellPoint(0);

  vtkNew<vtkPolyData> pData;
  pData->SetPoints(pts);
  pData->SetVerts(verts);
  pData->GetPointData()->SetNormals(norms);
  pData->GetPointData()->SetScalars(scalars);

  // Splat point and generate isosurface
  vtkNew<vtkGaussianSplatter> splat;
  splat->SetInputData(pData);
  splat->SetModelBounds(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
  splat->SetSampleDimensions(75, 75, 75);
  splat->SetRadius(0.5);
  splat->SetEccentricity(5.0);
  splat->SetExponentFactor(-3.25);
  vtkNew<vtkContourFilter> contour;
  contour->SetInputConnection(splat->GetOutputPort());
  contour->SetValue(0, 0.9);
  vtkNew<vtkPolyDataMapper> splatMapper;
  splatMapper->SetInputConnection(contour->GetOutputPort());
  vtkNew<vtkActor> splatActor;
  splatActor->SetMapper(splatMapper);

  // Create outline
  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(splat->GetOutputPort());
  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());
  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(colors->GetColor3d("Brown").GetData());

  // Create cone to indicate direction
  vtkNew<vtkConeSource> cone;
  cone->SetResolution(24);
  vtkNew<vtkPolyDataMapper> coneMapper;
  coneMapper->SetInputConnection(cone->GetOutputPort());
  vtkNew<vtkActor> coneActor;
  coneActor->SetMapper(coneMapper);
  coneActor->SetScale(0.75, 0.75, 0.75);
  coneActor->RotateZ(45.0);
  coneActor->AddPosition(0.50, 0.50, 0.0);
  coneActor->GetProperty()->SetColor(colors->GetColor3d("DeepPink").GetData());
  //
  // Rendering stuff
  //
  aren->SetBackground(colors->GetColor3d("Beige").GetData());
  aren->AddActor(splatActor);
  aren->AddActor(outlineActor);
  aren->AddActor(coneActor);

  renWin->SetSize(640, 640);
  renWin->SetWindowName("SingleSplat");
  renWin->Render();

  // interact with data
  iren->Start();

  return EXIT_SUCCESS;
}
