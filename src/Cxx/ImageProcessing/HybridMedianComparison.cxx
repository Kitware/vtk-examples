#include <vtkCamera.h>
#include <vtkDataArray.h>
#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageHybridMedian2D.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMathematics.h>
#include <vtkImageMedian3D.h>
#include <vtkImageNoiseSource.h>
#include <vtkImageProperty.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageThreshold.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <vector>

namespace {
void AddShotNoise(vtkImageData* inputImage, vtkImageData* outputImage,
                  double noiseAmplitude, double noiseFraction, int extent[6]);
}

int main(int argc, char* argv[])
{
  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename e.g. TestPattern.png"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);
  reader->Update();

  int scalarRange[2];
  scalarRange[0] =
      reader->GetOutput()->GetPointData()->GetScalars()->GetRange()[0];
  scalarRange[1] =
      reader->GetOutput()->GetPointData()->GetScalars()->GetRange()[1];
  std::cout << "Range: " << scalarRange[0] << ", " << scalarRange[1]
            << std::endl;
  int middleSlice = (reader->GetOutput()->GetExtent()[5] -
                     reader->GetOutput()->GetExtent()[4]) /
      2;

  // Work with double images
  vtkNew<vtkImageCast> cast;
  cast->SetInputConnection(reader->GetOutputPort());
  cast->SetOutputScalarTypeToDouble();
  cast->Update();

  vtkNew<vtkImageData> originalData;
  originalData->DeepCopy(cast->GetOutput());

  vtkNew<vtkImageData> noisyData;

  AddShotNoise(originalData, noisyData, 2000.0, .1,
               reader->GetOutput()->GetExtent());
  vtkNew<vtkImageMedian3D> median;
  median->SetInputData(noisyData);
  median->SetKernelSize(5, 5, 1);

  vtkNew<vtkImageHybridMedian2D> hybridMedian1;
  hybridMedian1->SetInputData(noisyData);
  vtkNew<vtkImageHybridMedian2D> hybridMedian;
  hybridMedian->SetInputConnection(hybridMedian1->GetOutputPort());

  int colorWindow = (scalarRange[1] - scalarRange[0]);
  int colorLevel = colorWindow / 2;
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputData(originalData);
  originalActor->GetProperty()->SetColorWindow(colorWindow);
  originalActor->GetProperty()->SetColorLevel(colorLevel);
  originalActor->GetProperty()->SetInterpolationTypeToNearest();
  originalActor->SetDisplayExtent(
      reader->GetDataExtent()[0], reader->GetDataExtent()[1],
      reader->GetDataExtent()[2], reader->GetDataExtent()[3], middleSlice,
      middleSlice);

  vtkNew<vtkImageActor> noisyActor;
  noisyActor->GetMapper()->SetInputData(noisyData);
  noisyActor->GetProperty()->SetColorWindow(colorWindow);
  noisyActor->GetProperty()->SetColorLevel(colorLevel);
  noisyActor->GetProperty()->SetInterpolationTypeToNearest();
  noisyActor->SetDisplayExtent(originalActor->GetDisplayExtent());

  vtkNew<vtkImageActor> hybridMedianActor;
  hybridMedianActor->GetMapper()->SetInputConnection(
      hybridMedian->GetOutputPort());
  hybridMedianActor->GetProperty()->SetColorWindow(colorWindow);
  hybridMedianActor->GetProperty()->SetColorLevel(colorLevel);
  hybridMedianActor->GetProperty()->SetInterpolationTypeToNearest();
  hybridMedianActor->SetDisplayExtent(originalActor->GetDisplayExtent());

  vtkNew<vtkImageActor> medianActor;
  medianActor->GetMapper()->SetInputConnection(median->GetOutputPort());
  medianActor->GetProperty()->SetColorWindow(colorWindow);
  medianActor->GetProperty()->SetColorLevel(colorLevel);
  medianActor->GetProperty()->SetInterpolationTypeToNearest();
  medianActor->SetDisplayExtent(originalActor->GetDisplayExtent());

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->AddActor(originalActor);
  vtkNew<vtkRenderer> noisyRenderer;
  noisyRenderer->AddActor(noisyActor);
  vtkNew<vtkRenderer> hybridRenderer;
  hybridRenderer->AddActor(hybridMedianActor);
  vtkNew<vtkRenderer> medianRenderer;
  medianRenderer->AddActor(medianActor);

  std::vector<vtkSmartPointer<vtkRenderer>> renderers;
  renderers.push_back(originalRenderer);
  renderers.push_back(noisyRenderer);
  renderers.push_back(hybridRenderer);
  renderers.push_back(medianRenderer);

  // Setup viewports for the renderers
  int rendererSize = 400;
  unsigned int xGridDimensions = 2;
  unsigned int yGridDimensions = 2;

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(rendererSize * xGridDimensions,
                        rendererSize * yGridDimensions);
  for (int row = 0; row < static_cast<int>(yGridDimensions); row++)
  {
    for (int col = 0; col < static_cast<int>(xGridDimensions); col++)
    {
      int index = row * xGridDimensions + col;
      // (xmin, ymin, xmax, ymax)
      double viewport[4] = {
          static_cast<double>(col) / xGridDimensions,
          static_cast<double>(yGridDimensions - (row + 1)) / yGridDimensions,
          static_cast<double>(col + 1) / xGridDimensions,
          static_cast<double>(yGridDimensions - row) / yGridDimensions};
      renderers[index]->SetViewport(viewport);
      renderWindow->AddRenderer(renderers[index]);
    }
  }
  renderWindow->SetWindowName("HybridMedianComparison");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Renderers share one camera
  renderWindow->Render();
  renderers[0]->GetActiveCamera()->Dolly(1.5);
  renderers[0]->ResetCameraClippingRange();

  for (size_t r = 1; r < renderers.size(); ++r)
  {
    renderers[r]->SetActiveCamera(renderers[0]->GetActiveCamera());
  }
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
namespace {
void AddShotNoise(vtkImageData* inputImage, vtkImageData* outputImage,
                  double noiseAmplitude, double noiseFraction, int extent[6])
{
  vtkNew<vtkImageNoiseSource> shotNoiseSource;
  shotNoiseSource->SetWholeExtent(extent);
  shotNoiseSource->SetMinimum(0.0);
  shotNoiseSource->SetMaximum(1.0);

  vtkNew<vtkImageThreshold> shotNoiseThresh1;
  shotNoiseThresh1->SetInputConnection(shotNoiseSource->GetOutputPort());
  shotNoiseThresh1->ThresholdByLower(1.0 - noiseFraction);
  shotNoiseThresh1->SetInValue(0);
  shotNoiseThresh1->SetOutValue(noiseAmplitude);

  vtkNew<vtkImageThreshold> shotNoiseThresh2;
  shotNoiseThresh2->SetInputConnection(shotNoiseSource->GetOutputPort());
  shotNoiseThresh2->ThresholdByLower(noiseFraction);
  shotNoiseThresh2->SetInValue(1.0 - noiseAmplitude);
  shotNoiseThresh2->SetOutValue(0.0);

  vtkNew<vtkImageMathematics> shotNoise;
  shotNoise->SetInputConnection(0, shotNoiseThresh1->GetOutputPort());
  shotNoise->SetInputConnection(1, shotNoiseThresh2->GetOutputPort());
  shotNoise->SetOperationToAdd();

  vtkNew<vtkImageMathematics> add;
  add->SetInputData(0, inputImage);
  add->SetInputConnection(1, shotNoise->GetOutputPort());
  add->SetOperationToAdd();
  add->Update();
  outputImage->DeepCopy(add->GetOutput());
}
} // namespace
