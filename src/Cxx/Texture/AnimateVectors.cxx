#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkGlyph3D.h>
#include <vtkLineSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredPointsReader.h>
#include <vtkTexture.h>
#include <vtkThresholdPoints.h>

#include <vector>

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " carotid.vtk vecAnim1.vtk ..."
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  // Setup render window, renderer, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // read data
  //
  // create pipeline
  //
  vtkNew<vtkStructuredPointsReader> reader;
  reader->SetFileName(argv[1]);

  vtkNew<vtkThresholdPoints> threshold;
  threshold->SetInputConnection(reader->GetOutputPort());
  threshold->ThresholdByUpper(200);

  vtkNew<vtkLineSource> line;
  line->SetResolution(1);

  vtkNew<vtkGlyph3D> lines;
  lines->SetInputConnection(threshold->GetOutputPort());
  lines->SetSourceConnection(line->GetOutputPort());
  lines->SetScaleFactor(0.005);
  lines->SetScaleModeToScaleByScalar();
  lines->Update();

  vtkNew<vtkPolyDataMapper> vectorMapper;
  vectorMapper->SetInputConnection(lines->GetOutputPort());
  vectorMapper->SetScalarRange(lines->GetOutput()->GetScalarRange());

  vtkNew<vtkActor> vectorActor;
  vectorActor->SetMapper(vectorMapper);
  vectorActor->GetProperty()->SetOpacity(0.99);
  vectorActor->GetProperty()->SetLineWidth(1.5);

  // outline
  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  //  texture maps
  std::vector<vtkSmartPointer<vtkTexture>> textureMaps;
  for (int i = 2; i < argc; ++i)
  {
    vtkNew<vtkStructuredPointsReader> tmap;
    tmap->SetFileName(argv[i]);

    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(tmap->GetOutputPort());
    texture->InterpolateOff();
    texture->RepeatOff();
    textureMaps.push_back(texture);
  }
  vectorActor->SetTexture(textureMaps[0]);

  // Add the actors to the renderer, set the background and size
  //
  renderer->AddActor(vectorActor);
  renderer->AddActor(outlineActor);

  vtkNew<vtkCamera> cam1;
  cam1->SetClippingRange(17.4043, 870.216);
  cam1->SetFocalPoint(136.71, 104.025, 23);
  cam1->SetPosition(204.747, 258.939, 63.7925);
  cam1->SetViewUp(-0.102647, -0.210897, 0.972104);
  cam1->Zoom(1.2);
  renderer->SetActiveCamera(cam1);

  renderer->SetBackground(colors->GetColor3d("Wheat").GetData());
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("AnimateVectors");

  // go into loop
  for (int j = 0; j < 100; ++j)
  {
    for (size_t i = 0; i < textureMaps.size(); ++i)
    {
      vectorActor->SetTexture(textureMaps[i]);
      renderWindow->Render();
    }
  }
  interactor->Start();
  return EXIT_SUCCESS;
}
