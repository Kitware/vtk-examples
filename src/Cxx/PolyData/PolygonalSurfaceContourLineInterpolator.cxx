#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkContourWidget.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygonalSurfaceContourLineInterpolator.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTriangleFilter.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkSmartPointer<vtkPolyData> polyData;
  if (argc < 2)
  {
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetThetaResolution(40);
    sphereSource->SetPhiResolution(20);
    sphereSource->Update();

    polyData = sphereSource->GetOutput();
  }
  else
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();
    polyData = reader->GetOutput();
  }

  // The Dijkistra interpolator will not accept cells that aren't triangles
  vtkNew<vtkTriangleFilter> triangleFilter;
  triangleFilter->SetInputData(polyData);
  triangleFilter->Update();

  auto pd = triangleFilter->GetOutput();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(triangleFilter->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetInterpolationToFlat();
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

  // Create the render window, renderer and interactor.

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("PolygonalSurfaceContourLineInterpolator");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer, set the background and size

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("CadetBlue").GetData());

  // Here comes the contour widget stuff...

  vtkNew<vtkContourWidget> contourWidget;
  contourWidget->SetInteractor(interactor);
  vtkSmartPointer<vtkOrientedGlyphContourRepresentation> rep =
      dynamic_cast<vtkOrientedGlyphContourRepresentation*>(
          contourWidget->GetRepresentation());
  rep->GetLinesProperty()->SetColor(colors->GetColor3d("Crimson").GetData());
  rep->GetLinesProperty()->SetLineWidth(3.0);

  vtkNew<vtkPolygonalSurfacePointPlacer> pointPlacer;
  pointPlacer->AddProp(actor);
  pointPlacer->GetPolys()->AddItem(pd);
  rep->SetPointPlacer(pointPlacer);

  vtkNew<vtkPolygonalSurfaceContourLineInterpolator> interpolator;
  interpolator->GetPolys()->AddItem(pd);
  rep->SetLineInterpolator(interpolator);

  renderWindow->Render();
  interactor->Initialize();

  contourWidget->EnabledOn();

  interactor->Start();

  return EXIT_SUCCESS;
}
