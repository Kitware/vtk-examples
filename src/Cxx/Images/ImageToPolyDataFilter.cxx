#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkImageQuantizeRGBToIndex.h>
#include <vtkImageToPolyDataFilter.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTriangleFilter.h>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  if (argc < 2)
  {
    std::cerr << "Required arguments: filename.png e.g. Gourds.png"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkPNGReader> reader;
  reader->SetFileName(argv[1]);

  vtkNew<vtkImageQuantizeRGBToIndex> quant;
  quant->SetInputConnection(reader->GetOutputPort());
  quant->SetNumberOfColors(16);

  vtkNew<vtkImageToPolyDataFilter> i2pd;
  i2pd->SetInputConnection(quant->GetOutputPort());
  i2pd->SetLookupTable(quant->GetLookupTable());
  i2pd->SetColorModeToLUT();
  i2pd->SetOutputStyleToPolygonalize();
  i2pd->SetError(0);
  i2pd->DecimationOn();
  i2pd->SetDecimationError(0.0);
  i2pd->SetSubImageSize(25);

  // Need a triangle filter because the polygons are complex and concave
  vtkNew<vtkTriangleFilter> tf;
  tf->SetInputConnection(i2pd->GetOutputPort());

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(tf->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetRepresentationToWireframe();

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  renderWindow->SetSize(300, 250);
  renderWindow->SetWindowName("ImageToPolyDataFilter");

  renderWindow->Render();
  interactor->Initialize();
  interactor->Start();

  return EXIT_SUCCESS;
}
