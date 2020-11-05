#include <vtkActor.h>
#include <vtkButtonWidget.h>
#include <vtkCoordinate.h>
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
#include <vtkTexturedButtonRepresentation2D.h>

#include <array>

namespace {
void CreateImage(vtkImageData* image, std::string const& color1,
                 std::string const& color2);
}

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create two images for texture
  vtkNew<vtkImageData> image1;
  vtkNew<vtkImageData> image2;
  unsigned char banana[3] = {227, 207, 87};
  unsigned char tomato[3] = {255, 99, 71};
  CreateImage(image1, "Banana", "Tomato");
  CreateImage(image2, "Tomato", "Banana");

  // Create some geometry
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("TexturedButtonWidget");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Create the widget and its representation
  vtkNew<vtkTexturedButtonRepresentation2D> buttonRepresentation;
  buttonRepresentation->SetNumberOfStates(2);
  buttonRepresentation->SetButtonTexture(0, image1);
  buttonRepresentation->SetButtonTexture(1, image2);

  vtkNew<vtkButtonWidget> buttonWidget;
  buttonWidget->SetInteractor(renderWindowInteractor);
  buttonWidget->SetRepresentation(buttonRepresentation);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("MidnightBLue").GetData());

  renderWindow->SetSize(640, 480);
  renderWindow->Render();

  // Place the widget. Must be done after a render so that the
  // viewport is defined.
  // Here the widget placement is in normalized display coordinates
  vtkNew<vtkCoordinate> upperRight;
  upperRight->SetCoordinateSystemToNormalizedDisplay();
  upperRight->SetValue(1.0, 1.0);

  double bds[6];
  double sz = 50.0;
  bds[0] = upperRight->GetComputedDisplayValue(renderer)[0] - sz;
  bds[1] = bds[0] + sz;
  bds[2] = upperRight->GetComputedDisplayValue(renderer)[1] - sz;
  bds[3] = bds[2] + sz;
  bds[4] = bds[5] = 0.0;

  // Scale to 1, default is .5
  buttonRepresentation->SetPlaceFactor(1);
  buttonRepresentation->PlaceWidget(bds);

  buttonWidget->On();

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
void CreateImage(vtkImageData* image, std::string const& color1,
                 std::string const& color2)
{
  vtkNew<vtkNamedColors> colors;

  std::array<unsigned char, 3> dc1{0, 0, 0};
  std::array<unsigned char, 3> dc2{0, 0, 0};
  auto c1 = colors->GetColor3ub(color1).GetData();
  auto c2 = colors->GetColor3ub(color2).GetData();
  for (auto i = 0; i < 3; ++i)
  {
    dc1[i] = c1[i];
    dc2[i] = c2[i];
  }

  // Specify the size of the image data
  image->SetDimensions(10, 10, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

  int* dims = image->GetDimensions();

  // Fill the image with
  for (int y = 0; y < dims[1]; y++)
  {
    for (int x = 0; x < dims[0]; x++)
    {
      unsigned char* pixel =
          static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
      for (int i = 0; i < 3; ++i)
      {
        if (x < 5)
        {
          pixel[i] = dc1[i];
        }
        else
        {
          pixel[i] = dc2[i];
        }
      }
    }
  }
}
} // namespace
