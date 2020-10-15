#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNew.h>
#include <vtkCellData.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkUnsignedCharArray.h>

#include <vtkNamedColors.h>

int main(int, char*[])
{
  // Provide some geometry
  int resolutionX = 5;
  int resolutionY = 3;

  vtkNew<vtkPlaneSource> aPlane;
  aPlane->SetXResolution(resolutionX);
  aPlane->SetYResolution(resolutionY);
  aPlane->Update();

  // Create cell data
  vtkMath::RandomSeed(8775070); // for reproducibility
  vtkNew<vtkUnsignedCharArray> cellData;
  cellData->SetNumberOfComponents(3);
  cellData->SetNumberOfTuples(aPlane->GetOutput()->GetNumberOfCells());
  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(8775070);
  for (int i = 0; i < aPlane->GetOutput()->GetNumberOfCells(); i++)
  {
    float rgb[3];
    rgb[0] = randomSequence->GetRangeValue(64, 255);
    randomSequence->Next();
    rgb[1] = randomSequence->GetRangeValue(64, 255);
    randomSequence->Next();
    rgb[2] = randomSequence->GetRangeValue(64, 255);
    randomSequence->Next();
    cellData->InsertTuple(i, rgb);
  }

  aPlane->GetOutput()->GetCellData()->SetScalars(cellData);

  // Setup actor and mapper
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(aPlane->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Setup render window, renderer, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ColorCellsWithRGB");
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
