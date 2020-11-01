#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkQuadric.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>

namespace {
void Other();
void Sphere();
void Cone();
void Ellipsoid();
void Cylinder();
void HyperboloidOneSheet();
void HyperboloidTwoSheets();
void HyperbolicParaboloid();
void EllipticParaboloid();

void PlotFunction(vtkQuadric* quadric, double value);
} // namespace

int main(int, char*[])
{
  // Other();
  // Sphere();
  // Cone();
  // Ellipsoid();
  // Cylinder();
  // HyperboloidOneSheet();
  // HyperboloidTwoSheets();
  // HyperbolicParaboloid();
  EllipticParaboloid();

  return EXIT_SUCCESS;
}

namespace {
void Sphere()
{
  // create the quadric function definition
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(1, 1, 1, 0, 0, 0, 0, 0, 0, 0);

  // F(x,y,z) = a0*x^2 + a1*y^2 + a2*z^2 + a3*x*y + a4*y*z + a5*x*z + a6*x +
  // a7*y + a8*z + a9 F(x,y,z) = 1*x^2 + 1*y^2 + 1*z^2

  PlotFunction(quadric, 1.0);
}

void EllipticParaboloid()
{
  // create the quadric function definition
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(1, 1, 0, 0, 0, 0, 0, 0, -1, 0);

  // F(x,y,z) = a0*x^2 + a1*y^2 + a2*z^2 + a3*x*y + a4*y*z + a5*x*z + a6*x +
  // a7*y + a8*z + a9 F(x,y,z) = 1*x^2 + 1*y^2

  PlotFunction(quadric, 10.0);
}

void HyperbolicParaboloid()
{
  // create the quadric function definition
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(1, -1, 0, 0, 0, 0, 0, 0, 0, 0);

  // F(x,y,z) = a0*x^2 + a1*y^2 + a2*z^2 + a3*x*y + a4*y*z + a5*x*z + a6*x +
  // a7*y + a8*z + a9 F(x,y,z) = 1*x^2 - 1*y^2

  PlotFunction(quadric, 10.0);
}

void Cylinder()
{
  // create the quadric function definition
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(1, 1, 0, 0, 0, 0, 0, 0, 0, 0);

  // F(x,y,z) = a0*x^2 + a1*y^2 + a2*z^2 + a3*x*y + a4*y*z + a5*x*z + a6*x +
  // a7*y + a8*z + a9 F(x,y,z) = 1*x^2 + 1*y^2

  PlotFunction(quadric, 1.0);
}

void HyperboloidOneSheet()
{
  // create the quadric function definition
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(1, 1, -1, 0, 0, 0, 0, 0, 0, 0);

  // F(x,y,z) = a0*x^2 + a1*y^2 + a2*z^2 + a3*x*y + a4*y*z + a5*x*z + a6*x +
  // a7*y + a8*z + a9 F(x,y,z) = 1*x^2 + 1*y^2

  PlotFunction(quadric, 1.0);
}

void HyperboloidTwoSheets()
{
  // create the quadric function definition
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(1, 1, -1, 0, 0, 0, 0, 0, 0, 0);

  // F(x,y,z) = a0*x^2 + a1*y^2 + a2*z^2 + a3*x*y + a4*y*z + a5*x*z + a6*x +
  // a7*y + a8*z + a9 F(x,y,z) = 1*x^2 + 1*y^2

  PlotFunction(quadric, -1.0);
}

void Ellipsoid()
{
  // create the quadric function definition
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(1, 1, 2, 0, 0, 0, 0, 0, 0, 0);

  // F(x,y,z) = a0*x^2 + a1*y^2 + a2*z^2 + a3*x*y + a4*y*z + a5*x*z + a6*x +
  // a7*y + a8*z + a9 F(x,y,z) = 1*x^2 + 1*y^2 + 1*z^2

  PlotFunction(quadric, -1.0);
}

void Cone()
{
  // create the quadric function definition
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(1, 1, -1, 0, 0, 0, 0, 0, 0, 0);

  // F(x,y,z) = a0*x^2 + a1*y^2 + a2*z^2 + a3*x*y + a4*y*z + a5*x*z + a6*x +
  // a7*y + a8*z + a9 F(x,y,z) = 1*x^2 + 1*y^2 - 1*z^2
  PlotFunction(quadric, 0.0);
}

void Other()
{
  // create the quadric function definition
  vtkNew<vtkQuadric> quadric;
  quadric->SetCoefficients(.5, 1, .2, 0, .1, 0, 0, .2, 0, 0);

  // F(x,y,z) = a0*x^2 + a1*y^2 + a2*z^2 + a3*x*y + a4*y*z + a5*x*z + a6*x +
  // a7*y + a8*z + a9 F(x,y,z) = 0.5*x^2 + 1*y^2 + 0.2*z^2 + 0*x*y + 0.1*y*z +
  // 0*x*z + 0*x + 0.2*y + 0*z + 0
  PlotFunction(quadric, 1.0);
}

void PlotFunction(vtkQuadric* quadric, double value)
{

  vtkNew<vtkNamedColors> colors;

  // sample the quadric function
  vtkNew<vtkSampleFunction> sample;
  sample->SetSampleDimensions(50, 50, 50);
  sample->SetImplicitFunction(quadric);
  // double xmin = 0, xmax=1, ymin=0, ymax=1, zmin=0, zmax=1;
  double bounds[6]{-10, 11, -10, 10, -10, 10};
  sample->SetModelBounds(bounds);

  // Create five surfaces F(x,y,z) = constant between range specified
  /*
  vtkContourFilter *contours = vtkContourFilter::New();
  contours->SetInput(sample->GetOutput());
  contours->GenerateValues(5, 0.0, 1.2);
  */

  // create the 0 isosurface
  vtkNew<vtkContourFilter> contours;
  contours->SetInputConnection(sample->GetOutputPort());
  contours->GenerateValues(1, value, value);

  // map the contours to graphical primitives
  vtkNew<vtkPolyDataMapper> contourMapper;
  contourMapper->SetInputConnection(contours->GetOutputPort());
  contourMapper->SetScalarRange(0.0, 1.2);

  // create an actor for the contours
  vtkNew<vtkActor> contourActor;
  contourActor->SetMapper(contourMapper);

  // -- create a box around the function to indicate the sampling volume --

  // create outline
  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(sample->GetOutputPort());

  // map it to graphics primitives
  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  // create an actor for it
  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);
  outlineActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

  // setup the window
  vtkNew<vtkRenderer> ren1;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->SetWindowName("DisplayQuadricSurfaces");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // add the actors to the scene
  ren1->AddActor(contourActor);
  ren1->AddActor(outlineActor);
  ren1->SetBackground(colors->GetColor3d("AliceBlue").GetData());

  // render and interact
  renWin->Render();
  ren1->GetActiveCamera()->Azimuth(-55);
  ren1->GetActiveCamera()->Elevation(15);
  iren->Start();
}
} // namespace
