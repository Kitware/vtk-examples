#include <vtkExodusIIWriter.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkTimeSourceExample.h>

int main(int, char *[])
{
   vtkNew<vtkTimeSourceExample> timeSource;

  vtkNew<vtkExodusIIWriter> exodusWriter;
  exodusWriter->SetFileName("output.exii");
  exodusWriter->SetInputConnection (timeSource->GetOutputPort());
  exodusWriter->WriteAllTimeStepsOn ();
  exodusWriter->Write();

  return EXIT_SUCCESS;
}
