//
// This example reads ascii files where each line consists of points with its
// position (x,y,z) and (optionally) one scalar or binary files in RAW 3d file
// format.
//
// some standard vtk headers
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParticleReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

// needed to easily convert int to std::string
int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " Filename(.raw) e.g. Particles.raw"
              << std::endl;
    return EXIT_FAILURE;
  }

  std::string filePath = argv[1];
  // Particles.raw supplied by VTK is big endian encoded
  // std::string filePath = "C:\\VTK\\vtkdata-5.8.0\\Data\\Particles.raw";
  // Read the file
  vtkNew<vtkParticleReader> reader;

  reader->SetFileName(filePath.c_str());
  // if nothing gets displayed or totally wrong, swap the endianness
  reader->SetDataByteOrderToBigEndian();
  reader->Update();

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(reader->GetOutputPort());
  std::cout << "number of pieces: " << mapper->GetNumberOfPieces() << std::endl;
  mapper->SetScalarRange(4, 9);

  vtkNew<vtkActor> actor;

  actor->SetMapper(mapper);
  actor->GetProperty()->SetPointSize(4);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("ParticleReader");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
