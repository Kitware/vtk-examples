#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkLookupTable.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStreamTracer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkTubeFilter.h>

//// LOx post CFD case study

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " postxyz.bin postq.bin" << std::endl;
    return EXIT_FAILURE;
  }
  // read data
  //
  vtkNew<vtkMultiBlockPLOT3DReader> pl3d;
  pl3d->AutoDetectFormatOn();
  pl3d->SetXYZFileName(argv[1]);
  pl3d->SetQFileName(argv[2]);
  pl3d->SetScalarFunctionNumber(153);
  pl3d->SetVectorFunctionNumber(200);
  pl3d->Update();

  vtkStructuredGrid* sg =
      dynamic_cast<vtkStructuredGrid*>(pl3d->GetOutput()->GetBlock(0));

  vtkNew<vtkNamedColors> colors;

  // blue to red lut
  //
  vtkNew<vtkLookupTable> lut;
  lut->SetHueRange(0.667, 0.0);

  std::vector<double*> seeds;
  double seed1[3] = {-0.74, 0.0, 0.3};
  double seed2[3] = {-0.74, 0.0, 1.0};
  double seed3[3] = {-0.74, 0.0, 2.0};
  double seed4[3] = {-0.74, 0.0, 3.0};
  seeds.push_back(seed1);
  seeds.push_back(seed2);
  seeds.push_back(seed3);
  seeds.push_back(seed4);

  std::vector<vtkSmartPointer<vtkRenderer>> renderers;

  for (size_t s = 0; s < seeds.size(); ++s)
  {
    // computational planes
    vtkNew<vtkStructuredGridGeometryFilter> floorComp;
    floorComp->SetExtent(0, 37, 0, 75, 0, 0);
    floorComp->SetInputData(sg);
    floorComp->Update();

    vtkNew<vtkPolyDataMapper> floorMapper;
    floorMapper->SetInputConnection(floorComp->GetOutputPort());
    floorMapper->ScalarVisibilityOff();
    floorMapper->SetLookupTable(lut);

    vtkNew<vtkActor> floorActor;
    floorActor->SetMapper(floorMapper);
    floorActor->GetProperty()->SetRepresentationToWireframe();
    floorActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());
    floorActor->GetProperty()->SetLineWidth(2);

    vtkNew<vtkStructuredGridGeometryFilter> postComp;
    postComp->SetExtent(10, 10, 0, 75, 0, 37);
    postComp->SetInputData(sg);

    vtkNew<vtkPolyDataMapper> postMapper;
    postMapper->SetInputConnection(postComp->GetOutputPort());
    postMapper->SetLookupTable(lut);
    postMapper->SetScalarRange(sg->GetScalarRange());
    // postMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> postActor;
    postActor->SetMapper(postMapper);
    // postActor->GetProperty()->SetRepresentationToWireframe();
    postActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

    // streamers
    //
    // spherical seed points
    vtkNew<vtkPointSource> rake;
    rake->SetCenter(seeds[s]);
    rake->SetNumberOfPoints(10);

    vtkNew<vtkStreamTracer> streamers;
    streamers->SetInputConnection(pl3d->GetOutputPort());

    // streamers SetSource [rake GetOutput]
    streamers->SetSourceConnection(rake->GetOutputPort());
    streamers->SetMaximumPropagation(250);
    streamers->SetInitialIntegrationStep(.2);
    streamers->SetMinimumIntegrationStep(.01);
    streamers->SetIntegratorType(2);
    streamers->Update();

    vtkNew<vtkTubeFilter> tubes;
    tubes->SetInputConnection(streamers->GetOutputPort());
    tubes->SetNumberOfSides(8);
    tubes->SetRadius(0.08);
    tubes->SetVaryRadius(0);

    vtkNew<vtkPolyDataMapper> mapTubes;

    mapTubes->SetInputConnection(tubes->GetOutputPort());
    mapTubes->SetScalarRange(sg->GetScalarRange());

    vtkNew<vtkActor> tubesActor;
    tubesActor->SetMapper(mapTubes);

    vtkNew<vtkRenderer> renderer;

    renderer->AddActor(floorActor);
    renderer->AddActor(postActor);
    renderer->AddActor(tubesActor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    renderers.push_back(renderer);
  }

  vtkNew<vtkRenderWindow> renderWindow;

  // Setup viewports for the renderers
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

  vtkNew<vtkCamera> camera;
  camera->SetFocalPoint(0.918037, -0.0779233, 2.69513);
  camera->SetPosition(0.840735, -23.6176, 8.50211);
  camera->SetViewUp(0.00227904, 0.239501, 0.970893);
  camera->SetClippingRange(1, 100);

  renderers[0]->SetActiveCamera(camera);
  for (size_t r = 0; r < renderers.size(); ++r)
  {
    renderWindow->AddRenderer(renderers[r]);
    if (r > 0)
    {
      renderers[r]->SetActiveCamera(camera);
    }
  }
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderWindow->SetSize(512, 512);
  renderWindow->SetWindowName("LOxSeeds");

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
