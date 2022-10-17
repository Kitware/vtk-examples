#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkAssignAttribute.h>
#include <vtkCamera.h>
#include <vtkExtractEdges.h>
#include <vtkGlyph3D.h>
#include <vtkGradientFilter.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTubeFilter.h>
#include <vtkUnstructuredGridReader.h>

// Note that:
// vtkExtractEdges moved from vtkFiltersExtraction to vtkFiltersCore in
// VTK commit d9981b9aeb93b42d1371c6e295d76bfdc18430bd

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " Filename(.vtk) e.g. uGridEx.vtk "
              << std::endl;
  }
  // Create the reader for the data.
  // This is the data that will be rendered.
  std::string filename = argv[1];
  std::cout << "Loading " << filename.c_str() << std::endl;
  vtkNew<vtkUnstructuredGridReader> reader;
  reader->SetFileName(filename.c_str());

  vtkNew<vtkExtractEdges> edges;
  edges->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkTubeFilter> tubes;
  tubes->SetInputConnection(edges->GetOutputPort());
  tubes->SetRadius(0.0625);
  tubes->SetVaryRadiusToVaryRadiusOff();
  tubes->SetNumberOfSides(32);

  vtkNew<vtkPolyDataMapper> tubesMapper;
  tubesMapper->SetInputConnection(tubes->GetOutputPort());
  tubesMapper->SetScalarRange(0.0, 26.0);

  vtkNew<vtkActor> tubesActor;
  tubesActor->SetMapper(tubesMapper);

  vtkNew<vtkGradientFilter> gradients;
  gradients->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkAssignAttribute> vectors;
  vectors->SetInputConnection(gradients->GetOutputPort());
  vectors->Assign("Gradients", vtkDataSetAttributes::VECTORS,
                  vtkAssignAttribute::POINT_DATA);

  vtkNew<vtkArrowSource> arrow;

  vtkNew<vtkGlyph3D> glyphs;
  glyphs->SetInputConnection(0, vectors->GetOutputPort());
  glyphs->SetInputConnection(1, arrow->GetOutputPort());
  glyphs->ScalingOn();
  glyphs->SetScaleModeToScaleByVector();
  glyphs->SetScaleFactor(0.25);
  glyphs->OrientOn();
  glyphs->ClampingOff();
  glyphs->SetVectorModeToUseVector();
  glyphs->SetIndexModeToOff();

  vtkNew<vtkPolyDataMapper> glyphMapper;
  glyphMapper->SetInputConnection(glyphs->GetOutputPort());
  glyphMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> glyphActor;
  glyphActor->SetMapper(glyphMapper);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(tubesActor);
  renderer->AddActor(glyphActor);
  renderer->SetBackground(0.328125, 0.347656, 0.425781);

  vtkNew<vtkRenderWindow> renwin;
  renwin->AddRenderer(renderer);
  renwin->SetSize(350, 500);
  renwin->SetWindowName("GradientFilter");

  renderer->ResetCamera();
  vtkCamera* camera = renderer->GetActiveCamera();
  camera->Elevation(-80.0);
  camera->OrthogonalizeViewUp();
  camera->Azimuth(135.0);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renwin);
  renwin->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
