#include <vtkMaskPoints.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVertexGlyphFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a set of points
  vtkNew<vtkPointSource> pointsSource;
  pointsSource->SetNumberOfPoints(40);
  pointsSource->Update();

  std::cout << "There are " << pointsSource->GetOutput()->GetNumberOfPoints()
            << " input points." << std::endl;

  // Create a point set
  vtkNew<vtkMaskPoints> maskPoints;
  maskPoints->SetOnRatio(2); // keep every 2nd point (half the number of points)
  maskPoints->SetInputConnection(pointsSource->GetOutputPort());
  maskPoints->Update();

  vtkNew<vtkVertexGlyphFilter> glyphFilter;
  glyphFilter->SetInputConnection(maskPoints->GetOutputPort());
  glyphFilter->Update();

  std::cout << "There are " << maskPoints->GetOutput()->GetNumberOfPoints()
            << " masked points." << std::endl;

  vtkNew<vtkPolyDataMapper> inputMapper;
  inputMapper->SetInputConnection(pointsSource->GetOutputPort());
  vtkNew<vtkActor> inputActor;
  inputActor->SetMapper(inputMapper);
  inputActor->GetProperty()->SetPointSize(5);
  inputActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  vtkNew<vtkPolyDataMapper> maskedMapper;
  maskedMapper->SetInputConnection(glyphFilter->GetOutputPort());
  vtkNew<vtkActor> maskedActor;
  maskedActor->SetMapper(maskedMapper);
  maskedActor->GetProperty()->SetPointSize(5);
  maskedActor->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("MaskPoints");

  // And one interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup both renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("Chocolate").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  leftRenderer->AddActor(inputActor);
  rightRenderer->AddActor(maskedActor);

  leftRenderer->ResetCamera();
  rightRenderer->ResetCamera();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
