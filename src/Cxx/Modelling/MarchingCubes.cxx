#include <vtkActor.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageData.h>
#include <vtkMarchingCubes.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkVoxelModeller.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkImageData> volume;
  double isoValue;
  if (argc < 3)
  {
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetPhiResolution(20);
    sphereSource->SetThetaResolution(20);
    sphereSource->Update();

    double bounds[6];
    sphereSource->GetOutput()->GetBounds(bounds);
    for (unsigned int i = 0; i < 6; i += 2)
    {
      double range = bounds[i + 1] - bounds[i];
      bounds[i] = bounds[i] - .1 * range;
      bounds[i + 1] = bounds[i + 1] + .1 * range;
    }
    vtkNew<vtkVoxelModeller> voxelModeller;
    voxelModeller->SetSampleDimensions(50, 50, 50);
    voxelModeller->SetModelBounds(bounds);
    voxelModeller->SetScalarTypeToFloat();
    voxelModeller->SetMaximumDistance(0.1);

    voxelModeller->SetInputConnection(sphereSource->GetOutputPort());
    voxelModeller->Update();
    isoValue = 0.5;
    volume->DeepCopy(voxelModeller->GetOutput());
  }
  else
  {
    vtkNew<vtkDICOMImageReader> reader;
    reader->SetDirectoryName(argv[1]);
    reader->Update();
    volume->DeepCopy(reader->GetOutput());
    isoValue = atof(argv[2]);
  }

  vtkNew<vtkMarchingCubes> surface;
  surface->SetInputData(volume);
  surface->ComputeNormalsOn();
  surface->SetValue(0, isoValue);

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("MarchingCubes");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(surface->GetOutputPort());
  mapper->ScalarVisibilityOff();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  renderer->AddActor(actor);

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
