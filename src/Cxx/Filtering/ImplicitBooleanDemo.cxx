#include <vtkActor.h>
#include <vtkBox.h>
#include <vtkCamera.h>
#include <vtkColorSeries.h>
#include <vtkContourFilter.h>
#include <vtkImplicitBoolean.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSphere.h>


int main(int /* argc */, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // create a sphere
  vtkNew<vtkSphere> sphere;
  sphere->SetRadius(1);
  sphere->SetCenter(1, 0, 0);

  // create a box
  vtkNew<vtkBox> box;
  box->SetBounds(-1, 1, -1, 1, -1, 1);

  // combine the two implicit functions
  vtkNew<vtkImplicitBoolean> boolean;
  boolean->AddFunction(box);
  boolean->AddFunction(sphere);

  std::vector<vtkNew<vtkRenderer>> ren;
  ren.push_back(vtkNew<vtkRenderer>());
  ren.push_back(vtkNew<vtkRenderer>());
  ren.push_back(vtkNew<vtkRenderer>());
  ren[0]->SetViewport(0, 0, 1.0 / 3.0, 1);         // Difference
  ren[1]->SetViewport(1.0 / 3.0, 0, 2.0 / 3.0, 1); // Union
  ren[2]->SetViewport(2.0 / 3.0, 0, 1, 1);         // Intersection

  // Shared camera
  vtkNew<vtkCamera> camera;
  camera->Azimuth(30.0);
  camera->Elevation(30.0);

  vtkNew<vtkColorSeries> colorSeries;
  colorSeries->SetColorScheme(vtkColorSeries::BREWER_DIVERGING_SPECTRAL_3);

  for (int i = 0; i < 3; ++i)
  {
    if (i == 0)
    {
      boolean->SetOperationTypeToDifference();
    }
    else if (i == 1)
    {
      boolean->SetOperationTypeToUnion();
    }
    else
    {
      boolean->SetOperationTypeToIntersection();
    }

    // The sample function generates a distance function from the implicit
    // function. This is then contoured to get a polygonal surface.
    vtkNew<vtkSampleFunction> sample;
    sample->SetImplicitFunction(boolean);
    sample->SetModelBounds(-1, 2, -1, 1, -1, 1);
    sample->SetSampleDimensions(100, 100, 100);
    sample->ComputeNormalsOff();

    // contour
    vtkNew<vtkContourFilter> surface;
    surface->SetInputConnection(sample->GetOutputPort());
    surface->SetValue(0, 0.0);
    surface->Update();

    vtkNew<vtkPolyData> polyData;
    polyData->DeepCopy(surface->GetOutput());

    // mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    //    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetColor(colors->GetColor3d("peacock").GetData());

    // add the actor
    ren[i]->SetBackground(colorSeries->GetColor(i).GetRed() / 255.0,
                          colorSeries->GetColor(i).GetGreen() / 255.0,
                          colorSeries->GetColor(i).GetBlue() / 255.0);
    ren[i]->SetActiveCamera(camera);
    ren[i]->AddActor(actor);
  }

  // render window
  vtkNew<vtkRenderWindow> renwin;
  renwin->AddRenderer(ren[0]);
  renwin->AddRenderer(ren[1]);
  renwin->AddRenderer(ren[2]);

  // An interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renwin);

  renwin->SetSize(900, 300);
  renwin->SetWindowName("ImplicitBooleanDemo");
  renwin->Render();
  ren[1]->ResetCamera();
  renwin->Render();

  // Start
  interactor->Initialize();
  interactor->Start();

  return EXIT_SUCCESS;
}
