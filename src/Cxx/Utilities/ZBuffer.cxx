// This demo creates depth map for a polydata instance by extracting
// exact ZBuffer values.
#include <vtkActor.h>
#include <vtkBMPWriter.h>
#include <vtkImageShiftScale.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVersion.h>
#include <vtkWindowToImageFilter.h>
#include <vtkXMLPolyDataReader.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " input(.vtp) output(.bmp)"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkPolyDataMapper> mapper;
  vtkNew<vtkActor> actor;
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderWindowInteractor> interactor;
  vtkNew<vtkXMLPolyDataReader> fileReader;
  vtkNew<vtkWindowToImageFilter> filter;
  vtkNew<vtkBMPWriter> imageWriter;
  vtkNew<vtkImageShiftScale> scale;

  // Read .vtp file
  fileReader->SetFileName(argv[1]);

  // Build visualization enviroment
  mapper->SetInputConnection(fileReader->GetOutputPort());
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Tan").GetData());

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  renWin->AddRenderer(renderer);
  renWin->SetWindowName("WindowModifiedEvent");

  interactor->SetRenderWindow(renWin);
  renWin->Render();

  // Create Depth Map
  filter->SetInput(renWin);
#if VTK_MAJOR_VERSION >= 8 || VTK_MAJOR_VERSION == 8 && VTK_MINOR_VERSION >= 90
  filter->SetScale(1);
#else
  filter->SetMagnification(1);
#endif
  filter->SetInputBufferTypeToZBuffer(); // Extract z buffer value

  scale->SetOutputScalarTypeToUnsignedChar();
  scale->SetInputConnection(filter->GetOutputPort());
  scale->SetShift(0);
  scale->SetScale(-255);

  // Write depth map as a .bmp image
  imageWriter->SetFileName(argv[2]);
  imageWriter->SetInputConnection(scale->GetOutputPort());
  imageWriter->Write();

  return EXIT_SUCCESS;
}
