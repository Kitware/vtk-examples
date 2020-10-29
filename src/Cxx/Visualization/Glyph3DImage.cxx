#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkGlyph3DMapper.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int argc, char* argv[])
{
  // Verify command line arguments
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " BinaryImage" << std::endl;
    return EXIT_FAILURE;
  }

  // Read file
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);

  // Create anything you want here, we will use a point
  // Create the geometry of a point (the coordinate)
  vtkNew<vtkPoints> points;
  const float p[3] = {0.0, 0.0, 0.0};
  points->InsertNextPoint(p);

  vtkNew<vtkCellArray> vertices;
  vtkIdType pid[1];
  pid[0] = points->InsertNextPoint(p);
  vertices->InsertNextCell(1, pid);

  // Create a polydata object
  vtkNew<vtkPolyData> point;
  point->SetPoints(points);
  point->SetVerts(vertices);

  vtkNew<vtkGlyph3DMapper> glyph3Dmapper;
  glyph3Dmapper->SetSourceData(point);
  glyph3Dmapper->SetInputConnection(reader->GetOutputPort());
  glyph3Dmapper->Update();

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkActor> actor;
  actor->SetMapper(glyph3Dmapper);
  actor->GetProperty()->SetPointSize(3);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Glyph3DImage");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render and interact
  renderWindow->SetSize(640, 480);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
