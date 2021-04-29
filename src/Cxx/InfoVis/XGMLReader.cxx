#include <vtkGraphLayoutStrategy.h>
#include <vtkGraphLayoutView.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSimple2DLayoutStrategy.h>
#include <vtkUndirectedGraph.h>
#include <vtkViewTheme.h>
#include <vtkXGMLReader.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  if (argc != 2)
  {
    std::cout << "Required parameters: Filename e.g. fsm.gml" << std::endl;
    return EXIT_FAILURE;
  }

  std::string inputFilename = argv[1];

  vtkNew<vtkXGMLReader> reader;
  reader->SetFileName(inputFilename.c_str());
  reader->Update();

  vtkUndirectedGraph* g = reader->GetOutput();

  vtkNew<vtkViewTheme> theme;
  theme->SetLineWidth(1);
  theme->SetPointSize(5);
  theme->SetCellOpacity(0.99);
  theme->SetOutlineColor(colors->GetColor3d("Gray").GetData());
  // Vertices
  theme->SetPointColor(colors->GetColor3d("Chartreuse").GetData());
  theme->SetSelectedPointColor(colors->GetColor3d("Magenta").GetData());
  theme->SetPointHueRange(1.0, 1.0);
  theme->SetPointSaturationRange(1.0, 1.0);
  theme->SetPointValueRange(0.0, 1.0);
  // theme->SetPointAlphaRange(0.2, 0.8);
  // Edges
  theme->SetCellColor(colors->GetColor3d("Honeydew").GetData());
  theme->SetSelectedCellColor(colors->GetColor3d("Cyan").GetData());
  theme->SetCellHueRange(0.1, 0.1);
  theme->SetCellSaturationRange(0.2, 1.0);
  theme->SetCellValueRange(0.5, 1.0);
  // theme->SetCellAlphaRange(0.2, 0.8);

  vtkNew<vtkSimple2DLayoutStrategy> simple2D;

  vtkNew<vtkGraphLayoutView> graphLayoutView;
  graphLayoutView->AddRepresentationFromInput(g);
  graphLayoutView->ApplyViewTheme(theme);
  // If we create a layout object directly, just set the pointer to this method.
  // graphLayoutView->SetLayoutStrategy(simple2D);
  graphLayoutView->SetLayoutStrategyToSimple2D();

  graphLayoutView->ResetCamera();

  graphLayoutView->GetRenderer()->GradientBackgroundOn();
  graphLayoutView->GetRenderer()->SetBackground2(
      colors->GetColor3d("DarkSlateGray").GetData());
  graphLayoutView->GetRenderer()->SetBackground(
      colors->GetColor3d("Black").GetData());

  graphLayoutView->GetRenderWindow()->SetSize(600, 600);
  graphLayoutView->GetRenderWindow()->SetWindowName("XGMLReader");

  graphLayoutView->Render();

  graphLayoutView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
