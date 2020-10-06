#include <vtkImageCanvasSource2D.h>
#include <vtkJPEGWriter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>

#include <array>

int main(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  std::string outputFilename = "output.jpg";

  vtkNew<vtkNamedColors> colors;

  std::array<double, 3> drawColor1{0, 0, 0};
  std::array<double, 3> drawColor2{0, 0, 0};
  auto color1 = colors->GetColor3ub("DeepSkyBlue").GetData();
  auto color2 = colors->GetColor3ub("PaleGoldenrod").GetData();
  for (auto i = 0; i < 3; ++i)
  {
    drawColor1[i] = color1[i];
    drawColor2[i] = color2[i];
  }

  // Create a 100x100 image to save into the jpeg file
  int extent[6] = {0, 99, 0, 99, 0, 0};
  vtkNew<vtkImageCanvasSource2D> imageSource;
  imageSource->SetExtent(extent);
  imageSource->SetScalarTypeToUnsignedChar(); // vtkJPEGWriter only accepts
                                              // unsigned char input
  imageSource->SetNumberOfScalarComponents(
      3); // 3 color channels: Red, Green and Blue

  // Fill the whole image with a bluish background
  imageSource->SetDrawColor(drawColor1.data());
  imageSource->FillBox(extent[0], extent[1], extent[2], extent[3]);

  // Paint a 30x30 yellowish square into the image
  imageSource->SetDrawColor(drawColor2.data());
  imageSource->FillBox(40, 70, 20, 50);

  vtkNew<vtkJPEGWriter> writer;
  writer->SetFileName(outputFilename.c_str());
  writer->SetInputConnection(imageSource->GetOutputPort());
  writer->Write();

  return EXIT_SUCCESS;
}
