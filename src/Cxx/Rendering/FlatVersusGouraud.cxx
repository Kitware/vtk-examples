#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkCylinderSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkQuadric.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSphereSource.h>

namespace {
void CreateSphere(vtkRenderer*, bool);
void CreateCylinder(vtkRenderer*, bool);
void CreateIsoSurface(vtkRenderer*, bool);
void CreateModel(vtkRenderer*, bool, char*);
} // namespace

int main(int argc, char* argv[])
{

  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " filename" << std::endl;
    std::cout << "where: filename is the file cow.obj" << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<vtkSmartPointer<vtkRenderer>> renderers;

  // Create the 8 renderers
  vtkNew<vtkRenderer> flatSphereRenderer;
  renderers.push_back(flatSphereRenderer);
  vtkNew<vtkRenderer> flatCylinderRenderer;
  renderers.push_back(flatCylinderRenderer);
  vtkNew<vtkRenderer> flatIsoSurfaceRenderer;
  renderers.push_back(flatIsoSurfaceRenderer);
  vtkNew<vtkRenderer> flatModelRenderer;
  renderers.push_back(flatModelRenderer);
  vtkNew<vtkRenderer> smoothSphereRenderer;
  renderers.push_back(smoothSphereRenderer);
  vtkNew<vtkRenderer> smoothCylinderRenderer;
  renderers.push_back(smoothCylinderRenderer);
  vtkNew<vtkRenderer> smoothIsoSurfaceRenderer;
  renderers.push_back(smoothIsoSurfaceRenderer);
  vtkNew<vtkRenderer> smoothModelRenderer;
  renderers.push_back(smoothModelRenderer);

  // Add the actors
  CreateSphere(flatSphereRenderer, true);
  CreateCylinder(flatCylinderRenderer, true);
  CreateIsoSurface(flatIsoSurfaceRenderer, true);
  CreateModel(flatModelRenderer, true, argv[1]);

  CreateSphere(smoothSphereRenderer, false);
  CreateCylinder(smoothCylinderRenderer, false);
  CreateIsoSurface(smoothIsoSurfaceRenderer, false);
  CreateModel(smoothModelRenderer, false, argv[1]);

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("FlatVersusGouraud");

  // Setup viewports for the renderers
  int rendererSize = 256;
  unsigned int xGridDimensions = 4;
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

  vtkNew<vtkNamedColors> colors;
  for (size_t r = 0; r < renderers.size(); ++r)
  {
    renderers[r]->SetBackground(colors->GetColor3d("SlateGray").GetData());
    renderers[r]->GetActiveCamera()->Azimuth(20);
    renderers[r]->GetActiveCamera()->Elevation(30);
    renderers[r]->ResetCamera();
    if (r > 3)
    {
      renderers[r]->SetActiveCamera(renderers[r - 4]->GetActiveCamera());
    }
    renderWindow->AddRenderer(renderers[r]);
  }
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {
void CreateSphere(vtkRenderer* renderer, bool flat)
{
  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkSphereSource> sphere;
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphere->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());
  if (flat)
  {
    actor->GetProperty()->SetInterpolationToFlat();
  }
  else
  {
    actor->GetProperty()->SetInterpolationToGouraud();
  }
  renderer->AddActor(actor);
}

void CreateCylinder(vtkRenderer* renderer, bool flat)
{
  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkCylinderSource> cylinder;
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(cylinder->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());
  if (flat)
  {
    actor->GetProperty()->SetInterpolationToFlat();
  }
  else
  {
    actor->GetProperty()->SetInterpolationToGouraud();
  }
  renderer->AddActor(actor);
}

void CreateIsoSurface(vtkRenderer* renderer, bool flat)
{
  double range[2];
  // Sample quadric function
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(1, 2, 3, 0, 1, 0, 0, 0, 0, 0);

  vtkNew<vtkSampleFunction> sample;
  sample->SetSampleDimensions(25, 25, 25);
  sample->SetImplicitFunction(quadric);

  // Generate implicit surface
  vtkNew<vtkContourFilter> contour;
  contour->SetInputConnection(sample->GetOutputPort());
  range[0] = 1.0;
  range[1] = 6.0;
  contour->GenerateValues(5, range);

  // Map contour
  vtkNew<vtkPolyDataMapper> contourMapper;
  contourMapper->SetInputConnection(contour->GetOutputPort());
  contourMapper->SetScalarRange(0, 7);

  vtkNew<vtkActor> actor;
  actor->SetMapper(contourMapper);
  if (flat)
  {
    actor->GetProperty()->SetInterpolationToFlat();
  }
  else
  {
    actor->GetProperty()->SetInterpolationToGouraud();
  }
  renderer->AddActor(actor);
  return;
}

void CreateModel(vtkRenderer* renderer, bool flat, char* fileName)
{
  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkOBJReader> reader;
  reader->SetFileName(fileName);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Tan").GetData());
  if (flat)
  {
    actor->GetProperty()->SetInterpolationToFlat();
  }
  else
  {
    actor->GetProperty()->SetInterpolationToGouraud();
  }
  renderer->AddActor(actor);
}
} // namespace
