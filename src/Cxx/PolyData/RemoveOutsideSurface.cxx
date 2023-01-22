#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCellData.h>
#include <vtkCellIterator.h>
#include <vtkCellLocator.h>
#include <vtkConnectivityFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericCell.h>
#include <vtkIdTypeArray.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  // PolyData to process
  vtkSmartPointer<vtkPolyData> polyData;

  if (argc > 1)
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    vtkNew<vtkAppendPolyData> appendFilter;

    vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
    randomSequence->SetSeed(8775070);

    for (int i = 0; i < 20; ++i)
    {
      // Small spheres with most polgons
      vtkNew<vtkSphereSource> sphereSource1;
      sphereSource1->SetThetaResolution(10);
      sphereSource1->SetPhiResolution(10);
      double x, y, z;
      x = randomSequence->GetRangeValue(4.0, 14.0);
      randomSequence->Next();
      y = randomSequence->GetRangeValue(-3.0, 5.0);
      randomSequence->Next();
      // z = randomSequence->GetRangeValue(4.0, 14.0);
      randomSequence->Next();
      sphereSource1->SetCenter(x, y, x);
      sphereSource1->SetRadius(1.0);
      appendFilter->AddInputConnection(sphereSource1->GetOutputPort());
    }

    // Large sphere with least polygons
    vtkNew<vtkSphereSource> sphereSource2;
    sphereSource2->SetRadius(10);
    sphereSource2->SetCenter(10, 1, 10);

    appendFilter->AddInputConnection(sphereSource2->GetOutputPort());
    appendFilter->Update();
    polyData = appendFilter->GetOutput();
  }

  double center[3], bounds[6];
  polyData->GetCenter(center);
  std::cout << "Center of data: " << center[0] << ", " << center[1] << ", "
            << center[2] << std::endl;
  polyData->GetPoints()->GetBounds(bounds);
  std::cout << "Bounds of data: " << bounds[0] << ", " << bounds[1] << ", "
            << bounds[2] << ", " << bounds[3] << ", " << bounds[4] << ", "
            << bounds[5] << std::endl;

  // Extract all regions and label cells with region number
  vtkNew<vtkConnectivityFilter> connectivityFilter;
  connectivityFilter->SetInputData(polyData);
  connectivityFilter->SetExtractionModeToAllRegions();
  connectivityFilter->ColorRegionsOn();
  connectivityFilter->Update();

  // Build a cell locator.
  vtkNew<vtkCellLocator> cellLocator;
  cellLocator->SetDataSet(connectivityFilter->GetOutput());
  cellLocator->BuildLocator();

  // Now fire a ray from outside the bounds to the center and find a
  // cell. This cell should be on the outside surface
  double rayStart[3];
  for (unsigned int i = 0; i < 3; i++)
  {
    rayStart[i] = bounds[2 * i + 1] * 1.1;
  }

  vtkIdType cellId = -1;
  double xyz[3], t, pcoords[3];
  int subId;

  cellLocator->IntersectWithLine(rayStart, center, 0.0001, t, xyz, pcoords,
                                 subId, cellId);

  vtkIdTypeArray* cd = dynamic_cast<vtkIdTypeArray*>(
      connectivityFilter->GetOutput()->GetCellData()->GetScalars());
  vtkIdType outsideRegionId = cd->GetTuple1(cellId);
  std::cout << "Id of cell on outside surface: " << cellId << std::endl;
  std::cout << "CellData at " << cellId << ": " << outsideRegionId << std::endl;

  // Build a polydata from cells that are not in the outside surface
  // Iterate over the original cells
  vtkNew<vtkPolyData> insidePolyData;
  insidePolyData->Allocate();
  insidePolyData->SetPoints(connectivityFilter->GetOutput()->GetPoints());

  vtkNew<vtkGenericCell> cell;
  auto it = connectivityFilter->GetOutput()->NewCellIterator();
  vtkIdType originalCellId = 0;
  for (it->InitTraversal(); !it->IsDoneWithTraversal();
       it->GoToNextCell(), ++originalCellId)
  {
    //  Retain cell if it is NOT an outside cell
    if (cd->GetTuple1(originalCellId) != outsideRegionId)
    {
      it->GetCell(cell);
      insidePolyData->InsertNextCell(it->GetCellType(), cell->GetPointIds());
    }
  }
  it->Delete();

  // Create a mapper and actor for original data
  vtkNew<vtkPolyDataMapper> originalMapper;
  originalMapper->SetInputData(polyData);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->BackfaceCullingOn();
  originalActor->GetProperty()->SetOpacity(.5);
  originalActor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  // Create a mapper and actor for extracted data
  vtkNew<vtkPolyDataMapper> extractedMapper;
  extractedMapper->SetInputData(insidePolyData);

  vtkNew<vtkActor> extractedActor;
  extractedActor->GetProperty()->SetColor(
      colors->GetColor3d("Peacock").GetData());
  extractedActor->SetMapper(extractedMapper);
  extractedActor->GetProperty()->SetOpacity(0.5);
  extractedActor->GetProperty()->BackfaceCullingOn();

  // Create a renderer
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(originalActor);
  renderer->AddActor(extractedActor);

  renderer->GradientBackgroundOn();
  renderer->SetBackground2(colors->GetColor3d("Beige").GetData());
  renderer->SetBackground(colors->GetColor3d("Burlywood").GetData());

  extractedActor->SetPosition((bounds[1] - bounds[0]) / 1.9, 0, 0);
  originalActor->SetPosition(-(bounds[1] - bounds[0]) / 1.9, 0, 0);
  // Create a render window
  vtkNew<vtkRenderWindow> renwin;
  renwin->AddRenderer(renderer);
  renwin->SetSize(512, 512);
  renwin->SetWindowName("RemoveOutsideSurface");

  // Create an interactor
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renwin);
  renwin->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
