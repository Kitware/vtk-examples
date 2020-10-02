#include <vtkActor.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkKMeansStatistics.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataWriter.h>

#include <sstream>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create 2 clusters, one near (0,0,0) and the other near (3,3,3)
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

  vtkNew<vtkKMeansStatistics> kMeansStatistics;
  kMeansStatistics->SetInputData(vtkStatisticsAlgorithm::INPUT_DATA, inputData);
  kMeansStatistics->SetColumnStatus(inputData->GetColumnName(0), 1);
  kMeansStatistics->SetColumnStatus(inputData->GetColumnName(1), 1);
  kMeansStatistics->SetColumnStatus(inputData->GetColumnName(2), 1);
  // kMeansStatistics->SetColumnStatus( "Testing", 1 );
  kMeansStatistics->RequestSelectedColumns();
  kMeansStatistics->SetAssessOption(true);
  kMeansStatistics->SetDefaultNumberOfClusters(2);
  kMeansStatistics->Update();

  // Display the results
  kMeansStatistics->GetOutput()->Dump();

  vtkNew<vtkIntArray> clusterArray;
  clusterArray->SetNumberOfComponents(1);
  clusterArray->SetName("ClusterId");

  for (int r = 0; r < kMeansStatistics->GetOutput()->GetNumberOfRows(); r++)
  {
    vtkVariant v = kMeansStatistics->GetOutput()->GetValue(
        r, kMeansStatistics->GetOutput()->GetNumberOfColumns() - 1);
    std::cout << "Point " << r << " is in cluster " << v.ToInt() << std::endl;
    clusterArray->InsertNextValue(v.ToInt());
  }

  // Output the cluster centers
  auto outputMetaDS =
      dynamic_cast<vtkMultiBlockDataSet*>(kMeansStatistics->GetOutputDataObject(
          vtkStatisticsAlgorithm::OUTPUT_MODEL));
  auto outputMeta = dynamic_cast<vtkTable*>(outputMetaDS->GetBlock(0));
  auto coord0 =
      dynamic_cast<vtkDoubleArray*>(outputMeta->GetColumnByName("coord 0"));
  auto coord1 =
      dynamic_cast<vtkDoubleArray*>(outputMeta->GetColumnByName("coord 1"));
  auto coord2 =
      dynamic_cast<vtkDoubleArray*>(outputMeta->GetColumnByName("coord 2"));
  std::cout << "CLuster centers:" << std::endl;
  for (unsigned int i = 0; i < coord0->GetNumberOfTuples(); ++i)
  {
    std::cout << coord0->GetValue(i) << " " << coord1->GetValue(i) << " "
              << coord2->GetValue(i) << std::endl;
  }

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);
  // polydata->GetPointData()->AddArray(clusterArray);
  polydata->GetPointData()->SetScalars(clusterArray);

  vtkNew<vtkXMLPolyDataWriter> writer;
  writer->SetFileName("output.vtp");
  writer->SetInputData(polydata);
  writer->Write();

  // Display
  vtkNew<vtkVertexGlyphFilter> glyphFilter;
  glyphFilter->SetInputData(polydata);
  glyphFilter->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(glyphFilter->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetPointSize(4);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("KMeansClustering");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("OliveDrab").GetData());

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  renderWindowInteractor->SetInteractorStyle(style);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
