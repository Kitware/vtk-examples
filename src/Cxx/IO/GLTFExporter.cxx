#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkGLTFExporter.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMath.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include <array>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: GLTFExporter file.gltf e.g. GLTFExporter.gltf"
              << std::endl;
    return EXIT_FAILURE;
  }
  vtkNew<vtkNamedColors> colors;

  // Set the background color.
  vtkColor3d backgroundColor = colors->GetColor3d("SlateGray");

  // Create an arrow.
  vtkNew<vtkArrowSource> arrowSource;

  // Generate a random start and end point
  double startPoint[3];
  double endPoint[3];
  vtkNew<vtkMinimalStandardRandomSequence> rng;
  rng->SetSeed(8775070); // For testing.
  for (auto i = 0; i < 3; ++i)
  {
    rng->Next();
    startPoint[i] = rng->GetRangeValue(-10, 10);
    rng->Next();
    endPoint[i] = rng->GetRangeValue(-10, 10);
  }

  // Compute a basis
  double normalizedX[3];
  double normalizedY[3];
  double normalizedZ[3];

  // The X axis is a vector from start to end
  vtkMath::Subtract(endPoint, startPoint, normalizedX);
  double length = vtkMath::Norm(normalizedX);
  vtkMath::Normalize(normalizedX);

  // The Z axis is an arbitrary vector cross X
  double arbitrary[3];
  for (auto i = 0; i < 3; ++i)
  {
    rng->Next();
    arbitrary[i] = rng->GetRangeValue(-10, 10);
  }
  vtkMath::Cross(normalizedX, arbitrary, normalizedZ);
  vtkMath::Normalize(normalizedZ);

  // The Y axis is Z cross X
  vtkMath::Cross(normalizedZ, normalizedX, normalizedY);
  vtkNew<vtkMatrix4x4> matrix;

  // Create the direction cosine matrix
  matrix->Identity();
  for (auto i = 0; i < 3; i++)
  {
    matrix->SetElement(i, 0, normalizedX[i]);
    matrix->SetElement(i, 1, normalizedY[i]);
    matrix->SetElement(i, 2, normalizedZ[i]);
  }

  // Apply the transforms
  vtkNew<vtkTransform> transform;
  transform->Translate(startPoint);
  transform->Concatenate(matrix);
  transform->Scale(length, length, length);

  // Transform the polydata
  vtkNew<vtkTransformPolyDataFilter> transformPD;
  transformPD->SetTransform(transform);
  transformPD->SetInputConnection(arrowSource->GetOutputPort());

  // Create a mapper and actor for the arrow
  vtkNew<vtkPolyDataMapper> mapper;
  vtkNew<vtkActor> actor;
#ifdef USER_MATRIX
  mapper->SetInputConnection(arrowSource->GetOutputPort());
  actor->SetUserMatrix(transform->GetMatrix());
#else
  mapper->SetInputConnection(transformPD->GetOutputPort());
#endif
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Cyan").GetData());

  // Create spheres for start and end point
  vtkNew<vtkSphereSource> sphereStartSource;
  sphereStartSource->SetCenter(startPoint);
  sphereStartSource->SetRadius(0.8);
  vtkNew<vtkPolyDataMapper> sphereStartMapper;
  sphereStartMapper->SetInputConnection(sphereStartSource->GetOutputPort());
  vtkNew<vtkActor> sphereStart;
  sphereStart->SetMapper(sphereStartMapper);
  sphereStart->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

  vtkNew<vtkSphereSource> sphereEndSource;
  sphereEndSource->SetCenter(endPoint);
  sphereEndSource->SetRadius(0.8);
  vtkNew<vtkPolyDataMapper> sphereEndMapper;
  sphereEndMapper->SetInputConnection(sphereEndSource->GetOutputPort());
  vtkNew<vtkActor> sphereEnd;
  sphereEnd->SetMapper(sphereEndMapper);
  sphereEnd->GetProperty()->SetColor(colors->GetColor3d("Magenta").GetData());

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("GLTFExporter");

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindowInteractor->SetInteractorStyle(style);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->AddActor(sphereStart);
  renderer->AddActor(sphereEnd);
  renderer->SetBackground(backgroundColor.GetData());

  // Render and interact
  renderWindow->Render();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->ResetCameraClippingRange();

  renderWindowInteractor->Start();

  vtkNew<vtkGLTFExporter> writer;
  writer->SetFileName(argv[1]);
  writer->InlineDataOn();
  writer->SetRenderWindow(renderWindow);
  writer->Write();

  return EXIT_SUCCESS;
}
