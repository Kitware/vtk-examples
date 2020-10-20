#include <vtkActor.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

int main(int argc, char* argv[])
{
  // Parse command line arguments
  if (argc != 2)
  {
    std::cout << "Required arguments: Filename e.g. cowHead.vtp" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  // float range[2];
  double range[2];
  reader->GetOutput()->GetPointData()->GetScalars()->GetRange(range);
  // std::cout << "Range: " << range[0] << " , " << range[1] << std::endl;

  vtkNew<vtkBandedPolyDataContourFilter> bf;
  bf->SetInputConnection(reader->GetOutputPort());
  int numContours = 30;
  bf->GenerateValues(numContours, range);
  bf->GenerateContourEdgesOn();
  bf->Update();

  // Color the contours
  bf->GetOutput(1)->GetPointData()->SetScalars(
      bf->GetOutput()->GetPointData()->GetScalars());

  // Make sure the mapper uses the new colors
  bf->GetOutput(0)->GetPointData()->SetActiveScalars("Scalars");

  {
    vtkNew<vtkXMLPolyDataWriter> writer;
    writer->SetInputConnection(bf->GetOutputPort());
    writer->SetFileName("output.vtp");
    writer->Update();
  }

  {
    vtkNew<vtkXMLPolyDataWriter> writer;
    writer->SetInputConnection(bf->GetOutputPort(1));
    writer->SetFileName("ContourEdges.vtp");
    writer->Update();
  }

  /*
  // See which isocontours are being generated
  double values[10];
  bf->GetValues(values);
  for(unsigned int i = 0; i < 10; i++)
    {
    cout << values[i] << " ";
    }
  cout << endl;
  */

  // Color actor
  vtkNew<vtkPolyDataMapper> colorMapper;
  colorMapper->SetInputConnection(bf->GetOutputPort(0));
  colorMapper->SetScalarRange(range);

  vtkNew<vtkActor> colorActor;
  colorActor->SetMapper(colorMapper);

  // Edge actor
  vtkNew<vtkPolyDataMapper> edgeMapper;
  edgeMapper->SetInputConnection(bf->GetOutputPort(1));
  edgeMapper->SetScalarRange(range);

  vtkNew<vtkActor> edgeActor;
  edgeActor->SetMapper(edgeMapper);
  edgeActor->GetProperty()->SetLineWidth(5);

  // Create the RenderWindow, Renderer and both Actors

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("PolyDataIsoLines");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer
  renderer->AddActor(colorActor);
  renderer->AddActor(edgeActor);

  renderer->SetBackground(colors->GetColor3d("Silver").GetData());
  renderWindow->Render();

  interactor->Start();

  return EXIT_SUCCESS;
}
