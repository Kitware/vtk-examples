#include <vtkNew.h>
#include <vtkTimerLog.h>

int main(int, char*[])
{
  vtkNew<vtkTimerLog> timerLog;

  std::cout << "Current time: " << timerLog->GetUniversalTime() << std::endl;

  timerLog->MarkEvent("opened file");

  timerLog->MarkEvent("did other stuff");

  std::cout << "Timer log:" << *timerLog << std::endl;

  return EXIT_SUCCESS;
}
