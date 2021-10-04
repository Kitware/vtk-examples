#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkMeshQuality.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkThreshold.h>
#include <vtkTriangleFilter.h>
#include <vtkUnstructuredGrid.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkTriangleFilter> triangleFilter;
  triangleFilter->SetInputConnection(sphereSource->GetOutputPort());
  triangleFilter->Update();

  // Create a mapper and actor
  vtkNew<vtkDataSetMapper> sphereMapper;
  sphereMapper->SetInputConnection(triangleFilter->GetOutputPort());
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  vtkPolyData* mesh = triangleFilter->GetOutput();
  cout << "There are " << mesh->GetNumberOfCells() << " cells." << endl;

  vtkNew<vtkMeshQuality> qualityFilter;
  qualityFilter->SetInputData(mesh);
  qualityFilter->SetTriangleQualityMeasureToArea();
  qualityFilter->Update();

  vtkDataSet* qualityMesh = qualityFilter->GetOutput();
  auto qualityArray = dynamic_cast<vtkDoubleArray*>(
      qualityMesh->GetCellData()->GetArray("Quality"));

  cout << "There are " << qualityArray->GetNumberOfTuples() << " values."
       << endl;

  for (vtkIdType i = 0; i < qualityArray->GetNumberOfTuples(); i++)
  {
    double val = qualityArray->GetValue(i);
    cout << "value " << i << " : " << val << endl;
  }

  vtkNew<vtkThreshold> selectCells;
  //selectCells->ThresholdByLower(.02);
  selectCells->SetLowerThreshold(0.02);
  selectCells->SetThresholdFunction(vtkThreshold::THRESHOLD_LOWER);
  selectCells->SetInputArrayToProcess(0, 0, 0,
                                      vtkDataObject::FIELD_ASSOCIATION_CELLS,
                                      vtkDataSetAttributes::SCALARS);
  selectCells->SetInputData(qualityMesh);
  selectCells->Update();

  vtkUnstructuredGrid* ug = selectCells->GetOutput();

  // Create a mapper and actor
  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputData(ug);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetRepresentationToWireframe();
  actor->GetProperty()->SetLineWidth(5);
  actor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("HighlightBadCells");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->AddActor(sphereActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
