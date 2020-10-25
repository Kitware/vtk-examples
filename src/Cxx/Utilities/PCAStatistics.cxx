#include <vtkDoubleArray.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNew.h>
#include <vtkPCAStatistics.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

int main(int, char*[])
{
  // Each one of these arrays is a single component of
  // the data. That is, if you have 3D spatial data (x,y,z)
  // you need to construct an array of all the x values,
  // an array of all the y values, and an array of all the
  // z values.

  // Construct a data set of 3 3D points.

  // These would be all of your "x" values.
  const char m0Name[] = "M0";
  vtkNew<vtkDoubleArray> dataset1Arr;
  dataset1Arr->SetNumberOfComponents(1);
  dataset1Arr->SetName(m0Name);
  dataset1Arr->InsertNextValue(0);
  dataset1Arr->InsertNextValue(1);
  dataset1Arr->InsertNextValue(0);

  // These would be all of your "y" values.
  const char m1Name[] = "M1";
  vtkNew<vtkDoubleArray> dataset2Arr;
  dataset2Arr->SetNumberOfComponents(1);
  dataset2Arr->SetName(m1Name);
  dataset2Arr->InsertNextValue(0);
  dataset2Arr->InsertNextValue(0);
  dataset2Arr->InsertNextValue(1);

  // These would be all of your "z" values.
  const char m2Name[] = "M2";
  vtkNew<vtkDoubleArray> dataset3Arr;
  dataset3Arr->SetNumberOfComponents(1);
  dataset3Arr->SetName(m2Name);
  dataset3Arr->InsertNextValue(0);
  dataset3Arr->InsertNextValue(0);
  dataset3Arr->InsertNextValue(0);

  vtkNew<vtkTable> datasetTable;
  datasetTable->AddColumn(dataset1Arr);
  datasetTable->AddColumn(dataset2Arr);
  datasetTable->AddColumn(dataset3Arr);

  vtkNew<vtkPCAStatistics> pcaStatistics;
  pcaStatistics->SetInputData(vtkStatisticsAlgorithm::INPUT_DATA, datasetTable);
  pcaStatistics->SetColumnStatus("M0", 1);
  pcaStatistics->SetColumnStatus("M1", 1);
  pcaStatistics->SetColumnStatus("M2", 1);
  pcaStatistics->RequestSelectedColumns();
  pcaStatistics->SetDeriveOption(true);
  pcaStatistics->Update();

  ///////// Eigenvalues ////////////
  vtkNew<vtkDoubleArray> eigenvalues;
  pcaStatistics->GetEigenvalues(eigenvalues);
  //  double eigenvaluesGroundTruth[3] = {.5, .166667, 0};
  for (vtkIdType i = 0; i < eigenvalues->GetNumberOfTuples(); i++)
  {
    std::cout << "Eigenvalue " << i << " = " << eigenvalues->GetValue(i)
              << std::endl;
  }

  ///////// Eigenvectors ////////////
  vtkNew<vtkDoubleArray> eigenvectors;

  pcaStatistics->GetEigenvectors(eigenvectors);
  for (vtkIdType i = 0; i < eigenvectors->GetNumberOfTuples(); i++)
  {
    std::cout << "Eigenvector " << i << " : ";
    double* evec = new double[eigenvectors->GetNumberOfComponents()];
    eigenvectors->GetTuple(i, evec);
    for (vtkIdType j = 0; j < eigenvectors->GetNumberOfComponents(); j++)
    {
      std::cout << evec[j] << " ";
      vtkNew<vtkDoubleArray> eigenvectorSingle;
      pcaStatistics->GetEigenvector(i, eigenvectorSingle);
    }
    delete[] evec;
    std::cout << std::endl;
  }

  return EXIT_SUCCESS;
}
