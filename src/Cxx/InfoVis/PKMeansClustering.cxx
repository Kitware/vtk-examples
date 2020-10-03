#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNew.h>
#include <vtkPKMeansStatistics.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkTable.h>
#include <vtkXMLPolyDataWriter.h>

// display
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVertexGlyphFilter.h>

#include <sstream>

int main(int, char*[])
{
  // create 2 clusters, one near (0,0,0) and the other near (3,3,3)
  vtkNew<vtkPoints> points;

  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(3.0, 3.0, 3.0);
  points->InsertNextPoint(0.1, 0.1, 0.1);
  points->InsertNextPoint(3.1, 3.1, 3.1);
  points->InsertNextPoint(0.2, 0.2, 0.2);
  points->InsertNextPoint(3.2, 3.2, 3.2);

  // Get the points into the format needed for KMeans
  vtkNew<vtkTable> inputData;

  for (int c = 0; c < 3; ++c)
  {
    std::stringstream colName;
    colName << "coord " << c;
    vtkNew<vtkDoubleArray> doubleArray;
    doubleArray->SetNumberOfComponents(1);
    doubleArray->SetName(colName.str().c_str());
    doubleArray->SetNumberOfTuples(points->GetNumberOfPoints());

    for (int r = 0; r < points->GetNumberOfPoints(); ++r)
    {
      double p[3];
      points->GetPoint(r, p);

      doubleArray->SetValue(r, p[c]);
    }

    inputData->AddColumn(doubleArray);
  }

  vtkNew<vtkPKMeansStatistics> pKMeansStatistics;
  // vtkNew<vtkKMeansStatistics> pKMeansStatistics;
  // pks->SetMaxNumIterations( 10 );
  pKMeansStatistics->SetInputData(vtkStatisticsAlgorithm::INPUT_DATA,
                                  inputData);
  pKMeansStatistics->SetColumnStatus(inputData->GetColumnName(0), 1);
  pKMeansStatistics->SetColumnStatus(inputData->GetColumnName(1), 1);
  pKMeansStatistics->SetColumnStatus(inputData->GetColumnName(2), 1);
  pKMeansStatistics->RequestSelectedColumns();
  pKMeansStatistics->SetAssessOption(true);
  pKMeansStatistics->SetDefaultNumberOfClusters(2);
  pKMeansStatistics->Update();

  // Display the results
  pKMeansStatistics->GetOutput()->Dump();

  vtkNew<vtkIntArray> clusterArray;
  clusterArray->SetNumberOfComponents(1);
  clusterArray->SetName("ClusterId");

  for (unsigned int r = 0;
       r < pKMeansStatistics->GetOutput()->GetNumberOfRows(); r++)
  {
    vtkVariant v = pKMeansStatistics->GetOutput()->GetValue(
        r, pKMeansStatistics->GetOutput()->GetNumberOfColumns() - 1);
    std::cout << "Point " << r << " is in cluster " << v.ToInt() << std::endl;
    clusterArray->InsertNextValue(v.ToInt());
  }

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);
  polydata->GetPointData()->SetScalars(clusterArray);

  // Output the cluster centers

  auto outputMetaDS = dynamic_cast<vtkMultiBlockDataSet*>(
      pKMeansStatistics->GetOutputDataObject(
          vtkStatisticsAlgorithm::OUTPUT_MODEL));
  auto outputMeta = dynamic_cast<vtkTable*>(outputMetaDS->GetBlock(0));
  // auto outputMeta = dynamic_cast<vtkTable*>( outputMetaDS->GetBlock( 1 ) );
  auto coord0 =
      dynamic_cast<vtkDoubleArray*>(outputMeta->GetColumnByName("coord 0"));
  auto coord1 =
      dynamic_cast<vtkDoubleArray*>(outputMeta->GetColumnByName("coord 1"));
  auto coord2 =
      dynamic_cast<vtkDoubleArray*>(outputMeta->GetColumnByName("coord 2"));

  for (unsigned int i = 0; i < coord0->GetNumberOfTuples(); ++i)
  {
    std::cout << coord0->GetValue(i) << " " << coord1->GetValue(i) << " "
              << coord2->GetValue(i) << std::endl;
  }

  return EXIT_SUCCESS;
}
