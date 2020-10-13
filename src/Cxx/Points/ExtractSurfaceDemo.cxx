#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkExtractSurface.h>
#include <vtkGlyph3D.h>
#include <vtkMaskPoints.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPCANormalEstimation.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSignedDistance.h>
#include <vtkSmartPointer.h>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>

#include <vtksys/SystemTools.hxx>

namespace {
void MakeGlyphs(vtkPolyData* src, double size, vtkGlyph3D* glyph);

vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
} // namespace

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  auto polyData = ReadPolyData(argc > 1 ? argv[1] : "");

  double bounds[6];
  polyData->GetBounds(bounds);
  double range[3];
  for (int i = 0; i < 3; ++i)
  {
    range[i] = bounds[2 * i + 1] - bounds[2 * i];
  }

  int sampleSize = 15;

  std::cout << "Sample size is: " << sampleSize << std::endl;
  // Do we need to estimate normals?
  vtkNew<vtkSignedDistance> distance;
  vtkNew<vtkPCANormalEstimation> normals;
  if (polyData->GetPointData()->GetNormals())
  {
    std::cout << "Using normals from input file" << std::endl;
    distance->SetInputData(polyData);
  }
  else
  {
    std::cout << "Estimating normals using PCANormalEstimation" << std::endl;
    normals->SetInputData(polyData);
    normals->SetSampleSize(sampleSize);
    normals->SetNormalOrientationToGraphTraversal();
    normals->FlipNormalsOn();
    distance->SetInputConnection(normals->GetOutputPort());
  }
  std::cout << "Range: " << range[0] << ", " << range[1] << ", " << range[2]
            << std::endl;
  int dimension = 512;
  double radius;
  radius = range[0] / static_cast<double>(dimension) * 3; // ~3 voxels
  std::cout << "Radius: " << radius << std::endl;

  distance->SetRadius(radius);
  distance->SetDimensions(dimension, dimension, dimension);
  distance->SetBounds(bounds[0] - range[0] * .1, bounds[1] + range[0] * .1,
                      bounds[2] - range[1] * .1, bounds[3] + range[1] * .1,
                      bounds[4] - range[2] * .1, bounds[5] + range[2] * .1);

  vtkNew<vtkExtractSurface> surface;
  surface->SetInputConnection(distance->GetOutputPort());
  surface->SetRadius(radius * .99);
  surface->HoleFillingOn();
  surface->Update();

  vtkNew<vtkGlyph3D> glyph3D;
  if (polyData->GetPointData()->GetNormals())
  {
    MakeGlyphs(polyData, radius * 2.0, glyph3D.GetPointer());
  }
  else
  {
    MakeGlyphs(normals->GetOutput(), radius * 2.0, glyph3D.GetPointer());
  }
  vtkNew<vtkPolyDataMapper> surfaceMapper;
  surfaceMapper->SetInputConnection(surface->GetOutputPort());
  surfaceMapper->ScalarVisibilityOff();

  vtkNew<vtkProperty> backProp;
  backProp->SetColor(colors->GetColor3d("Banana").GetData());

  vtkNew<vtkActor> surfaceActor;
  surfaceActor->SetMapper(surfaceMapper);
  surfaceActor->GetProperty()->SetColor(colors->GetColor3d("Coral").GetData());
  surfaceActor->SetBackfaceProperty(backProp);

  vtkNew<vtkPolyDataMapper> glyph3DMapper;
  glyph3DMapper->SetInputConnection(glyph3D->GetOutputPort());

  vtkNew<vtkActor> glyph3DActor;
  glyph3DActor->SetMapper(glyph3DMapper);
  glyph3DActor->GetProperty()->SetColor(
      colors->GetColor3d("MidnightBlue").GetData());

  // Create graphics stuff
  //
  vtkNew<vtkRenderer> ren1;
  ren1->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->SetSize(512, 512);
  renWin->SetWindowName("ExtractSurfaceDemo");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(surfaceActor);
  ren1->AddActor(glyph3DActor);

  // Generate an interesting view
  //
  ren1->ResetCamera();
  ren1->GetActiveCamera()->Azimuth(120);
  ren1->GetActiveCamera()->Elevation(30);
  ren1->GetActiveCamera()->Dolly(1.0);
  ren1->ResetCameraClippingRange();

  renWin->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
namespace {
//-----------------------------------------------------------------------------
void MakeGlyphs(vtkPolyData* src, double size, vtkGlyph3D* glyph)
{
  vtkNew<vtkMaskPoints> maskPts;
  maskPts->SetOnRatio(20);
  maskPts->RandomModeOn();
  maskPts->SetInputData(src);
  // Source for the glyph filter
  vtkNew<vtkArrowSource> arrow;
  arrow->SetTipResolution(16);
  arrow->SetTipLength(0.3);
  arrow->SetTipRadius(0.1);

  glyph->SetSourceConnection(arrow->GetOutputPort());
  glyph->SetInputConnection(maskPts->GetOutputPort());
  glyph->SetVectorModeToUseNormal();
  //  glyph->SetColorModeToColorByVector();
  glyph->SetScaleModeToScaleByVector();
  glyph->SetScaleFactor(size);
  glyph->OrientOn();
  glyph->Update();
}

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
    vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
    randomSequence->SetSeed(8775070);

    vtkNew<vtkPointSource> points;
    points->SetNumberOfPoints(100000);
    points->SetRadius(10.0);
    double x, y, z;
    // random position
    x = randomSequence->GetRangeValue(-100, 100);
    randomSequence->Next();
    y = randomSequence->GetRangeValue(-100, 100);
    randomSequence->Next();
    z = randomSequence->GetRangeValue(-100, 100);
    randomSequence->Next();
    points->SetCenter(x, y, z);
    points->SetDistributionToShell();
    points->Update();
    polyData = points->GetOutput();
  }
  return polyData;
}
} // namespace
