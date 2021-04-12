#include <vtkActor.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkImageData.h>
#include <vtkImageMathematics.h>
#include <vtkImageThreshold.h>
#include <vtkLookupTable.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSmartPointer.h>
#include <vtkSphere.h>
#include <vtkWindowedSincPolyDataFilter.h>

namespace {
vtkSmartPointer<vtkLookupTable> MakeColors(unsigned int n);

vtkSmartPointer<vtkImageData> MakeBlob(int n, double radius);
} // namespace

int main(int /* argc */, char* /* argv */[])
{
  int n = 20;
  double radius = 8;

  vtkSmartPointer<vtkImageData> blob = MakeBlob(n, radius);

  vtkNew<vtkDiscreteMarchingCubes> discrete;
  discrete->SetInputData(blob);
  discrete->GenerateValues(n, 1, n);

  unsigned int smoothingIterations = 15;
  double passBand = 0.001;
  double featureAngle = 120.0;

  vtkNew<vtkWindowedSincPolyDataFilter> smoother;
  smoother->SetInputConnection(discrete->GetOutputPort());
  smoother->SetNumberOfIterations(smoothingIterations);
  smoother->BoundarySmoothingOff();
  smoother->FeatureEdgeSmoothingOff();
  smoother->SetFeatureAngle(featureAngle);
  smoother->SetPassBand(passBand);
  smoother->NonManifoldSmoothingOn();
  smoother->NormalizeCoordinatesOn();
  smoother->Update();

  vtkSmartPointer<vtkLookupTable> lut = MakeColors(n);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(smoother->GetOutputPort());
  mapper->SetLookupTable(lut);
  mapper->SetScalarRange(0, lut->GetNumberOfColors());

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkRenderer> ren1;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->SetWindowName("SmoothDiscreteMarchingCubes");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  ren1->AddActor(actor);

  vtkNew<vtkNamedColors> colors;
  ren1->SetBackground(colors->GetColor3d("Burlywood").GetData());

  renWin->Render();

  iren->Start();

  return EXIT_SUCCESS;

  return EXIT_SUCCESS;
}

namespace {
vtkSmartPointer<vtkImageData> MakeBlob(int n, double radius)
{
  vtkNew<vtkImageData> blobImage;

  double maxR = 50 - 2.0 * radius;
  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(5071);
  for (int i = 0; i < n; ++i)
  {
    vtkNew<vtkSphere> sphere;
    sphere->SetRadius(radius);
    auto x = randomSequence->GetRangeValue(-maxR, maxR);
    randomSequence->Next();
    auto y = randomSequence->GetRangeValue(-maxR, maxR);
    randomSequence->Next();
    auto z = randomSequence->GetRangeValue(-maxR, maxR);
    randomSequence->Next();

    sphere->SetCenter(int(x), int(y), int(z));

    vtkNew<vtkSampleFunction> sampler;
    sampler->SetImplicitFunction(sphere);
    sampler->SetOutputScalarTypeToFloat();
    sampler->SetSampleDimensions(100, 100, 100);
    sampler->SetModelBounds(-50, 50, -50, 50, -50, 50);

    vtkNew<vtkImageThreshold> thres;
    thres->SetInputConnection(sampler->GetOutputPort());
    thres->ThresholdByLower(radius * radius);
    thres->ReplaceInOn();
    thres->ReplaceOutOn();
    thres->SetInValue(i + 1);
    thres->SetOutValue(0);
    thres->Update();
    if (i == 0)
    {
      blobImage->DeepCopy(thres->GetOutput());
    }

    vtkNew<vtkImageMathematics> maxValue;
    maxValue->SetInputData(0, blobImage);
    maxValue->SetInputData(1, thres->GetOutput());
    maxValue->SetOperationToMax();
    maxValue->Modified();
    maxValue->Update();

    blobImage->DeepCopy(maxValue->GetOutput());
  }
  return blobImage;
}

// Generate some random colors
vtkSmartPointer<vtkLookupTable> MakeColors(unsigned int n)
{
  vtkNew<vtkLookupTable> lut;
  lut->SetNumberOfColors(n);
  lut->SetTableRange(0, n - 1);
  lut->SetScaleToLinear();
  lut->Build();
  lut->SetTableValue(0, 0, 0, 0, 1);

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(5071);
  for (int i = 1; i < static_cast<int>(n); ++i)
  {
    auto r = randomSequence->GetRangeValue(0.4, 1);
    randomSequence->Next();
    auto g = randomSequence->GetRangeValue(0.4, 1);
    randomSequence->Next();
    auto b = randomSequence->GetRangeValue(0.4, 1);
    randomSequence->Next();
    lut->SetTableValue(i, r, g, b, 1.0);
  }
  return lut;
}
} // namespace
