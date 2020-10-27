#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkHedgeHog.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredPointsReader.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " carotid.vtk" << std::endl;
    return EXIT_SUCCESS;
  }

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkStructuredPointsReader> reader;
  reader->SetFileName(argv[1]);

  vtkNew<vtkHedgeHog> hhog;
  hhog->SetInputConnection(reader->GetOutputPort());
  hhog->SetScaleFactor(0.3);

  vtkNew<vtkLookupTable> lut;
  //  lut->SetHueRange(.667, 0.0);
  lut->Build();

  vtkNew<vtkPolyDataMapper> hhogMapper;
  hhogMapper->SetInputConnection(hhog->GetOutputPort());
  hhogMapper->SetScalarRange(50, 550);
  hhogMapper->SetLookupTable(lut);

  vtkNew<vtkActor> hhogActor;
  hhogActor->SetMapper(hhogMapper);

  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(
      colors->GetColor3d("Black").GetData());

  vtkNew<vtkRenderer> aRenderer;
  vtkNew<vtkRenderWindow> aRenderWindow;
  aRenderWindow->AddRenderer(aRenderer);

  vtkNew<vtkRenderWindowInteractor> anInteractor;
  anInteractor->SetRenderWindow(aRenderWindow);
  aRenderWindow->SetSize(640, 480);
  aRenderWindow->SetWindowName("ComplexV");

  aRenderer->AddActor(outlineActor);
  aRenderer->AddActor(hhogActor);

  aRenderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Generate an interesting view

  aRenderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
  aRenderer->GetActiveCamera()->SetPosition(1, 0, 0);
  aRenderer->GetActiveCamera()->SetViewUp(0, 0, 1);
  aRenderer->ResetCamera();

  aRenderer->GetActiveCamera()->Azimuth(60);
  aRenderer->GetActiveCamera()->Elevation(30);
  aRenderer->GetActiveCamera()->Dolly(1.1);
  aRenderer->ResetCameraClippingRange();

  aRenderWindow->Render();

  // interact with data
  anInteractor->Start();

  return EXIT_SUCCESS;
}
