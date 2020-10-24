#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRIBExporter.h>
#include <vtkRIBProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>

#include <vtksys/SystemTools.hxx>

// static vtkSmartPointer<vtkRIBProperty> spatter(const char *sizes,
//                                                const char *specksize,
//                                                const char *spattercolor,
//                                                const char *basecolor);
namespace {
vtkSmartPointer<vtkRIBProperty> rmarble(const char* veining);
}
int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Parse command line arguments
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " Filename(.vtp) [freqency]"
              << std::endl;
    return EXIT_FAILURE;
  }
  const char* freq = "1";
  if (argc > 2)
  {
    freq = argv[2];
  }

  std::string filename = argv[1];
  std::string prefix =
      vtksys::SystemTools::GetFilenameWithoutExtension(filename);

  // Read all the data from the file
  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();

  vtkNew<vtkPolyDataNormals> normals;
  normals->SetInputConnection(reader->GetOutputPort());

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(normals->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  auto marble = rmarble(freq);
  actor->SetProperty(marble);
  actor->GetProperty()->SetColor(colors->GetColor3d("NavajoWhite").GetData());

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("PolyDataRIB");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

  // Generate an interesting view
  renderer->GetActiveCamera()->SetPosition(1, 0, 0);
  renderer->GetActiveCamera()->SetFocalPoint(0, 1, 0);
  renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Dolly(1.0);
  renderer->ResetCameraClippingRange();

  renderWindow->Render();
  renderWindowInteractor->Start();

  prefix = prefix + "_" + freq;
  vtkNew<vtkRIBExporter> aRib;
  aRib->BackgroundOn();
  aRib->SetInput(renderWindow);
  aRib->SetFilePrefix(prefix.c_str());
  aRib->Write();
  std::cout << "Exported RIB file is: " << prefix << ".rib" << std::endl;
  if (getenv("RMANTREE") == NULL)
  {
    std::cout << "To render the generated rib file, set the environment "
                 "variable RMANTREE to the base of your RenderMan install"
              << std::endl;
  }
  else
  {
    std::string rmantree = getenv("RMANTREE");
    std::cout << "To create a tif file run: " << std::endl;
    std::cout << rmantree << "bin/prman " << prefix << ".rib " << std::endl;
  }
  return EXIT_SUCCESS;
}

// vtkSmartPointer<vtkRIBProperty> spatter(const char *sizes,
//                                         const char *specksize,
//                                         const char *spattercolor,
//                                         const char *basecolor)
// {
//   vtkNew<vtkRIBProperty> spatterProp;
//   spatterProp->SetVariable("sizes", "float");
//   spatterProp->AddVariable("specksize", "float");
//   spatterProp->AddVariable("spattercolor", "color");
//   spatterProp->AddVariable("basecolor", "color");

//   spatterProp->SetSurfaceShaderParameter("sizes", sizes);
//   spatterProp->AddSurfaceShaderParameter("specksize", specksize);
//   spatterProp->AddSurfaceShaderParameter("spattercolor", spattercolor);
//   spatterProp->AddSurfaceShaderParameter("basecolor", basecolor);
//   spatterProp->SetSurfaceShader("spatter");

//   return spatterProp;
// }

namespace {
vtkSmartPointer<vtkRIBProperty> rmarble(const char* veining)
{
  vtkNew<vtkRIBProperty> rmarbleProp;
  rmarbleProp->SetVariable("veining", "float");
  rmarbleProp->SetSurfaceShaderParameter("veining", veining);
  rmarbleProp->SetSurfaceShader("rmarble");

  return rmarbleProp;
}
} // namespace
