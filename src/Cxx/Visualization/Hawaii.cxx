// Translated from hawaii.tcl

/*

Produce figure 6-12 from the VTK Textbook.
It is a translation of the original hawaii.tcl with a few additional
enhancements.
The image is centered on Honolulu, O'ahu.
Diamond Head is the crater lower left. Punchbowl is the crater in the centre.

The colorScheme option allows you to select a series of colour schemes.
0: The default, a lookup using a "Brewer" palette.
1: The original: A lookup table of 256 colours ranging from deep blue (water) to
yellow-white (mountain top).
2: A lookup table with a preset number of colours.

*/

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkColorSeries.h>
#include <vtkDataSetMapper.h>
#include <vtkElevationFilter.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>
#include <string>

namespace {
//! Create a lookup table.
/*!
@param lut - An indexed lookup table.
*/
void MakeLUT(int const& colorScheme, vtkLookupTable* lut);
} // namespace

int main(int argc, char* argv[])
{

  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " filename [colorScheme]" << std::endl;
    std::cout << "where: filename is the file honolulu.vtk." << std::endl;
    std::cout << "       colorScheme is 0, 1, or 2, default 0" << std::endl;
    return EXIT_FAILURE;
  }

  int colorScheme = 0;
  std::string fileName = argv[1];

  if (argc == 3)
  {
    colorScheme = std::abs(atoi(argv[2]));
    colorScheme = (colorScheme > 2) ? 0 : colorScheme;
  }

  vtkNew<vtkNamedColors> colors;

  // Set the background color.
  std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
  colors->SetColor("BkgColor", bkg.data());

  // Read a vtk file
  //
  vtkNew<vtkPolyDataReader> hawaii;
  hawaii->SetFileName(fileName.c_str());
  hawaii->Update();
  double bounds[6];
  hawaii->GetOutput()->GetBounds(bounds);

  vtkNew<vtkElevationFilter> elevation;
  elevation->SetInputConnection(hawaii->GetOutputPort());
  elevation->SetLowPoint(0, 0, 0);
  elevation->SetHighPoint(0, 0, 1000);
  elevation->SetScalarRange(0, 1000);

  vtkNew<vtkLookupTable> lut;
  MakeLUT(colorScheme, lut);

  vtkNew<vtkDataSetMapper> hawaiiMapper;
  hawaiiMapper->SetInputConnection(elevation->GetOutputPort());
  hawaiiMapper->SetScalarRange(0, 1000);
  hawaiiMapper->ScalarVisibilityOn();
  hawaiiMapper->SetLookupTable(lut);

  vtkNew<vtkActor> hawaiiActor;
  hawaiiActor->SetMapper(hawaiiMapper);

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size
  //
  ren->AddActor(hawaiiActor);
  // Match the window shape to the object.
  // renWin->SetSize(500, int(500 * bounds[1] / bounds[3]));
  renWin->SetSize(500, 500);
  renWin->SetWindowName("Hawaii");

  // Render the image.
  // Centered on Honolulu,
  // Diamond Head is the crater lower left.
  // Punchbowl is the crater in the centre.
  renWin->Render();
  ren->SetBackground(colors->GetColor3d("BkgColor").GetData());
  ren->GetActiveCamera()->Zoom(1.5);
  ren->GetActiveCamera()->Roll(-90);
  ren->ResetCameraClippingRange();

  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}

namespace {
void MakeLUT(int const& colorScheme, vtkLookupTable* lut)
{
  vtkNew<vtkNamedColors> colors;
  // Select a color scheme.
  switch (colorScheme)
  {
  case 0:
  default: {
    // Make the lookup using a Brewer palette.
    vtkNew<vtkColorSeries> colorSeries;
    colorSeries->SetNumberOfColors(8);
    int colorSeriesEnum = colorSeries->BREWER_DIVERGING_BROWN_BLUE_GREEN_8;
    colorSeries->SetColorScheme(colorSeriesEnum);
    colorSeries->BuildLookupTable(lut, colorSeries->ORDINAL);
    lut->SetNanColor(1, 0, 0, 1);
    break;
  }
  case 1: {
    // A lookup table of 256 colours ranging from
    //  deep blue(water) to yellow - white(mountain top)
    //  is used to color map this figure.
    lut->SetHueRange(0.7, 0);
    lut->SetSaturationRange(1.0, 0);
    lut->SetValueRange(0.5, 1.0);
    break;
  }
  case 2:
    // Make the lookup table with a preset number of colours.
    vtkNew<vtkColorSeries> colorSeries;
    colorSeries->SetNumberOfColors(8);
    colorSeries->SetColorSchemeName("Hawaii");
    colorSeries->SetColor(0, colors->GetColor3ub("turquoise_blue"));
    colorSeries->SetColor(1, colors->GetColor3ub("sea_green_medium"));
    colorSeries->SetColor(2, colors->GetColor3ub("sap_green"));
    colorSeries->SetColor(3, colors->GetColor3ub("green_dark"));
    colorSeries->SetColor(4, colors->GetColor3ub("tan"));
    colorSeries->SetColor(5, colors->GetColor3ub("beige"));
    colorSeries->SetColor(6, colors->GetColor3ub("light_beige"));
    colorSeries->SetColor(7, colors->GetColor3ub("bisque"));
    colorSeries->BuildLookupTable(lut, colorSeries->ORDINAL);
    lut->SetNanColor(1, 0, 0, 1);
  };
}
} // namespace
