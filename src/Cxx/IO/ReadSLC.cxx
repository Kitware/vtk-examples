#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkExtractVOI.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSLCReader.h>

// Author: Bharatesh Chakravarthi
// Affiliation: Virtual Environment Lab, Chung-Ang University, Seoul, South
// Korea

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    // Pass the filename (<filename>.slc) as parameter to read .slc file
    std::cout << "Required parameters: Filename(.slc) e.g vw_knee.slc"
              << std::endl;
    return EXIT_FAILURE;
  }

  double isoValue = 72.0;
  if (argc < 3)
  {
    isoValue = 72.0;
  }
  else
  {
    isoValue = std::atof(argv[2]);
  }
  std::string inputFilename = argv[1];

  // Using vtkSLCReader to read Volumetric file format(<filename.slc>)
  vtkNew<vtkSLCReader> reader;
  reader->SetFileName(inputFilename.c_str());
  reader->Update();

  // Implementing Marching Cubes Algorithm to create the surface using
  // vtkContourFilter object
  vtkNew<vtkContourFilter> cFilter;
  cFilter->SetInputConnection(reader->GetOutputPort());

  // Change the range(2nd and 3rd Paramater) based on your
  // requirement. recomended value for 1st parameter is above 1
  // cFilter->GenerateValues(5, 80.0, 100.0);
  cFilter->SetValue(0, isoValue);
  cFilter->Update();

  // Adding the outliner using vtkOutlineFilter object
  vtkNew<vtkOutlineFilter> outliner;
  outliner->SetInputConnection(reader->GetOutputPort());
  outliner->Update();

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(cFilter->GetOutputPort());
  mapper->SetScalarVisibility(0);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetDiffuse(0.8);
  actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Ivory").GetData());
  actor->GetProperty()->SetSpecular(0.8);
  actor->GetProperty()->SetSpecularPower(120.0);

  // extractVOI is used to fix the problem of subsampling of data and reduce
  // slow interaction and increase loading speed
  vtkNew<vtkExtractVOI> extractVOI;
  extractVOI->SetInputConnection(reader->GetOutputPort());
  extractVOI->SetSampleRate(2, 2, 2);
  extractVOI->Update();

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 512);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderWindow->Render();

  // Pick a good view
  vtkCamera* cam1 = renderer->GetActiveCamera();
  cam1->SetFocalPoint(0.0, 0.0, 0.0);
  cam1->SetPosition(0.0, -1.0, 0.0);
  cam1->SetViewUp(0.0, 0.0, -1.0);
  cam1->Azimuth(-90.0);
  renderer->ResetCamera();
  renderer->ResetCameraClippingRange();

  renderWindow->SetWindowName("ReadSLC");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
