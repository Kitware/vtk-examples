#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkMarchingSquares.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  std::string inputFileName;
  if (argc > 1)
  {
    inputFileName = argv[1];
  }
  int isoValue = 500;
  if (argc > 2)
  {
    isoValue = atoi(argv[2]);
  }

  vtkNew<vtkPNGReader> reader;
  if (!reader->CanReadFile(inputFileName.c_str()))
  {
    std::cerr << "Error: Could not read " << inputFileName << ".\n";
    return EXIT_FAILURE;
  }
  reader->SetFileName(inputFileName.c_str());
  reader->Update();

  vtkNew<vtkMarchingSquares> iso;
  iso->SetInputConnection(reader->GetOutputPort());
  iso->SetValue(0, isoValue);

  vtkNew<vtkDataSetMapper> isoMapper;
  isoMapper->SetInputConnection(iso->GetOutputPort());
  isoMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> isoActor;
  isoActor->SetMapper(isoMapper);
  isoActor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

  // Standard rendering classes
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->SetMultiSamples(0);
  renWin->AddRenderer(renderer);
  renWin->SetWindowName("MarchingSquares");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  renderer->AddActor(isoActor);

  // Standard testing code.
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
  renWin->SetSize(300, 300);

  vtkCamera* camera = renderer->GetActiveCamera();
  renderer->ResetCamera();
  camera->Azimuth(180);

  renWin->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
