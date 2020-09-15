// Demonstrates how to create and populate a VTK's Overlapping AMR Grid
// type Data

#include <vtkAMRBox.h>
#include <vtkAMRUtilities.h>
#include <vtkCompositeDataGeometryFilter.h>
#include <vtkContourFilter.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkOverlappingAMR.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphere.h>
#include <vtkUniformGrid.h>

namespace {
void MakeScalars(int dims[3], double origin[3], double spacing[3],
                 vtkFloatArray* scalars);
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create and populate the AMR dataset
  // The dataset should look like
  // Level 0
  //   uniform grid, dimensions 11, 11, 11, AMR box (0, 0, 0) - (9, 9, 9)
  // Level 1 - refinement ratio : 2
  //   uniform grid, dimensions 11, 11, 11, AMR box (0, 0, 0) - (9, 9, 9)
  //   uniform grid, dimensions 11, 11, 11, AMR box (10, 10, 10) - (19, 19, 19)
  // Use MakeScalars() above to fill the scalar arrays

  vtkNew<vtkOverlappingAMR> amr;
  int blocksPerLevel[] = {1, 2};
  amr->Initialize(2, blocksPerLevel);

  double origin[3] = {0.0, 0.0, 0.0};
  double spacing[3] = {1.0, 1.0, 1.0};
  int dims[3] = {11, 11, 11};

  vtkNew<vtkUniformGrid> ug1;
  // Geometry
  ug1->SetOrigin(origin);
  ug1->SetSpacing(spacing);
  ug1->SetDimensions(dims);

  // Data
  vtkNew<vtkFloatArray> scalars;
  ug1->GetPointData()->SetScalars(scalars);
  MakeScalars(dims, origin, spacing, scalars);

  int lo[3] = {0, 0, 0};
  int hi[3] = {9, 9, 9};
  vtkAMRBox box1(lo, hi);
  amr->SetAMRBox(0, 0, box1);
  amr->SetDataSet(0, 0, ug1);

  double spacing2[3] = {0.5, 0.5, 0.5};

  vtkNew<vtkUniformGrid> ug2;
  // Geometry
  ug2->SetOrigin(origin);
  ug2->SetSpacing(spacing2);
  ug2->SetDimensions(dims);

  // Data
  vtkNew<vtkFloatArray> scalars2;
  ug2->GetPointData()->SetScalars(scalars2);
  MakeScalars(dims, origin, spacing2, scalars2);

  int lo2[3] = {0, 0, 0};
  int hi2[3] = {9, 9, 9};
  vtkAMRBox box2(lo2, hi2);
  amr->SetAMRBox(1, 0, box2);
  amr->SetDataSet(1, 0, ug2);

  double origin3[3] = {5, 5, 5};

  vtkNew<vtkUniformGrid> ug3;
  // Geometry
  ug3->SetOrigin(origin3);
  ug3->SetSpacing(spacing2);
  ug3->SetDimensions(dims);

  // Data
  vtkNew<vtkFloatArray> scalars3;
  ug3->GetPointData()->SetScalars(scalars3);
  MakeScalars(dims, origin3, spacing2, scalars3);

  int lo3[3] = {10, 10, 10};
  int hi3[3] = {19, 19, 19};
  vtkAMRBox box3(lo3, hi3);
  amr->SetAMRBox(1, 1, box3);
  amr->SetDataSet(1, 1, ug3);

  amr->SetRefinementRatio(0, 2);

  vtkAMRUtilities::BlankCells(amr);

  vtkNew<vtkOutlineFilter> of;
  of->SetInputData(amr);

  vtkNew<vtkCompositeDataGeometryFilter> geomFilter;
  geomFilter->SetInputConnection(of->GetOutputPort());

  // Create an iso-surface - at 10
  vtkNew<vtkContourFilter> cf;
  cf->SetInputData(amr);
  cf->SetNumberOfContours(1);
  cf->SetValue(0, 10.0);

  vtkNew<vtkCompositeDataGeometryFilter> geomFilter2;
  geomFilter2->SetInputConnection(cf->GetOutputPort());

  // create the render window, renderer, and interactor
  vtkNew<vtkRenderer> aren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(aren);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // associate the geometry with a mapper and the mapper to an actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(of->GetOutputPort());

  vtkNew<vtkActor> actor1;
  actor1->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());
  actor1->SetMapper(mapper);

  // associate the geometry with a mapper and the mapper to an actor
  vtkNew<vtkPolyDataMapper> mapper2;
  mapper2->SetInputConnection(geomFilter2->GetOutputPort());

  vtkNew<vtkActor> actor2;
  actor2->SetMapper(mapper2);

  // add the actor to the renderer and start handling events
  aren->AddActor(actor1);
  aren->AddActor(actor2);

  aren->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  renWin->SetWindowName("OverlappingAMR");

  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}

namespace {
void MakeScalars(int dims[3], double origin[3], double spacing[3],
                 vtkFloatArray* scalars)
{
  // Implicit function used to compute scalars
  vtkNew<vtkSphere> sphere;
  sphere->SetRadius(3);
  sphere->SetCenter(5, 5, 5);

  scalars->SetNumberOfTuples(dims[0] * dims[1] * dims[2]);
  for (int k = 0; k < dims[2]; k++)
  {
    auto z = origin[2] + spacing[2] * k;
    for (int j = 0; j < dims[1]; j++)
    {
      auto y = origin[1] + spacing[1] * j;
      for (int i = 0; i < dims[0]; i++)
      {
        auto x = origin[0] + spacing[0] * i;
        scalars->SetValue(k * dims[0] * dims[1] + j * dims[0] + i,
                          sphere->EvaluateFunction(x, y, z));
      }
    }
  }
}
} // namespace
