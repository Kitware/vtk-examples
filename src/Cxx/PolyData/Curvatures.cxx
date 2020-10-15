#include <vtkActor.h>
#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>
#include <vtkCurvatures.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkTextProperty.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Parse command line arguments
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0]
              << " Filename(.vtp) e.g. cowHead.vtp -20 20" << std::endl;
    return EXIT_FAILURE;
  }

  // Create a polydata
  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(argv[1]);

  vtkNew<vtkCurvatures> curvaturesFilter;
  curvaturesFilter->SetInputConnection(reader->GetOutputPort());
  curvaturesFilter->SetCurvatureTypeToMinimum();
  curvaturesFilter->SetCurvatureTypeToMaximum();
  curvaturesFilter->SetCurvatureTypeToGaussian();
  curvaturesFilter->SetCurvatureTypeToMean();
  curvaturesFilter->Update();

  // Get scalar range from command line if present, otherwise use
  // range of computed curvature
  double scalarRange[2];
  if (argc >= 4)
  {
    scalarRange[0] = atof(argv[2]);
    scalarRange[1] = atof(argv[3]);
  }
  else
  {
    curvaturesFilter->GetOutput()->GetScalarRange(scalarRange);
  }

  int scheme = 16;
  if (argc >= 5)
  {
    scheme = atoi(argv[4]);
  }

  // Build a lookup table
  vtkNew<vtkColorSeries> colorSeries;
  colorSeries->SetColorScheme(scheme);
  std::cout << "Using color scheme #: " << colorSeries->GetColorScheme()
            << " is " << colorSeries->GetColorSchemeName() << std::endl;

  vtkNew<vtkColorTransferFunction> lut;
  lut->SetColorSpaceToHSV();

  // Use a color series to create a transfer function
  int numColors = colorSeries->GetNumberOfColors();
  for (int i = 0; i < numColors; i++)
  {
    vtkColor3ub color = colorSeries->GetColor(i);
    double dColor[3];
    dColor[0] = static_cast<double>(color[0]) / 255.0;
    dColor[1] = static_cast<double>(color[1]) / 255.0;
    dColor[2] = static_cast<double>(color[2]) / 255.0;
    double t = scalarRange[0] +
        (scalarRange[1] - scalarRange[0]) / (numColors - 1) * i;
    lut->AddRGBPoint(t, dColor[0], dColor[1], dColor[2]);
  }

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(curvaturesFilter->GetOutputPort());
  mapper->SetLookupTable(lut);
  mapper->SetScalarRange(scalarRange);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Create a scalar bar
  vtkNew<vtkScalarBarActor> scalarBar;
  scalarBar->SetLookupTable(mapper->GetLookupTable());
  scalarBar->SetTitle(
      curvaturesFilter->GetOutput()->GetPointData()->GetScalars()->GetName());
  scalarBar->UnconstrainedFontSizeOn();
  scalarBar->SetNumberOfLabels(5);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(600, 600);
  renderWindow->SetWindowName("Curvatures");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->AddActor2D(scalarBar);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
