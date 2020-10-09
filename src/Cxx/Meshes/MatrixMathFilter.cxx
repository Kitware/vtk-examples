#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkMatrixMathFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLUnstructuredGridWriter.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  if (argc < 2)
  {
    std::cerr << "Required arguments: vtkFile e.g. tensors.vtk" << std::endl;
    return EXIT_FAILURE;
  }

  std::string filename = argv[1];

  std::cout << "filename: " << filename << std::endl;

  vtkNew<vtkUnstructuredGridReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();

  vtkNew<vtkDataSetSurfaceFilter> surfaceFilter;
  surfaceFilter->SetInputConnection(reader->GetOutputPort());
  surfaceFilter->Update();

  vtkNew<vtkMatrixMathFilter> matrixMathFilter;
  // matrixMathFilter->SetOperationToDeterminant();
  matrixMathFilter->SetOperationToEigenvalue();
  matrixMathFilter->SetInputConnection(surfaceFilter->GetOutputPort());
  matrixMathFilter->Update();
  matrixMathFilter->GetOutput()->GetPointData()->SetActiveScalars("Eigenvalue");

  vtkNew<vtkXMLPolyDataWriter> writer;
  writer->SetInputConnection(matrixMathFilter->GetOutputPort());
  writer->SetFileName("output.vtp");
  writer->Write();

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(matrixMathFilter->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("MatrixMathFilter");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DimGray").GetData());
  renderer->GetActiveCamera()->Azimuth(45);
  renderer->GetActiveCamera()->Pitch(-22.5);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Zoom(0.95);

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
