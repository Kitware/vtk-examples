#include <vtkNew.h>
#include <vtkSphereSource.h>

#include "vtkTestMultipleInputPortsFilter.h"

int main(int /* argc */, char* /* argv */[])
{
  vtkNew<vtkSphereSource> sphereSource1;
  sphereSource1->SetThetaResolution(5);
  sphereSource1->Update();

  vtkNew<vtkSphereSource> sphereSource2;
  sphereSource2->SetThetaResolution(10);
  sphereSource2->Update();

  vtkNew<vtkTestMultipleInputPortsFilter> filter;
  filter->SetInputConnection(0, sphereSource1->GetOutputPort());
  filter->SetInputConnection(1, sphereSource2->GetOutputPort());
  filter->Update();

  return EXIT_SUCCESS;
}
