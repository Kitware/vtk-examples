#include <vtkImageCanvasSource2D.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGWriter.h>

#include <array>

int main(int argc, char* argv[])
{
  std::string outputFilename;
  if (argc > 1)
  {
    outputFilename = argv[1];
  }
  else
  {
    outputFilename = "output.png";
  }

  vtkNew<vtkNamedColors> colors;

  std::array<double, 3> drawColor1{0, 0, 0};
  std::array<double, 3> drawColor2{0, 0, 0};
  auto color1 = colors->GetColor3ub("MediumOrchid").GetData();
  auto color2 = colors->GetColor3ub("DarkGreen").GetData();
  for (auto i = 0; i < 3; ++i)
  {
    drawColor1[i] = color1[i];
    drawColor2[i] = color2[i];
  }

  int extent[6] = {0, 99, 0, 99, 0, 0};
  vtkNew<vtkImageCanvasSource2D> imageSource;
  imageSource->SetExtent(extent);
  imageSource->SetScalarTypeToUnsignedChar();
  imageSource->SetNumberOfScalarComponents(3);
  imageSource->SetDrawColor(drawColor1.data());
  imageSource->FillBox(0, 99, 0, 99);
  imageSource->SetDrawColor(drawColor2.data());
  imageSource->FillBox(40, 70, 20, 50);
  imageSource->Update();

  vtkNew<vtkImageCast> castFilter;
  castFilter->SetOutputScalarTypeToUnsignedChar();
  castFilter->SetInputConnection(imageSource->GetOutputPort());
  castFilter->Update();

  vtkNew<vtkPNGWriter> writer;
  writer->SetFileName(outputFilename.c_str());
  writer->SetInputConnection(castFilter->GetOutputPort());
  writer->Write();

  return EXIT_SUCCESS;
}
