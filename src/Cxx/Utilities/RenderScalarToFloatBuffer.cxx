// Renders a scalar from a polydata into a float buffer
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCameraPass.h>
#include <vtkDataSetReader.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkFloatArray.h>
#include <vtkImageActor.h>
#include <vtkImageMapToColors.h>
#include <vtkImageSliceMapper.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderPassCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSequencePass.h>
#include <vtkValuePass.h>
#include <vtkVersion.h>
#include <vtkWindowToImageFilter.h>
#include <vtkXMLImageDataWriter.h>

int main(int argc, char* argv[])
{
  if (argc < 4)
  {
    std::cout << "Usage: " << argv[0]
              << " input(.vtk) array_name output(.vti) e.g. uGridEx.vtk "
                 "scalars RenderScalarToFloatBuffer.vti"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyDataMapper> mapper;
  vtkNew<vtkActor> actor;
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderWindowInteractor> iRen;
  vtkNew<vtkDataSetReader> fileReader;

  // Read .vtk file
  fileReader->SetFileName(argv[1]);

  // Render the scalar into an image
  vtkNew<vtkDataSetSurfaceFilter> surface;
  surface->SetInputConnection(fileReader->GetOutputPort());
  mapper->SetInputConnection(surface->GetOutputPort());
  actor->SetMapper(mapper);
  renderer->AddActor(actor);
  renWin->AddRenderer(renderer);
  iRen->SetRenderWindow(renWin);
  renWin->SetMultiSamples(0);
  renderer->ResetCamera();
  vtkCamera* camera = renderer->GetActiveCamera();
  camera->Elevation(-90);

  vtkNew<vtkValuePass> valuePass;
#if VTK_MAJOR_VERSION < 9
  valuePass->SetRenderingMode(vtkValuePass::FLOATING_POINT);
#endif
  valuePass->SetInputArrayToProcess(VTK_SCALAR_MODE_USE_POINT_FIELD_DATA,
                                    argv[2]);
  valuePass->SetInputComponentToProcess(0);
  vtkNew<vtkRenderPassCollection> passes;
  passes->AddItem(valuePass);
  vtkNew<vtkSequencePass> sequence;
  sequence->SetPasses(passes);
  vtkNew<vtkCameraPass> cameraPass;
  cameraPass->SetDelegatePass(sequence);
  renderer->SetPass(cameraPass);
  renWin->Render();
  // Get the rendered image
  vtkFloatArray* buffer = valuePass->GetFloatImageDataArray(renderer);
  int* ext = valuePass->GetFloatImageExtents();
  buffer->SetName(argv[2]);
  vtkNew<vtkImageData> image;
  image->SetExtent(ext);
  image->GetPointData()->SetScalars(buffer);

  // Write the image
  vtkNew<vtkXMLImageDataWriter> writer;
  writer->SetFileName(argv[3]);
  writer->SetInputDataObject(image);
  writer->Write();

  // Show the image on the screen
  renWin->RemoveRenderer(renderer);
  vtkNew<vtkLookupTable> lut;
  lut->SetHueRange(0.6, 0);
  lut->SetSaturationRange(1.0, 0);
  lut->SetValueRange(0.5, 1.0);
  lut->SetTableRange(image->GetScalarRange());
  vtkNew<vtkImageMapToColors> mapColors;
  mapColors->SetLookupTable(lut);
  mapColors->SetInputDataObject(image);
  vtkNew<vtkImageActor> imageActor;
  imageActor->GetMapper()->SetInputConnection(mapColors->GetOutputPort());
  vtkNew<vtkRenderer> ren;
  ren->AddActor(imageActor);
  ren->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
  renWin->AddRenderer(ren);
  renWin->SetWindowName("RenderScalarToFloatBuffer");
  renWin->Render();
  iRen->Start();

  return EXIT_SUCCESS;
}
