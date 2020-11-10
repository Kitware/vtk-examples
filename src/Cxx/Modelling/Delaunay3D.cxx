#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellIterator.h>
#include <vtkCleanPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkDelaunay3D.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLPolyDataReader.h>

#include <sstream>

int main(int argc, char* argv[])
{
  float alpha = 0.5;
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " filename.vtp [alpha] e.g. Bunny.vtp"
              << std::endl;
    return EXIT_FAILURE;
  }
  if (argc > 2)
  {
    alpha = atof(argv[2]);
  }
  // Read the file
  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(argv[1]);

  vtkNew<vtkNamedColors> color;

  vtkNew<vtkDataSetMapper> originalMapper;
  originalMapper->SetInputConnection(reader->GetOutputPort());
  originalMapper->ScalarVisibilityOff();
  originalMapper->SetScalarModeToUseCellData();

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetColor(color->GetColor3d("tomato").GetData());
  originalActor->GetProperty()->SetInterpolationToFlat();

  // Clean the polydata. This will remove duplicate points that may be
  // present in the input data.
  vtkNew<vtkCleanPolyData> cleaner;
  cleaner->SetInputConnection(reader->GetOutputPort());

  // Generate a tetrahedral mesh from the input points. By
  // default, the generated volume is the convex hull of the points.
  vtkNew<vtkDelaunay3D> delaunay3D;
  delaunay3D->SetInputConnection(cleaner->GetOutputPort());

  vtkNew<vtkDataSetMapper> delaunayMapper;
  delaunayMapper->SetInputConnection(delaunay3D->GetOutputPort());
  delaunayMapper->SetScalarModeToUseCellData();

  vtkNew<vtkActor> delaunayActor;
  delaunayActor->SetMapper(delaunayMapper);
  delaunayActor->GetProperty()->SetColor(color->GetColor3d("banana").GetData());
  delaunayActor->GetProperty()->SetInterpolationToFlat();

  // Generate a mesh from the input points. If Alpha is non-zero, then
  // tetrahedra, triangles, edges and vertices that lie within the
  // alpha radius are output.
  vtkNew<vtkDelaunay3D> delaunay3DAlpha;
  delaunay3DAlpha->SetInputConnection(cleaner->GetOutputPort());
  delaunay3DAlpha->SetAlpha(alpha);
  delaunay3DAlpha->Update();
  std::cout << "Alpha: " << alpha << std::endl;

  vtkNew<vtkUnsignedCharArray> cellData;
  cellData->SetNumberOfComponents(3);

  int numTetras = 0;
  int numLines = 0;
  int numTris = 0;
  int numVerts = 0;

  auto it = delaunay3DAlpha->GetOutput()->NewCellIterator();
  for (it->InitTraversal(); !it->IsDoneWithTraversal(); it->GoToNextCell())
  {
    if (it->GetCellType() == VTK_TETRA)
    {
      numTetras++;
      cellData->InsertNextTypedTuple(color->GetColor3ub("Banana").GetData());
    }
    else if (it->GetCellType() == VTK_LINE)
    {
      numLines++;
      cellData->InsertNextTypedTuple(color->GetColor3ub("Peacock").GetData());
    }
    else if (it->GetCellType() == VTK_TRIANGLE)
    {
      numTris++;
      cellData->InsertNextTypedTuple(color->GetColor3ub("Tomato").GetData());
    }
    else if (it->GetCellType() == VTK_VERTEX)
    {
      numVerts++;
      cellData->InsertNextTypedTuple(color->GetColor3ub("Lime").GetData());
    }
  }
  it->Delete();

  std::stringstream ss;
  ss << "numTetras: " << numTetras << std::endl;
  ss << "numLines: " << numLines << std::endl;
  ss << "numTris: " << numTris << std::endl;
  ss << "numVerts: " << numVerts;

  // Set the cell color depending on cell type
  delaunay3DAlpha->GetOutput()->GetCellData()->SetScalars(cellData);

  vtkNew<vtkDataSetMapper> delaunayAlphaMapper;
  delaunayAlphaMapper->SetInputConnection(delaunay3DAlpha->GetOutputPort());
  delaunayAlphaMapper->SetScalarModeToUseCellData();

  vtkNew<vtkActor> delaunayAlphaActor;
  delaunayAlphaActor->SetMapper(delaunayAlphaMapper);
  delaunayAlphaActor->GetProperty()->SetPointSize(5.0);
  delaunayAlphaActor->GetProperty()->SetInterpolationToFlat();

  // Visualize

  vtkNew<vtkTextProperty> textProperty;
  textProperty->SetFontSize(16);
  textProperty->SetColor(color->GetColor3d("Black").GetData());

  vtkNew<vtkTextMapper> textMapper;
  textMapper->SetInput(ss.str().c_str());
  textMapper->SetTextProperty(textProperty);

  vtkNew<vtkActor2D> textActor;
  textActor->SetMapper(textMapper);
  textActor->SetPosition(10, 10);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.33, 1.0};
  double centerViewport[4] = {0.33, 0.0, 0.66, 1.0};
  double rightViewport[4] = {0.66, 0.0, 1.0, 1.0};

  // Shared camera
  vtkNew<vtkCamera> sharedCamera;

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetActiveCamera(sharedCamera);

  vtkNew<vtkRenderer> delaunayRenderer;
  delaunayRenderer->SetActiveCamera(sharedCamera);

  vtkNew<vtkRenderer> delaunayAlphaRenderer;
  delaunayAlphaRenderer->SetActiveCamera(sharedCamera);

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(900, 300);
  renderWindow->SetWindowName("Delaunay3D");

  renderWindow->AddRenderer(originalRenderer);
  originalRenderer->SetViewport(leftViewport);
  renderWindow->AddRenderer(delaunayRenderer);
  delaunayRenderer->SetViewport(centerViewport);
  renderWindow->AddRenderer(delaunayAlphaRenderer);
  delaunayAlphaRenderer->SetViewport(rightViewport);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  originalRenderer->AddActor(originalActor);
  delaunayRenderer->AddActor(delaunayActor);
  delaunayAlphaRenderer->AddActor(delaunayAlphaActor);
  delaunayAlphaRenderer->AddViewProp(textActor);

  originalRenderer->SetBackground(color->GetColor3d("Slate_Grey").GetData());
  delaunayRenderer->SetBackground(color->GetColor3d("Light_Grey").GetData());
  delaunayAlphaRenderer->SetBackground(color->GetColor3d("Grey").GetData());

  originalRenderer->ResetCamera();
  renderWindow->Render();

  // Render and interact
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
