#include <vtkImageBlend.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageViewer2.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <array>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " InputImageFilename e.g. Gourds2.jpg"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> imgReader;
  imgReader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  imgReader->SetFileName(argv[1]);
  imgReader->Update();

  vtkImageData* image = imgReader->GetOutput();

  // Find center of image
  int center[2];
  center[0] = (image->GetExtent()[1] + image->GetExtent()[0]) / 2;
  center[1] = (image->GetExtent()[3] + image->GetExtent()[2]) / 2;

  // Pick a radius for the circle
  int radius;
  radius = (image->GetExtent()[1] < image->GetExtent()[3])
      ? image->GetExtent()[1] * 2 / 5
      : image->GetExtent()[3] * 2 / 5;

  std::array<double, 3> drawColor1{0, 0, 0};
  auto color1 = colors->GetColor3ub("Black").GetData();
  std::array<double, 3> drawColor2{0, 0, 0};
  auto color2 = colors->GetColor3ub("Seashell").GetData();
  for (auto i = 0; i < 3; ++i)
  {
    drawColor1[i] = color1[i];
    drawColor2[i] = color2[i];
  }

  // Draw a circle in the center of the image
  vtkNew<vtkImageCanvasSource2D> drawing;
  drawing->SetNumberOfScalarComponents(3);
  drawing->SetScalarTypeToUnsignedChar();
  drawing->SetExtent(image->GetExtent());
  drawing->SetDrawColor(drawColor1.data());
  drawing->FillBox(image->GetExtent()[0], image->GetExtent()[1],
                   image->GetExtent()[2], image->GetExtent()[3]);
  drawing->SetDrawColor(drawColor2.data());
  drawing->DrawCircle(center[0], center[1], radius);

  // Combine the images (blend takes multiple connections on the 0th
  // input port)
  vtkNew<vtkImageBlend> blend;
  blend->AddInputConnection(imgReader->GetOutputPort());
  blend->AddInputConnection(drawing->GetOutputPort());
  blend->SetOpacity(0, .6);
  blend->SetOpacity(1, .4);

  // Display the result
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  vtkNew<vtkImageViewer2> imageViewer;
  imageViewer->SetInputConnection(blend->GetOutputPort());
  imageViewer->SetSize(640, 512);
  imageViewer->SetupInteractor(renderWindowInteractor);
  imageViewer->GetRenderer()->ResetCamera();
  imageViewer->GetRenderer()->SetBackground(
      colors->GetColor3d("LightSlateGray").GetData());

  imageViewer->GetRenderWindow()->SetWindowName("DrawOnAnImage");
  imageViewer->GetRenderWindow()->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
