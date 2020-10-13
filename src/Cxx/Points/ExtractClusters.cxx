#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkEuclideanClusterExtraction.h>
#include <vtkGlyph3D.h>
#include <vtkLookupTable.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  // randomSequence->SetSeed(8775070);
  randomSequence->SetSeed(4355412);

  double limits = 10;
  double radius = 0.5;

  vtkNew<vtkAppendPolyData> append;
  for (unsigned i = 0; i < 30; ++i)
  {
    vtkNew<vtkPointSource> points;

    points->SetNumberOfPoints(800);
    points->SetRadius(2.5 * radius);
    double x, y, z;
    // random position
    x = randomSequence->GetRangeValue(-limits, limits);
    randomSequence->Next();
    y = randomSequence->GetRangeValue(-limits, limits);
    randomSequence->Next();
    z = randomSequence->GetRangeValue(-limits, limits);
    randomSequence->Next();
    points->SetCenter(x, y, z);
    points->SetDistributionToShell();

    append->AddInputConnection(points->GetOutputPort());
  }

  vtkNew<vtkEuclideanClusterExtraction> cluster;
  cluster->SetInputConnection(append->GetOutputPort());
  cluster->SetExtractionModeToAllClusters();
  cluster->SetRadius(radius);
  cluster->ColorClustersOn();
  cluster->Update();

  std::cout << "Found " << cluster->GetNumberOfExtractedClusters()
            << " clusters within radius " << radius << std::endl;

  // Create a lookup table to map point data to colors
  vtkNew<vtkLookupTable> lut;
  int tableSize = cluster->GetNumberOfExtractedClusters();
  lut->SetNumberOfTableValues(tableSize);
  lut->Build();

  // Fill in the lookup table
  for (unsigned int i = 0; static_cast<int>(i) < tableSize; ++i)
  {
    double r, g, b;
    r = randomSequence->GetRangeValue(0.25, 1.0);
    randomSequence->Next();
    g = randomSequence->GetRangeValue(0.25, 1.0);
    randomSequence->Next();
    b = randomSequence->GetRangeValue(0.25, 1.0);
    randomSequence->Next();
    lut->SetTableValue(i, r, g, b, 1.0);
  }

  vtkNew<vtkSphereSource> sphere;
  sphere->SetRadius(radius / 2.0);

  vtkNew<vtkGlyph3D> glyphs;
  glyphs->SetInputConnection(cluster->GetOutputPort());
  glyphs->SetSourceConnection(sphere->GetOutputPort());
  glyphs->ScalingOff();
  glyphs->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(glyphs->GetOutputPort());
  mapper->SetScalarRange(0, tableSize - 1);
  mapper->SetLookupTable(lut);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Create graphics stuff
  //
  vtkNew<vtkRenderer> ren1;
  ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->SetSize(640, 512);
  renWin->SetWindowName("ExtractClusters");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer
  //
  ren1->AddActor(actor);

  // Generate an interesting view
  //
  ren1->ResetCamera();
  ren1->GetActiveCamera()->Azimuth(120);
  ren1->GetActiveCamera()->Elevation(30);
  ren1->GetActiveCamera()->Dolly(1.5);
  ren1->ResetCameraClippingRange();

  renWin->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
