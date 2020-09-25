#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMathematics.h>
#include <vtkImageProperty.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageShiftScale.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSphere.h>

int main(int argc, char* argv[])
{

  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename e.g. AttenuationArtifact.pg"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);

  vtkNew<vtkImageCast> cast;
  cast->SetInputConnection(reader->GetOutputPort());
  cast->SetOutputScalarTypeToDouble();

  // get rid of discrete scalars);
  vtkNew<vtkImageGaussianSmooth> smooth;
  smooth->SetInputConnection(cast->GetOutputPort());
  smooth->SetStandardDeviations(0.8, 0.8, 0);

  vtkNew<vtkSphere> m1;
  m1->SetCenter(310, 130, 0);
  m1->SetRadius(0);

  vtkNew<vtkSampleFunction> m2;
  m2->SetImplicitFunction(m1);
  m2->SetModelBounds(0, 264, 0, 264, 0, 1);
  m2->SetSampleDimensions(264, 264, 1);

  vtkNew<vtkImageShiftScale> m3;
  m3->SetInputConnection(m2->GetOutputPort());
  m3->SetScale(0.000095);

  vtkNew<vtkImageMathematics> div;
  div->SetInputConnection(0, smooth->GetOutputPort());
  div->SetInputConnection(1, m3->GetOutputPort());
  div->SetOperationToMultiply();

  // Create actors
  vtkNew<vtkNamedColors> colors;

  double colorWindow = 256.0;
  double colorLevel = 127.5;
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(cast->GetOutputPort());
  originalActor->GetProperty()->SetColorWindow(colorWindow);
  originalActor->GetProperty()->SetColorLevel(colorLevel);

  vtkNew<vtkImageActor> filteredActor;
  filteredActor->GetMapper()->SetInputConnection(div->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double filteredViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkRenderer> filteredRenderer;
  filteredRenderer->SetViewport(filteredViewport);
  filteredRenderer->AddActor(filteredActor);
  filteredRenderer->ResetCamera();
  filteredRenderer->SetBackground(
      colors->GetColor3d("LightSlateGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(filteredRenderer);
  renderWindow->SetWindowName("Attenuation");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
