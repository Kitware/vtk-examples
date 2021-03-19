#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkProbeFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSplineFilter.h>
#include <vtkWindowLevelLookupTable.h>

#include <sstream>

namespace {
vtkSmartPointer<vtkPolyData> SweepLine(vtkPolyData* line, double direction[3],
                                       double distance, unsigned int cols);
}

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify arguments
  if (argc < 4)
  {
    std::cout << "Usage: " << argv[0] << " InputVolume PolyDataInput"
              << " Resolution" << std::endl;
    std::cout << "e.g. HeadMRVolume.mhd polyline.vtk 200" << std::endl;
    return EXIT_FAILURE;
  }

  // Parse arguments
  std::string volumeFileName = argv[1];
  std::string polyDataFileName = argv[2];
  std::stringstream ssResolution;
  ssResolution << argv[3];
  unsigned int resolution;
  ssResolution >> resolution;

  // Output arguments
  std::cout << "InputVolume: " << volumeFileName << std::endl
            << "PolyDataInput: " << polyDataFileName << std::endl
            << "Resolution: " << resolution << std::endl;

  // Read the volume data
  vtkNew<vtkImageReader2Factory> imageFactory;
  vtkSmartPointer<vtkImageReader2> imageReader;
  imageReader.TakeReference(
      imageFactory->CreateImageReader2(volumeFileName.c_str()));
  imageReader->SetFileName(volumeFileName.c_str());
  imageReader->Update();

  // Read the Polyline
  vtkNew<vtkPolyDataReader> polyLineReader;
  polyLineReader->SetFileName(polyDataFileName.c_str());
  polyLineReader->Update();

  vtkNew<vtkSplineFilter> spline;
  spline->SetInputConnection(polyLineReader->GetOutputPort());
  spline->SetSubdivideToSpecified();
  spline->SetNumberOfSubdivisions(resolution);

  // Sweep the line to form a surface
  double direction[3];
  direction[0] = 0.0;
  direction[1] = 0.0;
  direction[2] = 1.0;
  double distance = 164;
  spline->Update();
  auto surface =
      SweepLine(spline->GetOutput(), direction, distance, atoi(argv[3]));

  // Probe the volume with the extruded surface
  vtkNew<vtkProbeFilter> sampleVolume;
  sampleVolume->SetInputConnection(1, imageReader->GetOutputPort());
  sampleVolume->SetInputData(0, surface);

  // Compute a simple window/level based on scalar range
  vtkNew<vtkWindowLevelLookupTable> wlLut;
  double range = imageReader->GetOutput()->GetScalarRange()[1] -
      imageReader->GetOutput()->GetScalarRange()[0];
  double level = (imageReader->GetOutput()->GetScalarRange()[1] +
                  imageReader->GetOutput()->GetScalarRange()[0]) /
      2.0;
  wlLut->SetWindow(range);
  wlLut->SetLevel(level);

  // Create a mapper and actor.
  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(sampleVolume->GetOutputPort());
  mapper->SetLookupTable(wlLut);
  mapper->SetScalarRange(0, 255);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("CurvedReformation");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  // Set the camera for viewing medical images
  renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
  renderer->GetActiveCamera()->SetPosition(0, 0, 0);
  renderer->GetActiveCamera()->SetFocalPoint(0, 1, 0);
  renderer->ResetCamera();

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {

vtkSmartPointer<vtkPolyData> SweepLine(vtkPolyData* line, double direction[3],
                                       double distance, unsigned int cols)
{
  unsigned int rows = line->GetNumberOfPoints();
  double spacing = distance / cols;
  vtkNew<vtkPolyData> surface;

  // Generate the points
  cols++;
  unsigned int numberOfPoints = rows * cols;
  unsigned int numberOfPolys = (rows - 1) * (cols - 1);
  vtkNew<vtkPoints> points;
  points->Allocate(numberOfPoints);
  vtkNew<vtkCellArray> polys;
  polys->Allocate(numberOfPolys * 4);

  double x[3];
  unsigned int cnt = 0;
  for (unsigned int row = 0; row < rows; row++)
  {
    for (unsigned int col = 0; col < cols; col++)
    {
      double p[3];
      line->GetPoint(row, p);
      x[0] = p[0] + direction[0] * col * spacing;
      x[1] = p[1] + direction[1] * col * spacing;
      x[2] = p[2] + direction[2] * col * spacing;
      points->InsertPoint(cnt++, x);
    }
  }
  // Generate the quads
  vtkIdType pts[4];
  for (unsigned int row = 0; row < rows - 1; row++)
  {
    for (unsigned int col = 0; col < cols - 1; col++)
    {
      pts[0] = col + row * (cols);
      pts[1] = pts[0] + 1;
      pts[2] = pts[0] + cols + 1;
      pts[3] = pts[0] + cols;
      polys->InsertNextCell(4, pts);
    }
  }
  surface->SetPoints(points);
  surface->SetPolys(polys);

  return surface;
}
} // namespace
