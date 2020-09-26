#include <vtkActor.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkImageBlend.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Parse input arguments
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0]
              << " Input1Filename Input2Filename e.g. Ox.jpg Gourds2.jpg"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkLookupTable> imgFirstColorMap;
  imgFirstColorMap->SetRange(0.0, 255.0);
  imgFirstColorMap->SetHueRange(0.0, 0.1);
  imgFirstColorMap->SetValueRange(0.4, 0.8);
  imgFirstColorMap->Build();

  vtkNew<vtkLookupTable> imgSecondColorMap;
  imgSecondColorMap->SetRange(0.0, 255.0);
  imgSecondColorMap->SetHueRange(0.67, 0.68);
  imgSecondColorMap->SetValueRange(0.4, 0.8);
  imgSecondColorMap->Build();

  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> imgReader;
  imgReader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
  imgReader->SetFileName(argv[1]);

  vtkSmartPointer<vtkImageReader2> imgReaderMoving;
  imgReaderMoving.TakeReference(readerFactory->CreateImageReader2(argv[2]));
  imgReaderMoving->SetFileName(argv[2]);

  vtkNew<vtkImageMapToColors> firstColorMapper;
  firstColorMapper->SetInputConnection(imgReader->GetOutputPort());
  firstColorMapper->SetLookupTable(imgFirstColorMap);

  vtkNew<vtkImageMapToColors> secondColorMapper;
  secondColorMapper->SetInputConnection(imgReaderMoving->GetOutputPort());
  secondColorMapper->SetLookupTable(imgSecondColorMap);

  vtkNew<vtkImageBlend> imgBlender;
  imgBlender->SetOpacity(0, 0.5);
  imgBlender->SetOpacity(1, 0.5);
  imgBlender->AddInputConnection(firstColorMapper->GetOutputPort());
  imgBlender->AddInputConnection(secondColorMapper->GetOutputPort());

  vtkNew<vtkDataSetMapper> imgDataSetMapper;
  imgDataSetMapper->SetInputConnection(imgBlender->GetOutputPort());

  vtkNew<vtkActor> imgActor;
  imgActor->SetMapper(imgDataSetMapper);

  vtkNew<vtkRenderer> imgRenderer;
  imgRenderer->AddActor(imgActor);
  imgRenderer->SetBackground(colors->GetColor3d("BurlyWood").GetData());

  vtkNew<vtkRenderWindow> imgRenderWindow;
  imgRenderWindow->AddRenderer(imgRenderer);
  imgRenderWindow->SetWindowName("Colored2DImageFusion");

  vtkNew<vtkRenderWindowInteractor> imgInteractor;
  imgInteractor->SetRenderWindow(imgRenderWindow);
  imgRenderWindow->Render();
  imgInteractor->Initialize();
  imgInteractor->Start();

  return EXIT_SUCCESS;
}
