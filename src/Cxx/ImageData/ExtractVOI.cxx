#include <vtkExtractVOI.h>
#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
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

  int* inputDims = source->GetOutput()->GetDimensions();
  std::cout << "Dims: "
            << " x: " << inputDims[0] << " y: " << inputDims[1]
            << " z: " << inputDims[2] << std::endl;
  std::cout << "Number of points: " << source->GetOutput()->GetNumberOfPoints()
            << std::endl;
  std::cout << "Number of cells: " << source->GetOutput()->GetNumberOfCells()
            << std::endl;

  vtkNew<vtkExtractVOI> extractVOI;
  extractVOI->SetInputConnection(source->GetOutputPort());
  extractVOI->SetVOI(inputDims[0] / 4., 3. * inputDims[0] / 4.,
                     inputDims[1] / 4., 3. * inputDims[1] / 4., 0, 0);
  extractVOI->Update();

  vtkImageData* extracted = extractVOI->GetOutput();

  int* extractedDims = extracted->GetDimensions();
  std::cout << "Dims: "
            << " x: " << extractedDims[0] << " y: " << extractedDims[1]
            << " z: " << extractedDims[2] << std::endl;
  std::cout << "Number of points: " << extracted->GetNumberOfPoints()
            << std::endl;
  std::cout << "Number of cells: " << extracted->GetNumberOfCells()
            << std::endl;

  vtkNew<vtkImageCast> inputCastFilter;
  inputCastFilter->SetInputConnection(source->GetOutputPort());
  inputCastFilter->SetOutputScalarTypeToUnsignedChar();
  inputCastFilter->Update();

  vtkNew<vtkImageCast> extractedCastFilter;
  extractedCastFilter->SetInputData(extracted);
  extractedCastFilter->SetOutputScalarTypeToUnsignedChar();
  extractedCastFilter->Update();

  // Create actors
  vtkNew<vtkImageActor> inputActor;
  inputActor->GetMapper()->SetInputConnection(inputCastFilter->GetOutputPort());

  vtkNew<vtkImageActor> extractedActor;
  extractedActor->GetMapper()->SetInputConnection(
      extractedCastFilter->GetOutputPort());

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);

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
  leftRenderer->SetBackground(colors->GetColor3d("Burlywood").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("DarkTurquoise").GetData());

  leftRenderer->AddActor(inputActor);
  rightRenderer->AddActor(extractedActor);

  leftRenderer->ResetCamera();

  rightRenderer->ResetCamera();
  renderWindow->SetWindowName("ExtractVOI");
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
