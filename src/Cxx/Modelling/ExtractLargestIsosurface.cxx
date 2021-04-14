#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkMarchingCubes.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredPointsReader.h>
#include <vtkVersion.h>

// vtkFlyingEdges3D was introduced in VTK >= 8.2
#if VTK_MAJOR_VERSION >= 9 || (VTK_MAJOR_VERSION >= 8 && VTK_MINOR_VERSION >= 2)
#define USE_FLYING_EDGES
#else
#undef USE_FLYING_EDGES
#endif

#ifdef USE_FLYING_EDGES
#include <vtkFlyingEdges3D.h>
#else
#include <vtkMarchingCubes.h>
#endif

#include <array>

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0]
              << " InputFile(.vtk) Threshold e.g. brain.vtk 50 1" << std::endl;
    return EXIT_FAILURE;
  }
  const char* fileName = argv[1];
  float threshold = atof(argv[2]);
  auto extractLargest = true;
  if (argc == 4)
  {
    extractLargest = atoi(argv[3]) == 1;
  }

  vtkNew<vtkNamedColors> colors;

  std::array<unsigned char, 4> skinColor{{240, 184, 160, 255}};
  colors->SetColor("SkinColor", skinColor.data());
  std::array<unsigned char, 4> backColor{{255, 229, 200, 255}};
  colors->SetColor("BackfaceColor", backColor.data());
  std::array<unsigned char, 4> bkg{{51, 77, 102, 255}};

  // Load data
  vtkNew<vtkStructuredPointsReader> reader;
  reader->SetFileName(fileName);

  // Create a 3D model using flying edges or marching cubes
#ifdef USE_FLYING_EDGES
  vtkNew<vtkFlyingEdges3D> mc;
#else
  vtkNew<vtkMarchingCubes> mc;
#endif

  mc->SetInputConnection(reader->GetOutputPort());
  mc->ComputeNormalsOn();
  mc->ComputeGradientsOn();
  mc->SetValue(0, threshold); // second value acts as threshold

  // To remain largest region
  vtkNew<vtkPolyDataConnectivityFilter> confilter;
  confilter->SetInputConnection(mc->GetOutputPort());
  confilter->SetExtractionModeToLargestRegion();

  // Create a mapper
  vtkNew<vtkPolyDataMapper> mapper;
  if (extractLargest)
  {
    mapper->SetInputConnection(confilter->GetOutputPort());
  }
  else
  {
    mapper->SetInputConnection(mc->GetOutputPort());
  }
  mapper->ScalarVisibilityOff();

  // Visualize
  vtkNew<vtkActor> actor;
  actor->GetProperty()->SetColor(colors->GetColor3d("SkinColor").GetData());
  vtkNew<vtkProperty> backProp;
  backProp->SetDiffuseColor(colors->GetColor3d("BackfaceColor").GetData());
  actor->SetBackfaceProperty(backProp);
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderer->GetActiveCamera()->SetViewUp(0.0, 0.0, 1.0);
  renderer->GetActiveCamera()->SetPosition(0.0, 1.0, 0.0);
  renderer->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30.0);
  renderer->GetActiveCamera()->Elevation(30.0);
  vtkNew<vtkRenderWindow> renwin;
  renwin->AddRenderer(renderer);
  renwin->SetSize(640, 480);
  renwin->SetWindowName("ExtractLargestIsosurface");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renwin);
  renwin->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
