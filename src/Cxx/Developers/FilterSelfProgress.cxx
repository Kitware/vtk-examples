#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>

#include "vtkTestFilterSelfProgressFilter.h"

void ProgressFunction(vtkObject* caller, long unsigned int eventId,
                      void* clientData, void* callData);

int main(int /* argc */, char** /* argv */)
{
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkTestFilterSelfProgressFilter> testFilter;
  testFilter->SetInputConnection(sphereSource->GetOutputPort());
  testFilter->Update();

  return EXIT_SUCCESS;
}
