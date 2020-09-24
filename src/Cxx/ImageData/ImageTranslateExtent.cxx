#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkImageMapper.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkImageTranslateExtent.h>
#include <vtkProperty.h>
#include <vtkNamedColors.h>

namespace {
  void CreateColorImage(vtkImageData* const);
}

int main(int, char *[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkImageData> colorImage;
  CreateColorImage(colorImage); // This image has (0,0) at the bottom left corner

  int extent[6];
  colorImage->GetExtent(extent);
  std::cout << "Old extent: " << extent[0] << " " << extent[1] << " "
            << extent[2] << " " << extent[3] << " " << extent[4] << " "
            << extent[5] << std::endl;

  // This moves the (0,0) position in the image to the center of the image
  //colorImage->SetExtent(extent[0] - (extent[1] - extent[0]) / 2,
  //                      extent[0] + (extent[1] - extent[0]) / 2,
  //                      extent[2] - (extent[3] - extent[2]) / 2,
  //                      extent[2] + (extent[3] - extent[2]) / 2,
  //                      extent[4] - (extent[5] - extent[4]) / 2,
  //                      extent[4] + (extent[5] - extent[4]) / 2);

  int dimensions[3];
  colorImage->GetDimensions(dimensions);

  vtkNew<vtkImageTranslateExtent> translateExtent;
  translateExtent->SetTranslation(-dimensions[0] / 2, -dimensions[1] / 2, 0);
  translateExtent->SetInputData(colorImage);
  translateExtent->Update();
  colorImage->DeepCopy(translateExtent->GetOutput());

  colorImage->GetExtent(extent);
  std::cout << "New extent: " << extent[0] << " " << extent[1] << " "
            << extent[2] << " " << extent[3] << " " << extent[4] << " "
            << extent[5] << std::endl;

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(1.0);
  sphereSource->Update();

  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetColor(colors->GetColor3d("PeachPuff").GetData());

  vtkNew<vtkImageSliceMapper> imageSliceMapper;
  imageSliceMapper->SetInputData(colorImage);

  vtkNew<vtkImageSlice> imageSlice;
  imageSlice->SetMapper(imageSliceMapper);
  imageSlice->SetPosition(0, 0, 0);

  // Setup renderers
  vtkNew<vtkRenderer> renderer;
  renderer->AddViewProp(imageSlice);
  renderer->AddViewProp(sphereActor);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());


  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(300, 300);
  renderWindow->AddRenderer(renderer);

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  // Render and start interaction
  renderWindow->SetWindowName("ImageTranslateExtent");

  renderWindow->Render();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {

void CreateColorImage(vtkImageData* const image)
{
  image->SetDimensions(10, 10, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

    vtkNew<vtkNamedColors> colors;
  unsigned char r, g, b, a;
    colors->GetColor("Thistle", r, g, b, a);


  for (unsigned int x = 0; x < 10; x++)
  {
    for (unsigned int y = 0; y < 10; y++)
    {
      unsigned char* pixel =
          static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
      pixel[0] = r;
      pixel[1] = g;
      pixel[2] = b;
    }
  }
}

} // namespace
