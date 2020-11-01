#include <vtkCallbackCommand.h>
#include <vtkLODProp3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {
void RefreshCallback(vtkObject* vtkNotUsed(caller),
                     long unsigned int vtkNotUsed(eventId), void* clientData,
                     void* vtkNotUsed(callData));
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // High res sphere
  vtkNew<vtkSphereSource> highResSphereSource;
  int res = 100;
  highResSphereSource->SetThetaResolution(res);
  highResSphereSource->SetPhiResolution(res);
  highResSphereSource->Update();

  vtkNew<vtkPolyDataMapper> highResMapper;
  highResMapper->SetInputConnection(highResSphereSource->GetOutputPort());

  // Low res sphere
  vtkNew<vtkSphereSource> lowResSphereSource;

  vtkNew<vtkPolyDataMapper> lowResMapper;
  lowResMapper->SetInputConnection(lowResSphereSource->GetOutputPort());

  vtkNew<vtkProperty> propertyLowRes;
  propertyLowRes->SetDiffuseColor(
      colors->GetColor3d("BlanchedAlmond").GetData());
  propertyLowRes->SetInterpolationToFlat();

  vtkNew<vtkProperty> propertyHighRes;
  propertyHighRes->SetDiffuseColor(colors->GetColor3d("MistyRose").GetData());
  propertyHighRes->SetInterpolationToFlat();

  vtkNew<vtkLODProp3D> prop;
  prop->AddLOD(lowResMapper, propertyLowRes, 0.0);
  prop->AddLOD(highResMapper, propertyHighRes, 0.0);

  std::cout << "There are " << prop->GetNumberOfLODs() << " LODs" << std::endl;

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("LODProp3D");

  // prop->SetAllocatedRenderTime(1e-6,renderer);
  prop->SetAllocatedRenderTime(1e-12, renderer);

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(prop);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkCallbackCommand> refreshCallback;
  refreshCallback->SetCallback(RefreshCallback);
  refreshCallback->SetClientData(prop);

  renderWindow->AddObserver(vtkCommand::ModifiedEvent, refreshCallback);

  renderWindow->Render();

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
void RefreshCallback(vtkObject* vtkNotUsed(caller),
                     long unsigned int vtkNotUsed(eventId), void* clientData,
                     void* vtkNotUsed(callData))
{
  auto lodProp = static_cast<vtkLODProp3D*>(clientData);
  std::cout << "Last rendered LOD ID: " << lodProp->GetLastRenderedLODID()
            << std::endl;
}
} // namespace
