#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCellLocator.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTriangleFilter.h>
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
    // Small sphere with most polgons
    vtkNew<vtkSphereSource> sphereSource1;
    sphereSource1->SetThetaResolution(10);
    sphereSource1->SetPhiResolution(10);
    sphereSource1->SetCenter(5, 5, 5);
    sphereSource1->SetRadius(1.5);

    // Large sphere with least polygons
    vtkNew<vtkSphereSource> sphereSource2;
    sphereSource2->SetRadius(10);
    sphereSource2->SetCenter(10, 1, 10);

    vtkNew<vtkAppendPolyData> appendFilter;
    appendFilter->AddInputConnection(sphereSource1->GetOutputPort());
    appendFilter->AddInputConnection(sphereSource2->GetOutputPort());
    appendFilter->Update();
    polyData = appendFilter->GetOutput();
  }

  double center[3], bounds[6];
  polyData->GetCenter(center);
  std::cout << "Center of data is: " << center[0] << ", " << center[1] << ", "
            << center[2] << std::endl;
  polyData->GetPoints()->GetBounds(bounds);
  std::cout << "Bounds of data is: " << bounds[0] << ", " << bounds[1] << ", "
            << bounds[2] << ", " << bounds[3] << ", " << bounds[4] << ", "
            << bounds[5] << std::endl;

  // Build a cell locator.
  vtkNew<vtkCellLocator> cellLocator;
  cellLocator->SetDataSet(polyData);
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
  std::cout << "Id of cell on outside surface: " << cellId << std::endl;

  vtkNew<vtkPolyDataConnectivityFilter> connectivityFilter;
  connectivityFilter->SetInputData(polyData);
  connectivityFilter->SetExtractionModeToCellSeededRegions();
  connectivityFilter->InitializeSeedList();
  connectivityFilter->AddSeed(cellId);

  // Create a mapper and actor for original data
  vtkNew<vtkPolyDataMapper> originalMapper;
  originalMapper->SetInputData(polyData);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->BackfaceCullingOn();
  originalActor->GetProperty()->SetOpacity(0.6);
  originalActor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  // Create a mapper and actor for extracted data
  vtkNew<vtkPolyDataMapper> extractedMapper;
  extractedMapper->SetInputConnection(connectivityFilter->GetOutputPort());

  vtkNew<vtkActor> extractedActor;
  extractedActor->GetProperty()->SetColor(
      colors->GetColor3d("Peacock").GetData());
  extractedActor->SetMapper(extractedMapper);
  extractedActor->GetProperty()->SetOpacity(0.6);
  extractedActor->GetProperty()->BackfaceCullingOn();

  // Create a renderer
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(originalActor);
  renderer->AddActor(extractedActor);

  renderer->GradientBackgroundOn();
  renderer->SetBackground2(colors->GetColor3d("Beige").GetData());
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  extractedActor->SetPosition((bounds[1] - bounds[0]) / 1.9, 0, 0);
  originalActor->SetPosition(-(bounds[1] - bounds[0]) / 1.9, 0, 0);
  // Create a render window
  vtkNew<vtkRenderWindow> renwin;
  renwin->AddRenderer(renderer);
  renwin->SetSize(512, 512);
  renwin->SetWindowName("ExtractOutsideSurface");

  // Create an interactor
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renwin);
  renwin->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
