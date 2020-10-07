#include <vtkImageCanvasSource2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkXMLPImageDataWriter.h>

#include <array>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Convert our unsigned char colors to doubles
  // Set draw color needs doubles.
  auto color1 = colors->GetColor3ub("SteelBlue").GetData();
  auto color2 = colors->GetColor3ub("PaleGoldenrod").GetData();
  std::array<double, 3> sourceColor1{0, 0, 0};
  std::array<double, 3> sourceColor2{0, 0, 0};
  for (auto i = 0; i < 3; ++i)
  {
    sourceColor1[i] = color1[i];
    sourceColor2[i] = color2[i];
  }

  vtkNew<vtkImageCanvasSource2D> drawing;
  drawing->SetNumberOfScalarComponents(3);
  drawing->SetScalarTypeToUnsignedChar();
  drawing->SetExtent(0, 20, 0, 50, 0, 1);
  drawing->SetDrawColor(sourceColor1.data());
  drawing->FillBox(0, 9, 0, 9);
  drawing->SetDrawColor(sourceColor2.data());
  drawing->DrawCircle(5, 5, 3);

  int numberOfPieces = 4;

  vtkNew<vtkXMLPImageDataWriter> writer;
  writer->SetInputConnection(drawing->GetOutputPort());
  writer->SetFileName("Test.pvti");
  writer->SetNumberOfPieces(numberOfPieces);
  writer->SetEndPiece(numberOfPieces - 1);
  writer->SetStartPiece(0);
  writer->Update();

  return EXIT_SUCCESS;
}
