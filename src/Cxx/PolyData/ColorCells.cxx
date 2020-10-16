#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkMinimalStandardRandomSequence.h>


#include <algorithm>

namespace {
void RandomColors(vtkLookupTable* lut, int numberOfColors);
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;
  
  // Provide some geometry
  int resolution = 3;
  vtkNew<vtkPlaneSource> aPlane;
  aPlane->SetXResolution(resolution);
  aPlane->SetYResolution(resolution);

  // Create cell data
  vtkNew<vtkFloatArray> cellData;
  for (int i = 0; i < resolution * resolution; i++)
  {
    cellData->InsertNextValue(i + 1);
  }

  // Create a lookup table to map cell data to colors
  vtkNew<vtkLookupTable> lut;
  int tableSize = std::max(resolution * resolution + 1, 10);
  lut->SetNumberOfTableValues(tableSize);
  lut->Build();
  RandomColors(lut, tableSize);

  aPlane->Update(); // Force an update so we can set cell data
  aPlane->GetOutput()->GetCellData()->SetScalars(cellData);

  // Setup actor and mapper
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(aPlane->GetOutputPort());
  mapper->SetScalarRange(0, tableSize - 1);
  mapper->SetLookupTable(lut);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Setup render window, renderer, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ColorCells");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
void RandomColors(vtkLookupTable* lut, int numberOfColors)
{
  // Fill in a few known colors, the rest will be generated if needed
  vtkNew<vtkNamedColors> colors;
  lut->SetTableValue(0, colors->GetColor4d("Black").GetData());
  lut->SetTableValue(1, colors->GetColor4d("Banana").GetData());
  lut->SetTableValue(2, colors->GetColor4d("Tomato").GetData());
  lut->SetTableValue(3, colors->GetColor4d("Wheat").GetData());
  lut->SetTableValue(4, colors->GetColor4d("Lavender").GetData());
  lut->SetTableValue(5, colors->GetColor4d("Flesh").GetData());
  lut->SetTableValue(6, colors->GetColor4d("Raspberry").GetData());
  lut->SetTableValue(7, colors->GetColor4d("Salmon").GetData());
  lut->SetTableValue(8, colors->GetColor4d("Mint").GetData());
  lut->SetTableValue(9, colors->GetColor4d("Peacock").GetData());

  // If the number of colors is larger than the number of specified colors,
  // generate some random colors.
  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(4355412);
  if (numberOfColors > 9)
  {
    for (auto i = 10; i < numberOfColors; ++i)
    {
      double r, g, b;
      r = randomSequence->GetRangeValue(0.6, 1.0);
      randomSequence->Next();
      g = randomSequence->GetRangeValue(0.6, 1.0);
      randomSequence->Next();
      b = randomSequence->GetRangeValue(0.6, 1.0);
      randomSequence->Next();
      lut->SetTableValue(i, r, g, b, 1.0);
    }
  }
}

}