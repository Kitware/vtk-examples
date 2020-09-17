#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkCleanPolyData.h>
#include <vtkConeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyData> input1;
  vtkNew<vtkPolyData> input2;

  if (argc == 1) // command line arguments not specified
  {
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetCenter(5, 0, 0);
    sphereSource->Update();

    input1->ShallowCopy(sphereSource->GetOutput());

    vtkNew<vtkConeSource> coneSource;
    coneSource->Update();

    input2->ShallowCopy(coneSource->GetOutput());
  }
  else
  {
    if (argc != 3)
    {
      std::cout << "argc = " << argc << std::endl;
      std::cout << "Required arguments: File1 File2" << std::endl;
      return EXIT_FAILURE;
    }
    std::string inputFilename1 = argv[1];
    std::string inputFilename2 = argv[2];
    vtkNew<vtkXMLPolyDataReader> reader1;
    reader1->SetFileName(inputFilename1.c_str());
    reader1->Update();
    input1->ShallowCopy(reader1->GetOutput());

    vtkNew<vtkXMLPolyDataReader> reader2;
    reader2->SetFileName(inputFilename2.c_str());
    reader2->Update();
    input2->ShallowCopy(reader2->GetOutput());
  }

  // Append the two meshes
  vtkNew<vtkAppendPolyData> appendFilter;
  appendFilter->AddInputData(input1);
  appendFilter->AddInputData(input2);

  // Remove any duplicate points.
  vtkNew<vtkCleanPolyData> cleanFilter;
  cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
  cleanFilter->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(cleanFilter->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor);

  // Render and interact
  renderWindowInteractor->Initialize();
  renderWindow->Render();
  renderer->SetBackground(colors->GetColor3d("deep_ochre").GetData());
  renderer->GetActiveCamera()->Zoom(0.9);
  renderWindow->SetWindowName("CombinePolyData");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
