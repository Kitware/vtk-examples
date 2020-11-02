#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRectilinearGrid.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkTransform.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkXMLRectilinearGridWriter.h>
#include <vtkXMLStructuredGridWriter.h>

namespace {
void ImageData(vtkImageData* data, const int gridSize);
void RectilinearGrid(vtkRectilinearGrid* data, const int gridSize);
void StructuredGrid(vtkStructuredGrid* data, const int gridSize);
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  int gridSize = 5;
  vtkNew<vtkImageData> imageData;
  ImageData(imageData, gridSize);
  vtkNew<vtkDataSetMapper> imageDataMapper;
  imageDataMapper->SetInputData(imageData);
  imageDataMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> imageDataActor;
  imageDataActor->GetProperty()->SetRepresentationToWireframe();
  imageDataActor->SetMapper(imageDataMapper);
  imageDataActor->GetProperty()->SetColor(
      colors->GetColor3d("MidnightBlue").GetData());

  vtkNew<vtkRectilinearGrid> rectilinearGrid;
  RectilinearGrid(rectilinearGrid, gridSize);
  vtkNew<vtkDataSetMapper> rectilinearGridMapper;
  rectilinearGridMapper->SetInputData(rectilinearGrid);

  vtkNew<vtkActor> rectilinearGridActor;
  rectilinearGridActor->GetProperty()->SetRepresentationToWireframe();
  rectilinearGridActor->SetMapper(rectilinearGridMapper);
  rectilinearGridActor->GetProperty()->SetColor(
      colors->GetColor3d("MidnightBlue").GetData());

  vtkNew<vtkStructuredGrid> structuredGrid;
  StructuredGrid(structuredGrid, gridSize);
  vtkNew<vtkDataSetMapper> structuredGridMapper;
  structuredGridMapper->SetInputData(structuredGrid);

  vtkNew<vtkActor> structuredGridActor;
  structuredGridActor->GetProperty()->SetRepresentationToWireframe();
  structuredGridActor->SetMapper(structuredGridMapper);
  structuredGridActor->GetProperty()->SetColor(
      colors->GetColor3d("MidnightBlue").GetData());

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("StructuredDataTypes");

  // And one interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.33, 1.0};
  double centerViewport[4] = {0.33, 0.0, 0.66, 1.0};
  double rightViewport[4] = {0.66, 0.0, 1.0, 1.0};

  // Setup three renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("BurlyWood").GetData());

  vtkNew<vtkRenderer> centerRenderer;
  renderWindow->AddRenderer(centerRenderer);
  centerRenderer->SetViewport(centerViewport);
  centerRenderer->SetBackground(colors->GetColor3d("CadetBlue").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("Plum").GetData());

  // Add the image on the left, rectilinear in the center, structured on the
  // right
  leftRenderer->AddActor(imageDataActor);
  centerRenderer->AddActor(rectilinearGridActor);
  rightRenderer->AddActor(structuredGridActor);

  leftRenderer->ResetCamera();
  centerRenderer->ResetCamera();
  rightRenderer->ResetCamera();
  // rightRenderer->GetActiveCamera()->Azimuth(30);
  // rightRenderer->GetActiveCamera()->Elevation(30);

  renderWindow->Render();
  interactor->Start();
  return EXIT_SUCCESS;
}

namespace {

void ImageData(vtkImageData* data, const int gridSize)
{

  data->SetExtent(0, gridSize - 1, 0, gridSize - 1, 0, gridSize - 1);
  data->AllocateScalars(VTK_DOUBLE, 1);

  vtkNew<vtkXMLImageDataWriter> writer;
  writer->SetFileName("imagedata.vti");
  writer->SetInputData(data);
  writer->Write();
}

void RectilinearGrid(vtkRectilinearGrid* data, const int gridSize)
{

  data->SetExtent(0, gridSize - 1, 0, gridSize - 1, 0, gridSize - 1);

  vtkNew<vtkDoubleArray> xCoords;
  xCoords->SetNumberOfComponents(1);
  vtkNew<vtkDoubleArray> yCoords;
  yCoords->SetNumberOfComponents(1);
  vtkNew<vtkDoubleArray> zCoords;
  zCoords->SetNumberOfComponents(1);

  for (int i = 0; i < gridSize; i++)
  {
    if (i == 0)
    {
      xCoords->InsertNextValue(0);
      yCoords->InsertNextValue(0);
      zCoords->InsertNextValue(0);
      continue;
    }
    double oldX = xCoords->GetValue(i - 1);
    double oldY = xCoords->GetValue(i - 1);
    double oldZ = xCoords->GetValue(i - 1);
    xCoords->InsertNextValue(oldX + i * i);
    yCoords->InsertNextValue(oldY + i * i);
    zCoords->InsertNextValue(oldZ + i * i);
  }

  data->SetXCoordinates(xCoords);
  data->SetYCoordinates(yCoords);
  data->SetZCoordinates(zCoords);

  vtkNew<vtkXMLRectilinearGridWriter> writer;
  writer->SetFileName("rectilineargrid.vtr");
  writer->SetInputData(data);
  writer->Write();
}

void StructuredGrid(vtkStructuredGrid* data, const int gridSize)
{
  data->SetExtent(0, gridSize - 1, 0, gridSize - 1, 0, gridSize - 1);
  vtkNew<vtkPoints> points;

  vtkNew<vtkTransform> transform;
  transform->RotateZ(30);

  for (int k = 0; k < gridSize; k++)
  {
    for (int j = 0; j < gridSize; j++)
    {
      for (int i = 0; i < gridSize; i++)
      {
        double p[4];
        p[0] = i;
        p[1] = j;
        p[2] = k;
        p[3] = 1;
        double pout[4];
        transform->MultiplyPoint(p, pout);

        points->InsertNextPoint(pout[0], pout[1], pout[2]);
      }
    }
  }

  data->SetPoints(points);

  vtkNew<vtkXMLStructuredGridWriter> writer;
  writer->SetFileName("structuredgrid.vts");
  writer->SetInputData(data);
  writer->Write();
}
} // namespace
