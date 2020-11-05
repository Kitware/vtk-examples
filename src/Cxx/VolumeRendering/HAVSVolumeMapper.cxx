#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkContourFilter.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkHAVSVolumeMapper.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSLCReader.h>
#include <vtkStdString.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkThreshold.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << "Required arguments: volumeVTKFile contourVTKFile"
              << std::endl;
    return EXIT_FAILURE;
  }

  // This is the data that will be volume rendered.
  std::string volumeFilename = argv[1]; // "/Data/ironProt.vtk";

  // Create a reader for the other data that will be contoured and
  // displayed as a polygonal mesh.
  std::string contourFilename = argv[2]; // "/Data/neghip.slc";

  // Create the standard renderer, render window, and interactor.
  vtkNew<vtkRenderer> ren1;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);
  iren->SetDesiredUpdateRate(3);

  // Create the reader for the data.
  vtkNew<vtkStructuredPointsReader> reader;
  reader->SetFileName(volumeFilename.c_str());

  vtkNew<vtkSLCReader> reader2;
  reader2->SetFileName(contourFilename.c_str());

  // Convert from vtkImageData to vtkUnstructuredGrid.
  // Remove any cells where all values are below 80.
  vtkNew<vtkThreshold> thresh;
  thresh->ThresholdByUpper(80);
  thresh->AllScalarsOff();
  thresh->SetInputConnection(reader->GetOutputPort());

  // Make sure we have only tetrahedra.
  vtkNew<vtkDataSetTriangleFilter> trifilter;
  trifilter->SetInputConnection(thresh->GetOutputPort());

  // Create transfer mapping scalar value to opacity.
  vtkNew<vtkPiecewiseFunction> opacityTransferFunction;
  opacityTransferFunction->AddPoint(80.0, 0.0);
  opacityTransferFunction->AddPoint(120.0, 0.2);
  opacityTransferFunction->AddPoint(255.0, 0.2);

  // Create transfer mapping scalar value to color.
  vtkNew<vtkColorTransferFunction> colorTransferFunction;
  colorTransferFunction->AddRGBPoint(80.0, 0.0, 0.0, 0.0);
  colorTransferFunction->AddRGBPoint(120.0, 0.0, 0.0, 1.0);
  colorTransferFunction->AddRGBPoint(160.0, 1.0, 0.0, 0.0);
  colorTransferFunction->AddRGBPoint(200.0, 0.0, 1.0, 0.0);
  colorTransferFunction->AddRGBPoint(255.0, 0.0, 1.0, 1.0);

  // The property describes how the data will look.
  vtkNew<vtkVolumeProperty> volumeProperty;
  volumeProperty->SetColor(colorTransferFunction);
  volumeProperty->SetScalarOpacity(opacityTransferFunction);
  volumeProperty->ShadeOff();
  volumeProperty->SetInterpolationTypeToLinear();

  // The mapper that renders the volume data.
  vtkNew<vtkHAVSVolumeMapper> volumeMapper;
  volumeMapper->SetInputConnection(trifilter->GetOutputPort());
  volumeMapper->SetLevelOfDetail(false);
  volumeMapper->SetGPUDataStructures(false);
  volumeMapper->SetKBufferSizeTo2();

  // The volume holds the mapper and the property and can be used to
  // position/orient the volume.
  vtkNew<vtkVolume> volume;
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);

  // Contour the second dataset.
  vtkNew<vtkContourFilter> contour;
  contour->SetValue(0, 80);
  contour->SetInputConnection(reader2->GetOutputPort());

  // Create a mapper for the polygonal data.
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(contour->GetOutputPort());
  mapper->ScalarVisibilityOff();

  // Create an actor for the polygonal data.
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // First test if mapper is supported
  renWin->SetSize(300, 300);
  renWin->Render();

  int supported = volumeMapper->SupportedByHardware(ren1);

  vtkNew<vtkTextActor> textActor;
  textActor->SetInput("Required Extensions\nNot Supported");
  textActor->SetDisplayPosition(150, 150);
  textActor->GetTextProperty()->SetJustificationToCentered();
  textActor->GetTextProperty()->SetFontSize(30);

  if (!supported)
  {
    ren1->AddViewProp(textActor);
  }
  else
  {
    ren1->AddViewProp(actor);
    ren1->AddViewProp(volume);
  }

  ren1->ResetCamera();
  ren1->GetActiveCamera()->Azimuth(20.0);
  ren1->GetActiveCamera()->Elevation(10.0);
  ren1->GetActiveCamera()->Zoom(1.5);

  // Test default settings
  renWin->Render();

  // Test kbuffer size 6
  volumeMapper->SetKBufferSizeTo6();
  renWin->Render();

  // Test GPU Data structures
  volumeMapper->SetGPUDataStructures(true);
  renWin->Render();

  // Test Field Level of Detail
  volumeMapper->SetLevelOfDetail(true);
  volumeMapper->SetLevelOfDetailMethodField();
  renWin->Render();

  // Test Area Level of Detail
  volumeMapper->SetLevelOfDetailMethodArea();
  renWin->Render();

  // Return to default KBuffer size and Level of Detail
  volumeMapper->SetLevelOfDetail(false);
  volumeMapper->SetKBufferSizeTo2();
  renWin->Render();

  iren->Start();

  return EXIT_SUCCESS;
}
