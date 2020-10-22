#include "SideBySideRenderWindowsQt.h"

#include <vtkNamedColors.h>
#include <vtkNew.h>
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
#include <vtkNew.h>
#include <vtkSphereSource.h>
#include <vtkVersion.h>

#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

// Constructor
SideBySideRenderWindowsQt::SideBySideRenderWindowsQt()
{
  this->setupUi(this);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkGenericOpenGLRenderWindow> renderWindowLeft;
  vtkNew<vtkGenericOpenGLRenderWindow> renderWindowRight;
#if VTK890
  this->qvtkWidgetLeft->setRenderWindow(renderWindowLeft);
  this->qvtkWidgetRight->setRenderWindow(renderWindowRight);
#else
  this->qvtkWidgetLeft->SetRenderWindow(renderWindowLeft);
  this->qvtkWidgetRight->SetRenderWindow(renderWindowRight);
#endif

  // Sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetPhiResolution(30);
  sphereSource->SetThetaResolution(30);
  sphereSource->Update();
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
  cubeSource->Update();
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
  rightRenderer->GetActiveCamera()->SetPosition(1.0, 0.8, 1.0);
  rightRenderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
  rightRenderer->SetBackground(colors->GetColor3d("LightSteelBlue").GetData());
  rightRenderer->ResetCamera();
  rightRenderer->GetActiveCamera()->Zoom(0.8);

  // VTK/Qt wedded
#if VTK890
  this->qvtkWidgetLeft->renderWindow()->AddRenderer(leftRenderer);
  this->qvtkWidgetRight->renderWindow()->AddRenderer(rightRenderer);
#else
  this->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(leftRenderer);
  this->qvtkWidgetRight->GetRenderWindow()->AddRenderer(rightRenderer);
#endif

  // Set up action signals and slots
  connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
}

void SideBySideRenderWindowsQt::slotExit()
{
  qApp->exit();
}
