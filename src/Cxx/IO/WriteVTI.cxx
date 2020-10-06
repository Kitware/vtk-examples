#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkXMLImageDataReader.h>
#include <vtkImageData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>

int main(int argc, char *argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Parse command line arguments
  if (argc != 2)
  {
    std::cout << "Required arguments: filename.vti" << std::endl;
    return EXIT_FAILURE;
  }

  std::string filename = argv[1];

  vtkNew<vtkImageData> imageData;
  imageData->SetDimensions(3, 4, 5);
  imageData->AllocateScalars(VTK_DOUBLE, 1);

  int* dims = imageData->GetDimensions();

  // Fill every entry of the image data with "2.0"
  for (int z = 0; z < dims[2]; z++)
  {
    for (int y = 0; y < dims[1]; y++)
    {
      for (int x = 0; x < dims[0]; x++)
      {
        double* pixel =
            static_cast<double*>(imageData->GetScalarPointer(x, y, z));
        pixel[0] = 2.0;
      }
    }
  }

  vtkNew<vtkXMLImageDataWriter> writer;
  writer->SetFileName(filename.c_str());
  writer->SetInputData(imageData);
  writer->Write();

  // Read the file (to test that it was written correctly)
  vtkNew<vtkXMLImageDataReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();

  // Convert the image to a polydata
  vtkNew<vtkImageDataGeometryFilter> imageDataGeometryFilter;
  imageDataGeometryFilter->SetInputConnection(reader->GetOutputPort());
  imageDataGeometryFilter->Update();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(imageDataGeometryFilter->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetPointSize(3);


  // Setup rendering
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("AliceBlue").GetData());
  renderer->ResetCamera();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("WriteVTI");


  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
