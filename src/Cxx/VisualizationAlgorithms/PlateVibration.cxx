// Translated from vib.tcl

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataReader.h>
#include <vtkProp3D.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVectorDot.h>
#include <vtkWarpVector.h>

#include <array>
#include <string>

int main(int argc, char* argv[])
{
  auto Scale = [](std::vector<double>& v, double scale) {
    std::transform(std::begin(v), std::end(v), std::begin(v),
                   [=](double const& n) { return n / scale; });
    return;
  };

  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " filename" << std::endl;
    std::cout << "where: filename is the file plate.vtk." << std::endl;
    std::cout
        << "Produces figure 6-14(a) Beam displacement from the VTK Textbook."
        << std::endl;
    return EXIT_FAILURE;
  }

  std::string fileName = argv[1];

  vtkNew<vtkNamedColors> colors;

  // Set the background color and plate color.
  std::array<unsigned char, 4> bar{{255, 160, 140}};
  colors->SetColor("PlateColor", bar.data());
  std::array<unsigned char, 4> bkg{{65, 99, 149}};
  colors->SetColor("BkgColor", bkg.data());

  // Read a vtk file
  //
  vtkNew<vtkPolyDataReader> plate;
  plate->SetFileName(fileName.c_str());
  plate->Update();
  double bounds[6];
  plate->GetOutput()->GetBounds(bounds);
  plate->SetVectorsName("mode2");

  vtkNew<vtkPolyDataNormals> normals;
  normals->SetInputConnection(plate->GetOutputPort());
  vtkNew<vtkWarpVector> warp;
  warp->SetInputConnection(normals->GetOutputPort());
  warp->SetScaleFactor(0.5);
  vtkNew<vtkVectorDot> color;
  color->SetInputConnection(warp->GetOutputPort());
  vtkNew<vtkDataSetMapper> plateMapper;
  plateMapper->SetInputConnection(warp->GetOutputPort());
  vtkNew<vtkActor> plateActor;
  plateActor->SetMapper(plateMapper);
  plateActor->GetProperty()->SetColor(
      colors->GetColor3d("PlateColor").GetData());
  plateActor->RotateX(-90);

  // Create the outline.
  //
  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(plate->GetOutputPort());
  vtkNew<vtkPolyDataMapper> spikeMapper;
  spikeMapper->SetInputConnection(outline->GetOutputPort());
  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(spikeMapper);
  outlineActor->RotateX(-90);
  outlineActor->GetProperty()->SetColor(colors->GetColor3d("White").GetData());

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size
  //
  ren->AddActor(plateActor);
  ren->AddActor(outlineActor);
  renWin->SetSize(500, 500);
  renWin->SetWindowName("PlateVibration");

  // Render the image.
  renWin->Render();
  ren->SetBackground(colors->GetColor3d("BkgColor").GetData());
  // This closely matches the original illustration.
  ren->GetActiveCamera()->SetPosition(-3.7, 13, 15.5);
  ren->ResetCameraClippingRange();

  iren->Start();

  return EXIT_SUCCESS;
}
