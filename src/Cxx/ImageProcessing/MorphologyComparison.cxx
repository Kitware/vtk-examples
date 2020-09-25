#include <vtkCamera.h>
#include <vtkDataArray.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageDilateErode3D.h>
#include <vtkImageMapper3D.h>
#include <vtkImageProperty.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageSeedConnectivity.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <vector>

int main(int argc, char* argv[])
{
  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename e.g. binary.pgm"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Read the image
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> reader;
  reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  reader->SetFileName(argv[1]);

  // Dilate
  vtkNew<vtkImageDilateErode3D> dilate;
  dilate->SetInputConnection(reader->GetOutputPort());
  dilate->SetDilateValue(0);
  dilate->SetErodeValue(255);
  dilate->SetKernelSize(31, 31, 1);

  // Erode
  vtkNew<vtkImageDilateErode3D> erode;
  erode->SetInputConnection(reader->GetOutputPort());
  erode->SetDilateValue(255);
  erode->SetErodeValue(0);
  erode->SetKernelSize(31, 31, 1);

  // Opening - dilate then erode
  vtkNew<vtkImageDilateErode3D> dilate1;
  dilate1->SetInputConnection(reader->GetOutputPort());
  dilate1->SetDilateValue(0);
  dilate1->SetErodeValue(255);
  dilate1->SetKernelSize(31, 31, 1);

  vtkNew<vtkImageDilateErode3D> erode1;
  erode1->SetInputConnection(dilate1->GetOutputPort());
  erode1->SetDilateValue(255);
  erode1->SetErodeValue(0);
  erode1->SetKernelSize(31, 31, 1);

  // Closing - erode then dilate
  vtkNew<vtkImageDilateErode3D> erode2;
  erode2->SetInputConnection(reader->GetOutputPort());
  erode2->SetDilateValue(255);
  erode2->SetErodeValue(0);
  erode2->SetKernelSize(31, 31, 1);

  vtkNew<vtkImageDilateErode3D> dilate2;
  dilate2->SetInputConnection(erode2->GetOutputPort());
  dilate2->SetDilateValue(0);
  dilate2->SetErodeValue(255);
  dilate2->SetKernelSize(31, 31, 1);

  // Connectivity
  vtkNew<vtkImageSeedConnectivity> con;
  con->SetInputConnection(reader->GetOutputPort());
  con->AddSeed(300, 200);
  con->SetInputConnectValue(0);
  con->SetOutputConnectedValue(0);
  con->SetOutputUnconnectedValue(255);

  // Actors
  vtkNew<vtkImageActor> originalActor;
  originalActor->GetMapper()->SetInputConnection(reader->GetOutputPort());
  originalActor->GetProperty()->SetInterpolationTypeToNearest();

  vtkNew<vtkImageActor> connectedActor;
  connectedActor->GetMapper()->SetInputConnection(con->GetOutputPort());
  connectedActor->GetProperty()->SetInterpolationTypeToNearest();

  vtkNew<vtkImageActor> erodeActor;
  erodeActor->GetMapper()->SetInputConnection(erode->GetOutputPort());
  erodeActor->GetProperty()->SetInterpolationTypeToNearest();

  vtkNew<vtkImageActor> dilateActor;
  dilateActor->GetMapper()->SetInputConnection(dilate->GetOutputPort());
  dilateActor->GetProperty()->SetInterpolationTypeToNearest();

  vtkNew<vtkImageActor> openingActor;
  openingActor->GetMapper()->SetInputConnection(dilate2->GetOutputPort());
  openingActor->GetProperty()->SetInterpolationTypeToNearest();

  vtkNew<vtkImageActor> closingActor;
  closingActor->GetMapper()->SetInputConnection(erode1->GetOutputPort());
  closingActor->GetProperty()->SetInterpolationTypeToNearest();

  // Setup renderers
  vtkNew<vtkRenderer> originalRenderer;
  originalRenderer->AddActor(originalActor);
  vtkNew<vtkRenderer> connectedRenderer;
  connectedRenderer->AddActor(connectedActor);
  vtkNew<vtkRenderer> dilateRenderer;
  dilateRenderer->AddActor(dilateActor);
  vtkNew<vtkRenderer> erodeRenderer;
  erodeRenderer->AddActor(erodeActor);
  vtkNew<vtkRenderer> closingRenderer;
  closingRenderer->AddActor(closingActor);
  vtkNew<vtkRenderer> openingRenderer;
  openingRenderer->AddActor(openingActor);

  std::vector<vtkSmartPointer<vtkRenderer>> renderers;
  renderers.push_back(originalRenderer);
  renderers.push_back(connectedRenderer);
  renderers.push_back(erodeRenderer);
  renderers.push_back(dilateRenderer);
  renderers.push_back(openingRenderer);
  renderers.push_back(closingRenderer);

  // Setup viewports for the renderers
  int rendererSize = 300;
  unsigned int xGridDimensions = 2;
  unsigned int yGridDimensions = 3;

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
  renderWindow->SetWindowName("MorphologyComparison");

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
