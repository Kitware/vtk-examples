#include <vtkSmartPointer.h>
#include <vtkMarchingCubes.h>
#include <vtkMetaImageReader.h>

#include <vtkSphereSource.h>
#include <vtkProbeFilter.h>
#include <vtkSphere.h>
#include <vtkClipDataSet.h>
#include <vtkImplicitVolume.h>
#include <vtkUnstructuredGrid.h>
#include <vtkLookupTable.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkProperty.h>

int main (int argc, char *argv[])
{
  if (argc < 2)
  {
    cout << "Usage: " << argv[0] << " file.mhd" << endl;
    return EXIT_FAILURE;
  }

  // Read the volume data
  vtkNew<vtkMetaImageReader> reader;
  reader->SetFileName (argv[1]);
  reader->Update();

  // An isosurface, or contour value of 500 is known to correspond to the
  // skin of the patient.
  vtkNew<vtkMarchingCubes> skinExtractor;
  skinExtractor->SetInputConnection(reader->GetOutputPort());
  skinExtractor->SetValue(0, 500);

  // Define a spherical clip function to clip the isosurface
  vtkNew<vtkSphere> clipFunction;
  clipFunction->SetRadius(50);
  clipFunction->SetCenter(73, 52, 15);

  // Clip the isosurface with a sphere
  vtkNew<vtkClipDataSet> skinClip;
  skinClip->SetInputConnection(skinExtractor->GetOutputPort());
  skinClip->SetClipFunction(clipFunction);
  skinClip->SetValue(0);
  skinClip->GenerateClipScalarsOn();
  skinClip->Update();

  vtkNew<vtkDataSetMapper> skinMapper;
  skinMapper->SetInputConnection(skinClip->GetOutputPort());
  skinMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> skin;
  skin->SetMapper(skinMapper);
  skin->GetProperty()->SetDiffuseColor(1, .49, .25);

  vtkNew<vtkProperty> backProp;
  backProp->SetDiffuseColor(0.8900, 0.8100, 0.3400);
  skin->SetBackfaceProperty(backProp);

  // Define a model for the "lens". Its geometry matches the implicit
  // sphere used to clip the isosurface
  vtkNew<vtkSphereSource> lensModel;
  lensModel->SetRadius(50);
  lensModel->SetCenter(73, 52, 15);
  lensModel->SetPhiResolution(201);
  lensModel->SetThetaResolution(101);

  // Sample the input volume with the lens model geometry
  vtkNew<vtkProbeFilter> lensProbe;
  lensProbe->SetInputConnection(lensModel->GetOutputPort());
  lensProbe->SetSourceConnection(reader->GetOutputPort());

  // Clip the lens data with the isosurface value
  vtkNew<vtkClipDataSet> lensClip;
  lensClip->SetInputConnection(lensProbe->GetOutputPort());
  lensClip->SetValue(500);
  lensClip->GenerateClipScalarsOff();
  lensClip->Update();

  // Define a suitable grayscale lut
  vtkNew<vtkLookupTable> bwLut;
  bwLut->SetTableRange (0, 2048);
  bwLut->SetSaturationRange (0, 0);
  bwLut->SetHueRange (0, 0);
  bwLut->SetValueRange (.2, 1);
  bwLut->Build();

  vtkNew<vtkDataSetMapper> lensMapper;
  lensMapper->SetInputConnection(lensClip->GetOutputPort());
  lensMapper->SetScalarRange(lensClip->GetOutput()->GetScalarRange());
  lensMapper->SetLookupTable(bwLut);

  vtkNew<vtkActor> lens;
  lens->SetMapper(lensMapper);

  // It is convenient to create an initial view of the data. The FocalPoint
  // and Position form a vector direction. Later on (ResetCamera() method)
  // this vector is used to position the camera to look at the data in
  // this direction.
  vtkNew<vtkCamera> aCamera;
  aCamera->SetViewUp (0, 0, -1);
  aCamera->SetPosition (0, -1, 0);
  aCamera->SetFocalPoint (0, 0, 0);
  aCamera->ComputeViewPlaneNormal();
  aCamera->Azimuth(30.0);
  aCamera->Elevation(30.0);

  // Create the renderer, the render window, and the interactor. The renderer
  // draws into the render window, the interactor enables mouse- and
  // keyboard-based interaction with the data within the render window.
  //
  vtkNew<vtkRenderer> aRenderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(aRenderer);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Actors are added to the renderer. An initial camera view is created.
  // The Dolly() method moves the camera towards the FocalPoint,
  // thereby enlarging the image.
  aRenderer->AddActor(lens);
  aRenderer->AddActor(skin);
  aRenderer->SetActiveCamera(aCamera);
  aRenderer->ResetCamera ();
  aCamera->Dolly(1.5);

  // Set a background color for the renderer and set the size of the
  // render window (expressed in pixels).
  aRenderer->SetBackground(.2, .3, .4);
  renWin->SetSize(640, 480);

  // Note that when camera movement occurs (as it does in the Dolly()
  // method), the clipping planes often need adjusting. Clipping planes
  // consist of two planes: near and far along the view direction. The
  // near plane clips out objects in front of the plane; the far plane
  // clips out objects behind the plane. This way only what is drawn
  // between the planes is actually rendered.
  aRenderer->ResetCameraClippingRange ();

  // Initialize the event loop and then start it.
  renWin->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
