#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <sstream>
#include <vector>

int main(int, char*[])
{
  std::vector<vtkSmartPointer<vtkRenderWindowInteractor>> interactors;

  for (unsigned int i = 0; i < 4; i++)
  {
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkRenderWindow> renderWindow;

    vtkNew<vtkRenderer> renderer;

    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

    interactors.push_back(renderWindowInteractor);

    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindow->Render();
    std::stringstream ss;
    ss << "Window " << i;
    renderWindow->SetWindowName(ss.str().c_str());
    renderWindow->SetPosition(i * 300, 0);

    // create a sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetCenter(0.0, 0.0, 0.0);
    sphereSource->SetRadius(5.0);
    sphereSource->Update();

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    renderer->ResetCamera();
  }

  interactors[3]->Start();

  return EXIT_SUCCESS;
}
