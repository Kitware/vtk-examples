// This program draws a checkerboard and clips it with two planes.

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkClipVolume.h>
#include <vtkColor.h>
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
#include <vtkRenderStepsPass.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {

constexpr auto IMAGESIZE{64};  // number of checkerboard squares on a side
constexpr auto CUBESIZE{20.0}; // physical linear dimension of entire system

// Color for the checkerboard image
constexpr auto DIM{0.5}; // amount to dim the dark squares by

// Offsets for clipping planes with normals in the X and Y directions
constexpr auto XOFFSET{8};
constexpr auto YOFFSET{8};

///////////////////

// Make the image data. A checkerboard pattern is used for
// simplicity.
vtkSmartPointer<vtkImageData> makeImage(int n, vtkColor3d fillColor,
                                        vtkColor3d checkerColor);
} // namespace

int main(int, char*[])
{

  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();

  // Define colors
  vtkNew<vtkNamedColors> colors;
  vtkColor3d backgroundColor = colors->GetColor3d("Wheat");
  vtkColor3d checkerColor = colors->GetColor3d("Tomato");
  vtkColor3d fillColor = colors->GetColor3d("Banana");

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(backgroundColor.GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);

  vtkNew<vtkRenderWindowInteractor> interactor;
  vtkNew<vtkInteractorStyleSwitch> style;
  interactor->SetInteractorStyle(style);
  interactor->SetRenderWindow(renderWindow);

  auto image = makeImage(IMAGESIZE, fillColor, checkerColor);

  // Clipping planes in the X and Y direction.
  vtkNew<vtkDoubleArray> normals;
  vtkNew<vtkPoints> clipPts;
  normals->SetNumberOfComponents(3);
  double xnorm[3] = {-1., 0., 0.};
  double ynorm[3] = {0., -1., 0.};
  double xpt[3] = {XOFFSET, 0., 0.};
  double ypt[3] = {0., YOFFSET, 0.};
  normals->InsertNextTuple(xnorm);
  normals->InsertNextTuple(ynorm);
  clipPts->InsertNextPoint(xpt);
  clipPts->InsertNextPoint(ypt);
  vtkNew<vtkPlanes> clipPlanes;
  clipPlanes->SetNormals(normals);
  clipPlanes->SetPoints(clipPts);

  vtkNew<vtkClipVolume> clipper;
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
  renderWindow->SetWindowName("ClipVolume");
  renderWindow->Render();

  interactor->Start();

  return EXIT_SUCCESS;
  ;
}

namespace {
// Make the image data. A checkerboard pattern is used for
// simplicity.
vtkSmartPointer<vtkImageData> makeImage(int n, vtkColor3d fillColor,
                                        vtkColor3d checkerColor)
{
  vtkNew<vtkImageData> image0;
  image0->SetDimensions(n, n, n);
  image0->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
  image0->SetSpacing(CUBESIZE / n, CUBESIZE / n, CUBESIZE / n);
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

  vtkNew<vtkLookupTable> lut;
  lut->SetNumberOfTableValues(2);
  lut->SetTableRange(0, 1);
  lut->SetTableValue(0, fillColor.GetRed(), fillColor.GetGreen(),
                     fillColor.GetBlue(), 1.0);
  lut->SetTableValue(1, checkerColor.GetRed(), checkerColor.GetGreen(),
                     checkerColor.GetBlue(), 1.0);

  auto map = vtkSmartPointer<vtkImageMapToColors>::New();
  map->SetLookupTable(lut);
  map->SetOutputFormatToRGBA();
  map->SetInputData(image0);
  map->GetExecutive()->Update();

  return map->GetOutput();
}

} // namespace
