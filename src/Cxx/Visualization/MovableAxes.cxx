#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkAssemblyPath.h>
#include <vtkAxesActor.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkFollower.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DCollection.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkVectorText.h>

namespace {
//----------------------------------------------------------------------------
class vtkPositionCallback : public vtkCommand
{
public:
  static vtkPositionCallback* New()
  {
    return new vtkPositionCallback;
  }

  void Execute(vtkObject* vtkNotUsed(caller), unsigned long vtkNotUsed(event),
               void* vtkNotUsed(callData))
  {
    this->Axes->InitPathTraversal();
    vtkAssemblyPath* path = 0;
    int count = 0;
    vtkFollower* followers[3] = {this->XLabel, this->YLabel, this->ZLabel};
    int followerId = 0;
    while ((path = this->Axes->GetNextPath()) != NULL)
    {
      if (count++ > 2)
      {
        vtkProp3D* prop3D =
            static_cast<vtkProp3D*>(path->GetLastNode()->GetViewProp());
        if (prop3D)
        {
          prop3D->PokeMatrix(path->GetLastNode()->GetMatrix());
          followers[followerId]->SetPosition(prop3D->GetCenter());
          followerId++;
          prop3D->PokeMatrix(NULL);
        }
      }
    }
  }

  vtkPositionCallback() : XLabel(0), YLabel(0), ZLabel(0), Axes(0)
  {
  }

  vtkFollower* XLabel;
  vtkFollower* YLabel;
  vtkFollower* ZLabel;
  vtkAssembly* Axes;
};

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkConeSource> coneSource;
  coneSource->Update();

  //  vtkPolyData* cone = coneSource->GetOutput();

  // create a mapper
  vtkNew<vtkPolyDataMapper> coneMapper;
  coneMapper->SetInputConnection(coneSource->GetOutputPort());

  // create an actor
  vtkNew<vtkActor> coneActor;
  coneActor->SetMapper(coneMapper);
  coneActor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  // a renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("MovableAxes");

  // an interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // add the actors to the scene
  renderer->AddActor(coneActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // vtkAxesActor is currently not designed to work with
  // vtkInteractorStyleTrackballActor since it is a hybrid object containing
  // both vtkProp3D's and vtkActor2D's, the latter of which does not have a 3D
  // position that can be manipulated
  vtkNew<vtkAxesActor> axes;

  // get a copy of the axes' constituent 3D actors and put them into a
  // vtkAssembly so they can be manipulated as one prop
  vtkNew<vtkPropCollection> collection;
  axes->GetActors(collection);

  collection->InitTraversal();

  vtkNew<vtkAssembly> movableAxes;

  for (int i = 0; i < collection->GetNumberOfItems(); ++i)
  {
    movableAxes->AddPart((vtkProp3D*)collection->GetNextProp());
  }

  renderer->AddActor(movableAxes);

  // create our own labels that will follow and face the camera
  vtkNew<vtkFollower> xLabel;
  vtkNew<vtkVectorText> xText;
  vtkNew<vtkPolyDataMapper> xTextMapper;
  xText->SetText("X");
  xTextMapper->SetInputConnection(xText->GetOutputPort());
  xLabel->SetMapper(xTextMapper);
  xLabel->SetScale(0.3);
  xLabel->SetCamera(renderer->GetActiveCamera());
  xLabel->SetPosition(
      ((vtkProp3D*)collection->GetItemAsObject(3))->GetCenter()); // XAxisTip
  xLabel->PickableOff();
  renderer->AddActor(xLabel);

  vtkNew<vtkFollower> yLabel;
  vtkNew<vtkVectorText> yText;
  vtkNew<vtkPolyDataMapper> yTextMapper;
  yText->SetText("Y");
  yTextMapper->SetInputConnection(yText->GetOutputPort());
  yLabel->SetMapper(yTextMapper);
  yLabel->SetScale(0.3);
  yLabel->SetCamera(renderer->GetActiveCamera());
  yLabel->SetPosition(
      ((vtkProp3D*)collection->GetItemAsObject(4))->GetCenter()); // YAxisTip
  yLabel->PickableOff();
  renderer->AddActor(yLabel);

  vtkNew<vtkFollower> zLabel;
  vtkNew<vtkVectorText> zText;
  vtkNew<vtkPolyDataMapper> zTextMapper;
  zText->SetText("Z");
  zTextMapper->SetInputConnection(zText->GetOutputPort());
  zLabel->SetMapper(zTextMapper);
  zLabel->SetScale(0.3);
  zLabel->SetCamera(renderer->GetActiveCamera());
  zLabel->SetPosition(
      ((vtkProp3D*)collection->GetItemAsObject(5))->GetCenter()); // ZAxisTip
  zLabel->PickableOff();
  renderer->AddActor(zLabel);

  // custom callback to set the positions of the labels
  vtkNew<vtkPositionCallback> callback;
  callback->XLabel = xLabel;
  callback->YLabel = yLabel;
  callback->ZLabel = zLabel;
  callback->Axes = movableAxes;

  renderer->ResetCamera();
  renderWindow->Render();

  vtkNew<vtkInteractorStyleTrackballActor> style;

  renderWindowInteractor->SetInteractorStyle(style);
  style->AddObserver(vtkCommand::InteractionEvent, callback);

  // begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
