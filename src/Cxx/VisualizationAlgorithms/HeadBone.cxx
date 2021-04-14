#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkMergePoints.h>
#include <vtkMetaImageReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
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

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " FullHead.mhd" << std::endl;
    return EXIT_FAILURE;
  }

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> ren1;

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // create pipeline
  //

  vtkNew<vtkMetaImageReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkNew<vtkMergePoints> locator;
  locator->SetDivisions(64, 64, 92);
  locator->SetNumberOfPointsPerBucket(2);
  locator->AutomaticOff();

#ifdef USE_FLYING_EDGES
  vtkNew<vtkFlyingEdges3D> iso;
#else
  vtkNew<vtkMarchingCubes> iso;
#endif
  iso->SetInputConnection(reader->GetOutputPort());
  iso->ComputeGradientsOn();
  iso->ComputeScalarsOff();
  iso->SetValue(0, 1150);
#ifndef USE_FLYING_EDGES
  iso->SetLocator(locator);
#endif

  vtkNew<vtkPolyDataMapper> isoMapper;
  isoMapper->SetInputConnection(iso->GetOutputPort());
  isoMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> isoActor;
  isoActor->SetMapper(isoMapper);
  isoActor->GetProperty()->SetColor(colors->GetColor3d("Ivory").GetData());

  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(outlineActor);
  ren1->AddActor(isoActor);
  ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());
  ren1->GetActiveCamera()->SetFocalPoint(0, 0, 0);
  ren1->GetActiveCamera()->SetPosition(0, -1, 0);
  ren1->GetActiveCamera()->SetViewUp(0, 0, -1);
  ren1->ResetCamera();
  ren1->GetActiveCamera()->Dolly(1.5);
  ren1->ResetCameraClippingRange();

  renWin->SetSize(640, 480);
  renWin->SetWindowName("HeadBone");

  renWin->Render();
  iren->Start();
  return EXIT_SUCCESS;
}
