#include <vtkActor.h>
#include <vtkContourFilter.h>
#include <vtkGaussianSplatter.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyData> input;

  if (argc == 1) // Generate the data
  {
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(.01);
    sphereSource->Update();
    input->ShallowCopy(sphereSource->GetOutput());
  }
  else // Read the data from a file
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();

    input->ShallowCopy(reader->GetOutput());
  }

  vtkNew<vtkGaussianSplatter> splatter;
  splatter->SetInputData(input);

  unsigned int n = 200;
  splatter->SetSampleDimensions(n, n, n);
  splatter->SetRadius(.02);
  splatter->SetExponentFactor(-10);
  splatter->SetEccentricity(2);
  splatter->Update();

  vtkNew<vtkContourFilter> isoSurface;
  isoSurface->SetInputConnection(splatter->GetOutputPort());
  isoSurface->SetValue(0, .95 * splatter->GetRadius());

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(isoSurface->GetOutputPort());
  mapper->ScalarVisibilityOff();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("EmbedPointsIntoVolume");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
