#include <vtkActor.h>
#include <vtkDelimitedTextReader.h>
#include <vtkDoubleArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>
#include <vtkVersionMacros.h> // For version macros
#include <vtkVertexGlyphFilter.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename e.g DelimitedData.txt"
              << std::endl;
    return EXIT_FAILURE;
  }

  std::string inputFilename = argv[1];

  vtkNew<vtkDelimitedTextReader> reader;
  reader->SetFileName(inputFilename.c_str());
  reader->DetectNumericColumnsOn();
  reader->SetFieldDelimiterCharacters(" ");
  reader->Update();

  vtkTable* table = reader->GetOutput();

  vtkNew<vtkPoints> points;
  vtkNew<vtkDoubleArray> normals;

  normals->SetNumberOfComponents(3); // 3d normals (ie x,y,z)

  std::cout << "Table has " << table->GetNumberOfRows() << " rows."
            << std::endl;
  std::cout << "Table has " << table->GetNumberOfColumns() << " columns."
            << std::endl;

  for (vtkIdType i = 0; i < table->GetNumberOfRows(); i++)
  {
    std::cout << "x: " << (table->GetValue(i, 0)).ToDouble()
              << " y: " << (table->GetValue(i, 1)).ToDouble()
              << " z: " << (table->GetValue(i, 2)).ToDouble();

    points->InsertNextPoint((table->GetValue(i, 0)).ToDouble(),
                            (table->GetValue(i, 1)).ToDouble(),
                            (table->GetValue(i, 2)).ToDouble());

    double n[3];
    n[0] = (table->GetValue(i, 3)).ToDouble();
    n[1] = (table->GetValue(i, 4)).ToDouble();
    n[2] = (table->GetValue(i, 5)).ToDouble();

    std::cout << " n: " << n[0] << " " << n[1] << " " << n[2] << std::endl;
    normals->InsertNextTuple(n);
  }

  std::cout << "There are " << points->GetNumberOfPoints() << " points."
            << std::endl;

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);
  polydata->GetPointData()->SetNormals(normals);

  vtkNew<vtkVertexGlyphFilter> glyphFilter;
  glyphFilter->SetInputData(polydata);
  glyphFilter->Update();

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(glyphFilter->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetPointSize(30);
  actor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("DelimitedTextReader");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Mint").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
