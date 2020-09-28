#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageMapper3D.h>
#include <vtkImageRange3D.h>
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

  // Parse input arguments
  if (argc != 2)
  {
    std::cout << "Required parameters: Filename.jpg e.g. Gourds2.jpg"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);

  // Create actors
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkImageRange3D> rangeFilter;
  rangeFilter->SetInputConnection(reader->GetOutputPort());
  rangeFilter->SetKernelSize(5, 5, 5);
  rangeFilter->Update();

  vtkNew<vtkImageCast> rangeCastFilter;
  rangeCastFilter->SetInputConnection(rangeFilter->GetOutputPort());
  rangeCastFilter->SetOutputScalarTypeToUnsignedChar();
  rangeCastFilter->Update();

  vtkNew<vtkImageActor> rangeActor;
  rangeActor->GetMapper()->SetInputConnection(rangeCastFilter->GetOutputPort());

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rangeViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->AddActor(originalActor);
  originalRenderer->ResetCamera();
  originalRenderer->SetBackground(
      colors->GetColor3d("CornflowerBlue").GetData());

  vtkNew<vtkRenderer> rangeRenderer;
  rangeRenderer->SetViewport(rangeViewport);
  rangeRenderer->AddActor(rangeActor);
  rangeRenderer->ResetCamera();
  rangeRenderer->SetBackground(
      colors->GetColor3d("SteelBlue").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->AddRenderer(originalRenderer);
  renderWindow->AddRenderer(rangeRenderer);
  renderWindow->SetWindowName("ImageRange3D");


  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
