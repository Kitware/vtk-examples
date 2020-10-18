#include <vtkActor.h>
#include <vtkBYUReader.h>
#include <vtkContourFilter.h>
#include <vtkImplicitModeller.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

#include <vtksys/SystemTools.hxx>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
}

int main(int argc, char* argv[])
{
  vtkSmartPointer<vtkPolyData> polyData = ReadPolyData(argc > 1 ? argv[1] : "");
  ;

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkImplicitModeller> implicitModeller;
  implicitModeller->SetSampleDimensions(50, 50, 50);
  implicitModeller->SetInputData(polyData);
  implicitModeller->AdjustBoundsOn();
  implicitModeller->SetAdjustDistance(.1); // Adjust by 10%
  implicitModeller->SetMaximumDistance(.1);

  // Compute the range to select a reasonable contour value
  double bounds[6];
  polyData->GetBounds(bounds);
  double xrange = bounds[1] - bounds[0];

  // Create the 0 isosurface
  vtkNew<vtkContourFilter> contourFilter;
  contourFilter->SetInputConnection(implicitModeller->GetOutputPort());
  contourFilter->SetValue(0, xrange / 30.0); // 30% of xrange

  // Visualize
  vtkNew<vtkRenderer> renderer;

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(contourFilter->GetOutputPort());
  mapper->ScalarVisibilityOff();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Sienna").GetData());
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Wheat").GetData());

  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("ImplicitModeller");

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName)
{
  vtkSmartPointer<vtkPolyData> polyData;
  std::string extension =
      vtksys::SystemTools::GetFilenameExtension(std::string(fileName));
  if (extension == ".ply")
  {
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtp")
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtk")
  {
    vtkNew<vtkPolyDataReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".obj")
  {
    vtkNew<vtkOBJReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".stl")
  {
    vtkNew<vtkSTLReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".g")
  {
    vtkNew<vtkBYUReader> reader;
    reader->SetGeometryFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    vtkNew<vtkSphereSource> sphere;
    sphere->Update();
    polyData = sphere->GetOutput();
  }
  return polyData;
}
} // namespace
