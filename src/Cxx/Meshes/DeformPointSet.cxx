#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkDeformPointSet.h>
#include <vtkElevationFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyData> input;
  double bounds[6];

  if (argc == 1)
  {
    // Create a sphere to warp
    vtkNew<vtkSphereSource> sphere;
    sphere->SetThetaResolution(51);
    sphere->SetPhiResolution(17);
    sphere->Update();
    sphere->GetOutput()->GetBounds(bounds);

    // Generate some scalars on the polydata
    vtkNew<vtkElevationFilter> ele;
    ele->SetInputConnection(sphere->GetOutputPort());
    ele->SetLowPoint(0, 0, -0.5);
    ele->SetHighPoint(0, 0, 0.5);
    ele->SetLowPoint((bounds[1] + bounds[0]) / 2.0,
                     (bounds[3] + bounds[2]) / 2.0, -bounds[5]);
    ele->SetHighPoint((bounds[1] + bounds[0]) / 2.0,
                      (bounds[3] + bounds[2]) / 2.0, bounds[5]);
    ele->Update();
    input->ShallowCopy(ele->GetOutput());
  }
  else
  {
    std::string inputFilename = argv[1];

    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(inputFilename.c_str());
    reader->Update();

    input->ShallowCopy(reader->GetOutput());
    input->GetBounds(bounds);
  }

  // Now create a control mesh, in this case a octagon that encloses
  // the point set
  vtkNew<vtkPoints> pts;
  pts->SetNumberOfPoints(6);
  pts->SetPoint(0, bounds[0] - .1 * (bounds[1] - bounds[0]),
                (bounds[3] + bounds[2]) / 2.0, (bounds[5] + bounds[4]) / 2.0);
  pts->SetPoint(1, bounds[1] + .1 * (bounds[1] - bounds[0]),
                (bounds[3] + bounds[2]) / 2.0, (bounds[5] + bounds[4]) / 2.0);
  pts->SetPoint(2, (bounds[1] + bounds[0]) / 2.0,
                bounds[2] - .1 * (bounds[3] - bounds[2]),
                (bounds[5] + bounds[4]) / 2.0);
  pts->SetPoint(3, (bounds[1] + bounds[0]) / 2.0,
                bounds[3] + .1 * (bounds[3] - bounds[2]),
                (bounds[5] + bounds[4]) / 2.0);
  pts->SetPoint(4, (bounds[1] + bounds[0]) / 2.0, (bounds[3] + bounds[2]) / 2.0,
                bounds[4] - .1 * (bounds[5] - bounds[4]));
  pts->SetPoint(5, (bounds[1] + bounds[0]) / 2.0, (bounds[3] + bounds[2]) / 2.0,
                bounds[5] + .1 * (bounds[5] - bounds[4]));

  vtkNew<vtkCellArray> tris;
  // clang-format off
  tris->InsertNextCell(3);
  tris->InsertCellPoint(2); tris->InsertCellPoint(0);  tris->InsertCellPoint(4);
  tris->InsertNextCell(3);
  tris->InsertCellPoint(1); tris->InsertCellPoint(2);  tris->InsertCellPoint(4);
  tris->InsertNextCell(3);
  tris->InsertCellPoint(3);  tris->InsertCellPoint(1);  tris->InsertCellPoint(4);
  tris->InsertNextCell(3);
  tris->InsertCellPoint(0);  tris->InsertCellPoint(3);  tris->InsertCellPoint(4);
  tris->InsertNextCell(3);
  tris->InsertCellPoint(0);  tris->InsertCellPoint(2);  tris->InsertCellPoint(5);
  tris->InsertNextCell(3);
  tris->InsertCellPoint(2);  tris->InsertCellPoint(1);  tris->InsertCellPoint(5);
  tris->InsertNextCell(3);
  tris->InsertCellPoint(1); tris->InsertCellPoint(3);  tris->InsertCellPoint(5);
  tris->InsertNextCell(3);
  tris->InsertCellPoint(3);  tris->InsertCellPoint(0);  tris->InsertCellPoint(5);
  // clang-format on

  vtkNew<vtkPolyData> pd;
  pd->SetPoints(pts);
  pd->SetPolys(tris);

  // Display the control mesh
  vtkNew<vtkPolyDataMapper> meshMapper;
  meshMapper->SetInputData(pd);
  vtkNew<vtkActor> meshActor;
  meshActor->SetMapper(meshMapper);
  meshActor->GetProperty()->SetRepresentationToWireframe();
  meshActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  // Do the intitial weight generation
  vtkNew<vtkDeformPointSet> deform;
  deform->SetInputData(input);
  deform->SetControlMeshData(pd);
  deform->Update(); // this creates the initial weights

  // Now move one point and deform
  double controlPoint[3];
  pts->GetPoint(5, controlPoint);
  pts->SetPoint(5, controlPoint[0], controlPoint[1],
                bounds[5] + .8 * (bounds[5] - bounds[4]));
  pts->Modified();

  // Display the warped polydata
  vtkNew<vtkPolyDataMapper> polyMapper;
  polyMapper->SetInputConnection(deform->GetOutputPort());
  vtkNew<vtkActor> polyActor;
  polyActor->SetMapper(polyMapper);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  renderer->AddActor(polyActor);
  renderer->AddActor(meshActor);

  renderer->GetActiveCamera()->SetPosition(1, 1, 1);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  renWin->SetSize(300, 300);
  renWin->SetWindowName("DeformPointSet");
  renWin->Render();

  iren->Start();

  return EXIT_SUCCESS;
}
