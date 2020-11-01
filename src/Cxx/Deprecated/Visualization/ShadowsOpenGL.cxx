// The scene consists of
// * 4 actors: a rectangle, a box, a cone and a sphere. The box, the cone and
// the sphere are above the rectangle.
// * 2 spotlights: one in the direction of the box, another one in the
// direction of the sphere. Both lights are above the box, the cone and
// the sphere.
#include <assert.h>
#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkCamera.h>
#include <vtkCameraPass.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkDepthPeelingPass.h>
#include <vtkFrameBufferObject.h>
#include <vtkFrustumSource.h>
#include <vtkImageData.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkImageSinusoidSource.h>
#include <vtkInformation.h>
#include <vtkLight.h>
#include <vtkLightActor.h>
#include <vtkLightCollection.h>
#include <vtkLightsPass.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkOpaquePass.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOverlayPass.h>
#include <vtkPlaneSource.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderPassCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSequencePass.h>
#include <vtkShadowMapBakerPass.h>
#include <vtkShadowMapPass.h>
#include <vtkSphereSource.h>
#include <vtkTranslucentPass.h>
#include <vtkVolumetricPass.h>

namespace {
// For each spotlight, add a light frustum wireframe representation and a cone
// wireframe representation, colored with the light color.
void AddLightActors(vtkRenderer* r);
} // namespace

int main(int, char*[])
{
  vtkNew<vtkRenderWindowInteractor> interactor;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(400, 400);
  renderWindow->SetMultiSamples(0);

  renderWindow->SetAlphaBitPlanes(1);
  interactor->SetRenderWindow(renderWindow);

  vtkNew<vtkOpenGLRenderer> renderer;
  renderWindow->AddRenderer(renderer);

  bool supported = vtkFrameBufferObject::IsSupported(
      renderWindow); // adapted from line 182 of vtkShadowMapPass.cxx

  if (!supported)
  {
    std::cerr << "Shadow rendering is not supported by the current video"
              << " driver!" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkCameraPass> cameraP;

  vtkNew<vtkOpaquePass> opaque;

  vtkNew<vtkDepthPeelingPass> peeling;
  peeling->SetMaximumNumberOfPeels(200);
  peeling->SetOcclusionRatio(0.1);

  vtkNew<vtkTranslucentPass> translucent;
  peeling->SetTranslucentPass(translucent);

  vtkNew<vtkVolumetricPass> volume;
  vtkNew<vtkOverlayPass> overlay;

  vtkNew<vtkLightsPass> lights;

  vtkNew<vtkSequencePass> opaqueSequence;

  vtkNew<vtkRenderPassCollection> passes2;
  passes2->AddItem(lights);
  passes2->AddItem(opaque);
  opaqueSequence->SetPasses(passes2);

  vtkNew<vtkCameraPass> opaqueCameraPass;
  opaqueCameraPass->SetDelegatePass(opaqueSequence);

  vtkNew<vtkShadowMapBakerPass> shadowsBaker;
  shadowsBaker->SetOpaquePass(opaqueCameraPass);
  shadowsBaker->SetResolution(1024);
  // To cancel self-shadowing.
  shadowsBaker->SetPolygonOffsetFactor(3.1f);
  shadowsBaker->SetPolygonOffsetUnits(10.0f);

  vtkNew<vtkShadowMapPass> shadows;
  shadows->SetShadowMapBakerPass(shadowsBaker);
  shadows->SetOpaquePass(opaqueSequence);

  vtkNew<vtkSequencePass> seq;
  vtkNew<vtkRenderPassCollection> passes;
  passes->AddItem(shadowsBaker);
  passes->AddItem(shadows);
  passes->AddItem(lights);
  passes->AddItem(peeling);
  passes->AddItem(volume);
  passes->AddItem(overlay);
  seq->SetPasses(passes);
  cameraP->SetDelegatePass(seq);

  renderer->SetPass(cameraP);

  vtkNew<vtkPlaneSource> rectangleSource;
  rectangleSource->SetOrigin(-5.0, 0.0, 5.0);
  rectangleSource->SetPoint1(5.0, 0.0, 5.0);
  rectangleSource->SetPoint2(-5.0, 0.0, -5.0);
  rectangleSource->SetResolution(100, 100);

  vtkNew<vtkPolyDataMapper> rectangleMapper;
  rectangleMapper->SetInputConnection(rectangleSource->GetOutputPort());

  rectangleMapper->SetScalarVisibility(0);

  vtkNew<vtkActor> rectangleActor;
  vtkNew<vtkInformation> rectangleKeyProperties;
  rectangleKeyProperties->Set(vtkShadowMapBakerPass::OCCLUDER(),
                              0); // dummy val.
  rectangleKeyProperties->Set(vtkShadowMapBakerPass::RECEIVER(),
                              0); // dummy val.
  rectangleActor->SetPropertyKeys(rectangleKeyProperties);

  rectangleActor->SetMapper(rectangleMapper);
  rectangleActor->SetVisibility(1);
  rectangleActor->GetProperty()->SetColor(1.0, 1.0, 1.0);

  vtkNew<vtkCubeSource> boxSource;
  boxSource->SetXLength(2.0);
  vtkNew<vtkPolyDataNormals> boxNormals;
  boxNormals->SetInputConnection(boxSource->GetOutputPort());
  boxNormals->SetComputePointNormals(0);
  boxNormals->SetComputeCellNormals(1);
  boxNormals->Update();
  boxNormals->GetOutput()->GetPointData()->SetNormals(0);

  vtkNew<vtkPolyDataMapper> boxMapper;
  boxMapper->SetInputConnection(boxNormals->GetOutputPort());
  boxMapper->SetScalarVisibility(0);

  vtkNew<vtkActor> boxActor;
  vtkNew<vtkInformation> boxKeyProperties;
  boxKeyProperties->Set(vtkShadowMapBakerPass::OCCLUDER(), 0); // dummy val.
  boxKeyProperties->Set(vtkShadowMapBakerPass::RECEIVER(), 0); // dummy val.
  boxActor->SetPropertyKeys(boxKeyProperties);

  boxActor->SetMapper(boxMapper);
  boxActor->SetVisibility(1);
  boxActor->SetPosition(-2.0, 2.0, 0.0);
  boxActor->GetProperty()->SetColor(1.0, 0.0, 0.0);

  vtkNew<vtkConeSource> coneSource;
  coneSource->SetResolution(24);
  coneSource->SetDirection(1.0, 1.0, 1.0);
  vtkNew<vtkPolyDataMapper> coneMapper;
  coneMapper->SetInputConnection(coneSource->GetOutputPort());
  coneMapper->SetScalarVisibility(0);

  vtkNew<vtkActor> coneActor;
  vtkNew<vtkInformation> coneKeyProperties;
  coneKeyProperties->Set(vtkShadowMapBakerPass::OCCLUDER(), 0); // dummy val.
  coneKeyProperties->Set(vtkShadowMapBakerPass::RECEIVER(), 0); // dummy val.
  coneActor->SetPropertyKeys(coneKeyProperties);

  coneActor->SetMapper(coneMapper);
  coneActor->SetVisibility(1);
  coneActor->SetPosition(0.0, 1.0, 1.0);
  coneActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
  //  coneActor->GetProperty()->SetLighting(false);

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetThetaResolution(32);
  sphereSource->SetPhiResolution(32);
  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  sphereMapper->SetScalarVisibility(0);

  vtkNew<vtkActor> sphereActor;
  vtkNew<vtkInformation> sphereKeyProperties;
  sphereKeyProperties->Set(vtkShadowMapBakerPass::OCCLUDER(), 0); // dummy val.
  sphereKeyProperties->Set(vtkShadowMapBakerPass::RECEIVER(), 0); // dummy val.
  sphereActor->SetPropertyKeys(sphereKeyProperties);
  sphereActor->SetMapper(sphereMapper);

  sphereActor->SetVisibility(1);
  sphereActor->SetPosition(2.0, 2.0, -1.0);
  sphereActor->GetProperty()->SetColor(1.0, 1.0, 0.0);

  renderer->AddViewProp(rectangleActor);
  renderer->AddViewProp(boxActor);
  renderer->AddViewProp(coneActor);
  renderer->AddViewProp(sphereActor);

  // Spotlights.

  // lighting the box.
  vtkNew<vtkLight> l1;
  l1->SetPosition(-4.0, 4.0, -1.0);
  l1->SetFocalPoint(boxActor->GetPosition());
  l1->SetColor(1.0, 1.0, 1.0);
  l1->SetPositional(1);
  renderer->AddLight(l1);
  l1->SetSwitch(1);

  // lighting the sphere
  vtkNew<vtkLight> l2;
  l2->SetPosition(4.0, 5.0, 1.0);
  l2->SetFocalPoint(sphereActor->GetPosition());
  l2->SetColor(1.0, 0.0, 1.0);
  //  l2->SetColor(1.0,1.0,1.0);
  l2->SetPositional(1);
  renderer->AddLight(l2);
  l2->SetSwitch(1);

  AddLightActors(renderer);

  renderer->SetBackground(0.66, 0.66, 0.66);
  renderer->SetBackground2(157.0 / 255.0 * 0.66, 186 / 255.0 * 0.66,
                           192.0 / 255.0 * 0.66);
  renderer->SetGradientBackground(true);

  renderWindow->Render();
  if (peeling->GetLastRenderingUsedDepthPeeling())
  {
    std::cout << "depth peeling was used" << std::endl;
  }
  else
  {
    std::cout << "depth peeling was not used (alpha blending instead)"
              << std::endl;
  }

  renderer->ResetCamera();
  vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
  camera->Azimuth(40.0);
  camera->Elevation(10.0);

  renderWindow->Render();

  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {
// For each spotlight, add a light frustum wireframe representation and a cone
// wireframe representation, colored with the light color.
void AddLightActors(vtkRenderer* r)
{
  assert("pre: r_exists" && r != 0);

  vtkLightCollection* lights = r->GetLights();

  lights->InitTraversal();
  vtkLight* l = lights->GetNextItem();
  while (l != 0)
  {
    double angle = l->GetConeAngle();
    if (l->LightTypeIsSceneLight() && l->GetPositional() &&
        angle < 180.0) // spotlight
    {
      vtkNew<vtkLightActor> la;
      la->SetLight(l);
      r->AddViewProp(la);
    }
    l = lights->GetNextItem();
  }
}
} // namespace
