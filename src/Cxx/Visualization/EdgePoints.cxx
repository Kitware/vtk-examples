#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkEdgePoints.h>
#include <vtkMetaImageReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>
#include <string>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    cout << "Usage: " << argv[0] << " file.mhd e.g. mhd 500" << endl;
    return EXIT_FAILURE;
  }

  double isoValue = 0.0;
  if (argc > 2)
  {
    isoValue = std::stod(std::string(argv[2]));
  }
  vtkNew<vtkNamedColors> colors;

  std::array<unsigned char, 4> isoColor{{255, 125, 64}};
  colors->SetColor("IsoColor", isoColor.data());
  std::array<unsigned char, 4> bkg{{51, 77, 102, 255}};
  colors->SetColor("BkgColor", bkg.data());

  vtkNew<vtkRenderer> aRenderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(aRenderer);
  renWin->SetWindowName("EdgePoints");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkMetaImageReader> reader;
  reader->SetFileName(argv[1]);

  vtkNew<vtkEdgePoints> isoExtractor;
  isoExtractor->SetInputConnection(reader->GetOutputPort());
  isoExtractor->SetValue(isoValue);

  vtkNew<vtkPolyDataMapper> isoMapper;
  isoMapper->SetInputConnection(isoExtractor->GetOutputPort());
  isoMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> iso;
  iso->SetMapper(isoMapper);
  iso->GetProperty()->SetDiffuseColor(colors->GetColor3d("Bisque").GetData());

  // An outline provides context around the data.
  //
  vtkNew<vtkOutlineFilter> outlineData;
  outlineData->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkPolyDataMapper> mapOutline;
  mapOutline->SetInputConnection(outlineData->GetOutputPort());

  vtkNew<vtkActor> outline;
  outline->SetMapper(mapOutline);
  outline->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  // It is convenient to create an initial view of the data. The FocalPoint
  // and Position form a vector direction. Later on (ResetCamera() method)
  // this vector is used to position the camera to look at the data in
  // this direction.
  vtkNew<vtkCamera> aCamera;
  aCamera->SetViewUp(0, 0, -1);
  aCamera->SetPosition(0, -1, 0);
  aCamera->SetFocalPoint(0, 0, 0);
  aCamera->ComputeViewPlaneNormal();
  aCamera->Azimuth(30.0);
  aCamera->Elevation(30.0);

  // Actors are added to the renderer. An initial camera view is created.
  // The Dolly() method moves the camera towards the FocalPoint,
  // thereby enlarging the image.
  aRenderer->AddActor(outline);
  aRenderer->AddActor(iso);
  aRenderer->SetActiveCamera(aCamera);
  aRenderer->ResetCamera();
  aCamera->Dolly(1.1);

  // Set a background color for the renderer and set the size of the
  // render window (expressed in pixels).
  aRenderer->SetBackground(colors->GetColor3d("BkgColor").GetData());
  renWin->SetSize(640, 480);

  // Note that when camera movement occurs (as it does in the Dolly()
  // method), the clipping planes often need adjusting. Clipping planes
  // consist of two planes: near and far along the view direction. The
  // near plane clips out objects in front of the plane; the far plane
  // clips out objects behind the plane. This way only what is drawn
  // between the planes is actually rendered.
  aRenderer->ResetCameraClippingRange();

  // Initialize the event loop and then start it.
  renWin->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
