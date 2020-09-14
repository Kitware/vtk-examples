#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOutlineGlowPass.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderStepsPass.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <iomanip>
#include <iostream>

namespace {
/**
 * Check the VTK version.
 *
 * @param major: Major version.
 * @param major: Minor version.
 * @param major: Build version.
 *
 * @return True if the requested VTK version is greater or equal to the actual
 * VTK version.
 */
bool VTKVersionOk(unsigned long long const& major,
                  unsigned long long const& minor,
                  unsigned long long const& build);
} // namespace

int main(int, char*[])
{
  if (!VTKVersionOk(9, 0, 20200909))
  {
    std::cerr
        << "You need VTK version 9.0.20200909 or greater to run this program."
        << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderWindowInteractor> iren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->SetMultiSamples(0);

  iren->SetRenderWindow(renWin);

  // Set up the renderers
  // One for the object and the other for the outline
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderer> rendererOutline;
  rendererOutline->SetLayer(1);
  renWin->SetNumberOfLayers(2);
  renWin->AddRenderer(rendererOutline);
  renWin->AddRenderer(renderer);

  // Create an arrow.
  vtkNew<vtkArrowSource> arrowSource;
  // arrowSource->SetShaftRadius(1.0);
  // arrowSource->SetTipLength(1.0);
  arrowSource->Update();

  // Create mapper and actor for the main renderer
  vtkNew<vtkPolyDataMapper> mapperMain;
  mapperMain->SetInputConnection(arrowSource->GetOutputPort());

  vtkNew<vtkActor> actorMain;
  actorMain->SetMapper(mapperMain);
  actorMain->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("LimeGreen").GetData());

  renderer->AddActor(actorMain);

  // Lets make the outline glow!
  // Create the render pass
  vtkNew<vtkRenderStepsPass> basicPasses;
  vtkNew<vtkOutlineGlowPass> glowPass;
  glowPass->SetDelegatePass(basicPasses);

  // Apply the render pass to the highlight renderer
  rendererOutline->SetPass(glowPass);

  // Create mapper and actor for the outline
  vtkNew<vtkPolyDataMapper> mapperOutline;
  mapperOutline->SetInputConnection(arrowSource->GetOutputPort());

  vtkNew<vtkActor> actorOutline;
  actorOutline->SetMapper(mapperOutline);
  actorOutline->GetProperty()->SetColor(
      colors->GetColor3d("Magenta").GetData());
  actorOutline->GetProperty()->LightingOff();

  rendererOutline->AddActor(actorOutline);

  renWin->SetSize(600, 600);

  renderer->GradientBackgroundOn();
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
  renderer->SetBackground2(colors->GetColor3d("DarkSlateBlue").GetData());

  renderer->ResetCamera();
  vtkCamera* camera = renderer->GetActiveCamera();
  camera->Roll(45.0);
  camera->Azimuth(-30.0);
  camera->Elevation(-15.0);
  renderer->ResetCamera();
  // Now set the active camera for the outline
  rendererOutline->SetActiveCamera(camera);

  renWin->SetWindowName("OutlineGlowPass");

  renWin->Render();

  iren->Start();
  return EXIT_SUCCESS;
}

namespace {
bool VTKVersionOk(unsigned long long const& major,
                  unsigned long long const& minor,
                  unsigned long long const& build)
{
  unsigned long long neededVersion =
      10000000000ULL * major + 100000000ULL * minor + build;
#ifndef VTK_VERSION_NUMBER
  auto ver = vtkSmartPointer<vtkVersion>();
  unsigned long long vtk_version_number =
      10000000000ULL * ver->GetVTKMajorVersion() +
      100000000ULL * ver->GetVTKMinorVersion() + ver->GetVTKBuildVersion();
  if (vtk_version_number >= neededVersion)
  {
    return true;
  }
  return false;
#else
  if (VTK_VERSION_NUMBER >= neededVersion)
  {
    return true;
  }
  return false;
#endif
}
} // namespace
