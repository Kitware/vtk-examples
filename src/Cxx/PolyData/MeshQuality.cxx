#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkColorSeries.h>
#include <vtkDataSet.h>
#include <vtkDoubleArray.h>
#include <vtkLookupTable.h>
#include <vtkMeshQuality.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTriangleFilter.h>

void MakeLUT(vtkLookupTable* lut)
{
  // Make the lookup table.
  vtkNew<vtkColorSeries> colorSeries;
  // Select a color scheme.
  int colorSeriesEnum;
  colorSeriesEnum = colorSeries->BREWER_DIVERGING_BROWN_BLUE_GREEN_9;
  // colorSeriesEnum = colorSeries->BREWER_DIVERGING_SPECTRAL_10;
  // colorSeriesEnum = colorSeries->BREWER_DIVERGING_SPECTRAL_3;
  // colorSeriesEnum = colorSeries->BREWER_DIVERGING_PURPLE_ORANGE_9;
  // colorSeriesEnum = colorSeries->BREWER_SEQUENTIAL_BLUE_PURPLE_9;
  // colorSeriesEnum = colorSeries->BREWER_SEQUENTIAL_BLUE_GREEN_9;
  // colorSeriesEnum = colorSeries->BREWER_QUALITATIVE_SET3;
  // colorSeriesEnum = colorSeries->CITRUS;
  colorSeries->SetColorScheme(colorSeriesEnum);

  colorSeries->BuildLookupTable(lut);
  lut->SetNanColor(1, 0, 0, 1);
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkTriangleFilter> triangleFilter;
  triangleFilter->SetInputConnection(sphereSource->GetOutputPort());
  triangleFilter->Update();

  vtkPolyData* mesh = triangleFilter->GetOutput();
  std::cout << "There are " << mesh->GetNumberOfCells() << " cells."
            << std::endl;

  vtkNew<vtkMeshQuality> qualityFilter;
  qualityFilter->SetInputData(mesh);
  qualityFilter->SetTriangleQualityMeasureToArea();
  qualityFilter->Update();

  auto qualityArray = dynamic_cast<vtkDoubleArray*>(
      qualityFilter->GetOutput()->GetCellData()->GetArray("Quality"));

  std::cout << "There are " << qualityArray->GetNumberOfTuples() << " values."
            << std::endl;

  for (vtkIdType i = 0; i < qualityArray->GetNumberOfTuples(); i++)
  {
    double val = qualityArray->GetValue(i);
    std::cout << "value " << i << " : " << val << std::endl;
  }

  vtkNew<vtkPolyData> polydata;
  polydata->ShallowCopy(qualityFilter->GetOutput());

  // Visualize

  vtkNew<vtkLookupTable> lut;
  MakeLUT(lut);
  lut->SetTableRange(polydata->GetScalarRange());
  lut->IndexedLookupOff();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polydata);
  mapper->SetScalarRange(polydata->GetScalarRange());
  mapper->SetLookupTable(lut);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("MeshQuality");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
