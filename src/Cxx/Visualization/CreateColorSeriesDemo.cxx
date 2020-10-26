#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkColorSeries.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <map>
#include <string>
#include <vector>

namespace {
void CreateLookupTableVTKBlue(vtkLookupTable* lut, int tableSize = 0);
void CreateLookupTableVTKBrown(vtkLookupTable* lut, int tableSize = 0);
void CreateLookupTableVTKRed(vtkLookupTable* lu, int tableSize = 0);
void CreateLookupTableVTKOrange(vtkLookupTable* lu, int tableSize = 0);
void CreateLookupTableVTKWhite(vtkLookupTable* lu, int tableSize = 0);
void CreateLookupTableVTKGrey(vtkLookupTable* lu, int tableSize = 0);
void CreateLookupTableVTKMagenta(vtkLookupTable* lu, int tableSize = 0);
void CreateLookupTableVTKCyan(vtkLookupTable* lu, int tableSize = 0);
void CreateLookupTableVTKYellow(vtkLookupTable* lu, int tableSize = 0);
void CreateLookupTableVTKGreen(vtkLookupTable* lu, int tableSize = 0);
} // namespace

int main(int argc, char* argv[])
{
  std::string seriesName = "Red";
  if (argc > 1)
  {
    seriesName = argv[1];
  }

  // Provide some geometry
  int resolution = 6;
  vtkNew<vtkPlaneSource> aPlane;
  aPlane->SetXResolution(resolution);
  aPlane->SetYResolution(resolution);

  // Create cell data
  vtkNew<vtkFloatArray> cellData;
  for (int i = 0; i < resolution * resolution; i++)
  {
    cellData->InsertNextValue(i);
  }
  aPlane->Update(); // Force an update so we can set cell data
  aPlane->GetOutput()->GetCellData()->SetScalars(cellData);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkLookupTable> lut;
  if (seriesName == "Blue")
  {
    CreateLookupTableVTKBlue(lut, resolution * resolution + 1);
  }
  else if (seriesName == "Brown")
  {
    CreateLookupTableVTKBrown(lut, resolution * resolution + 1);
  }
  else if (seriesName == "Red")
  {
    CreateLookupTableVTKRed(lut, resolution * resolution + 1);
  }
  else if (seriesName == "Orange")
  {
    CreateLookupTableVTKOrange(lut, resolution * resolution + 1);
  }
  else if (seriesName == "White")
  {
    CreateLookupTableVTKWhite(lut, resolution * resolution + 1);
  }
  else if (seriesName == "Grey")
  {
    CreateLookupTableVTKGrey(lut, resolution * resolution + 1);
  }
  else if (seriesName == "Magenta")
  {
    CreateLookupTableVTKMagenta(lut, resolution * resolution + 1);
  }
  else if (seriesName == "Cyan")
  {
    CreateLookupTableVTKCyan(lut, resolution * resolution + 1);
  }
  else if (seriesName == "Yellow")
  {
    CreateLookupTableVTKYellow(lut, resolution * resolution + 1);
  }
  else if (seriesName == "Green")
  {
    CreateLookupTableVTKGreen(lut, resolution * resolution + 1);
  }
  else
  {
    std::cout << "Bad series name: " << seriesName << std::endl;
    return EXIT_FAILURE;
  }

  // Setup actor and mapper
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetLookupTable(lut);
  mapper->SetInputConnection(aPlane->GetOutputPort());
  mapper->SetScalarModeToUseCellData();
  mapper->SetScalarRange(0, resolution * resolution + 1);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->EdgeVisibilityOn();

  // Setup render window, renderer, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("CreateColorSeriesDemo");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
void CreateLookupTableVTKBlue(vtkLookupTable* lut, int size)
{
  vtkNew<vtkColorSeries> myColors;
  myColors->SetColorSchemeByName("VTKBlueColors");

  vtkNew<vtkNamedColors> nc;

  myColors->AddColor(nc->GetColor3ub("alice_blue"));
  myColors->AddColor(nc->GetColor3ub("blue"));
  myColors->AddColor(nc->GetColor3ub("blue_light"));
  myColors->AddColor(nc->GetColor3ub("blue_medium"));
  myColors->AddColor(nc->GetColor3ub("cadet"));
  myColors->AddColor(nc->GetColor3ub("cobalt"));
  myColors->AddColor(nc->GetColor3ub("cornflower"));
  myColors->AddColor(nc->GetColor3ub("cerulean"));
  myColors->AddColor(nc->GetColor3ub("dodger_blue"));
  myColors->AddColor(nc->GetColor3ub("indigo"));
  myColors->AddColor(nc->GetColor3ub("manganese_blue"));
  myColors->AddColor(nc->GetColor3ub("midnight_blue"));
  myColors->AddColor(nc->GetColor3ub("navy"));
  myColors->AddColor(nc->GetColor3ub("peacock"));
  myColors->AddColor(nc->GetColor3ub("powder_blue"));
  myColors->AddColor(nc->GetColor3ub("royal_blue"));
  myColors->AddColor(nc->GetColor3ub("slate_blue"));
  myColors->AddColor(nc->GetColor3ub("slate_blue_dark"));
  myColors->AddColor(nc->GetColor3ub("slate_blue_light"));
  myColors->AddColor(nc->GetColor3ub("slate_blue_medium"));
  myColors->AddColor(nc->GetColor3ub("sky_blue"));
  myColors->AddColor(nc->GetColor3ub("sky_blue_deep"));
  myColors->AddColor(nc->GetColor3ub("sky_blue_light"));
  myColors->AddColor(nc->GetColor3ub("steel_blue"));
  myColors->AddColor(nc->GetColor3ub("steel_blue_light"));
  myColors->AddColor(nc->GetColor3ub("turquoise_blue"));
  myColors->AddColor(nc->GetColor3ub("ultramarine"));

  int numberOfColors = myColors->GetNumberOfColors();
  std::cout << "Number of colors: " << numberOfColors << std::endl;
  lut->SetNumberOfTableValues(size == 0 ? numberOfColors : size);
  lut->SetTableRange(0, lut->GetNumberOfTableValues());
  for (int i = 0; i < lut->GetNumberOfTableValues(); ++i)
  {
    vtkColor3ub color = myColors->GetColorRepeating(i);
    lut->SetTableValue(i, color.GetRed() / 255., color.GetGreen() / 255.,
                       color.GetBlue() / 255., 1.);
  }
}

void CreateLookupTableVTKBrown(vtkLookupTable* lut, int size)
{
  vtkNew<vtkColorSeries> myColors;
  myColors->SetColorSchemeByName("VTKBrownColors");

  vtkNew<vtkNamedColors> nc;

  myColors->AddColor(nc->GetColor3ub("beige"));
  myColors->AddColor(nc->GetColor3ub("brown"));
  myColors->AddColor(nc->GetColor3ub("brown_madder"));
  myColors->AddColor(nc->GetColor3ub("brown_ochre"));
  myColors->AddColor(nc->GetColor3ub("burlywood"));
  myColors->AddColor(nc->GetColor3ub("burnt_sienna"));
  myColors->AddColor(nc->GetColor3ub("burnt_umber"));
  myColors->AddColor(nc->GetColor3ub("chocolate"));
  myColors->AddColor(nc->GetColor3ub("deep_ochre"));
  myColors->AddColor(nc->GetColor3ub("flesh"));
  myColors->AddColor(nc->GetColor3ub("flesh_ochre"));
  myColors->AddColor(nc->GetColor3ub("gold_ochre"));
  myColors->AddColor(nc->GetColor3ub("greenish_umber"));
  myColors->AddColor(nc->GetColor3ub("khaki"));
  myColors->AddColor(nc->GetColor3ub("khaki_dark"));
  myColors->AddColor(nc->GetColor3ub("light_beige"));
  myColors->AddColor(nc->GetColor3ub("peru"));
  myColors->AddColor(nc->GetColor3ub("rosy_brown"));
  myColors->AddColor(nc->GetColor3ub("raw_sienna"));
  myColors->AddColor(nc->GetColor3ub("raw_umber"));
  myColors->AddColor(nc->GetColor3ub("sepia"));
  myColors->AddColor(nc->GetColor3ub("sienna"));
  myColors->AddColor(nc->GetColor3ub("saddle_brown"));
  myColors->AddColor(nc->GetColor3ub("sandy_brown"));
  myColors->AddColor(nc->GetColor3ub("tan"));
  myColors->AddColor(nc->GetColor3ub("van_dyke_brown"));
  int numberOfColors = myColors->GetNumberOfColors();
  std::cout << "Number of colors: " << numberOfColors << std::endl;
  lut->SetNumberOfTableValues(size == 0 ? numberOfColors : size);
  lut->SetTableRange(0, lut->GetNumberOfTableValues());
  for (int i = 0; i < lut->GetNumberOfTableValues(); ++i)
  {
    vtkColor3ub color = myColors->GetColorRepeating(i);
    lut->SetTableValue(i, color.GetRed() / 255., color.GetGreen() / 255.,
                       color.GetBlue() / 255., 1.);
  }
}

void CreateLookupTableVTKRed(vtkLookupTable* lut, int size)
{
  vtkNew<vtkColorSeries> myColors;
  myColors->SetColorSchemeByName("VTKRedColors");

  vtkNew<vtkNamedColors> nc;

  myColors->AddColor(nc->GetColor3ub("alizarin_crimson"));
  myColors->AddColor(nc->GetColor3ub("brick"));
  myColors->AddColor(nc->GetColor3ub("cadmium_red_deep"));
  myColors->AddColor(nc->GetColor3ub("coral"));
  myColors->AddColor(nc->GetColor3ub("coral_light"));
  myColors->AddColor(nc->GetColor3ub("deep_pink"));
  myColors->AddColor(nc->GetColor3ub("english_red"));
  myColors->AddColor(nc->GetColor3ub("firebrick"));
  myColors->AddColor(nc->GetColor3ub("geranium_lake"));
  myColors->AddColor(nc->GetColor3ub("hot_pink"));
  myColors->AddColor(nc->GetColor3ub("indian_red"));
  myColors->AddColor(nc->GetColor3ub("light_salmon"));
  myColors->AddColor(nc->GetColor3ub("madder_lake_deep"));
  myColors->AddColor(nc->GetColor3ub("maroon"));
  myColors->AddColor(nc->GetColor3ub("pink"));
  myColors->AddColor(nc->GetColor3ub("pink_light"));
  myColors->AddColor(nc->GetColor3ub("raspberry"));
  myColors->AddColor(nc->GetColor3ub("red"));
  myColors->AddColor(nc->GetColor3ub("rose_madder"));
  myColors->AddColor(nc->GetColor3ub("salmon"));
  myColors->AddColor(nc->GetColor3ub("tomato"));
  myColors->AddColor(nc->GetColor3ub("venetian_red"));
  int numberOfColors = myColors->GetNumberOfColors();
  std::cout << "Number of colors: " << numberOfColors << std::endl;
  lut->SetNumberOfTableValues(size == 0 ? numberOfColors : size);
  lut->SetTableRange(0, lut->GetNumberOfTableValues());
  for (int i = 0; i < lut->GetNumberOfTableValues(); ++i)
  {
    vtkColor3ub color = myColors->GetColorRepeating(i);
    lut->SetTableValue(i, color.GetRed() / 255., color.GetGreen() / 255.,
                       color.GetBlue() / 255., 1.);
  }
}

void CreateLookupTableVTKGrey(vtkLookupTable* lut, int size)
{
  vtkNew<vtkColorSeries> myColors;

  vtkNew<vtkNamedColors> nc;

  myColors->SetColorSchemeByName("VTKGreyColors");
  myColors->AddColor(nc->GetColor3ub("cold_grey"));
  myColors->AddColor(nc->GetColor3ub("dim_grey"));
  myColors->AddColor(nc->GetColor3ub("grey"));
  myColors->AddColor(nc->GetColor3ub("light_grey"));
  myColors->AddColor(nc->GetColor3ub("slate_grey"));
  myColors->AddColor(nc->GetColor3ub("slate_grey_dark"));
  myColors->AddColor(nc->GetColor3ub("slate_grey_light"));
  myColors->AddColor(nc->GetColor3ub("warm_grey"));

  int numberOfColors = myColors->GetNumberOfColors();
  std::cout << "Number of colors: " << numberOfColors << std::endl;
  lut->SetNumberOfTableValues(size == 0 ? numberOfColors : size);
  lut->SetTableRange(0, lut->GetNumberOfTableValues());
  for (int i = 0; i < lut->GetNumberOfTableValues(); ++i)
  {
    vtkColor3ub color = myColors->GetColorRepeating(i);
    lut->SetTableValue(i, color.GetRed() / 255., color.GetGreen() / 255.,
                       color.GetBlue() / 255., 1.);
  }
}

void CreateLookupTableVTKWhite(vtkLookupTable* lut, int size)
{
  vtkNew<vtkColorSeries> myColors;

  vtkNew<vtkNamedColors> nc;

  myColors->SetColorSchemeByName("VTKWhiteColors");
  myColors->AddColor(nc->GetColor3ub("antique_white"));
  myColors->AddColor(nc->GetColor3ub("azure"));
  myColors->AddColor(nc->GetColor3ub("bisque"));
  myColors->AddColor(nc->GetColor3ub("blanched_almond"));
  myColors->AddColor(nc->GetColor3ub("cornsilk"));
  myColors->AddColor(nc->GetColor3ub("eggshell"));
  myColors->AddColor(nc->GetColor3ub("floral_white"));
  myColors->AddColor(nc->GetColor3ub("gainsboro"));
  myColors->AddColor(nc->GetColor3ub("ghost_white"));
  myColors->AddColor(nc->GetColor3ub("honeydew"));
  myColors->AddColor(nc->GetColor3ub("ivory"));
  myColors->AddColor(nc->GetColor3ub("lavender"));
  myColors->AddColor(nc->GetColor3ub("lavender_blush"));
  myColors->AddColor(nc->GetColor3ub("lemon_chiffon"));
  myColors->AddColor(nc->GetColor3ub("linen"));
  myColors->AddColor(nc->GetColor3ub("mint_cream"));
  myColors->AddColor(nc->GetColor3ub("misty_rose"));
  myColors->AddColor(nc->GetColor3ub("moccasin"));
  myColors->AddColor(nc->GetColor3ub("navajo_white"));
  myColors->AddColor(nc->GetColor3ub("old_lace"));
  myColors->AddColor(nc->GetColor3ub("papaya_whip"));
  myColors->AddColor(nc->GetColor3ub("peach_puff"));
  myColors->AddColor(nc->GetColor3ub("seashell"));
  myColors->AddColor(nc->GetColor3ub("snow"));
  myColors->AddColor(nc->GetColor3ub("thistle"));
  myColors->AddColor(nc->GetColor3ub("titanium_white"));
  myColors->AddColor(nc->GetColor3ub("wheat"));
  myColors->AddColor(nc->GetColor3ub("white"));
  myColors->AddColor(nc->GetColor3ub("white_smoke"));
  myColors->AddColor(nc->GetColor3ub("zinc_white"));

  int numberOfColors = myColors->GetNumberOfColors();
  std::cout << "Number of colors: " << numberOfColors << std::endl;
  lut->SetNumberOfTableValues(size == 0 ? numberOfColors : size);
  lut->SetTableRange(0, lut->GetNumberOfTableValues());
  for (int i = 0; i < lut->GetNumberOfTableValues(); ++i)
  {
    vtkColor3ub color = myColors->GetColorRepeating(i);
    lut->SetTableValue(i, color.GetRed() / 255., color.GetGreen() / 255.,
                       color.GetBlue() / 255., 1.);
  }
}

void CreateLookupTableVTKOrange(vtkLookupTable* lut, int size)
{
  vtkNew<vtkColorSeries> myColors;
  myColors->SetColorSchemeByName("VTKOrangeColors");

  vtkNew<vtkNamedColors> nc;

  myColors->AddColor(nc->GetColor3ub("cadmium_orange"));
  myColors->AddColor(nc->GetColor3ub("cadmium_red_light"));
  myColors->AddColor(nc->GetColor3ub("carrot"));
  myColors->AddColor(nc->GetColor3ub("dark_orange"));
  myColors->AddColor(nc->GetColor3ub("mars_orange"));
  myColors->AddColor(nc->GetColor3ub("mars_yellow"));
  myColors->AddColor(nc->GetColor3ub("orange"));
  myColors->AddColor(nc->GetColor3ub("orange_red"));
  myColors->AddColor(nc->GetColor3ub("yellow_ochre"));
  int numberOfColors = myColors->GetNumberOfColors();
  std::cout << "Number of colors: " << numberOfColors << std::endl;
  lut->SetNumberOfTableValues(size == 0 ? numberOfColors : size);
  lut->SetTableRange(0, lut->GetNumberOfTableValues());
  for (int i = 0; i < lut->GetNumberOfTableValues(); ++i)
  {
    vtkColor3ub color = myColors->GetColorRepeating(i);
    lut->SetTableValue(i, color.GetRed() / 255., color.GetGreen() / 255.,
                       color.GetBlue() / 255., 1.);
  }
}

void CreateLookupTableVTKMagenta(vtkLookupTable* lut, int size)
{
  vtkNew<vtkColorSeries> myColors;
  myColors->SetColorSchemeByName("VTKMagentaColors");

  vtkNew<vtkNamedColors> nc;

  myColors->AddColor(nc->GetColor3ub("blue_violet"));
  myColors->AddColor(nc->GetColor3ub("cobalt_violet_deep"));
  myColors->AddColor(nc->GetColor3ub("magenta"));
  myColors->AddColor(nc->GetColor3ub("orchid"));
  myColors->AddColor(nc->GetColor3ub("orchid_dark"));
  myColors->AddColor(nc->GetColor3ub("orchid_medium"));
  myColors->AddColor(nc->GetColor3ub("permanent_red_violet"));
  myColors->AddColor(nc->GetColor3ub("plum"));
  myColors->AddColor(nc->GetColor3ub("purple"));
  myColors->AddColor(nc->GetColor3ub("purple_medium"));
  myColors->AddColor(nc->GetColor3ub("ultramarine_violet"));
  myColors->AddColor(nc->GetColor3ub("violet"));
  myColors->AddColor(nc->GetColor3ub("violet_dark"));
  myColors->AddColor(nc->GetColor3ub("violet_red"));
  myColors->AddColor(nc->GetColor3ub("violet_red_medium"));
  myColors->AddColor(nc->GetColor3ub("violet_red_pale"));

  int numberOfColors = myColors->GetNumberOfColors();
  std::cout << "Number of colors: " << numberOfColors << std::endl;
  lut->SetNumberOfTableValues(size == 0 ? numberOfColors : size);
  lut->SetTableRange(0, lut->GetNumberOfTableValues());
  for (int i = 0; i < lut->GetNumberOfTableValues(); ++i)
  {
    vtkColor3ub color = myColors->GetColorRepeating(i);
    lut->SetTableValue(i, color.GetRed() / 255., color.GetGreen() / 255.,
                       color.GetBlue() / 255., 1.);
  }
}

void CreateLookupTableVTKCyan(vtkLookupTable* lut, int size)
{
  vtkNew<vtkColorSeries> myColors;
  myColors->SetColorSchemeByName("VTKCyanColors");

  vtkNew<vtkNamedColors> nc;

  myColors->AddColor(nc->GetColor3ub("aquamarine"));
  myColors->AddColor(nc->GetColor3ub("aquamarine_medium"));
  myColors->AddColor(nc->GetColor3ub("cyan"));
  myColors->AddColor(nc->GetColor3ub("cyan_white"));
  myColors->AddColor(nc->GetColor3ub("turquoise"));
  myColors->AddColor(nc->GetColor3ub("turquoise_dark"));
  myColors->AddColor(nc->GetColor3ub("turquoise_medium"));
  myColors->AddColor(nc->GetColor3ub("turquoise_pale"));

  int numberOfColors = myColors->GetNumberOfColors();
  std::cout << "Number of colors: " << numberOfColors << std::endl;
  lut->SetNumberOfTableValues(size == 0 ? numberOfColors : size);
  lut->SetTableRange(0, lut->GetNumberOfTableValues());
  for (int i = 0; i < lut->GetNumberOfTableValues(); ++i)
  {
    vtkColor3ub color = myColors->GetColorRepeating(i);
    lut->SetTableValue(i, color.GetRed() / 255., color.GetGreen() / 255.,
                       color.GetBlue() / 255., 1.);
  }
}

void CreateLookupTableVTKYellow(vtkLookupTable* lut, int size)
{
  vtkNew<vtkColorSeries> myColors;
  myColors->SetColorSchemeByName("VTKYellowColors");

  vtkNew<vtkNamedColors> nc;

  myColors->AddColor(nc->GetColor3ub("aureoline_yellow"));
  myColors->AddColor(nc->GetColor3ub("banana"));
  myColors->AddColor(nc->GetColor3ub("cadmium_lemon"));
  myColors->AddColor(nc->GetColor3ub("cadmium_yellow"));
  myColors->AddColor(nc->GetColor3ub("cadmium_yellow_light"));
  myColors->AddColor(nc->GetColor3ub("gold"));
  myColors->AddColor(nc->GetColor3ub("goldenrod"));
  myColors->AddColor(nc->GetColor3ub("goldenrod_dark"));
  myColors->AddColor(nc->GetColor3ub("goldenrod_light"));
  myColors->AddColor(nc->GetColor3ub("goldenrod_pale"));
  myColors->AddColor(nc->GetColor3ub("light_goldenrod"));
  myColors->AddColor(nc->GetColor3ub("melon"));
  myColors->AddColor(nc->GetColor3ub("naples_yellow_deep"));
  myColors->AddColor(nc->GetColor3ub("yellow"));
  myColors->AddColor(nc->GetColor3ub("yellow_light"));

  int numberOfColors = myColors->GetNumberOfColors();
  std::cout << "Number of colors: " << numberOfColors << std::endl;
  lut->SetNumberOfTableValues(size == 0 ? numberOfColors : size);
  lut->SetTableRange(0, lut->GetNumberOfTableValues());
  for (int i = 0; i < lut->GetNumberOfTableValues(); ++i)
  {
    vtkColor3ub color = myColors->GetColorRepeating(i);
    lut->SetTableValue(i, color.GetRed() / 255., color.GetGreen() / 255.,
                       color.GetBlue() / 255., 1.);
  }
}

void CreateLookupTableVTKGreen(vtkLookupTable* lut, int size)
{
  vtkNew<vtkColorSeries> myColors;
  myColors->SetColorSchemeByName("VTKGreenColors");

  vtkNew<vtkNamedColors> nc;

  myColors->AddColor(nc->GetColor3ub("chartreuse"));
  myColors->AddColor(nc->GetColor3ub("chrome_oxide_green"));
  myColors->AddColor(nc->GetColor3ub("cinnabar_green"));
  myColors->AddColor(nc->GetColor3ub("cobalt_green"));
  myColors->AddColor(nc->GetColor3ub("emerald_green"));
  myColors->AddColor(nc->GetColor3ub("forest_green"));
  myColors->AddColor(nc->GetColor3ub("green"));
  myColors->AddColor(nc->GetColor3ub("green_dark"));
  myColors->AddColor(nc->GetColor3ub("green_pale"));
  myColors->AddColor(nc->GetColor3ub("green_yellow"));
  myColors->AddColor(nc->GetColor3ub("lawn_green"));
  myColors->AddColor(nc->GetColor3ub("lime_green"));
  myColors->AddColor(nc->GetColor3ub("mint"));
  myColors->AddColor(nc->GetColor3ub("olive"));
  myColors->AddColor(nc->GetColor3ub("olive_drab"));
  myColors->AddColor(nc->GetColor3ub("olive_green_dark"));
  myColors->AddColor(nc->GetColor3ub("permanent_green"));
  myColors->AddColor(nc->GetColor3ub("sap_green"));
  myColors->AddColor(nc->GetColor3ub("sea_green"));
  myColors->AddColor(nc->GetColor3ub("sea_green_dark"));
  myColors->AddColor(nc->GetColor3ub("sea_green_medium"));
  myColors->AddColor(nc->GetColor3ub("sea_green_light"));
  myColors->AddColor(nc->GetColor3ub("spring_green"));
  myColors->AddColor(nc->GetColor3ub("spring_green_medium"));
  myColors->AddColor(nc->GetColor3ub("terre_verte"));
  myColors->AddColor(nc->GetColor3ub("viridian_light"));
  myColors->AddColor(nc->GetColor3ub("yellow_green"));

  int numberOfColors = myColors->GetNumberOfColors();
  std::cout << "Number of colors: " << numberOfColors << std::endl;
  lut->SetNumberOfTableValues(size == 0 ? numberOfColors : size);
  lut->SetTableRange(0, lut->GetNumberOfTableValues());
  for (int i = 0; i < lut->GetNumberOfTableValues(); ++i)
  {
    vtkColor3ub color = myColors->GetColorRepeating(i);
    lut->SetTableValue(i, color.GetRed() / 255., color.GetGreen() / 255.,
                       color.GetBlue() / 255., 1.);
  }
}
} // namespace
