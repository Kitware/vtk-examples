// Derived from VTK/Examples/Cxx/Medical3.cxx
// This example reads a volume dataset, extracts two isosurfaces that
// represent the skin and bone, creates three orthogonal planes
// (sagittal, axial, coronal), and displays them.
//
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkImageMapToColors.h>
#include <vtkImageMapper3D.h>
#include <vtkLookupTable.h>
#include <vtkMetaImageReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStripper.h>
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
  if (argc < 2)
  {
    cout << "Usage: " << argv[0] << " file.mhd  e.g. FullHead.mhd" << endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  std::array<unsigned char, 4> skinColor{{240, 184, 160, 255}};
  colors->SetColor("SkinColor", skinColor.data());
  std::array<unsigned char, 4> bkg{{51, 77, 102, 255}};
  colors->SetColor("BkgColor", bkg.data());

  // Create the renderer, the render window, and the interactor. The
  // renderer draws into the render window, the interactor enables
  // mouse- and keyboard-based interaction with the data within the
  // render window.
  //
  vtkNew<vtkRenderer> aRenderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(aRenderer);
  renWin->SetWindowName("MedicalDemo3");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Set a background color for the renderer and set the size of the
  // render window (expressed in pixels).
  aRenderer->SetBackground(colors->GetColor3d("BkgColor").GetData());
  renWin->SetSize(640, 480);

  // The following reader is used to read a series of 2D slices (images)
  // that compose the volume. The slice dimensions are set, and the
  // pixel spacing. The data Endianness must also be specified. The
  // reader uses the FilePrefix in combination with the slice number to
  // construct filenames using the format FilePrefix.%d. (In this case
  // the FilePrefix is the root name of the file: quarter.)
  vtkNew<vtkMetaImageReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  // An isosurface, or contour value of 500 is known to correspond to
  // the skin of the patient.
  // The triangle stripper is used to create triangle
  // strips from the isosurface; these render much faster on may
  // systems.
#ifdef USE_FLYING_EDGES
  vtkNew<vtkFlyingEdges3D> skinExtractor;
#else
  vtkNew<vtkMarchingCubes> skinExtractor;
#endif
  skinExtractor->SetInputConnection(reader->GetOutputPort());
  skinExtractor->SetValue(0, 500);
  skinExtractor->Update();

  vtkNew<vtkStripper> skinStripper;
  skinStripper->SetInputConnection(skinExtractor->GetOutputPort());
  skinStripper->Update();

  vtkNew<vtkPolyDataMapper> skinMapper;
  skinMapper->SetInputConnection(skinStripper->GetOutputPort());
  skinMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> skin;
  skin->SetMapper(skinMapper);
  skin->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("SkinColor").GetData());
  skin->GetProperty()->SetSpecular(0.3);
  skin->GetProperty()->SetSpecularPower(20);

  // An isosurface, or contour value of 1150 is known to correspond to
  // the bone of the patient.
  // The triangle stripper is used to create triangle
  // strips from the isosurface; these render much faster on may
  // systems.
#ifdef USE_FLYING_EDGES
  vtkNew<vtkFlyingEdges3D> boneExtractor;
#else
  vtkNew<vtkMarchingCubes> boneExtractor;
#endif
  boneExtractor->SetInputConnection(reader->GetOutputPort());
  boneExtractor->SetValue(0, 1150);

  vtkNew<vtkStripper> boneStripper;
  boneStripper->SetInputConnection(boneExtractor->GetOutputPort());

  vtkNew<vtkPolyDataMapper> boneMapper;
  boneMapper->SetInputConnection(boneStripper->GetOutputPort());
  boneMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> bone;
  bone->SetMapper(boneMapper);
  bone->GetProperty()->SetDiffuseColor(colors->GetColor3d("Ivory").GetData());

  // An outline provides context around the data.
  //
  vtkNew<vtkOutlineFilter> outlineData;
  outlineData->SetInputConnection(reader->GetOutputPort());
  outlineData->Update();

  vtkNew<vtkPolyDataMapper> mapOutline;
  mapOutline->SetInputConnection(outlineData->GetOutputPort());

  vtkNew<vtkActor> outline;
  outline->SetMapper(mapOutline);
  outline->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  // Now we are creating three orthogonal planes passing through the
  // volume. Each plane uses a different texture map and therefore has
  // different coloration.

  // Start by creating a black/white lookup table.
  vtkNew<vtkLookupTable> bwLut;
  bwLut->SetTableRange(0, 2000);
  bwLut->SetSaturationRange(0, 0);
  bwLut->SetHueRange(0, 0);
  bwLut->SetValueRange(0, 1);
  bwLut->Build(); // effective built

  // Now create a lookup table that consists of the full hue circle
  // (from HSV).
  vtkNew<vtkLookupTable> hueLut;
  hueLut->SetTableRange(0, 2000);
  hueLut->SetHueRange(0, 1);
  hueLut->SetSaturationRange(1, 1);
  hueLut->SetValueRange(1, 1);
  hueLut->Build(); // effective built

  // Finally, create a lookup table with a single hue but having a range
  // in the saturation of the hue.
  vtkNew<vtkLookupTable> satLut;
  satLut->SetTableRange(0, 2000);
  satLut->SetHueRange(0.6, 0.6);
  satLut->SetSaturationRange(0, 1);
  satLut->SetValueRange(1, 1);
  satLut->Build(); // effective built

  // Create the first of the three planes. The filter vtkImageMapToColors
  // maps the data through the corresponding lookup table created above. The
  // vtkImageActor is a type of vtkProp and conveniently displays an image on
  // a single quadrilateral plane. It does this using texture mapping and as
  // a result is quite fast. (Note: the input image has to be unsigned char
  // values, which the vtkImageMapToColors produces.) Note also that by
  // specifying the DisplayExtent, the pipeline requests data of this extent
  // and the vtkImageMapToColors only processes a slice of data.
  vtkNew<vtkImageMapToColors> sagittalColors;
  sagittalColors->SetInputConnection(reader->GetOutputPort());
  sagittalColors->SetLookupTable(bwLut);
  sagittalColors->Update();

  vtkNew<vtkImageActor> sagittal;
  sagittal->GetMapper()->SetInputConnection(sagittalColors->GetOutputPort());
  sagittal->SetDisplayExtent(128, 128, 0, 255, 0, 92);
  sagittal->ForceOpaqueOn();

  // Create the second (axial) plane of the three planes. We use the
  // same approach as before except that the extent differs.
  vtkNew<vtkImageMapToColors> axialColors;
  axialColors->SetInputConnection(reader->GetOutputPort());
  axialColors->SetLookupTable(hueLut);
  axialColors->Update();

  vtkNew<vtkImageActor> axial;
  axial->GetMapper()->SetInputConnection(axialColors->GetOutputPort());
  axial->SetDisplayExtent(0, 255, 0, 255, 46, 46);
  axial->ForceOpaqueOn();

  // Create the third (coronal) plane of the three planes. We use
  // the same approach as before except that the extent differs.
  vtkNew<vtkImageMapToColors> coronalColors;
  coronalColors->SetInputConnection(reader->GetOutputPort());
  coronalColors->SetLookupTable(satLut);
  coronalColors->Update();

  vtkNew<vtkImageActor> coronal;
  coronal->GetMapper()->SetInputConnection(coronalColors->GetOutputPort());
  coronal->SetDisplayExtent(0, 255, 128, 128, 0, 92);
  coronal->ForceOpaqueOn();

  // It is convenient to create an initial view of the data. The
  // FocalPoint and Position form a vector direction. Later on
  // (ResetCamera() method) this vector is used to position the camera
  // to look at the data in this direction.
  vtkNew<vtkCamera> aCamera;
  aCamera->SetViewUp(0, 0, -1);
  aCamera->SetPosition(0, -1, 0);
  aCamera->SetFocalPoint(0, 0, 0);
  aCamera->ComputeViewPlaneNormal();
  aCamera->Azimuth(30.0);
  aCamera->Elevation(30.0);

  // Actors are added to the renderer.
  aRenderer->AddActor(outline);
  aRenderer->AddActor(sagittal);
  aRenderer->AddActor(axial);
  aRenderer->AddActor(coronal);
  aRenderer->AddActor(skin);
  aRenderer->AddActor(bone);

  // Turn off bone for this example.
  bone->VisibilityOff();

  // Set skin to semi-transparent.
  skin->GetProperty()->SetOpacity(0.5);

  // An initial camera view is created. The Dolly() method moves
  // the camera towards the FocalPoint, thereby enlarging the image.
  aRenderer->SetActiveCamera(aCamera);

  // Calling Render() directly on a vtkRenderer is strictly forbidden.
  // Only calling Render() on the vtkRenderWindow is a valid call.
  renWin->Render();

  aRenderer->ResetCamera();
  aCamera->Dolly(1.5);

  // Note that when camera movement occurs (as it does in the Dolly()
  // method), the clipping planes often need adjusting. Clipping planes
  // consist of two planes: near and far along the view direction. The
  // near plane clips out objects in front of the plane; the far plane
  // clips out objects behind the plane. This way only what is drawn
  // between the planes is actually rendered.
  aRenderer->ResetCameraClippingRange();

  // interact with data
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
