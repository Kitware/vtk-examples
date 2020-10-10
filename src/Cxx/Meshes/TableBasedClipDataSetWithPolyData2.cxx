// This program draws a checkerboard ala an OOF3D image display and
// clips it with two planes.

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkExecutive.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkLookupTable.h>
#include <vtkMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRectilinearGrid.h>
#include <vtkRenderStepsPass.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTableBasedClipDataSet.h>

///////////////////
namespace {
// Make a rectilinear grid that shows image-like data, by creating a
// cubic cell at each point. A checkerboard pattern is used for
// simplicity.

vtkSmartPointer<vtkRectilinearGrid> makeImage(int n);

} // namespace

int main(int, char*[])
{
  constexpr int imageSize = 64; // number of checkerboard squares on a side

  // Offsets for clipping planes with normals in the X and Y directions
  constexpr int xOffset = 8;
  constexpr int yOffset = 8;

  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();

  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("Wheat").GetData());
  renderer->UseHiddenLineRemovalOn();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("TableBasedClipDataSetWithPolyData2");

  vtkNew<vtkRenderWindowInteractor> interactor;
  vtkNew<vtkInteractorStyleSwitch> style;
  interactor->SetInteractorStyle(style);
  interactor->SetRenderWindow(renderWindow);

  vtkSmartPointer<vtkRectilinearGrid> image = makeImage(imageSize);

  // Clipping planes in the X and Y direction.
  auto normals = vtkSmartPointer<vtkDoubleArray>::New();
  vtkNew<vtkPoints> clipPts;
  normals->SetNumberOfComponents(3);
  double xnorm[3] = {-1., 0., 0.};
  double ynorm[3] = {0., -1., 0.};
  double xpt[3] = {xOffset, 0., 0.};
  double ypt[3] = {0., yOffset, 0.};
  normals->InsertNextTuple(xnorm);
  normals->InsertNextTuple(ynorm);
  clipPts->InsertNextPoint(xpt);
  clipPts->InsertNextPoint(ypt);

  vtkNew<vtkPlanes> clipPlanes;
  clipPlanes->SetNormals(normals);
  clipPlanes->SetPoints(clipPts);

  vtkNew<vtkTableBasedClipDataSet> clipper;
  clipper->SetClipFunction(clipPlanes);
  clipper->SetInputData(image);

  vtkNew<vtkDataSetMapper> imageMapper;
  vtkNew<vtkActor> imageActor;
  imageActor->SetMapper(imageMapper);
  renderer->AddViewProp(imageActor);
  imageMapper->SetInputConnection(clipper->GetOutputPort());

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(120);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCameraClippingRange();
  renderWindow->Render();

  interactor->Start();
  return EXIT_SUCCESS;
}

namespace {

vtkSmartPointer<vtkRectilinearGrid> makeImage(int n)
{
  constexpr double cubeSize =
      20.0; // physical linear dimension of entire system

  // This is a simplification of a program that uses actual image data
  // as a source for the rectilinear grid.  In order to recreate the
  // same vtk calls, create a dummy image here.
  vtkNew<vtkImageData> image0;
  image0->SetDimensions(n, n, n);
  image0->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
  image0->SetSpacing(cubeSize / n, cubeSize / n, cubeSize / n);
  int checkerSize = n / 8;
  for (int z = 0; z < n; z++)
  {
    for (int y = 0; y < n; y++)
    {
      for (int x = 0; x < n; x++)
      {
        unsigned char* ptr = (unsigned char*)image0->GetScalarPointer(x, y, z);
        *ptr = (x / checkerSize + y / checkerSize + z / checkerSize) %
            2; // checkerboard
      }
    }
  }

  // Color for the checkerboard image
  // double r = 0.7;
  // double g = 0.7;
  // double b = 0.7;
  // double a = 1.0;
  // double dim = 0.5; // amount to dim the dark squares by

  vtkNew<vtkNamedColors> colors;
  // colors->GetColor("LightCoral", r, g, b, a);

  vtkNew<vtkLookupTable> lut;
  lut->SetNumberOfTableValues(2);
  lut->SetTableRange(0, 1);
  // lut->SetTableValue(0, r, g, b, a);
  // lut->SetTableValue(1, dim * r, dim * g, dim * b, a);
  // Or we can have fun ...
  lut->SetTableValue(0, colors->GetColor4d("Thistle").GetData());
  lut->SetTableValue(1, colors->GetColor4d("DarkSlateBlue").GetData());

  vtkNew<vtkImageMapToColors> map;
  map->SetLookupTable(lut);
  map->SetOutputFormatToRGBA();
  map->SetInputData(image0);
  map->GetExecutive()->Update();
  vtkImageData* image = map->GetOutput();

  // Convert the image to a rectilinear grid.  Each point in the image
  // becomes a cubic cell in the grid.

  vtkNew<vtkRectilinearGrid> rectgrid;

  int extent[6];
  image->GetExtent(extent);
  extent[1] += 1;
  extent[3] += 1;
  extent[5] += 1;
  rectgrid->SetExtent(extent);

  vtkNew<vtkDoubleArray> xcoords;
  vtkNew<vtkDoubleArray> ycoords;
  vtkNew<vtkDoubleArray> zcoords;
  xcoords->SetNumberOfValues(n + 1);
  ycoords->SetNumberOfValues(n + 1);
  zcoords->SetNumberOfValues(n + 1);
  double spacing[3];
  image->GetSpacing(spacing);
  for (vtkIdType i = 0; i <= n; i++)
  {
    xcoords->InsertValue(i, i * spacing[0]);
    ycoords->InsertValue(i, i * spacing[1]);
    zcoords->InsertValue(i, i * spacing[2]);
  }
  rectgrid->SetXCoordinates(xcoords);
  rectgrid->SetYCoordinates(ycoords);
  rectgrid->SetZCoordinates(zcoords);

  vtkPointData* pointData = image->GetPointData();
  vtkCellData* cellData = rectgrid->GetCellData();
  cellData->ShallowCopy(pointData);
  return rectgrid;
}

} // namespace
