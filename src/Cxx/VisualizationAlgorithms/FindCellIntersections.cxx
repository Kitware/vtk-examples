#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkNew.h>
#include <vtkCellArray.h>
#include <vtkCellLocator.h>
#include <vtkDataObject.h>
#include <vtkIdList.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkCellData.h>
#include <vtkLineSource.h>
#include <vtkUnsignedCharArray.h>
#include <vtkShrinkFilter.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " filename.vtu e.g. Disc_BiQuadraticQuads_0_0.vtu" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  // Get the filename from the command line
  std::string inputFilename = argv[1];

  // Read a xml unstructured grid dataset
  vtkNew<vtkXMLUnstructuredGridReader> reader;
  reader->SetFileName(inputFilename.c_str());
  reader->Update();

  // The line goes through the center of the data and runs along the x axis
  double center[3];
  reader->GetOutput()->GetCenter(center);
  double bounds[6];
  reader->GetOutput()->GetBounds(bounds);

  double startRay[3];
  startRay[0] = bounds[0] - center[0];
  startRay[1] = center[1];
  startRay[2] = center[2];

  double endRay[3];
  endRay[0] = bounds[1] + center[0];
  endRay[1] = center[1];
  endRay[2] = center[2];
  std::cout << "center: " << center[0] << ", " << center[1] << ", " << center[2]
            << std::endl;
  std::cout << "startRay: " << startRay[0] << ", " << startRay[1] << ", "
            << startRay[2] << std::endl;
  std::cout << "endRay: " << endRay[0] << ", " << endRay[1] << ", " << endRay[2]
            << std::endl;

  // Initialize all of the cell data colors
  vtkNew<vtkUnsignedCharArray> cellData;
  cellData->SetNumberOfComponents(3);
  cellData->SetNumberOfTuples(reader->GetOutput()->GetNumberOfCells());
  reader->GetOutput()->GetCellData()->SetScalars(cellData);

  // A lambda to scale the contents of the array x by 255
  auto scale = [](double* x) {
    for (int i = 0; i < 3; ++i)
    {
      x[i] = x[i] * 255;
    };
  };

  double rgb[3]{0, 0, 0};
  colors->GetColorRGB("Banana", rgb);
  scale(rgb);
  for (int i = 0; i < cellData->GetNumberOfTuples(); ++i)
  {
    cellData->InsertTuple(i, rgb);
  }

  // Find the cells that intersect the line and color those cells
  vtkNew<vtkIdList> cellIds;
  vtkNew<vtkCellLocator> locator;
  locator->SetDataSet(reader->GetOutput());
  locator->BuildLocator();
  locator->FindCellsAlongLine(startRay, endRay, 0.001, cellIds);

  colors->GetColorRGB("Tomato", rgb);
  scale(rgb);
  for (vtkIdType i = 0; i < cellIds->GetNumberOfIds(); ++i)
  {
    cellData->InsertTuple(cellIds->GetId(i), rgb);
  }

  // Shrink each cell to make them visible
  vtkNew<vtkShrinkFilter> shrink;
  shrink->SetInputConnection(reader->GetOutputPort());
  shrink->SetShrinkFactor(0.95);

  // Convert the cells to polydata
  vtkNew<vtkDataSetSurfaceFilter> surface;
  surface->SetInputConnection(shrink->GetOutputPort());
  surface->SetNonlinearSubdivisionLevel(2);
  surface->Update();

  // Create a line
  vtkNew<vtkLineSource> lineSource;
  lineSource->SetPoint1(startRay);
  lineSource->SetPoint2(endRay);
  vtkNew<vtkPolyDataMapper> lineMapper;
  lineMapper->SetInputConnection(lineSource->GetOutputPort());
  vtkNew<vtkActor> lineActor;
  lineActor->SetMapper(lineMapper);

  // Render the results
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(surface->GetOutputPort());
  mapper->ScalarVisibilityOn();
  mapper->SetScalarModeToUseCellData();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->AddActor(lineActor);
  renderer->SetBackground(colors->GetColor3d("Cobalt").GetData());

  // Make an oblique view
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCamera();

  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderWindowInteractor> iren;

  iren->SetRenderWindow(renWin);
  renWin->AddRenderer(renderer);
  renWin->SetSize(640, 480);
  renWin->SetWindowName("FindCellIntersections");

  renWin->Render();

  iren->Start();

  return EXIT_SUCCESS;
}
