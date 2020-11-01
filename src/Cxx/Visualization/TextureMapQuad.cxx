#include <string>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkJPEGReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>

int main(int argc, char* argv[])
{
  // Parse command line arguments
  if (argc != 2)
  {
    std::cerr << "Usage:" << argv[0] << " Filename e.g. Yinyang.jpg"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  std::string inputFilename = argv[1];

  // Read the image which will be the texture
  vtkNew<vtkJPEGReader> jPEGReader;
  jPEGReader->SetFileName(inputFilename.c_str());

  // Create a plane
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(0.0, 0.0, 0.0);
  points->InsertNextPoint(1.0, 0.0, 0.0);
  points->InsertNextPoint(1.0, 1.0, 0.0);
  points->InsertNextPoint(0.0, 2.0, 0.0);

  vtkNew<vtkCellArray> polygons;
  vtkNew<vtkPolygon> polygon;
  polygon->GetPointIds()->SetNumberOfIds(4); // make a quad
  polygon->GetPointIds()->SetId(0, 0);
  polygon->GetPointIds()->SetId(1, 1);
  polygon->GetPointIds()->SetId(2, 2);
  polygon->GetPointIds()->SetId(3, 3);

  polygons->InsertNextCell(polygon);

  vtkNew<vtkPolyData> quad;
  quad->SetPoints(points);
  quad->SetPolys(polygons);

  vtkNew<vtkFloatArray> textureCoordinates;
  textureCoordinates->SetNumberOfComponents(2);
  textureCoordinates->SetName("TextureCoordinates");

  float tuple[2] = {0.0, 0.0};
  textureCoordinates->InsertNextTuple(tuple);
  tuple[0] = 1.0;
  tuple[1] = 0.0;
  textureCoordinates->InsertNextTuple(tuple);
  tuple[0] = 1.0;
  tuple[1] = 1.0;
  textureCoordinates->InsertNextTuple(tuple);
  tuple[0] = 0.0;
  tuple[1] = 2.0;
  textureCoordinates->InsertNextTuple(tuple);

  quad->GetPointData()->SetTCoords(textureCoordinates);

  // Apply the texture
  vtkNew<vtkTexture> texture;
  texture->SetInputConnection(jPEGReader->GetOutputPort());

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(quad);

  vtkNew<vtkActor> texturedQuad;
  texturedQuad->SetMapper(mapper);
  texturedQuad->SetTexture(texture);
  texturedQuad->GetProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  // Visualize the textured plane
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(texturedQuad);
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
  renderer->ResetCamera();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("TextureMapQuad");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
