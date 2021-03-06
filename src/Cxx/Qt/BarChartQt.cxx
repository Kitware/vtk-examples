#include "BarChartQt.h"
#include "ui_BarChartQt.h"

#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkGenericOpenGLRenderWindow.h>
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

#include <array>

#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

#if VTK_VERSION_NUMBER >= 90000000000ULL
#define VTK900 1
#endif

namespace {
// Monthly circulation data
int data_2008[] = {10822, 10941, 9979,  10370, 9460, 11228,
                   15093, 12231, 10160, 9816,  9384, 7892};
int data_2009[] = {9058,  9474,  9979,  9408, 8900, 11569,
                   14688, 12231, 10294, 9585, 8957, 8590};
int data_2010[] = {9058,  10941, 9979,  10270, 8900, 11228,
                   14688, 12231, 10160, 9585,  9384, 8590};
} // namespace

// Constructor
BarChartQt::BarChartQt(QWidget* parent)
  : QMainWindow(parent), ui(new Ui::BarChartQt)
{
  this->ui->setupUi(this);

  vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
#if VTK890
  this->ui->qvtkWidget->setRenderWindow(renderWindow);
#else
  this->ui->qvtkWidget->SetRenderWindow(renderWindow);
#endif

  vtkNew<vtkNamedColors> colors;
  vtkColor3d backgroundColor = colors->GetColor3d("Seashell");
  vtkColor3d axisColor = colors->GetColor3d("Black");
  vtkColor3d titleColor = colors->GetColor3d("MidnightBlue");

  vtkNew<vtkChartXY> chart;

  // Set various properties
  vtkAxis* xAxis = chart->GetAxis(vtkAxis::BOTTOM);
  xAxis->SetTitle("Monthly");
  xAxis->GetTitleProperties()->SetColor(axisColor.GetData());
  xAxis->GetTitleProperties()->SetFontSize(16);
  xAxis->GetTitleProperties()->ItalicOn();
  xAxis->GetLabelProperties()->SetColor(axisColor.GetData());
  xAxis->SetGridVisible(true);
  xAxis->GetGridPen()->SetColor(colors->GetColor4ub("Black"));

  vtkAxis* yAxis = chart->GetAxis(vtkAxis::LEFT);
  yAxis->SetTitle("Circulation");
  yAxis->GetTitleProperties()->SetColor(axisColor.GetData());
  yAxis->GetTitleProperties()->SetFontSize(16);
  yAxis->GetTitleProperties()->ItalicOn();
  yAxis->GetLabelProperties()->SetColor(axisColor.GetData());
  yAxis->SetGridVisible(true);
  yAxis->GetGridPen()->SetColor(colors->GetColor4ub("Black"));

  chart->SetTitle("Circulation 2008, 2009, 2010");
  chart->GetTitleProperties()->SetFontSize(24);
  chart->GetTitleProperties()->SetColor(titleColor.GetData());
  chart->GetTitleProperties()->BoldOn();

  // Set up a 2D scene, add an XY chart to it
  vtkNew<vtkContextView> view;
  view->SetRenderWindow(renderWindow);
  view->GetRenderer()->SetBackground(backgroundColor.GetData());
  view->GetRenderWindow()->SetSize(640, 480);
  view->GetScene()->AddItem(chart);

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
  vtkPlot* line = 0;

  line = chart->AddPlot(vtkChart::BAR);
  line->SetInputData(table, 0, 1);
  auto rgba = colors->GetColor4ub("YellowGreen");
  line->SetColor(rgba[0], rgba[1], rgba[2], rgba[3]);

  line = chart->AddPlot(vtkChart::BAR);
  line->SetInputData(table, 0, 2);
  rgba = colors->GetColor4ub("Salmon");
  line->SetColor(rgba[0], rgba[1], rgba[2], rgba[3]);

  line = chart->AddPlot(vtkChart::BAR);
  line->SetInputData(table, 0, 3);
  rgba = colors->GetColor4ub("CornflowerBlue");
  line->SetColor(rgba[0], rgba[1], rgba[2], rgba[3]);

  view->GetRenderWindow()->SetMultiSamples(0);

#if !defined(VTK900)
  view->SetInteractor(this->ui->qvtkWidget->GetInteractor());
#endif
  // VTK/Qt wedded
#if VTK890
  this->ui->qvtkWidget->setRenderWindow(view->GetRenderWindow());
#else
  this->ui->qvtkWidget->SetRenderWindow(view->GetRenderWindow());
#endif

  // Set up action signals and slots
  connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
}

BarChartQt::~BarChartQt()
{
  delete this->ui;
}

void BarChartQt::slotExit()
{
  std::cout << "Exiting" << std::endl;
  qApp->exit();
}
