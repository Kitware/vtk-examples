#include <cmath>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProgrammableSource.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkReverseSense.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkXMLPolyDataReader.h>

namespace {
static vtkSmartPointer<vtkPolyData>
transform_back(vtkSmartPointer<vtkPoints> pt, vtkSmartPointer<vtkPolyData> pd);
}

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> namedColors;

  auto input = vtkSmartPointer<vtkPolyData>::New();
  if (argc > 1)
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();
    input = reader->GetOutput();
  }
  else
  {
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();
    input = sphereSource->GetOutput();
  }
  // Read some points
  // vtkNew<vtkPoints points;

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(input->GetPoints());

  // Construct the surface and create isosurface.
  vtkNew<vtkSurfaceReconstructionFilter> surf;
  surf->SetInputData(polydata);

  vtkNew<vtkContourFilter> contourFilter;
  contourFilter->SetInputConnection(surf->GetOutputPort());
  contourFilter->SetValue(0, 0.0);

  // Sometimes the contouring algorithm can create a volume whose gradient
  // vector and ordering of polygon (using the right hand rule) are
  // inconsistent. vtkReverseSense cures this problem.
  vtkNew<vtkReverseSense> reverse;
  reverse->SetInputConnection(contourFilter->GetOutputPort());
  reverse->ReverseCellsOn();
  reverse->ReverseNormalsOn();
  reverse->Update();

  // auto newSurf = transform_back(points, reverse->GetOutput());

  vtkNew<vtkPolyDataMapper> map;
  map->SetInputConnection(reverse->GetOutputPort());
  map->ScalarVisibilityOff();

  vtkNew<vtkActor> surfaceActor;
  surfaceActor->SetMapper(map);
  surfaceActor->GetProperty()->SetDiffuseColor(
      namedColors->GetColor3d("Tomato").GetData());
  surfaceActor->GetProperty()->SetSpecularColor(
      namedColors->GetColor3d("Seashell").GetData());
  surfaceActor->GetProperty()->SetSpecular(.4);
  surfaceActor->GetProperty()->SetSpecularPower(50);

  // Create the RenderWindow, Renderer and both Actors
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer, set the background and size
  renderer->AddActor(surfaceActor);
  renderer->SetBackground(namedColors->GetColor3d("Burlywood").GetData());
  renderWindow->SetSize(640, 480);

  interactor->Initialize();
  renderWindow->SetWindowName("SurfaceFromUnorganizedPointsWithPostProc");
  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {
vtkSmartPointer<vtkPolyData> transform_back(vtkSmartPointer<vtkPoints> pt,
                                            vtkSmartPointer<vtkPolyData> pd)
{
  // The reconstructed surface is transformed back to where the
  // original points are. (Hopefully) it is only a similarity
  // transformation.

  // 1. Get bounding box of pt, get its minimum corner (left, bottom, least-z),
  // at c0, pt_bounds

  // 2. Get bounding box of surface pd, get its minimum corner (left, bottom,
  // least-z), at c1, pd_bounds

  // 3. compute scale as:
  //       scale = (pt_bounds[1] - pt_bounds[0])/(pd_bounds[1] - pd_bounds[0]);

  // 4. transform the surface by T := T(pt_bounds[0], [2],
  // [4]).S(scale).T(-pd_bounds[0], -[2], -[4])

  // 1.
  double pt_bounds[6]; // (xmin,xmax, ymin,ymax, zmin,zmax)
  pt->GetBounds(pt_bounds);

  // 2.
  double pd_bounds[6]; // (xmin,xmax, ymin,ymax, zmin,zmax)
  pd->GetBounds(pd_bounds);

  //   // test, make sure it is isotropic
  //   std::cout<<(pt_bounds[1] - pt_bounds[0])/(pd_bounds[1] -
  //   pd_bounds[0])<<std::endl; std::cout<<(pt_bounds[3] -
  //   pt_bounds[2])/(pd_bounds[3] - pd_bounds[2])<<std::endl;
  //   std::cout<<(pt_bounds[5] - pt_bounds[4])/(pd_bounds[5] -
  //   pd_bounds[4])<<std::endl;
  //   // TEST

  // 3
  double scale = (pt_bounds[1] - pt_bounds[0]) / (pd_bounds[1] - pd_bounds[0]);

  // 4.
  vtkSmartPointer<vtkTransform> transp;
  transp->Translate(pt_bounds[0], pt_bounds[2], pt_bounds[4]);
  transp->Scale(scale, scale, scale);
  transp->Translate(-pd_bounds[0], -pd_bounds[2], -pd_bounds[4]);

  auto tpd = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

  tpd->SetInputData(pd);
  tpd->SetTransform(transp);
  tpd->Update();

  return tpd->GetOutput();
}
} // namespace
