#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageConstantPad.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMirrorPad.h>
#include <vtkImageProperty.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify input arguments
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename e.g. FullHead.mhd"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);
  reader->Update();

  // Pipelines
  vtkNew<vtkImageConstantPad> constantPad;
  constantPad->SetInputConnection(reader->GetOutputPort());
  constantPad->SetConstant(800);
  constantPad->SetOutputWholeExtent(-127, 383, -127, 383, 22, 22);

  vtkNew<vtkImageMirrorPad> mirrorPad;
  mirrorPad->SetInputConnection(reader->GetOutputPort());
  mirrorPad->SetOutputWholeExtent(constantPad->GetOutputWholeExtent());

  // Create actors

  vtkNew<vtkImageMapToWindowLevelColors> constantPadColor;
  constantPadColor->SetWindow(2000);
  constantPadColor->SetLevel(1000);
  constantPadColor->SetInputConnection(constantPad->GetOutputPort());

  vtkNew<vtkImageActor> constantPadActor;
  constantPadActor->GetMapper()->SetInputConnection(
      constantPadColor->GetOutputPort());
  constantPadActor->GetProperty()->SetInterpolationTypeToNearest();

  vtkNew<vtkImageMapToWindowLevelColors> mirrorPadColor;
  mirrorPadColor->SetWindow(2000);
  mirrorPadColor->SetLevel(1000);
  mirrorPadColor->SetInputConnection(mirrorPad->GetOutputPort());

  vtkNew<vtkImageActor> mirrorPadActor;
  mirrorPadActor->GetMapper()->SetInputConnection(
      mirrorPadColor->GetOutputPort());
  mirrorPadActor->GetProperty()->SetInterpolationTypeToNearest();

  // Setup renderers
  vtkNew<vtkRenderer> constantPadRenderer;
  constantPadRenderer->SetViewport(0.0, 0.0, 0.5, 1.0);
  constantPadRenderer->AddActor(constantPadActor);
  constantPadRenderer->ResetCamera();
  constantPadRenderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderer> mirrorPadRenderer;
  mirrorPadRenderer->SetViewport(0.5, 0.0, 1.0, 1.0);
  mirrorPadRenderer->AddActor(mirrorPadActor);
  mirrorPadRenderer->SetActiveCamera(constantPadRenderer->GetActiveCamera());
  mirrorPadRenderer->SetBackground(
      colors->GetColor3d("LightSlateGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("Pad");
  renderWindow->AddRenderer(constantPadRenderer);
  renderWindow->AddRenderer(mirrorPadRenderer);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  constantPadRenderer->GetActiveCamera()->Dolly(1.2);
  constantPadRenderer->ResetCameraClippingRange();
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
