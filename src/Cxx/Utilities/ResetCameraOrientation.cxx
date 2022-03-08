#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkConeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace {

//! The positional information relating to the camera.
struct Orientation
{
  void ToVector(const double* from, std::vector<double>& to)
  {
    for (size_t i = 0; i < to.size(); ++i)
    {
      to[i] = from[i];
    }
  };

  std::vector<double> position{0.0, 0.0, 0.0};
  std::vector<double> focalPoint{0.0, 0.0, 0.0};
  std::vector<double> viewUp{0.0, 0.0, 0.0};
  double distance = 0;
  std::vector<double> clipplingRange{0.0, 0.0};
  std::vector<double> orientation{0.0, 0.0, 0.0};
};

//! Produce a comma-separated string of numbers from a vector.
/*
 *
 * @param v - the vector of doubles.
 * @param precision - number of decimal places.
 * @param width - the width.
 * @return A comma-separated string.
 */
std::string CommaSeparatedList(std::vector<double> const& v,
                               const int precision = 6, const int width = 10);

//! Get the camera orientation.
/*
 * @param ren - the renderer.
 * @return The orientation parameters.
 */
Orientation GetOrientation(vtkRenderer* ren);

//! Set the camera orientation.
/*
 *
 * @param ren - the renderer.
 * @param p - The orientation parameters.
 * @return
 */
void SetOrientation(vtkRenderer* ren, Orientation const& p);

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkConeSource> cone;
  cone->SetHeight(3.0);
  cone->SetRadius(1.0);
  cone->SetResolution(10);

  vtkNew<vtkPolyDataMapper> coneMapper;
  coneMapper->SetInputConnection(cone->GetOutputPort());

  vtkNew<vtkActor> coneActor;
  coneActor->SetMapper(coneMapper);
  coneActor->GetProperty()->SetColor(colors->GetColor3d("Bisque").GetData());

  vtkNew<vtkRenderer> ren;
  ren->AddActor(coneActor);
  ren->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren);
  renWin->SetSize(600, 600);
  renWin->SetWindowName("ResetCameraOrientation");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  iren->SetInteractorStyle(style);

  vtkCamera* camera = ren->GetActiveCamera();
  camera->SetRoll(15);
  camera->Elevation(-15);
  camera->Azimuth(30);
  ren->ResetCamera();

  renWin->Render();
  Orientation originalOrient = GetOrientation(ren);
  std::cout << std::setw(23) << "Original orientation:"
            << CommaSeparatedList(originalOrient.orientation) << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));

  camera->SetPosition(-3.568189, 5.220048, 2.352639);
  camera->SetFocalPoint(-0.399044, -0.282865, 0.131438);
  camera->SetViewUp(0.623411, 0.573532, -0.531431);
  camera->SetDistance(6.727500);
  camera->SetClippingRange(3.001430, 11.434082);
  // No need to use ren->ResetCamera() as we have all the parameters.
  renWin->Render();
  Orientation newOrient = GetOrientation(ren);
  std::cout << std::setw(23)
            << "New orientation:" << CommaSeparatedList(newOrient.orientation)
            << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "Reloading the original orientation." << std::endl;
  SetOrientation(ren, originalOrient);
  renWin->Render();
  Orientation check = GetOrientation(ren);
  std::cout << std::setw(23)
            << "Final orientation:" << CommaSeparatedList(check.orientation)
            << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));

  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}

namespace {
std::string CommaSeparatedList(std::vector<double> const& v,
                               const int precision, const int width)
{
  std::ostringstream os;
  for (auto p = v.begin(); p < v.end() - 1; ++p)
  {
    os << std::fixed << std::setw(width) << std::setprecision(precision) << *p
       << ", ";
  }
  os << std::fixed << std::setw(width) << std::setprecision(precision)
     << v.back();
  return os.str();
}

Orientation GetOrientation(vtkRenderer* ren)
{
  Orientation p;

  vtkCamera* camera = ren->GetActiveCamera();
  p.ToVector(camera->GetPosition(), p.position);
  p.ToVector(camera->GetFocalPoint(), p.focalPoint);
  p.ToVector(camera->GetViewUp(), p.viewUp);
  p.distance = camera->GetDistance();
  p.ToVector(camera->GetClippingRange(), p.clipplingRange);
  p.ToVector(camera->GetOrientation(), p.orientation);

  return p;
}

void SetOrientation(vtkRenderer* ren, Orientation const& p)
{
  vtkCamera* camera = ren->GetActiveCamera();
  camera->SetPosition(p.position.data());
  camera->SetFocalPoint(p.focalPoint.data());
  camera->SetViewUp(p.viewUp.data());
  camera->SetDistance(p.distance);
  camera->SetClippingRange(p.clipplingRange.data());
}

} // namespace
