#include <math.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCell.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkDataSetAttributes.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMath.h>
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
#include <vtkSmartPointer.h>
#include <vtkTubeFilter.h>
#include <vtkUnsignedCharArray.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> nc;

  // Spiral tube
  unsigned int nV = 256;       // No. of vertices
  unsigned int nCyc = 5;       // No. of spiral cycles
  double rT1 = 0.1, rT2 = 0.5; // Start/end tube radii
  double rS = 2;               // Spiral radius
  double h = 10;               // Height
  unsigned int nTv = 8;        // No. of surface elements for each tube vertex

  // Create points and cells for the spiral
  vtkNew<vtkPoints> points;
  for (unsigned int i = 0; i < nV; i++)
  {
    // Spiral coordinates
    auto vX = rS * cos(2 * vtkMath::Pi() * nCyc * i / (nV - 1));
    auto vY = rS * sin(2 * vtkMath::Pi() * nCyc * i / (nV - 1));
    auto vZ = h * i / nV;
    points->InsertPoint(i, vX, vY, vZ);
  }

  vtkNew<vtkCellArray> lines;
  lines->InsertNextCell(nV);
  for (unsigned int i = 0; i < nV; i++)
  {
    lines->InsertCellPoint(i);
  }

  vtkNew<vtkPolyData> polyData;
  polyData->SetPoints(points);
  polyData->SetLines(lines);

  // Varying tube radius using sine-function
  vtkNew<vtkDoubleArray> tubeRadius;
  tubeRadius->SetName("TubeRadius");
  tubeRadius->SetNumberOfTuples(nV);
  for (unsigned int i = 0; i < nV; i++)
  {
    tubeRadius->SetTuple1(
        i, rT1 + (rT2 - rT1) * sin(vtkMath::Pi() * i / (nV - 1)));
  }
  polyData->GetPointData()->AddArray(tubeRadius);
  polyData->GetPointData()->SetActiveScalars("TubeRadius");

  // RBG array (could add Alpha channel too I guess...)
  // Varying from blue to red
  vtkNew<vtkUnsignedCharArray> colors;
  colors->SetName("Colors");
  colors->SetNumberOfComponents(3);
  colors->SetNumberOfTuples(nV);
  for (unsigned int i = 0; i < nV; i++)
  {
    colors->InsertTuple3(i, int(255 * i / (nV - 1)), 0,
                         int(255 * (nV - 1 - i) / (nV - 1)));
  }
  polyData->GetPointData()->AddArray(colors);

  vtkNew<vtkTubeFilter> tube;
  tube->SetInputData(polyData);
  tube->SetNumberOfSides(nTv);
  tube->SetVaryRadiusToVaryRadiusByAbsoluteScalar();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(tube->GetOutputPort());
  mapper->ScalarVisibilityOn();
  mapper->SetScalarModeToUsePointFieldData();
  mapper->SelectColorArray("Colors");

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(nc->GetColor3d("SteelBlue").GetData());

  // Make an oblique view
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCamera();

  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderWindowInteractor> iren;

  iren->SetRenderWindow(renWin);
  renWin->AddRenderer(renderer);
  renWin->SetSize(500, 500);
  renWin->Render();
  renWin->SetWindowName("TubesWithVaryingRadiusAndColors");

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  iren->SetInteractorStyle(style);

  iren->Start();

  return EXIT_SUCCESS;
}
