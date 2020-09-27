#include <vtkActor.h>
#include <vtkBarChartActor.h>
#include <vtkFieldData.h>
#include <vtkImageAccumulate.h>
#include <vtkImageData.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageMagnitude.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkIntArray.h>
#include <vtkLegendBoxActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int argc, char* argv[])
{
  // Handle the arguments
  if (argc < 2)
  {
    std::cout << "Required arguments: filename e.g. Ox.jpg" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);

  vtkNew<vtkImageMagnitude> magnitude;
  magnitude->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkIntArray> frequencies;

  vtkNew<vtkImageAccumulate> histogram;
  histogram->SetInputConnection(magnitude->GetOutputPort());
  histogram->SetComponentExtent(0, 255, 0, 0, 0, 0);
  histogram->SetComponentOrigin(0, 0, 0);
  histogram->SetComponentSpacing(1, 0, 0);
  histogram->IgnoreZeroOn();
  histogram->Update();

  int numberOfTuples = 64;
  frequencies->SetNumberOfComponents(1);
  frequencies->SetNumberOfTuples(numberOfTuples);
  vtkIdType* output =
      static_cast<vtkIdType*>(histogram->GetOutput()->GetScalarPointer());

  for (int j = 0; j < numberOfTuples; ++j)
  {
    frequencies->SetTuple1(j, *output++);
  }

  vtkNew<vtkDataObject> dataObject;

  dataObject->GetFieldData()->AddArray(frequencies);

  // Create a vtkBarChartActor
  vtkNew<vtkBarChartActor> barChart;

  barChart->SetInput(dataObject);
  barChart->SetTitle("Histogram");
  barChart->GetPositionCoordinate()->SetValue(0.1, 0.05, 0.0);
  barChart->GetPosition2Coordinate()->SetValue(0.95, 0.85, 0.0);
  barChart->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());
  barChart->GetLegendActor()->SetNumberOfEntries(
      dataObject->GetFieldData()->GetArray(0)->GetNumberOfTuples());
  barChart->LegendVisibilityOff();
  barChart->LabelVisibilityOff();

  int count = 0;
  for (int i = 0; i < numberOfTuples; ++i)
  {
    barChart->SetBarColor(count++, colors->GetColor3d("Tomato").GetData());
  }

  // Visualize the histogram
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(barChart);
  renderer->SetBackground(colors->GetColor3d("Peacock").GetData());
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("ImageAccumulateGreyscale");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Initialize the event loop and then start it
  renderWindow->Render();
  interactor->Initialize();
  interactor->Start();

  return EXIT_SUCCESS;
}
