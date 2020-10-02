#include <vtkImageData.h>
#include <vtkImplicitDataSet.h>
#include <vtkNew.h>
#include <vtkRTAnalyticSource.h>
#include <vtkSphereSource.h>

int main(int /* argc */, char* /* argv */[])
{
  vtkNew<vtkRTAnalyticSource> waveletSource;
  waveletSource->Update();

  vtkNew<vtkImplicitDataSet> implicitWavelet;
  implicitWavelet->SetDataSet(waveletSource->GetOutput());

  double x[3] = {0.5, 0, 0};
  // Value should roughly be 258.658.
  cout << "x: " << implicitWavelet->EvaluateFunction(x) << endl;

  return EXIT_SUCCESS;
}
