#include <vtkActor.h>
#include <vtkContourFilter.h>
#include <vtkDoubleArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredPoints.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkStructuredPoints> vol;
  vol->SetDimensions(26, 26, 26);
  vol->SetOrigin(-0.5, -0.5, -0.5);
  auto sp = 1.0 / 25.0;
  vol->SetSpacing(sp, sp, sp);

  vtkNew<vtkDoubleArray> scalars;
  scalars->SetNumberOfComponents(1);
  scalars->SetNumberOfTuples(26 * 26 * 26);
  for (auto k = 0; k < 26; k++)
  {
    auto z = -0.5 + k * sp;
    auto kOffset = k * 26 * 26;
    for (auto j = 0; j < 26; j++)
    {
      auto y = -0.5 + j * sp;
      auto jOffset = j * 26;
      for (auto i = 0; i < 26; i++)
      {
        auto x = -0.5 + i * sp;
        auto s = x * x + y * y + z * z - (0.4 * 0.4);
        auto offset = i + jOffset + kOffset;
        scalars->InsertTuple(offset, &s);
      }
    }
  }
  vol->GetPointData()->SetScalars(scalars);

  vtkNew<vtkContourFilter> contour;
  contour->SetInputData(vol);
  contour->SetValue(0, 0.0);

  vtkNew<vtkPolyDataMapper> volMapper;
  volMapper->SetInputConnection(contour->GetOutputPort());
  volMapper->ScalarVisibilityOff();
  vtkNew<vtkActor> volActor;
  volActor->SetMapper(volMapper);
  volActor->GetProperty()->EdgeVisibilityOn();
  volActor->GetProperty()->SetColor(colors->GetColor3d("Salmon").GetData());
  renderer->AddActor(volActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renWin->SetSize(512, 512);
  renWin->SetWindowName("Vol");

  // interact with data
  renWin->Render();

  iren->Start();

  return EXIT_SUCCESS;
}
