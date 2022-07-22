#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSphereSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkPointData.h>
#include <vtkProperty.h>

#include <QApplication>
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>

#include <cmath>
#include <cstdlib>
#include "math.h"

void randomize(vtkSphereSource* vtksphere, vtkMapper * vtkmapper, vtkGenericOpenGLRenderWindow* vtkwindow)
{
  double randAmp = 0.2 + ((std::rand() % 1000)/1000.0) * 0.2;
  double randThetaFreq = 1.0 + (std::rand() % 9);
  double randPhiFreq = 1.0 + (std::rand() % 9);
  vtksphere->Update();
  vtkSmartPointer<vtkPolyData> newsphere;
  newsphere.TakeReference(vtksphere->GetOutput()->NewInstance());
  newsphere->DeepCopy(vtksphere->GetOutput());
  vtkNew<vtkDoubleArray> height;
  height->SetName("Height");
  height->SetNumberOfComponents(1);
  height->SetNumberOfTuples(newsphere->GetNumberOfPoints());
  newsphere->GetPointData()->AddArray(height);
  for(int iP = 0; iP < newsphere->GetNumberOfPoints(); iP++)
  {
    double pt[3];
    newsphere->GetPoint(iP, pt);
    double theta = std::atan2(pt[1], pt[0]);
    double phi = std::atan2(pt[2], std::sqrt(std::pow(pt[0], 2) + std::pow(pt[1], 2)));
    double thisAmp = randAmp * std::cos(randThetaFreq*theta) * std::sin(randPhiFreq*phi);
    height->SetValue(iP, thisAmp);
    pt[0] += thisAmp*std::cos(theta)*std::cos(phi);
    pt[1] += thisAmp*std::sin(theta)*std::cos(phi);
    pt[2] += thisAmp*std::sin(phi);
    newsphere->GetPoints()->SetPoint(iP, pt);
  }
  newsphere->GetPointData()->SetScalars(height);
  vtkmapper->SetInputConnection(nullptr);
  vtkmapper->SetInputDataObject(nullptr);
  vtkmapper->SetInputDataObject(newsphere);
  vtkmapper->SetScalarModeToUsePointData();
  vtkmapper->ColorByArrayComponent("Height", 0);
  vtkwindow->Render();
}

int main(int argc, char** argv)
{
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

  QApplication app(argc, argv);

  // main window
  QMainWindow mainWindow;
  mainWindow.resize(1200, 900);

  // control area
  QDockWidget controlDock;
  mainWindow.addDockWidget(Qt::LeftDockWidgetArea, &controlDock);

  QLabel controlDockTitle("Control Dock");
  controlDockTitle.setMargin(20);
  controlDock.setTitleBarWidget(&controlDockTitle);

  QPointer<QVBoxLayout> dockLayout = new QVBoxLayout();
  QWidget layoutContainer;
  layoutContainer.setLayout(dockLayout);
  controlDock.setWidget(&layoutContainer);

  QPushButton randomizeButton;
  randomizeButton.setIcon(QIcon(":/icons/iconmnstr_random.svg"));
  dockLayout->addWidget(&randomizeButton);

  // render area
  QPointer<QVTKOpenGLNativeWidget> vtkRenderWidget = new QVTKOpenGLNativeWidget();
  mainWindow.setCentralWidget(vtkRenderWidget);
  
  // VTK part
  vtkNew<vtkGenericOpenGLRenderWindow> vtkwindow;
  vtkRenderWidget->setRenderWindow(vtkwindow.Get());

  vtkNew<vtkSphereSource> vtksphere;
  vtksphere->SetRadius(1.0);
  vtksphere->SetThetaResolution(100);
  vtksphere->SetPhiResolution(100);

  vtkNew<vtkDataSetMapper> vtkmapper;
  vtkmapper->SetInputConnection(vtksphere->GetOutputPort());

  vtkNew<vtkActor> vtkactor;
  vtkactor->SetMapper(vtkmapper);
  vtkactor->GetProperty()->SetEdgeVisibility(true);
  vtkactor->GetProperty()->SetRepresentationToSurface();

  vtkNew<vtkRenderer> vtkrenderer;
  vtkrenderer->AddActor(vtkactor);

  vtkwindow->AddRenderer(vtkrenderer);

  // setup initial status
  randomize(vtksphere, vtkmapper, vtkwindow);

  //connect the buttons
  QObject::connect(&randomizeButton, &QPushButton::released, [&vtksphere, &vtkmapper, &vtkwindow](){randomize(vtksphere, vtkmapper, vtkwindow);});

  mainWindow.show();

  return app.exec();
}
