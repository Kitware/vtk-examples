#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGridGeometryFilter.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  if (argc < 3)
  {
    std::cout << "Required parameters: XYZFilename.bin QFileName.bin e.g "
                 "combxyz.bin combq.bin"
              << std::endl;
    return EXIT_FAILURE;
  }

  std::string xyzFilename(argv[1]);
  std::string qFilename(argv[2]);

  vtkNew<vtkMultiBlockPLOT3DReader> reader;
  reader->SetXYZFileName(xyzFilename.c_str());
  reader->SetQFileName(qFilename.c_str());
  reader->SetScalarFunctionNumber(100);
  reader->SetVectorFunctionNumber(202);
  reader->Update();

  vtkNew<vtkStructuredGridGeometryFilter> geometryFilter;
  geometryFilter->SetInputData(reader->GetOutput()->GetBlock(0));
  geometryFilter->Update();

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(geometryFilter->GetOutputPort());
  mapper->ScalarVisibilityOff();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  renderer->ResetCamera();
  renderer->GetActiveCamera()->SetClippingRange(3.95297, 50);
  renderer->GetActiveCamera()->SetFocalPoint(8.88908, 0.595038, 29.3342);
  renderer->GetActiveCamera()->SetPosition(-12.3332, 31.7479, 41.2387);
  renderer->GetActiveCamera()->SetViewUp(0.060772, -0.319905, 0.945498);

  renderWindow->SetWindowName("ReadPLOT3D");
  renderWindow->SetSize(640, 480);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
