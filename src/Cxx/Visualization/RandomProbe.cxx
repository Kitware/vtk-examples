#include <vtkActor.h>
#include <vtkCone.h>
#include <vtkDataSetMapper.h>
#include <vtkGlyph3D.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkProbeFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkThreshold.h>
#include <vtkThresholdPoints.h>
#include <vtkUnsignedCharArray.h>

int main(int argc, char* argv[])
{
  int resolution = 50;
  if (argc > 1)
  {
    resolution = atoi(argv[1]);
  }

  vtkNew<vtkNamedColors> colors;

  // Create a sampled cone
  vtkNew<vtkCone> implicitCone;
  implicitCone->SetAngle(30.0);

  double radius = 1.0;
  vtkNew<vtkSampleFunction> sampledCone;
  sampledCone->SetSampleDimensions(resolution, resolution, resolution);
  double xMin = -radius * 2.0;
  double xMax = radius * 2.0;
  sampledCone->SetModelBounds(xMin, xMax, xMin, xMax, xMin, xMax);
  sampledCone->SetImplicitFunction(implicitCone);

  vtkNew<vtkThreshold> thresholdCone;
  thresholdCone->SetInputConnection(sampledCone->GetOutputPort());
  thresholdCone->SetLowerThreshold(0);
  thresholdCone->SetThresholdFunction(vtkThreshold::THRESHOLD_LOWER);

  vtkNew<vtkPointSource> randomPoints;
  randomPoints->SetCenter(0.0, 0.0, 0.0);
  randomPoints->SetNumberOfPoints(10000);
  randomPoints->SetDistributionToUniform();
  randomPoints->SetRadius(xMax);

  // Probe the cone dataset with random points
  vtkNew<vtkProbeFilter> randomProbe;
  randomProbe->SetInputConnection(0, randomPoints->GetOutputPort());
  randomProbe->SetInputConnection(1, thresholdCone->GetOutputPort());
  randomProbe->Update();
  randomProbe->GetOutput()->GetPointData()->SetActiveScalars(
      "vtkValidPointMask");

  vtkNew<vtkThresholdPoints> selectPoints;
  selectPoints->SetInputConnection(randomProbe->GetOutputPort());
  selectPoints->ThresholdByUpper(1.0);

  vtkNew<vtkSphereSource> sphere;
  sphere->SetRadius(0.025);

  vtkNew<vtkGlyph3D> glyph;
  glyph->SetSourceConnection(sphere->GetOutputPort());
  glyph->SetInputConnection(selectPoints->GetOutputPort());

  // Create a mapper and actor
  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(glyph->GetOutputPort());
  mapper->ScalarVisibilityOff();
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("RandomProbe");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
