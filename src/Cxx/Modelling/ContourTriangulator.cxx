#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourTriangulator.h>
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
  std::string inputFileName;
  if (argc > 1)
  {
    inputFileName = argv[1];
  }
  else
  {
    cout << "Usage: " << argv[0]
         << " png_file [iso value] e.g. fullhead15.png 500" << endl;
    return EXIT_FAILURE;
  }
  int isoValue = 500;
  if (argc > 2)
  {
    isoValue = atoi(argv[2]);
  }

  vtkNew<vtkNamedColors> colors;

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
  isoActor->GetProperty()->SetColor(
      colors->GetColor3d("MediumOrchid").GetData());

  vtkNew<vtkContourTriangulator> poly;
  poly->SetInputConnection(iso->GetOutputPort());

  vtkNew<vtkDataSetMapper> polyMapper;
  polyMapper->SetInputConnection(poly->GetOutputPort());
  polyMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> polyActor;
  polyActor->SetMapper(polyMapper);
  polyActor->GetProperty()->SetColor(colors->GetColor3d("Gray").GetData());

  // Standard rendering classes
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->SetMultiSamples(0);
  renWin->AddRenderer(renderer);
  renWin->SetWindowName("ContourTriangulator");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  renderer->AddActor(polyActor);
  renderer->AddActor(isoActor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
  renWin->SetSize(300, 300);

  vtkCamera* camera = renderer->GetActiveCamera();
  renderer->ResetCamera();
  camera->Azimuth(180);

  renWin->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
