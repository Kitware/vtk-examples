#include <vtkActor.h>
#include <vtkGreedyTerrainDecimation.h>
#include <vtkImageData.h>
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

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create an image
  vtkNew<vtkImageData> image;
  image->SetDimensions(3, 3, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(8775070);

  int dims[3];
  image->GetDimensions(dims);
  for (int i = 0; i < dims[0]; i++)
  {
    for (int j = 0; j < dims[1]; j++)
    {
      auto pixel =
          static_cast<unsigned char*>(image->GetScalarPointer(i, j, 0));
      // pixel[0] = vtkMath::Round(vtkMath::Random(0, 255));
      pixel[0] = vtkMath::Round(randomSequence->GetRangeValue(0.0, 5.0));
      randomSequence->Next();
    }
  }

  vtkNew<vtkGreedyTerrainDecimation> decimation;
  decimation->SetInputData(image);
  decimation->Update();

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(decimation->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetInterpolationToFlat();
  actor->GetProperty()->EdgeVisibilityOn();
  actor->GetProperty()->SetEdgeColor(colors->GetColor3d("Red").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("GreedyTerrainDecimation");

  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
