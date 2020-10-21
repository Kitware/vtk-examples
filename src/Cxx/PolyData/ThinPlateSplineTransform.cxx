#include <vtkActor2D.h>
#include <vtkBMPReader.h>
#include <vtkDataObject.h>
#include <vtkImageBlend.h>
#include <vtkImageGridSource.h>
#include <vtkImageMapToColors.h>
#include <vtkImageMapper.h>
#include <vtkImageReslice.h>
#include <vtkInteractorStyleTerrain.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkThinPlateSplineTransform.h>

// Warp an image with a thin plate spline

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " Filename.bmp e.g. masonry.bmp"
              << std::endl;
    return EXIT_FAILURE;
  }

  // First, create an image to warp
  vtkNew<vtkImageGridSource> imageGrid;
  imageGrid->SetGridSpacing(16, 16, 0);
  imageGrid->SetGridOrigin(0, 0, 0);
  imageGrid->SetDataExtent(0, 255, 0, 255, 0, 0);
  imageGrid->SetDataScalarTypeToUnsignedChar();

  vtkNew<vtkLookupTable> table;
  table->SetTableRange(0, 1);
  table->SetAlphaRange(0.0, 1.0);
  table->SetHueRange(0.15, 0.15);
  table->SetSaturationRange(1, 1);
  table->SetValueRange(0, 1);
  table->Build();

  vtkNew<vtkImageMapToColors> alpha;
  alpha->SetInputConnection(imageGrid->GetOutputPort());
  alpha->SetLookupTable(table);

  vtkNew<vtkBMPReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();
  auto dataExtent = reader->GetDataExtent();
  // Use dataExtent[1] and dataExtent[3] to set the render window size.

  vtkNew<vtkImageBlend> blend;
  blend->AddInputConnection(0, reader->GetOutputPort());
  blend->AddInputConnection(0, alpha->GetOutputPort());

  // Next, create a ThinPlateSpline transform

  vtkNew<vtkPoints> p1;
  p1->SetNumberOfPoints(8);
  p1->SetPoint(0, 0, 0, 0);
  p1->SetPoint(1, 0, 255, 0);
  p1->SetPoint(2, 255, 0, 0);
  p1->SetPoint(3, 255, 255, 0);
  p1->SetPoint(4, 96, 96, 0);
  p1->SetPoint(5, 96, 159, 0);
  p1->SetPoint(6, 159, 159, 0);
  p1->SetPoint(7, 159, 96, 0);

  vtkNew<vtkPoints> p2;
  p2->SetNumberOfPoints(8);
  p2->SetPoint(0, 0, 0, 0);
  p2->SetPoint(1, 0, 255, 0);
  p2->SetPoint(2, 255, 0, 0);
  p2->SetPoint(3, 255, 255, 0);
  p2->SetPoint(4, 96, 159, 0);
  p2->SetPoint(5, 159, 159, 0);
  p2->SetPoint(6, 159, 96, 0);
  p2->SetPoint(7, 96, 96, 0);

  vtkNew<vtkThinPlateSplineTransform> transform;
  transform->SetSourceLandmarks(p2);
  transform->SetTargetLandmarks(p1);
  transform->SetBasisToR2LogR();
  // You must invert the transform before passing it to vtkImageReslice
  transform->Inverse();

  vtkNew<vtkImageReslice> reslice;
  reslice->SetInputConnection(blend->GetOutputPort());
  reslice->SetResliceTransform(transform);
  reslice->SetInterpolationModeToLinear();
  vtkNew<vtkImageMapper> map;
  map->SetInputConnection(reslice->GetOutputPort());
  map->SetColorWindow(255.0);
  map->SetColorLevel(127.5);
  map->SetZSlice(0);
  vtkNew<vtkActor2D> act;
  act->SetMapper(map);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(act);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderWindow> window;
  window->SetSize(dataExtent[1], dataExtent[3]);
  window->SetWindowName("ThinPlateSplineTransform");

  window->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(window);
  vtkNew<vtkInteractorStyleTerrain> style;
  interactor->SetInteractorStyle(style);
  window->Render();
  interactor->Initialize();
  interactor->Start();

  return EXIT_SUCCESS;
}
