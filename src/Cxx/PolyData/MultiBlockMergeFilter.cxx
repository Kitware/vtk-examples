#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockMergeFilter.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{

  vtkNew<vtkSphereSource> sphereSource1;
  sphereSource1->Update();

  vtkNew<vtkSphereSource> sphereSource2;
  sphereSource2->SetCenter(10, 10, 10);
  sphereSource2->Update();

  vtkNew<vtkMultiBlockDataSet> multiBlockDataSet1;
  multiBlockDataSet1->SetNumberOfBlocks(1);
  multiBlockDataSet1->SetBlock(0, sphereSource1->GetOutput());

  vtkNew<vtkMultiBlockDataSet> multiBlockDataSet2;
  multiBlockDataSet2->SetNumberOfBlocks(1);
  multiBlockDataSet2->SetBlock(0, sphereSource2->GetOutput());

  vtkNew<vtkMultiBlockMergeFilter> multiBlockMergeFilter;
  multiBlockMergeFilter->AddInputData(multiBlockDataSet1);
  multiBlockMergeFilter->AddInputData(multiBlockDataSet2);
  multiBlockMergeFilter->Update();

  return EXIT_SUCCESS;
}
