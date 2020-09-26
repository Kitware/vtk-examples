#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageFlip.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create an image
  vtkNew<vtkImageMandelbrotSource> source;
  source->Update();

  vtkNew<vtkImageCast> castSource;
  castSource->SetOutputScalarTypeToUnsignedChar();
  castSource->SetInputConnection(source->GetOutputPort());
  castSource->Update();

  vtkNew<vtkImageFlip> flipXFilter;
  flipXFilter->SetFilteredAxis(0); // flip x axis
  flipXFilter->SetInputConnection(source->GetOutputPort());
  flipXFilter->Update();

  vtkNew<vtkImageCast> castXFilter;
  castXFilter->SetOutputScalarTypeToUnsignedChar();
  castXFilter->SetInputConnection(flipXFilter->GetOutputPort());
  castXFilter->Update();

  vtkNew<vtkImageFlip> flipYFilter;
  flipYFilter->SetFilteredAxis(1); // flip y axis
  flipYFilter->SetInputConnection(source->GetOutputPort());
  flipYFilter->Update();

  vtkNew<vtkImageCast> castYFilter;
  castYFilter->SetOutputScalarTypeToUnsignedChar();
  castYFilter->SetInputConnection(flipYFilter->GetOutputPort());
  castYFilter->Update();

  vtkNew<vtkImageFlip> flipZFilter;
  flipZFilter->SetFilteredAxis(2); // flip z axis
  flipZFilter->SetInputConnection(source->GetOutputPort());
  flipZFilter->Update();

  vtkNew<vtkImageCast> castZFilter;
  castZFilter->SetOutputScalarTypeToUnsignedChar();
  castZFilter->SetInputConnection(flipZFilter->GetOutputPort());
  castZFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> inputActor;
  inputActor->GetMapper()->SetInputConnection(castSource->GetOutputPort());

  vtkNew<vtkImageActor> xActor;
  xActor->GetMapper()->SetInputConnection(castXFilter->GetOutputPort());

  vtkNew<vtkImageActor> yActor;
  yActor->GetMapper()->SetInputConnection(castYFilter->GetOutputPort());

  vtkNew<vtkImageActor> zActor;
  zActor->GetMapper()->SetInputConnection(castZFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double inputViewport[4] = {0.0, 0.0, 0.25, 1.0};
  double xViewport[4] = {0.25, 0.0, 0.5, 1.0};
  double yViewport[4] = {0.5, 0.0, 0.75, 1.0};
  double zViewport[4] = {0.75, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> inputRenderer;
  inputRenderer->SetViewport(inputViewport);
  inputRenderer->AddActor(inputActor);
  inputRenderer->ResetCamera();
  inputRenderer->SetBackground(colors->GetColor3d("Snow").GetData());

  vtkNew<vtkRenderer> xRenderer;
  xRenderer->SetViewport(xViewport);
  xRenderer->AddActor(xActor);
  xRenderer->ResetCamera();
  xRenderer->SetBackground(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkRenderer> yRenderer;
  yRenderer->SetViewport(yViewport);
  yRenderer->AddActor(yActor);
  yRenderer->ResetCamera();
  yRenderer->SetBackground(colors->GetColor3d("Mint").GetData());

  vtkNew<vtkRenderer> zRenderer;
  zRenderer->SetViewport(zViewport);
  zRenderer->AddActor(zActor);
  zRenderer->ResetCamera();
  zRenderer->SetBackground(colors->GetColor3d("Peacock").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(1000, 250);
  renderWindow->AddRenderer(inputRenderer);
  renderWindow->AddRenderer(xRenderer);
  renderWindow->AddRenderer(yRenderer);
  renderWindow->AddRenderer(zRenderer);
  renderWindow->SetWindowName("Flip");

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  // Render and start interaction
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
