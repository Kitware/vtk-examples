#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDelimitedTextReader.h>
#include <vtkGaussianKernel.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPointGaussianMapper.h>
#include <vtkPointInterpolator.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkTableToPolyData.h>

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0]
              << " sparsePoints.txt InterpolatingOnSTL_final.stl" << std::endl;
    return EXIT_FAILURE;
  }
  std::string pointsFile = argv[1];
  std::string probeSurfaceFile = argv[2];

  // Read a points data
  vtkNew<vtkDelimitedTextReader> pointsReader;
  pointsReader->SetFileName(pointsFile.c_str());
  pointsReader->DetectNumericColumnsOn();
  pointsReader->SetFieldDelimiterCharacters("\t");
  pointsReader->SetHaveHeaders(true);

  vtkNew<vtkTableToPolyData> tablePoints;
  tablePoints->SetInputConnection(pointsReader->GetOutputPort());
  tablePoints->SetXColumn("x");
  tablePoints->SetYColumn("y");
  tablePoints->SetZColumn("z");
  tablePoints->Update();

  vtkPolyData* points = tablePoints->GetOutput();
  points->GetPointData()->SetActiveScalars("val");
  double* range = points->GetPointData()->GetScalars()->GetRange();

  // Read a probe surface
  vtkNew<vtkSTLReader> stlReader;
  stlReader->SetFileName(probeSurfaceFile.c_str());
  stlReader->Update();

  vtkPolyData* surface = stlReader->GetOutput();
  // double* bounds = surface->GetBounds();

  // Gaussian kernel
  vtkNew<vtkGaussianKernel> gaussianKernel;
  gaussianKernel->SetSharpness(2.0);
  gaussianKernel->SetRadius(12.0);

  vtkNew<vtkPointInterpolator> interpolator;
  interpolator->SetInputData(surface);
  interpolator->SetSourceData(points);
  interpolator->SetKernel(gaussianKernel);

  // Visualize
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(interpolator->GetOutputPort());
  mapper->SetScalarRange(range);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkPointGaussianMapper> pointsMapper;
  pointsMapper->SetInputData(points);
  pointsMapper->SetScalarRange(range);
  pointsMapper->SetScaleFactor(0.6);
  pointsMapper->EmissiveOff();
  // clang-format off
  pointsMapper->SetSplatShaderCode(
      "//VTK::Color::Impl\n"
      "float dist = dot(offsetVCVSOutput.xy,offsetVCVSOutput.xy);\n"
      "if (dist > 1.0) {\n"
      "  discard;\n"
      "} else {\n"
      "  float scale = (1.0 - dist);\n"
      "  ambientColor *= scale;\n"
      "  diffuseColor *= scale;\n"
      "};\n");
  // clang-format on

  vtkNew<vtkActor> pointsActor;
  pointsActor->SetMapper(pointsMapper);

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("PointInterpolator");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->AddActor(pointsActor);

  renderWindow->Render();
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Elevation(-45);

  renderWindow->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
