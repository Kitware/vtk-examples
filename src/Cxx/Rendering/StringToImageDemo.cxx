#include <vtkCamera.h>
#include <vtkFreeTypeTools.h>
#include <vtkImageBlend.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageIterator.h>
#include <vtkImageViewer2.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStdString.h>
#include <vtkTextProperty.h>
#include <vtkMinimalStandardRandomSequence.h>

#include <array>
//#include <random>

int main(int argc, char* argv[])
{
  vtkStdString text;
  if (argc < 2)
  {
    text = "Old Guys Rule";
  }
  else
  {
    text = argv[1];
  }

  // Create a blank, black image
  vtkNew<vtkImageCanvasSource2D> drawing;
  drawing->SetScalarTypeToUnsignedChar();
  drawing->SetNumberOfScalarComponents(3);
  drawing->SetExtent(0, 640, 0, 480, 0, 0);
  drawing->SetDrawColor(0, 0, 0);
  drawing->FillBox(0, 629, 0, 479);

  // Create an image that will hold the final image
  vtkNew<vtkImageBlend> final;
  final->AddInputConnection(drawing->GetOutputPort());
  final->SetOpacity(0, 1.0);
  final->Update();

  // Create an image of the string
  int dpi = 150;
  vtkFreeTypeTools* freeType = vtkFreeTypeTools::GetInstance();
  freeType->ScaleToPowerTwoOff();

  // Setup a property for the strings containing fixed parameters
  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkTextProperty> textProperty;
  textProperty->SetColor(colors->GetColor3d("Tomato").GetData());
  textProperty->SetVerticalJustificationToCentered();
  textProperty->SetJustificationToCentered();

  // Distributions for random parameters
  //std::mt19937 mt(4355412); // Standard mersenne_twister_engine
  //std::uniform_int_distribution<> extentXDist(0, 640);
  //std::uniform_int_distribution<> extentYDist(0, 480);
  //std::uniform_int_distribution<> fontDist(6, 42);
  //std::uniform_real_distribution<double> orientationDist(-90, 90);

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(8775070);


  // For each string, create an image and see if it overlaps with other images,
  // if so, skip it
  int tried = 600;
  int accepted = 0;
  for (int i = 0; i < tried; ++i)
  {
    //int fontSize = fontDist(mt);
    int fontSize = static_cast<int>(randomSequence->GetRangeValue(6, 42));
    randomSequence->Next();
    //textProperty->SetOrientation(orientationDist(mt));
    textProperty->SetOrientation(randomSequence->GetRangeValue(-90, 90));
    randomSequence->Next();
    textProperty->SetFontSize(fontSize);

    vtkNew<vtkImageData> textImage;
    freeType->RenderString(textProperty, text, dpi, textImage.GetPointer());

    // Set the extent of the text image
    std::array<int, 4> bb;
    freeType->GetBoundingBox(textProperty, text, dpi, bb.data());
    //int offsetX = extentXDist(mt);
    //int offsetY = extentYDist(mt);
    int offsetX = static_cast<int>(randomSequence->GetRangeValue(0, 640));
    randomSequence->Next();
    int offsetY = static_cast<int>(randomSequence->GetRangeValue(0, 480));
    randomSequence->Next();
    // Make sure the text image will fit on the final image
    if (offsetX + bb[1] - bb[0] < 639 && offsetY + bb[3] - bb[2] < 479)
    {
      textImage->SetExtent(offsetX, offsetX + bb[1] - bb[0], offsetY,
                           offsetY + bb[3] - bb[2], 0, 0);
      vtkNew<vtkImageData> image;
      final->Update();

      // Does the text image overlap with images on the final image
      vtkImageIterator<unsigned char> finalIt(final->GetOutput(),
                                              textImage->GetExtent());
      unsigned char R;
      bool good = true;
      while (!finalIt.IsAtEnd())
      {
        auto finalSpan = finalIt.BeginSpan();
        while (finalSpan != finalIt.EndSpan())
        {
          R = *finalSpan++;
          finalSpan++;
          finalSpan++;
          if (R != 0)
          {
            good = false;
            break;
          }
        }
        if (!good)
        {
          break;
        }
        finalIt.NextSpan();
      }
      if (good)
      {
        accepted++;
        image->DeepCopy(textImage);
        final->AddInputData(image);
        final->SetOpacity(i + 1, 1.0); // text: 100% opaque
        final->Update();
      }
    }
  }
  std::cout << "Tried " << tried << ", but only accepted " << accepted
            << std::endl;
  // Display the result
  vtkNew<vtkRenderWindowInteractor> interactor;

  vtkNew<vtkImageViewer2> imageViewer;
  imageViewer->SetInputData(final->GetOutput());
  imageViewer->SetSize(640, 512);
  imageViewer->SetupInteractor(interactor);
  imageViewer->GetRenderer()->SetBackground(
      colors->GetColor3d("Wheat").GetData());
  imageViewer->GetRenderer()->ResetCamera();
  imageViewer->GetRenderWindow()->SetWindowName("StringToImageDemo");

  auto camera = imageViewer->GetRenderer()->GetActiveCamera();
  camera->ParallelProjectionOn();
  camera->SetParallelScale(640 * 0.4);
  imageViewer->GetRenderWindow()->Render();
  interactor->Initialize();
  interactor->Start();

  return EXIT_SUCCESS;
}
