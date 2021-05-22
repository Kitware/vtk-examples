#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnsignedCharArray.h>

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
  vtkNew<vtkUnsignedCharArray> cellData;
  cellData->SetNumberOfComponents(3);
  cellData->SetNumberOfTuples(aPlane->GetOutput()->GetNumberOfCells());

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(8775070);
  auto min_r = 64.0;
  auto max_r = 255.0;
  for (int i = 0; i < aPlane->GetOutput()->GetNumberOfCells(); i++)
  {
    double rgb[3];
    for (auto j = 0; j < 3; ++j)
    {
      rgb[j] = randomSequence->GetRangeValue(min_r, max_r);
      randomSequence->Next();
    }
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
