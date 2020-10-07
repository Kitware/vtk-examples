#include <vtkImageData.h>
#include <vtkInteractorStyleImage.h>
#include <vtkTIFFWriter.h>
#include <vtkSmartPointer.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkTIFFReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>

#include <array>

int main(int argc, char *argv[])
{
  vtkNew<vtkNamedColors> colors;

  if (argc != 2)
  {
    std::cout << "Required parameters: OutputFilename.tif" << std::endl;
    return EXIT_FAILURE;
  }

  std::string filename = argv[1];

  // Convert our unsigned char colors to doubles
  // Set draw color needs doubles.
  auto color1 = colors->GetColor4ub("SteelBlue").GetData();
  auto color2 = colors->GetColor4ub("PaleGoldenrod").GetData();
  std::array<double, 4> sourceColor1{0, 0, 0, 0};
  std::array<double, 4> sourceColor2{0, 0, 0, 0};
  // Leave alpha at zero
  for (auto i = 0; i < 3; ++i)
  {
    sourceColor1[i] = color1[i];
    sourceColor2[i] = color2[i];
  }

  vtkNew<vtkImageCanvasSource2D> imageSource;
  imageSource->SetScalarTypeToUnsignedChar();
  imageSource->SetExtent(0, 9, 0, 9, 0, 0);
  imageSource->SetNumberOfScalarComponents(3);
  imageSource->SetDrawColor(sourceColor1.data());
  imageSource->FillBox(0, 9, 0, 9);
  imageSource->SetDrawColor(sourceColor2.data());
  imageSource->FillBox(5, 7, 5, 7);
  imageSource->Update();

  vtkNew<vtkTIFFWriter> tiffWriter;
  tiffWriter->SetFileName(filename.c_str());
  tiffWriter->SetInputConnection(imageSource->GetOutputPort());
  tiffWriter->Write();

  // Read and display for verification
  vtkNew<vtkTIFFReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();

  vtkNew<vtkImageActor> actor;
  actor->GetMapper()->SetInputConnection(reader->GetOutputPort());

  // Setup renderer
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
  renderer->ResetCamera();

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("WriteTIFF");

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  // Render and start interaction
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
