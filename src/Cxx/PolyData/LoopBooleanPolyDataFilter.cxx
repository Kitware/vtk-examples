#include <vtkActor.h>
#include <vtkBYUReader.h>
#include <vtkCleanPolyData.h>
#include <vtkLoopBooleanPolyDataFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTriangleFilter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtksys/SystemTools.hxx>

#include <vtkCamera.h>
namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
void PositionCamera(vtkRenderer* renderer, double* viewUp, double* position);
} // namespace

int main(int argc, char* argv[])
{
  vtkSmartPointer<vtkPolyData> input1;
  vtkSmartPointer<vtkPolyData> input2;

  std::string operation("intersection");

  if (argc == 4)
  {
    auto poly1 = ReadPolyData(argv[1]);
    vtkNew<vtkTriangleFilter> tri1;
    tri1->SetInputData(poly1);
    vtkNew<vtkCleanPolyData> clean1;
    clean1->SetInputConnection(tri1->GetOutputPort());
    clean1->Update();
    input1 = clean1->GetOutput();

    auto poly2 = ReadPolyData(argv[3]);
    vtkNew<vtkTriangleFilter> tri2;
    tri2->SetInputData(poly2);
    tri2->Update();
    vtkNew<vtkCleanPolyData> clean2;
    clean2->SetInputConnection(tri2->GetOutputPort());
    clean2->Update();
    input2 = clean2->GetOutput();
    operation = argv[2];
  }
  else
  {
    vtkNew<vtkSphereSource> sphereSource1;
    sphereSource1->SetCenter(.25, 0, 0);
    sphereSource1->SetPhiResolution(21);
    sphereSource1->SetThetaResolution(21);
    sphereSource1->Update();
    input1 = sphereSource1->GetOutput();

    vtkNew<vtkSphereSource> sphereSource2;
    sphereSource2->Update();
    input2 = sphereSource2->GetOutput();

    if (argc == 2)
    {
      operation = argv[1];
    }
  }

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyDataMapper> input1Mapper;
  input1Mapper->SetInputData(input1);
  input1Mapper->ScalarVisibilityOff();
  vtkNew<vtkActor> input1Actor;
  input1Actor->SetMapper(input1Mapper);
  input1Actor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Tomato").GetData());
  input1Actor->GetProperty()->SetSpecular(.6);
  input1Actor->GetProperty()->SetSpecularPower(20);
  input1Actor->SetPosition(input1->GetBounds()[1] - input1->GetBounds()[0], 0,
                           0);
  vtkNew<vtkPolyDataMapper> input2Mapper;
  input2Mapper->SetInputData(input2);
  input2Mapper->ScalarVisibilityOff();
  vtkNew<vtkActor> input2Actor;
  input2Actor->SetMapper(input2Mapper);
  input2Actor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Mint").GetData());
  input2Actor->GetProperty()->SetSpecular(.6);
  input2Actor->GetProperty()->SetSpecularPower(20);
  input2Actor->SetPosition(-(input1->GetBounds()[1] - input1->GetBounds()[0]),
                           0, 0);
  vtkNew<vtkLoopBooleanPolyDataFilter> booleanOperation;
  if (operation == "union")
  {
    booleanOperation->SetOperationToUnion();
  }
  else if (operation == "intersection")
  {
    booleanOperation->SetOperationToIntersection();
  }
  else if (operation == "difference")
  {
    booleanOperation->SetOperationToDifference();
  }
  else
  {
    std::cout << "Unknown operation: " << operation << std::endl;
    return EXIT_FAILURE;
  }
  booleanOperation->SetInputData(0, input1);
  booleanOperation->SetInputData(1, input2);

  vtkNew<vtkPolyDataMapper> booleanOperationMapper;
  booleanOperationMapper->SetInputConnection(booleanOperation->GetOutputPort());
  booleanOperationMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> booleanOperationActor;
  booleanOperationActor->SetMapper(booleanOperationMapper);
  booleanOperationActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Banana").GetData());
  booleanOperationActor->GetProperty()->SetSpecular(.6);
  booleanOperationActor->GetProperty()->SetSpecularPower(20);

  vtkNew<vtkRenderer> renderer;
  renderer->AddViewProp(input1Actor);
  renderer->AddViewProp(input2Actor);
  renderer->AddViewProp(booleanOperationActor);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("LoopBooleanPolyDataFilter");

  double viewUp[3] = {0.0, 0.0, 1.0};
  double position[3] = {0.0, -1.0, 0.0};
  PositionCamera(renderer, viewUp, position);
  renderer->GetActiveCamera()->Dolly(1.5);
  renderer->ResetCameraClippingRange();

  vtkNew<vtkRenderWindowInteractor> renWinInteractor;
  renWinInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  renWinInteractor->Start();

  return EXIT_SUCCESS;
}
namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName)
{
  vtkSmartPointer<vtkPolyData> polyData;
  std::string extension =
      vtksys::SystemTools::GetFilenameExtension(std::string(fileName));
  if (extension == ".ply")
  {
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtp")
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".obj")
  {
    vtkNew<vtkOBJReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".stl")
  {
    vtkNew<vtkSTLReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtk")
  {
    vtkNew<vtkPolyDataReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".g")
  {
    vtkNew<vtkBYUReader> reader;
    reader->SetGeometryFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    vtkNew<vtkSphereSource> source;
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}

void PositionCamera(vtkRenderer* renderer, double* viewUp, double* position)
{
  renderer->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);
  renderer->GetActiveCamera()->SetViewUp(viewUp);
  renderer->GetActiveCamera()->SetPosition(position);
  renderer->ResetCamera();
  return;
}
} // namespace
