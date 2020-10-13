#include <vtkBoundedPointSource.h>
#include <vtkCamera.h>
#include <vtkCone.h>
#include <vtkCylinder.h>
#include <vtkExtractPoints.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphere.h>
#include <vtkSphereSource.h>
#include <vtkSuperquadric.h>

#include <vector>

int main(int /*argc*/, char* /* argv */[])
{
  vtkNew<vtkNamedColors> colors;

  // Create implicit functions
  vtkNew<vtkCone> cone;
  cone->SetAngle(30.0);
  vtkNew<vtkSphere> sphere;
  vtkNew<vtkCylinder> cylinder;
  vtkNew<vtkSuperquadric> superquadric;
  superquadric->SetPhiRoundness(2.5);
  superquadric->SetThetaRoundness(0.5);

  // Store the functions
  std::vector<vtkSmartPointer<vtkImplicitFunction>> functions;
  functions.push_back(sphere);
  functions.push_back(cone);
  functions.push_back(cylinder);
  functions.push_back(superquadric);

  vtkNew<vtkBoundedPointSource> pointSource;
  pointSource->SetNumberOfPoints(100000);

  // Rows and columns
  unsigned int gridXDimensions = 2;
  unsigned int gridYDimensions = 2;

  // Need a renderer even if there is no actor
  std::vector<vtkSmartPointer<vtkRenderer>> renderers;
  double background[6] = {0.4, 0.5, 0.6, 0.6, 0.5, 0.4};
  for (size_t i = 0; i < gridXDimensions * gridYDimensions; i++)
  {
    renderers.push_back(vtkSmartPointer<vtkRenderer>::New());
    renderers[i]->SetBackground(background);
    auto fwdIt = std::rotate(background, background + 1, background + 6);
  }

  // Glyphs
  double radius = 0.02;
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(radius);

  // One render window
  vtkNew<vtkRenderWindow> renderWindow;
  int rendererSize = 256;
  renderWindow->SetSize(rendererSize * gridXDimensions,
                        rendererSize * gridYDimensions);
  renderWindow->SetWindowName("ExtractPointsDemo");

  // Create a pipeline for each function
  for (int row = 0; row < static_cast<int>(gridYDimensions); row++)
  {
    for (int col = 0; col < static_cast<int>(gridXDimensions); col++)
    {
      int index = row * gridXDimensions + col;

      // (xmin, ymin, xmax, ymax)
      double viewport[4] = {static_cast<double>(col) * rendererSize /
                                (gridXDimensions * rendererSize),
                            static_cast<double>(gridYDimensions - (row + 1)) *
                                rendererSize / (gridYDimensions * rendererSize),
                            static_cast<double>(col + 1) * rendererSize /
                                (gridXDimensions * rendererSize),
                            static_cast<double>(gridYDimensions - row) *
                                rendererSize /
                                (gridYDimensions * rendererSize)};
      renderWindow->AddRenderer(renderers[index]);
      renderers[index]->SetViewport(viewport);
      if (index > static_cast<int>(functions.size() - 1))
      {
        continue;
      }
      // Define the pipeline
      vtkNew<vtkExtractPoints> extract;
      extract->SetInputConnection(pointSource->GetOutputPort());
      extract->SetImplicitFunction(functions[index]);

      vtkNew<vtkGlyph3DMapper> glyph;
      glyph->SetInputConnection(extract->GetOutputPort());
      glyph->SetSourceConnection(sphereSource->GetOutputPort());
      glyph->ScalingOff();

      vtkNew<vtkActor> glyphActor;
      glyphActor->SetMapper(glyph);
      glyphActor->GetProperty()->SetColor(
          colors->GetColor3d("MistyRose").GetData());

      renderers[index]->AddActor(glyphActor);
      renderers[index]->ResetCamera();
      renderers[index]->GetActiveCamera()->Azimuth(30);
      renderers[index]->GetActiveCamera()->Elevation(-30);
      renderers[index]->GetActiveCamera()->Dolly(1.1);
      renderers[index]->ResetCameraClippingRange();
    }
  }
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renderWindow);

  renderWindow->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
