/*
 * This example shows how to combine data from both the imaging
 *  and graphics pipelines. The vtkMergeData filter is used to
 *  merge the data from each together.
 */
#include <vtkActor.h>
#include <vtkBMPReader.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkImageLuminance.h>
#include <vtkMergeFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkWarpScalar.h>

#include <array>
#include <iomanip>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " fileName" << std::endl;
    std::cout << "where: fileName is the masonry.bmp file." << std::endl;
    return EXIT_FAILURE;
  }

  std::string fileName = argv[1];

  // Set the background color.
  std::array<unsigned char, 4> bkg{{60, 93, 144, 255}};
  colors->SetColor("BkgColor", bkg.data());

  // Read in an image and compute a luminance value-> The image is extracted
  // as a set of polygons (vtkImageDataGeometryFilter). We then will
  // warp the plane using the scalar (luminance) values.
  //
  vtkNew<vtkBMPReader> reader;
  reader->SetFileName(fileName.c_str());
  // Convert the image to a grey scale.
  vtkNew<vtkImageLuminance> luminance;
  luminance->SetInputConnection(reader->GetOutputPort());
  // Pass the data to the pipeline as polygons.
  vtkNew<vtkImageDataGeometryFilter> geometry;
  geometry->SetInputConnection(luminance->GetOutputPort());
  // Warp the data in a direction perpendicular to the image plane.
  vtkNew<vtkWarpScalar> warp;
  warp->SetInputConnection(geometry->GetOutputPort());
  warp->SetScaleFactor(-0.1);

  // Use vtkMergeFilter to combine the original image with the warped geometry.
  vtkNew<vtkMergeFilter> merge;
  merge->SetGeometryConnection(warp->GetOutputPort());
  merge->SetScalarsConnection(reader->GetOutputPort());
  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(merge->GetOutputPort());
  mapper->SetScalarRange(0, 255);
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Create the rendering window, renderer, and interactive renderer.
  //
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size.
  ren->AddActor(actor);
  ren->ResetCamera();
  ren->SetBackground(colors->GetColor3d("BkgColor").GetData());
  // ren->GetActiveCamera()->Azimuth(20);
  // ren->GetActiveCamera()->Elevation(30);
  // ren->ResetCameraClippingRange();
  // ren->GetActiveCamera()->Zoom(1.3);
  ren->GetActiveCamera()->SetPosition(-100, -130, 325);
  ren->GetActiveCamera()->SetFocalPoint(105, 114, -29);
  ren->GetActiveCamera()->SetViewUp(0.51, 0.54, 0.67);
  ren->ResetCameraClippingRange();

  renWin->SetSize(512, 512);
  renWin->SetWindowName("ImageWarp");

  // Render the image.
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
