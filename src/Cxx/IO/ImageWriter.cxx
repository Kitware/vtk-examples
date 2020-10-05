#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <vtkBMPWriter.h>
#include <vtkImageWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkPNMWriter.h>
#include <vtkPostScriptWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkWindowToImageFilter.h>

#include <array>
#include <locale>
#include <string>

namespace {

/**
 *  Write the render window view to an image file.
 *
 *  Image types supported are:
 *    BMP, JPEG, PNM, PNG, PostScript, TIFF.
 *  The default parameters are used for all writers, change as needed.
 *
 *  @param fileName The file name, if no extension then PNG is assumed.
 *  @param renWin The render window.
 *  @param rgba Used to set the buffer type.
 *
 */
void WriteImage(std::string const& fileName, vtkRenderWindow* renWin,
                bool rgba = true);
} // namespace

int main(int, char*[])
{

  vtkNew<vtkNamedColors> colors;

  // Set the background color.
  std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
  colors->SetColor("BkgColor", bkg.data());

  // Create the rendering window, renderer, and interactive renderer.
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Create the source.
  vtkNew<vtkSphereSource> source;
  source->SetCenter(0, 0, 0);
  source->SetRadius(5.0);

  // mapper
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(source->GetOutputPort());

  // actor
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // color the actor
  actor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

  // assign actor to the renderer
  ren->AddActor(actor);
  ren->SetBackground(colors->GetColor3d("BkgColor").GetData());

  renWin->SetWindowName("ImageWriter");
  renWin->Render();

  std::vector<std::string> ext = {{""},      {".png"}, {".jpg"}, {".ps"},
                                  {".tiff"}, {".bmp"}, {".pnm"}};
  std::vector<std::string> filenames;
  std::transform(ext.begin(), ext.end(), std::back_inserter(filenames),
                 [](const std::string& e) { return "ImageWriter" + e; });
  filenames[0] = filenames[0] + '1';
  for (auto f : filenames)
  {
    WriteImage(f, renWin, false);
  }

  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}

namespace {

void WriteImage(std::string const& fileName, vtkRenderWindow* renWin, bool rgba)
{
  if (!fileName.empty())
  {
    std::string fn = fileName;
    std::string ext;
    auto found = fn.find_last_of(".");
    if (found == std::string::npos)
    {
      ext = ".png";
      fn += ext;
    }
    else
    {
      ext = fileName.substr(found, fileName.size());
    }
    std::locale loc;
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [=](char const& c) { return std::tolower(c, loc); });
    auto writer = vtkSmartPointer<vtkImageWriter>::New();
    if (ext == ".bmp")
    {
      writer = vtkSmartPointer<vtkBMPWriter>::New();
    }
    else if (ext == ".jpg")
    {
      writer = vtkSmartPointer<vtkJPEGWriter>::New();
    }
    else if (ext == ".pnm")
    {
      writer = vtkSmartPointer<vtkPNMWriter>::New();
    }
    else if (ext == ".ps")
    {
      if (rgba)
      {
        rgba = false;
      }
      writer = vtkSmartPointer<vtkPostScriptWriter>::New();
    }
    else if (ext == ".tiff")
    {
      writer = vtkSmartPointer<vtkTIFFWriter>::New();
    }
    else
    {
      writer = vtkSmartPointer<vtkPNGWriter>::New();
    }
    vtkNew<vtkWindowToImageFilter> window_to_image_filter;
    window_to_image_filter->SetInput(renWin);
    window_to_image_filter->SetScale(1); // image quality
    if (rgba)
    {
      window_to_image_filter->SetInputBufferTypeToRGBA();
    }
    else
    {
      window_to_image_filter->SetInputBufferTypeToRGB();
    }
    // Read from the front buffer.
    window_to_image_filter->ReadFrontBufferOff();
    window_to_image_filter->Update();

    writer->SetFileName(fn.c_str());
    writer->SetInputConnection(window_to_image_filter->GetOutputPort());
    writer->Write();
  }
  else
  {
    std::cerr << "No filename provided." << std::endl;
  }

  return;
}

} // namespace
