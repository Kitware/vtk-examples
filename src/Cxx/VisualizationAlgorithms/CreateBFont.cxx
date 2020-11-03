#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkClipPolyData.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNMReader.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " image.pgm e.g. B.pgm" << std::endl;
    return EXIT_FAILURE;
  }
  // Now create the RenderWindow, Renderer and Interactor
  //
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> ren1;

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkPNMReader> imageIn;
  imageIn->SetFileName(argv[1]);

  vtkNew<vtkImageGaussianSmooth> gaussian;
  gaussian->SetStandardDeviations(2, 2);
  gaussian->SetDimensionality(2);
  gaussian->SetRadiusFactors(1, 1);
  gaussian->SetInputConnection(imageIn->GetOutputPort());

  vtkNew<vtkImageDataGeometryFilter> geometry;
  geometry->SetInputConnection(gaussian->GetOutputPort());

  vtkNew<vtkClipPolyData> aClipper;
  aClipper->SetInputConnection(geometry->GetOutputPort());
  aClipper->SetValue(127.5);
  aClipper->GenerateClipScalarsOff();
  aClipper->InsideOutOn();
  aClipper->GetOutput()->GetPointData()->CopyScalarsOff();
  aClipper->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(aClipper->GetOutputPort());
  mapper->ScalarVisibilityOff();

  vtkNew<vtkActor> letter;
  letter->SetMapper(mapper);

  ren1->AddActor(letter);
  letter->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("LampBlack").GetData());
  letter->GetProperty()->SetRepresentationToWireframe();

  ren1->SetBackground(colors->GetColor3d("WhiteSmoke").GetData());
  ren1->ResetCamera();
  ren1->GetActiveCamera()->Dolly(1.2);
  ren1->ResetCameraClippingRange();

  renWin->SetSize(640, 480);
  renWin->SetWindowName("CreateBFont");

  // render the image
  //
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
