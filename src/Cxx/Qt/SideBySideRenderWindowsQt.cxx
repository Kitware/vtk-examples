#include "SideBySideRenderWindowsQt.h"
#include "ui_SideBySideRenderWindowsQt.h"

#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkVersion.h>

#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

// Constructor
SideBySideRenderWindowsQt::SideBySideRenderWindowsQt(QWidget* parent)
  : QMainWindow(parent), ui(new Ui::SideBySideRenderWindowsQt)
{
  this->ui->setupUi(this);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkGenericOpenGLRenderWindow> renderWindowLeft;
  vtkNew<vtkGenericOpenGLRenderWindow> renderWindowRight;
#if VTK890
  this->ui->qvtkWidgetLeft->setRenderWindow(renderWindowLeft);
  this->ui->qvtkWidgetRight->setRenderWindow(renderWindowRight);
#else
  this->ui->qvtkWidgetLeft->SetRenderWindow(renderWindowLeft);
  this->ui->qvtkWidgetRight->SetRenderWindow(renderWindowRight);
#endif

  // Sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetPhiResolution(30);
  sphereSource->SetThetaResolution(30);
  vtkNew<vtkElevationFilter> sphereElev;
  sphereElev->SetInputConnection(sphereSource->GetOutputPort());
  sphereElev->SetLowPoint(0, -1.0, 0);
  sphereElev->SetHighPoint(0, 1.0, 0);
  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereElev->GetOutputPort());
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);

  // Cube
  vtkNew<vtkCubeSource> cubeSource;
  cubeSource->SetXLength(0.8);
  cubeSource->SetYLength(0.8);
  cubeSource->SetZLength(0.8);
  vtkNew<vtkElevationFilter> cubeElev;
  cubeElev->SetInputConnection(cubeSource->GetOutputPort());
  cubeElev->SetLowPoint(0, -1.0, 0);
  cubeElev->SetHighPoint(0, 1.0, 0);
  vtkNew<vtkPolyDataMapper> cubeMapper;
  cubeMapper->SetInputConnection(cubeElev->GetOutputPort());
  vtkNew<vtkActor> cubeActor;
  cubeActor->SetMapper(cubeMapper);

  // VTK Renderer
  vtkNew<vtkRenderer> leftRenderer;
  leftRenderer->AddActor(sphereActor);
  leftRenderer->SetBackground(colors->GetColor3d("LightSteelBlue").GetData());

  vtkNew<vtkRenderer> rightRenderer;

  // Add Actor to renderer
  rightRenderer->AddActor(cubeActor);
  rightRenderer->GetActiveCamera()->Azimuth(60);
  rightRenderer->ResetCamera();
  rightRenderer->GetActiveCamera()->Zoom(0.8);
  rightRenderer->SetBackground(colors->GetColor3d("LightSteelBlue").GetData());

  // VTK/Qt wedded
#if VTK890
  this->ui->qvtkWidgetLeft->renderWindow()->AddRenderer(leftRenderer);
  this->ui->qvtkWidgetRight->renderWindow()->AddRenderer(rightRenderer);
#else
  this->ui->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(leftRenderer);
  this->ui->qvtkWidgetRight->GetRenderWindow()->AddRenderer(rightRenderer);
#endif

  // Set up action signals and slots
  connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
}

void SideBySideRenderWindowsQt::slotExit()
{
  qApp->exit();
}
