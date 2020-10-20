#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkQuantizePolyDataPoints.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkGlyph3DMapper.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>


int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(100);
  pointSource->Update();

  std::cout << "There are " << pointSource->GetNumberOfPoints() << " points."
            << std::endl;

  vtkNew<vtkQuantizePolyDataPoints> quantizeFilter;
  quantizeFilter->SetInputConnection(pointSource->GetOutputPort());
  quantizeFilter->SetQFactor(.1);
  quantizeFilter->Update();

  vtkPolyData* quantized = quantizeFilter->GetOutput();
  std::cout << "There are " << quantized->GetNumberOfPoints()
            << " quantized points." << std::endl;

  for (vtkIdType i = 0; i < pointSource->GetOutput()->GetNumberOfPoints(); i++)
  {
    double pOrig[3];
    double pQuantized[3];
    pointSource->GetOutput()->GetPoint(i, pOrig);
    quantized->GetPoints()->GetPoint(i, pQuantized);

    std::cout << "Point " << i << " : (" << pOrig[0] << ", " << pOrig[1] << ", "
              << pOrig[2] << ")"
              << " (" << pQuantized[0] << ", " << pQuantized[1] << ", "
              << pQuantized[2] << ")" << std::endl;
  }

  double radius = 0.02;
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(radius);

  vtkNew<vtkGlyph3DMapper> inputMapper;
  inputMapper->SetInputConnection(pointSource->GetOutputPort());
  inputMapper->SetSourceConnection(sphereSource->GetOutputPort());
  inputMapper->ScalarVisibilityOff();
  inputMapper->ScalingOff();

  vtkNew<vtkActor> inputActor;
  inputActor->SetMapper(inputMapper);
  inputActor->GetProperty()->SetColor(
      colors->GetColor3d("Orchid").GetData());


  vtkNew<vtkGlyph3DMapper> quantizedMapper;
  quantizedMapper->SetInputConnection(quantizeFilter->GetOutputPort());
  quantizedMapper->SetSourceConnection(sphereSource->GetOutputPort());
  quantizedMapper->ScalarVisibilityOff();
  quantizedMapper->ScalingOff();

  vtkNew<vtkActor> quantizedActor;
  quantizedActor->SetMapper(quantizedMapper);
  quantizedActor->GetProperty()->SetColor(
      colors->GetColor3d("Orchid").GetData());

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(640, 360);

  // And one interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);
  renderWindow->SetWindowName("QuantizePolyDataPoints");

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup both renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("Bisque").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("PaleTurquoise").GetData());

  leftRenderer->AddActor(inputActor);
  rightRenderer->AddActor(quantizedActor);

  leftRenderer->ResetCamera();

  rightRenderer->SetActiveCamera(leftRenderer->GetActiveCamera());

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
