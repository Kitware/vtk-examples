#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkLookupTable.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>

// This example demonstrates the use and manipulation of lookup tables.

// First create a simple pipeline that reads a structured grid
// and then extracts a plane from the grid. The plane will be colored
// differently by using different lookup tables.
//
// Note: The Update method is manually invoked because it causes the
// reader to read; later on we use the output of the reader to set
// a range for the scalar values.
int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0]
              << " filexyz.bin fileq.bin e.g.: combxyz.bin combq.bin"
              << std::endl;
    return EXIT_FAILURE;
  }
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkMultiBlockPLOT3DReader> pl3d;
  pl3d->SetXYZFileName(argv[1]);
  pl3d->SetQFileName(argv[2]);
  pl3d->SetScalarFunctionNumber(100);
  pl3d->SetVectorFunctionNumber(202);
  pl3d->Update();

  vtkStructuredGrid* pl3dOutput =
      dynamic_cast<vtkStructuredGrid*>(pl3d->GetOutput()->GetBlock(0));

  std::vector<vtkSmartPointer<vtkRenderer>> renderers;

  // Create the 4 renderers
  vtkNew<vtkRenderer> grayScaleRenderer;
  renderers.push_back(grayScaleRenderer);
  vtkNew<vtkRenderer> rainbowBlueRedRenderer;
  renderers.push_back(rainbowBlueRedRenderer);
  vtkNew<vtkRenderer> rainbowRedBlueRenderer;
  renderers.push_back(rainbowRedBlueRenderer);
  vtkNew<vtkRenderer> largeContrastRenderer;
  renderers.push_back(largeContrastRenderer);

  std::vector<vtkSmartPointer<vtkLookupTable>> luts;

  vtkNew<vtkLookupTable> grayScaleLut;
  grayScaleLut->SetHueRange(0, 0);
  grayScaleLut->SetSaturationRange(0, 0);
  grayScaleLut->SetValueRange(0.2, 1.0);
  grayScaleLut->SetNumberOfColors(256);
  grayScaleLut->SetHueRange(0.0, 0.667);
  grayScaleLut->Build();
  luts.push_back(grayScaleLut);

  vtkNew<vtkLookupTable> rainbowBlueRedLut;
  rainbowBlueRedLut->SetNumberOfColors(256);
  rainbowBlueRedLut->SetHueRange(0.667, 0.0);
  rainbowBlueRedLut->Build();
  luts.push_back(rainbowBlueRedLut);

  vtkNew<vtkLookupTable> rainbowRedBlueLut;
  rainbowRedBlueLut->SetNumberOfColors(256);
  rainbowRedBlueLut->SetHueRange(0.0, 0.667);
  rainbowRedBlueLut->Build();
  luts.push_back(rainbowRedBlueLut);

  vtkNew<vtkLookupTable> highContrastLut;
  highContrastLut->SetNumberOfColors(256);
  highContrastLut->Build();

  for (int l = 0; l < 16; ++l)
  {
    highContrastLut->SetTableValue(l * 16, 1, 0, 0, 1);
    highContrastLut->SetTableValue(l * 16 + 1, 0, 1, 0, 1);
    highContrastLut->SetTableValue(l * 16 + 2, 0, 0, 1, 1);
    highContrastLut->SetTableValue(l * 16 + 3, 0, 0, 0, 1);
  }
  luts.push_back(highContrastLut);

  vtkNew<vtkRenderWindow> renderWindow;
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  int rendererSize = 256;
  unsigned int xGridDimensions = 2;
  unsigned int yGridDimensions = 2;
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
    }
  }

  vtkNew<vtkStructuredGridGeometryFilter> plane;
  plane->SetInputData(pl3dOutput);
  plane->SetExtent(1, 100, 1, 100, 7, 7);

  for (size_t r = 0; r < renderers.size(); ++r)
  {
    vtkSmartPointer<vtkLookupTable> lut = luts[r];
    vtkNew<vtkPolyDataMapper> planeMapper;
    planeMapper->SetLookupTable(lut);
    planeMapper->SetInputConnection(plane->GetOutputPort());
    planeMapper->SetScalarRange(pl3dOutput->GetScalarRange());

    vtkNew<vtkActor> planeActor;
    planeActor->SetMapper(planeMapper);

    // This creates an outline around the data.
    vtkNew<vtkStructuredGridOutlineFilter> outline;
    outline->SetInputData(pl3dOutput);

    vtkNew<vtkPolyDataMapper> outlineMapper;
    outlineMapper->SetInputConnection(outline->GetOutputPort());

    vtkNew<vtkActor> outlineActor;
    outlineActor->SetMapper(outlineMapper);

    // Create the RenderWindow, Renderer and both Actors
    //
    renderers[r]->AddActor(outlineActor);
    renderers[r]->AddActor(planeActor);

    renderers[r]->SetBackground(colors->GetColor3d("SlateGray").GetData());
    renderers[r]->TwoSidedLightingOff();
    renderers[r]->GetActiveCamera()->Azimuth(20);
    renderers[r]->GetActiveCamera()->Elevation(30);
    renderers[r]->ResetCamera();
    if (r > 0)
    {
      renderers[r]->SetActiveCamera(renderers[0]->GetActiveCamera());
    }
    renderWindow->AddRenderer(renderers[r]);
  }

  // vtkNew<vtkLookupTable> lut;

  // Add the actors to the renderer, set the background and size
  //

  renderWindow->SetSize(rendererSize * xGridDimensions,
                        rendererSize * yGridDimensions);
  renderWindow->SetWindowName("Rainbow");

  interactor->Initialize();

  vtkCamera* cam1 = renderers[0]->GetActiveCamera();
  cam1->SetClippingRange(3.95297, 50);
  cam1->SetFocalPoint(8.88908, 0.595038, 29.3342);
  cam1->SetPosition(-12.3332, 31.7479, 41.2387);
  cam1->SetViewUp(0.060772, -0.319905, 0.945498);

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
