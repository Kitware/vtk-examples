#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3DMapper.h>
#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageDotProduct.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMathematics.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkXMLImageDataWriter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create an image
  vtkNew<vtkImageData> image1;
  image1->SetExtent(0, 1, 0, 1, 0, 0);
  image1->AllocateScalars(VTK_FLOAT, 3);

  int coord[3];
  float* pixel;

  // Fill the image with vectors
  coord[0] = 0;
  coord[1] = 0;
  coord[2] = 0;
  pixel = static_cast<float*>(image1->GetScalarPointer(coord));
  pixel[0] = 1.0;
  pixel[1] = 0.0;
  pixel[2] = 0.0;

  coord[0] = 0;
  coord[1] = 1;
  coord[2] = 0;
  pixel = static_cast<float*>(image1->GetScalarPointer(coord));
  pixel[0] = 0.0;
  pixel[1] = 1.0;
  pixel[2] = 0.0;

  coord[0] = 1;
  coord[1] = 0;
  coord[2] = 0;
  pixel = static_cast<float*>(image1->GetScalarPointer(coord));
  pixel[0] = 1.0;
  pixel[1] = 0.0;
  pixel[2] = 0.0;

  coord[0] = 1;
  coord[1] = 1;
  coord[2] = 0;
  pixel = static_cast<float*>(image1->GetScalarPointer(coord));
  pixel[0] = 0.0;
  pixel[1] = 1.0;
  pixel[2] = 0.0;

  // vtkNew<vtkXMLImageDataWriter> writer;
  // writer->SetInputData(image1);
  // writer->SetFileName("test.vti");
  // writer->Write();

  // Create another image
  vtkNew<vtkImageData> image2;
  image2->SetExtent(0, 1, 0, 1, 0, 0);
  image2->AllocateScalars(VTK_FLOAT, 3);

  // Fill the image with vectors
  coord[0] = 0;
  coord[1] = 0;
  coord[2] = 0;
  pixel = static_cast<float*>(image2->GetScalarPointer(coord));
  pixel[0] = 1.0;
  pixel[1] = 0.0;
  pixel[2] = 0.0;

  coord[0] = 0;
  coord[1] = 1;
  coord[2] = 0;
  pixel = static_cast<float*>(image2->GetScalarPointer(coord));
  pixel[0] = 1.0;
  pixel[1] = 0.0;
  pixel[2] = 0.0;

  coord[0] = 1;
  coord[1] = 0;
  coord[2] = 0;
  pixel = static_cast<float*>(image2->GetScalarPointer(coord));
  pixel[0] = 0.5;
  pixel[1] = 0.0;
  pixel[2] = 0.0;

  coord[0] = 1;
  coord[1] = 1;
  coord[2] = 0;
  pixel = static_cast<float*>(image2->GetScalarPointer(coord));
  pixel[0] = 0.5;
  pixel[1] = 0.0;
  pixel[2] = 0.0;

  // Compute the dot product of the images pixel wise
  vtkNew<vtkImageDotProduct> dotProductFilter;
  dotProductFilter->SetInput1Data(image1);
  dotProductFilter->SetInput2Data(image2);
  dotProductFilter->Update();
  std::cout << "Output is of type: "
            << dotProductFilter->GetOutput()->GetScalarTypeAsString()
            << std::endl;

  vtkNew<vtkImageMathematics> imageMath;
  imageMath->SetOperationToMultiplyByK();
  imageMath->SetConstantK(255.0);
  imageMath->SetInputConnection(dotProductFilter->GetOutputPort());
  imageMath->Update();

  vtkNew<vtkImageCast> imageCast;
  imageCast->SetOutputScalarTypeToUnsignedChar();
  imageCast->SetInputConnection(imageMath->GetOutputPort());
  imageCast->Update();

  vtkNew<vtkImageActor> dotProductActor;
  dotProductActor->GetMapper()->SetInputConnection(imageCast->GetOutputPort());

  // Display output to the terminal
  for (vtkIdType i = 0; i < 2; i++)
  {
    for (vtkIdType j = 0; j < 2; j++)
    {
      coord[0] = i;
      coord[1] = j;
      coord[2] = 0;
      pixel = static_cast<float*>(
          dotProductFilter->GetOutput()->GetScalarPointer(coord));
      std::cout << "Pixel (" << i << ", " << j << ") : " << pixel[0]
                << std::endl;
    }
  }

  image1->GetPointData()->SetActiveVectors("ImageScalars");
  image2->GetPointData()->SetActiveVectors("ImageScalars");

  vtkNew<vtkArrowSource> arrowSource;

  vtkNew<vtkGlyph3DMapper> glyph3Dmapper1;
  glyph3Dmapper1->SetSourceConnection(arrowSource->GetOutputPort());
  glyph3Dmapper1->SetInputData(image1);
  glyph3Dmapper1->Update();

  vtkNew<vtkActor> actor1;
  actor1->SetMapper(glyph3Dmapper1);

  vtkNew<vtkGlyph3DMapper> glyph3Dmapper2;
  glyph3Dmapper2->SetSourceConnection(arrowSource->GetOutputPort());
  glyph3Dmapper2->SetInputData(image2);
  glyph3Dmapper2->Update();

  vtkNew<vtkActor> actor2;
  actor2->SetMapper(glyph3Dmapper2);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.33, 1.0};
  double centerViewport[4] = {0.33, 0.0, 0.66, 1.0};
  double rightViewport[4] = {0.66, 0.0, 1.0, 1.0};

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> leftRenderer;
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("DimGray").GetData());

  vtkNew<vtkRenderer> centerRenderer;
  centerRenderer->SetViewport(centerViewport);
  centerRenderer->SetBackground(colors->GetColor3d("DimGray").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("DimGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(leftRenderer);
  renderWindow->AddRenderer(centerRenderer);
  renderWindow->AddRenderer(rightRenderer);
  renderWindow->SetWindowName("DotProduct");
  renderWindow->SetSize(600, 200);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actor to the scene
  leftRenderer->AddActor(actor1);
  centerRenderer->AddActor(actor2);
  rightRenderer->AddActor(dotProductActor);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
