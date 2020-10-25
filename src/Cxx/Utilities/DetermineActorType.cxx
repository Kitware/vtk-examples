#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkCamera.h>
#include <vtkCubeAxesActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Cube axes
  vtkNew<vtkCubeAxesActor> cubeAxesActor;

  // Create a renderer and render window
  vtkNew<vtkRenderer> renderer;
  cubeAxesActor->SetCamera(renderer->GetActiveCamera());
  cubeAxesActor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  renderer->AddActor(actor);
  renderer->AddActor(cubeAxesActor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  // Determine the types of the actors - method 1
  {
    std::cout << "Method 1:" << std::endl;
    vtkActorCollection* actorCollection = renderer->GetActors();
    actorCollection->InitTraversal();

    for (vtkIdType i = 0; i < actorCollection->GetNumberOfItems(); i++)
    {
      vtkActor* nextActor = actorCollection->GetNextActor();
      std::cout << "nextActor " << i << " : " << nextActor->GetClassName()
                << std::endl;
      std::string className = nextActor->GetClassName();
      std::string wantedClass = "vtkCubeAxesActor";
      if (className == wantedClass)
      {
        std::cout << "nextActor " << i << " is a vtkCubeAxesActor!"
                  << std::endl;
      }
      else
      {
        std::cout << "nextActor " << i << " is NOT a vtkCubeAxesActor!"
                  << std::endl;
      }
    }
  }

  // Determine the types of the actors - method 2
  {
    std::cout << "Method 2:" << std::endl;
    vtkActorCollection* actorCollection = renderer->GetActors();
    actorCollection->InitTraversal();

    for (vtkIdType i = 0; i < actorCollection->GetNumberOfItems(); i++)
    {
      vtkActor* nextActor = actorCollection->GetNextActor();
      std::cout << "nextActor " << i << " : " << nextActor->GetClassName()
                << std::endl;
      if (nextActor->IsA("vtkCubeAxesActor"))
      {
        std::cout << "nextActor " << i << " is a vtkCubeAxesActor!"
                  << std::endl;
      }
      else
      {
        std::cout << "nextActor " << i << " is NOT a vtkCubeAxesActor!"
                  << std::endl;
      }
    }
  }

  // Determine the types of the actors - method 3
  {
    std::cout << "Method 3:" << std::endl;
    vtkActorCollection* actorCollection = renderer->GetActors();
    actorCollection->InitTraversal();

    for (vtkIdType i = 0; i < actorCollection->GetNumberOfItems(); i++)
    {
      vtkActor* nextActor = actorCollection->GetNextActor();
      std::cout << "nextActor " << i << " : " << nextActor->GetClassName()
                << std::endl;

      if (dynamic_cast<vtkCubeAxesActor*>(nextActor) != 0)
      {
        std::cout << "nextActor " << i << " is a vtkCubeAxesActor!"
                  << std::endl;
      }
      else
      {
        std::cout << "nextActor " << i << " is NOT a vtkCubeAxesActor!"
                  << std::endl;
      }
    }
  }

  // Determine the types of the actors - method 4
  {
    std::cout << "Method 4:" << std::endl;
    vtkActorCollection* actorCollection = renderer->GetActors();
    actorCollection->InitTraversal();

    for (vtkIdType i = 0; i < actorCollection->GetNumberOfItems(); i++)
    {
      vtkActor* nextActor = actorCollection->GetNextActor();
      std::cout << "nextActor " << i << " : " << nextActor->GetClassName()
                << std::endl;

      if (dynamic_cast<vtkCubeAxesActor*>(nextActor) != 0)
      {
        std::cout << "nextActor " << i << " is a vtkCubeAxesActor!"
                  << std::endl;
      }
      else
      {
        std::cout << "nextActor " << i << " is NOT a vtkCubeAxesActor!"
                  << std::endl;
      }
    }
  }

  // Render the scene
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("DetermineActorType");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  renderer->ResetCamera();
  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(0, 0, 8.09748);
  camera->SetFocalPoint(0, 0, 0);
  camera->SetViewUp(0, 1, 0);
  camera->SetDistance(8.09748);
  camera->SetClippingRange(6.0265, 10.7239);

  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
