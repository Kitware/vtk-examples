//
/*
 * Create an iso-surface of the Lorenz attractor.
 *
 * Here we visualize a Lorenz strange attractor by integrating the Lorenz
 * equations in a volume. The number of visits in each voxel is recorded as a
 * scalar function. The surface is extracted via marching cubes using a visit
 * value of 50. The number of integration steps is 10 million, in a volume of
 * dimensions 200 x 200 * x 200. The surface roughness is caused by the discrete
 * nature of the evaluation function.
 */

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShortArray.h>
#include <vtkStructuredPoints.h>

int main(int, char*[])
{
  double Pr = 10.0; // The Lorenz parameters
  double b = 2.667;
  double r = 28.0;
  double x, y, z;       // starting (and current) x, y, z
  double h = 0.01;      // integration step size
  int resolution = 200; // slice resolution
  int iter = 10000000;  // number of iterations
  double xmin = -30.0;  // x, y, z range for voxels
  double xmax = 30.0;
  double ymin = -30.0;
  double ymax = 30.0;
  double zmin = -10.0;
  double zmax = 60.0;

  void options(int, char**);

  // take a stab at an integration step size
  auto xIncr = resolution / (xmax - xmin);
  auto yIncr = resolution / (ymax - ymin);
  auto zIncr = resolution / (zmax - zmin);

  printf("The Lorenz Attractor\n");
  printf("	Pr = %f\n", Pr);
  printf("	b = %f\n", b);
  printf("	r = %f\n", r);
  printf("	integration step size = %f\n", h);
  printf("	slice resolution = %d\n", resolution);
  printf("	# of iterations = %d\n", iter);
  printf("	specified range:\n");
  printf("		x: %f, %f\n", xmin, xmax);
  printf("		y: %f, %f\n", ymin, ymax);
  printf("		z: %f, %f\n", zmin, zmax);

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(8775070);
  x = randomSequence->GetRangeValue(xmin, xmax);
  randomSequence->Next();
  y = randomSequence->GetRangeValue(ymin, ymax);
  randomSequence->Next();
  z = randomSequence->GetRangeValue(zmin, zmax);
  randomSequence->Next();
  printf("	starting at %f, %f, %f\n", x, y, z);

  // allocate memory for the slices
  auto sliceSize = resolution * resolution;
  auto numPts = sliceSize * resolution;
  vtkNew<vtkShortArray> scalars;
  auto s = scalars->WritePointer(0, numPts);
  for (auto i = 0; i < numPts; i++)
  {
    s[i] = 0;
  }
  for (auto j = 0; j < iter; j++)
  {
    // integrate to next time step
    auto xx = x + h * Pr * (y - x);
    auto yy = y + h * (x * (r - z) - y);
    auto zz = z + h * (x * y - (b * z));

    x = xx;
    y = yy;
    z = zz;

    // calculate voxel index
    if (x < xmax && x > xmin && y < ymax && y > ymin && z < zmax && z > zmin)
    {
      auto xxx = static_cast<short>(static_cast<double>(xx - xmin) * xIncr);
      auto yyy = static_cast<short>(static_cast<double>(yy - ymin) * yIncr);
      auto zzz = static_cast<short>(static_cast<double>(zz - zmin) * zIncr);
      auto index = xxx + yyy * resolution + zzz * sliceSize;
      s[index] += 1;
    }
  }

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkStructuredPoints> volume;
  volume->GetPointData()->SetScalars(scalars);
  volume->SetDimensions(resolution, resolution, resolution);
  volume->SetOrigin(xmin, ymin, zmin);
  volume->SetSpacing((xmax - xmin) / resolution, (ymax - ymin) / resolution,
                     (zmax - zmin) / resolution);

  printf("	contouring...\n");
  // do the graphics dance
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // create iso-surface
  vtkNew<vtkContourFilter> contour;
  contour->SetInputData(volume);
  contour->SetValue(0, 50);

  // create mapper
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(contour->GetOutputPort());
  mapper->ScalarVisibilityOff();

  // create actor
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("DodgerBlue").GetData());

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("PaleGoldenrod").GetData());

  renWin->SetSize(640, 480);
  renWin->SetWindowName("Lorenz");

  // interact with data
  renWin->Render();

  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(-67.645167, -25.714343, 63.483516);
  camera->SetFocalPoint(3.224902, -4.398594, 29.552112);
  camera->SetViewUp(-0.232264, 0.965078, 0.121151);
  camera->SetDistance(81.414176);
  camera->SetClippingRange(18.428905, 160.896031);

  iren->Start();

  return EXIT_SUCCESS;
}
