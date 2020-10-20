#include <vtkActor.h>
#include <vtkCardinalSpline.h>
#include <vtkCellArray.h>
#include <vtkGlyph3D.h>
#include <vtkMath.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkSplineFilter.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  auto polyData = vtkSmartPointer<vtkPolyData>::New();
  if (argc > 1)
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    unsigned int numberOfPoints = 10;
    vtkNew<vtkPoints> points;
    vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
    randomSequence->SetSeed(8775070);
    for (unsigned int i = 0; i < numberOfPoints; ++i)
    {
      double x, y, z;
      // random position and radius
      x = randomSequence->GetRangeValue(-1.0, 1.0);
      randomSequence->Next();
      y = randomSequence->GetRangeValue(-1.0, 1.0);
      randomSequence->Next();
      z = randomSequence->GetRangeValue(-1.0, 1.0);
      randomSequence->Next();
      points->InsertNextPoint(x, y, z);
    }
    vtkNew<vtkCellArray> lines;
    lines->InsertNextCell(numberOfPoints);
    for (unsigned int i = 0; i < numberOfPoints; ++i)
    {
      lines->InsertCellPoint(i);
    }
    polyData->SetPoints(points);
    polyData->SetLines(lines);
  }

  vtkNew<vtkCardinalSpline> spline;
  spline->SetLeftConstraint(2);
  spline->SetLeftValue(0.0);
  spline->SetRightConstraint(2);
  spline->SetRightValue(0.0);

  vtkNew<vtkSplineFilter> splineFilter;
  splineFilter->SetInputData(polyData);
  splineFilter->SetNumberOfSubdivisions(polyData->GetNumberOfPoints() * 10);
  splineFilter->SetSpline(spline);

  vtkNew<vtkPolyDataMapper> splineMapper;
  splineMapper->SetInputConnection(splineFilter->GetOutputPort());

  vtkNew<vtkActor> splineActor;
  splineActor->SetMapper(splineMapper);

  vtkNew<vtkSphereSource> originalNodes;
  originalNodes->SetRadius(.04);
  originalNodes->SetPhiResolution(10);
  originalNodes->SetThetaResolution(10);

  vtkNew<vtkGlyph3D> glyphOriginal;
  glyphOriginal->SetInputData(polyData);
  glyphOriginal->SetSourceConnection(originalNodes->GetOutputPort());

  vtkNew<vtkSphereSource> newNodes;
  newNodes->SetRadius(.02);
  newNodes->SetPhiResolution(10);
  newNodes->SetThetaResolution(10);

  vtkNew<vtkGlyph3D> glyphNew;
  glyphNew->SetInputConnection(splineFilter->GetOutputPort());
  glyphNew->SetSourceConnection(newNodes->GetOutputPort());

  vtkNew<vtkPolyDataMapper> originalMapper;
  originalMapper->SetInputConnection(glyphOriginal->GetOutputPort());

  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetColor(
      colors->GetColor3d("Banana").GetData());
  originalActor->GetProperty()->SetOpacity(.6);

  vtkNew<vtkPolyDataMapper> newMapper;
  newMapper->SetInputConnection(glyphNew->GetOutputPort());

  vtkNew<vtkActor> newActor;
  newActor->SetMapper(newMapper);
  newActor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ResamplePolyLine");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(originalActor);
  renderer->AddActor(newActor);
  renderer->AddActor(splineActor);

  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
