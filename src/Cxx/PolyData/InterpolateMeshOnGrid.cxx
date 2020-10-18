#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellLocator.h>
#include <vtkDataSetMapper.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLine.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProbeFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTriangle.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkWarpScalar.h>
#include <vtkXMLPolyDataWriter.h>

#include <iostream>

namespace {

// Get the camera position.
void CameraModifiedCallback(vtkObject* caller,
                            long unsigned int vtkNotUsed(eventId),
                            void* vtkNotUsed(clientData),
                            void* vtkNotUsed(callData));
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a random set of 100 points in the xy-plane in (0,10)x(0,10).
  // If you had instead started with a set of (x,y,z) points, you must copy
  // the zvalues into a FloatArray and set it as the data set's scalars,
  // and then set the z-coordinates to zero
  vtkNew<vtkPoints> randomPoints;

  vtkNew<vtkFloatArray> zvalues;
  zvalues->SetName("ZValues");

  unsigned int gridSize = 10;
  float maxHeight = 1;
  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(8775070);
  for (unsigned int i = 0; i < 100; ++i)
  {
    double x, y, z;
    // random position and radius
    x = randomSequence->GetRangeValue(0, gridSize);
    randomSequence->Next();
    y = randomSequence->GetRangeValue(0, gridSize);
    randomSequence->Next();
    z = randomSequence->GetRangeValue(0, maxHeight);
    randomSequence->Next();

    // randomPoints->InsertNextPoint ( x, y, z);
    randomPoints->InsertNextPoint(x, y, 0);
    zvalues->InsertNextValue(z);
  }

  // Add the grid points to a polydata object
  vtkNew<vtkPolyData> randomPolyData;
  randomPolyData->SetPoints(randomPoints);
  randomPolyData->GetPointData()->SetScalars(zvalues);

  // Triangulate the grid points. If you do not have a mesh (points
  // only), the output will not be interpolated!
  vtkNew<vtkDelaunay2D> randomDelaunay;
  randomDelaunay->SetInputData(randomPolyData);
  randomDelaunay->Update();

  // Create a grid of points to interpolate over
  vtkNew<vtkPoints> gridPoints;
  for (unsigned int x = 0; x < gridSize; x++)
  {
    for (unsigned int y = 0; y < gridSize; y++)
    {
      gridPoints->InsertNextPoint(x, y, 0);
    }
  }

  // Create a dataset from the grid points
  vtkNew<vtkPolyData> gridPolyData;
  gridPolyData->SetPoints(gridPoints);

  // Perform the interpolation
  vtkNew<vtkProbeFilter> probeFilter;
  probeFilter->SetSourceConnection(randomDelaunay->GetOutputPort());
  probeFilter->SetInputData(
      gridPolyData); //
                     // Interpolate 'Source' at these points
  probeFilter->Update();

  // Map the output zvalues to the z-coordinates of the data so that
  // we get a surface, rather than a flat grid with interpolated
  // scalars.
  vtkNew<vtkWarpScalar> gridWarpScalar;
  gridWarpScalar->SetInputConnection(probeFilter->GetOutputPort());
  gridWarpScalar->Update();

  //////// Setup outputs ////////
  // Output random points
  // Map the output zvalues to the z-coordinates of the data
  vtkNew<vtkWarpScalar> randomWarpScalar;
  randomWarpScalar->SetInputConnection(randomDelaunay->GetOutputPort());
  randomWarpScalar->Update();

  vtkNew<vtkXMLPolyDataWriter> randomWriter;
  randomWriter->SetFileName("randomSurface.vtp");
  randomWriter->SetInputConnection(randomWarpScalar->GetOutputPort());
  randomWriter->Write();

  // Mesh the output grid points
  vtkNew<vtkDelaunay2D> gridDelaunay;
  gridDelaunay->SetInputConnection(gridWarpScalar->GetOutputPort());

  vtkNew<vtkXMLPolyDataWriter> gridWriter;
  gridWriter->SetFileName("gridSurface.vtp");
  gridWriter->SetInputConnection(gridDelaunay->GetOutputPort());
  gridWriter->Write();

  ////////// Setup visualization ////////
  vtkNew<vtkDataSetMapper> randomMapper;
  randomMapper->SetInputConnection(randomWarpScalar->GetOutputPort());
  randomMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> randomActor;
  randomActor->SetMapper(randomMapper);
  randomActor->GetProperty()->SetColor(colors->GetColor3d("Salmon").GetData());
  randomActor->GetProperty()->SetPointSize(3);

  vtkNew<vtkDataSetMapper> gridMapper;
  gridMapper->SetInputConnection(gridDelaunay->GetOutputPort());
  gridMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> gridActor;
  gridActor->SetMapper(gridMapper);
  gridActor->GetProperty()->SetColor(colors->GetColor3d("SteelBlue").GetData());
  gridActor->GetProperty()->SetPointSize(3);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("InterpolateMeshOnGrid");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  renderWindowInteractor->SetInteractorStyle(style);

  renderer->AddActor(randomActor);
  renderer->AddActor(gridActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(5.87551, -15.9539, 17.5858);
  camera->SetFocalPoint(4.94546, 4.90463, 0.498443);
  camera->SetViewUp(-0.00568191, 0.633548, 0.773683);
  camera->SetDistance(26.98);
  camera->SetClippingRange(18.197, 38.1011);

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
