#include <vtkActor.h>
#include <vtkCleanPolyData.h>
#include <vtkDistancePolyDataFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

int main(int argc, char* argv[])
{
  vtkSmartPointer<vtkPolyData> input1;
  vtkSmartPointer<vtkPolyData> input2;
  if (argc == 3)
  {
    //     std::cerr << "Usage: " << argv[0]
    //               << " filename1.vtk"
    //               << " filename2.vtk" << std::endl;
    vtkNew<vtkPolyDataReader> reader1;
    reader1->SetFileName(argv[1]);
    reader1->Update();
    input1 = reader1->GetOutput();

    vtkNew<vtkPolyDataReader> reader2;
    reader2->SetFileName(argv[2]);
    reader2->Update();
    input2 = reader2->GetOutput();
  }
  else
  {
    vtkNew<vtkSphereSource> sphereSource1;
    sphereSource1->SetCenter(1, 0, 0);
    sphereSource1->SetPhiResolution(21);
    sphereSource1->SetThetaResolution(21);
    sphereSource1->Update();
    input1 = sphereSource1->GetOutput();

    vtkNew<vtkSphereSource> sphereSource2;
    sphereSource2->SetPhiResolution(21);
    sphereSource2->SetThetaResolution(21);
    sphereSource2->Update();
    input2 = sphereSource2->GetOutput();
  }

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkCleanPolyData> clean1;
  clean1->SetInputData(input1);

  vtkNew<vtkCleanPolyData> clean2;
  clean2->SetInputData(input2);

  vtkNew<vtkDistancePolyDataFilter> distanceFilter;

  distanceFilter->SetInputConnection(0, clean1->GetOutputPort());
  distanceFilter->SetInputConnection(1, clean2->GetOutputPort());
  distanceFilter->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(distanceFilter->GetOutputPort());
  mapper->SetScalarRange(
      distanceFilter->GetOutput()->GetPointData()->GetScalars()->GetRange()[0],
      distanceFilter->GetOutput()->GetPointData()->GetScalars()->GetRange()[1]);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkScalarBarActor> scalarBar;
  scalarBar->SetLookupTable(mapper->GetLookupTable());
  scalarBar->SetTitle("Distance");
  scalarBar->SetNumberOfLabels(4);
  scalarBar->UnconstrainedFontSizeOn();

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());
  renderer->SetBackground2(colors->GetColor3d("Gold").GetData());
  renderer->GradientBackgroundOn();

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  renWin->SetSize(600, 500);
  renWin->SetWindowName("DistancePolyDataFilter");

  vtkNew<vtkRenderWindowInteractor> renWinInteractor;
  renWinInteractor->SetRenderWindow(renWin);

  renderer->AddActor(actor);
  renderer->AddActor2D(scalarBar);

  renWin->Render();
  renWinInteractor->Start();

  return EXIT_SUCCESS;
}
