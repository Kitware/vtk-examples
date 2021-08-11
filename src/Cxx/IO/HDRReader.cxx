#include "vtkHDRReader.h"
#include "vtkImageData.h"
#include "vtkImageViewer.h"
#include "vtkNew.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include <vtkCallbackCommand.h>

namespace {
class ColorCallback : public vtkCallbackCommand
{
public:
  static ColorCallback* New()
  {
    return new ColorCallback;
  }
  // Here we Create a vtkCallbackCommand and reimplement it.
  void Execute(vtkObject* caller, unsigned long evId, void*) override
  {
    // Note the use of reinterpret_cast to cast the caller to the expected type.
    // Just do this to demonstrate who called callback and the event that
    // triggered it.
    // std::cout << interactor->GetClassName() << "  Event Id: " << evId
    //          << std::endl;

    std::cout << "Color window: " << imageViewer->GetColorWindow();
    std::cout << " level: " << imageViewer->GetColorLevel() << std::endl;
  }
  ColorCallback() : imageViewer(nullptr)
  {
  }
  // Set pointers to any clientData or callData here.
  vtkImageViewer* imageViewer;

private:
  ColorCallback(const ColorCallback&) = delete;
  void operator=(const ColorCallback&) = delete;
};

} // namespace

int main(int argc, char* argv[])
{
  if (argc <= 1)
  {
    cout << "Usage: " << argv[0] << " <hdr file>" << endl;
    cout << "For example: Skyboxes/spiaggia_di_mondello_1k.hdr"
         << std::endl;
    return EXIT_FAILURE;
  }

  std::string filename = argv[1];

  vtkNew<vtkHDRReader> reader;

  // Check the image can be read
  if (!reader->CanReadFile(filename.c_str()))
  {
    cerr << "CanReadFile failed for " << filename.c_str() << "\n";
    return EXIT_FAILURE;
  }

  reader->SetFileName(filename.c_str());
  reader->UpdateInformation();

  // Whole extent
  const int* we = reader->GetDataExtent();
  const int extents[6] = {we[0], we[1], we[2], we[3], 0, 0};
  reader->UpdateExtent(extents);
  // Visualize
  vtkNew<vtkImageViewer> imageViewer;
  imageViewer->SetInputData(reader->GetOutput());

  imageViewer->SetColorWindow(1);
  imageViewer->SetColorLevel(1);
  imageViewer->SetPosition(0, 100);

  vtkNew<ColorCallback> getColorWindow;
  getColorWindow->imageViewer = imageViewer;

  vtkNew<vtkRenderWindowInteractor> iren;
  imageViewer->SetupInteractor(iren);
  imageViewer->GetRenderWindow()->SetWindowName("HDRReader");
  imageViewer->Render();

  iren->AddObserver(vtkCommand::EndInteractionEvent, getColorWindow);

  iren->Start();

  return EXIT_SUCCESS;
}
