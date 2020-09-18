#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractSelection.h>
#include <vtkHardwareSelector.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSelection.h>
#include <vtkSphereSource.h>

// Define interaction style
class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
  vtkNew<vtkNamedColors> colors;
  static KeyPressInteractorStyle* New();
  vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);

  virtual void OnKeyPress() override
  {
    // Get the keypress
    std::string key = this->Interactor->GetKeySym();

    // "s" for "s"elect
    if (key.compare("s") == 0)
    {
      vtkNew<vtkHardwareSelector> selector;
      selector->SetRenderer(this->Interactor->GetRenderWindow()
                                ->GetRenderers()
                                ->GetFirstRenderer());
      int* temp = this->Interactor->GetRenderWindow()->GetSize();
      unsigned int windowSize[4];
      windowSize[0] = temp[2];
      windowSize[1] = temp[3];
      windowSize[2] = temp[0];
      windowSize[3] = temp[1];
      /*
        for(unsigned int i = 0; i < 4; i++)
          {
          windowSize[i] = temp[i];
          }
        */
      selector->SetArea(windowSize);
      selector->SetFieldAssociation(vtkDataObject::FIELD_ASSOCIATION_CELLS);
      vtkSelection* selection = selector->Select();
      std::cout << "Selection has " << selection->GetNumberOfNodes()
                << " nodes." << std::endl;

      vtkNew<vtkExtractSelection> extractSelection;
      extractSelection->SetInputData(0, this->Data);
      extractSelection->SetInputData(1, selection);
      extractSelection->Update();

      vtkNew<vtkDataSetMapper> mapper;
      mapper->SetInputConnection(extractSelection->GetOutputPort());

      vtkNew<vtkActor> actor;
      actor->SetMapper(mapper);
      actor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
      this->Renderer->AddActor(actor);
    }

    // Forward events
    vtkInteractorStyleTrackballCamera::OnKeyPress();
  }

  vtkPolyData* Data;
  vtkRenderer* Renderer;
};
vtkStandardNewMacro(KeyPressInteractorStyle);

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(5.0);
  sphereSource->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetMultiSamples(0); // Turn off anti-aliasing
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<KeyPressInteractorStyle> style;
  style->Renderer = renderer;
  renderWindowInteractor->SetInteractorStyle(style);
  style->SetCurrentRenderer(renderer);
  style->Data = sphereSource->GetOutput();

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Mint").GetData());

  // Render and interact
  renderWindow->SetWindowName("ExtractVisibleCells");
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
