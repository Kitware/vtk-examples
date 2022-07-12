#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkColor.h>
#include <vtkColorSeries.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkIntArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPen.h>
#include <vtkPlot.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>
#include <vtkTextProperty.h>
#include <vtkVersion.h>

#if VTK_VERSION_NUMBER >= 90220220630ULL
#define VTK_HAS_SETCOLORF 1
#endif

// Monthly circulation data
static int data_2008[] = {10822, 10941, 9979,  10370, 9460, 11228,
                          15093, 12231, 10160, 9816,  9384, 7892};
static int data_2009[] = {9058,  9474,  9979,  9408, 8900, 11569,
                          14688, 12231, 10294, 9585, 8957, 8590};
static int data_2010[] = {9058,  10941, 9979,  10270, 8900, 11228,
                          14688, 12231, 10160, 9585,  9384, 8590};

int main(int, char*[])
{
  // Colors
  vtkNew<vtkColorSeries> colorSeries;
  // colorSeries->SetColorScheme(
  //    vtkColorSeries::BREWER_SEQUENTIAL_YELLOW_ORANGE_BROWN_3);
  colorSeries->SetColorScheme(vtkColorSeries::BREWER_DIVERGING_SPECTRAL_3);
  // colorSeries->SetColorScheme(vtkColorSeries::BREWER_DIVERGING_PURPLE_ORANGE_3);
  // colorSeries->SetColorScheme(
  //    vtkColorSeries::BREWER_DIVERGING_BROWN_BLUE_GREEN_3);
  // colorSeries->SetColorScheme(vtkColorSeries::BREWER_SEQUENTIAL_BLUE_GREEN_3);
  // colorSeries->SetColorScheme(vtkColorSeries::BREWER_SEQUENTIAL_BLUE_PURPLE_3);

  vtkNew<vtkNamedColors> colors;
  vtkColor3d backgroundColor = colors->GetColor3d("Seashell");
  vtkColor3d axisColor = colors->GetColor3d("Black");
  vtkColor3d titleColor = colors->GetColor3d("MidnightBlue");

  // Set up a 2D scene, add an XY chart to it
  vtkNew<vtkContextView> view;
  view->GetRenderer()->SetBackground(backgroundColor.GetData());
  view->GetRenderWindow()->SetSize(640, 480);
  view->GetRenderWindow()->SetWindowName("BarChart");

  vtkNew<vtkChartXY> chart;
  view->GetScene()->AddItem(chart);

  // Set various properties
  vtkAxis* xAxis = chart->GetAxis(vtkAxis::BOTTOM);
  xAxis->SetTitle("Monthly");
  xAxis->GetTitleProperties()->SetColor(axisColor.GetData());
  xAxis->GetTitleProperties()->SetFontSize(16);
  xAxis->GetTitleProperties()->ItalicOn();
  xAxis->GetLabelProperties()->SetColor(axisColor.GetData());
  xAxis->GetGridPen()->SetColor(colors->GetColor4ub("Black"));

  vtkAxis* yAxis = chart->GetAxis(vtkAxis::LEFT);
  yAxis->SetTitle("Circulation");
  yAxis->GetTitleProperties()->SetColor(axisColor.GetData());
  yAxis->GetTitleProperties()->SetFontSize(16);
  yAxis->GetTitleProperties()->ItalicOn();
  yAxis->GetLabelProperties()->SetColor(axisColor.GetData());
  yAxis->GetGridPen()->SetColor(colors->GetColor4ub("Black"));

  chart->SetTitle("Circulation 2008, 2009, 2010");
  chart->GetTitleProperties()->SetFontSize(24);
  chart->GetTitleProperties()->SetColor(titleColor.GetData());
  chart->GetTitleProperties()->BoldOn();

  // Create a table with some points in it...
  vtkNew<vtkTable> table;

  vtkNew<vtkIntArray> arrMonth;
  arrMonth->SetName("Month");
  table->AddColumn(arrMonth);

  vtkNew<vtkIntArray> arr2008;
  arr2008->SetName("2008");
  table->AddColumn(arr2008);

  vtkNew<vtkIntArray> arr2009;
  arr2009->SetName("2009");
  table->AddColumn(arr2009);

  vtkNew<vtkIntArray> arr2010;
  arr2010->SetName("2010");
  table->AddColumn(arr2010);

  table->SetNumberOfRows(12);
  for (int i = 0; i < 12; i++)
  {
    table->SetValue(i, 0, i + 1);
    table->SetValue(i, 1, data_2008[i]);
    table->SetValue(i, 2, data_2009[i]);
    table->SetValue(i, 3, data_2010[i]);
  }

  // Add multiple line plots, setting the colors etc
#if VTK_HAS_SETCOLORF
  vtkPlot* line = 0;
  line = chart->AddPlot(vtkChart::BAR);
  line->SetColorF(colorSeries->GetColor(0).GetRed() / 255.0,
                  colorSeries->GetColor(0).GetGreen() / 255.0,
                  colorSeries->GetColor(0).GetBlue() / 255.0);
  line->SetInputData(table, 0, 1);

  line = chart->AddPlot(vtkChart::BAR);
  line->SetColorF(colorSeries->GetColor(1).GetRed() / 255.0,
                  colorSeries->GetColor(1).GetGreen() / 255.0,
                  colorSeries->GetColor(1).GetBlue() / 255.0);
  line->SetInputData(table, 0, 2);

  line = chart->AddPlot(vtkChart::BAR);
  line->SetColorF(colorSeries->GetColor(2).GetRed() / 255.0,
                  colorSeries->GetColor(2).GetGreen() / 255.0,
                  colorSeries->GetColor(2).GetBlue() / 255.0);
  line->SetInputData(table, 0, 3);
#else
  vtkPlot* line = 0;
  line = chart->AddPlot(vtkChart::BAR);
  line->SetColor(colorSeries->GetColor(0).GetRed() / 255.0,
                 colorSeries->GetColor(0).GetGreen() / 255.0,
                 colorSeries->GetColor(0).GetBlue() / 255.0);
  line->SetInputData(table, 0, 1);

  line = chart->AddPlot(vtkChart::BAR);
  line->SetColor(colorSeries->GetColor(1).GetRed() / 255.0,
                 colorSeries->GetColor(1).GetGreen() / 255.0,
                 colorSeries->GetColor(1).GetBlue() / 255.0);
  line->SetInputData(table, 0, 2);

  line = chart->AddPlot(vtkChart::BAR);
  line->SetColor(colorSeries->GetColor(2).GetRed() / 255.0,
                 colorSeries->GetColor(2).GetGreen() / 255.0,
                 colorSeries->GetColor(2).GetBlue() / 255.0);
  line->SetInputData(table, 0, 3);
#endif

  // Finally render the scene and compare the image to a reference image
  view->GetRenderWindow()->SetMultiSamples(0);
  view->GetRenderWindow()->Render();
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
