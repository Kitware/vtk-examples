#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>

#include "vtkTestProgressReportFilter.h"

void ProgressFunction(vtkObject* caller, long unsigned int eventId,
                      void* clientData, void* callData);

int main(int, char*[])
{
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->Update();

  vtkNew<vtkCallbackCommand> progressCallback;
  progressCallback->SetCallback(ProgressFunction);

  vtkNew<vtkTestProgressReportFilter> testFilter;
  testFilter->SetInputConnection(sphereSource->GetOutputPort());
  testFilter->AddObserver(vtkCommand::ProgressEvent, progressCallback);
  testFilter->Update();

  return EXIT_SUCCESS;
}

void ProgressFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId),
                      void* vtkNotUsed(clientData), void* vtkNotUsed(callData))
{
  vtkTestProgressReportFilter* testFilter =
      static_cast<vtkTestProgressReportFilter*>(caller);
  std::cout << "Progress: " << testFilter->GetProgress() << std::endl;
}
