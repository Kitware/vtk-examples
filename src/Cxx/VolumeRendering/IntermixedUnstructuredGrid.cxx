/* converted from:
 *
https://github.com/Kitware/VTK/blob/49fbbe479dfdf47c86c02d6cae48c41b9c844b04/Examples/VolumeRendering/Tcl/IntermixedUnstructuredGrid.tcl
*
*/

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkContourFilter.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSLCReader.h>
#include <vtkStructuredPointsReader.h>
#include <vtkThreshold.h>
#include <vtkUnstructuredGridVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0]
              << " file.vtk file.slc e.g. ironProt.vtk neghip.slc" << std::endl;
    return EXIT_FAILURE;
  }

  // Create the reader for the data
  // This is the data the will be volume rendered
  vtkNew<vtkStructuredPointsReader> reader;
  reader->SetFileName(argv[1]);

  // create a reader for the other data that will
  // be contoured and displayed as a polygonal mesh
  vtkNew<vtkSLCReader> reader2;
  reader2->SetFileName(argv[2]);

  // convert from vtkImageData to vtkUnstructuredGrid, remove
  // any cells where all values are below 80
  vtkNew<vtkThreshold> thresh;
  thresh->SetUpperThreshold(80);
  thresh->SetThresholdFunction(vtkThreshold::THRESHOLD_UPPER);
  //thresh->ThresholdByUpper(80);
  thresh->AllScalarsOff();
  thresh->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkDataSetTriangleFilter> trifilter;
  trifilter->SetInputConnection(thresh->GetOutputPort());

  // Create transfer mapping scalar value to opacity
  vtkNew<vtkPiecewiseFunction> opacityTransferFunction;
  opacityTransferFunction->AddPoint(80, 0.0);
  opacityTransferFunction->AddPoint(120, 0.2);
  opacityTransferFunction->AddPoint(255, 0.2);

  // Create transfer mapping scalar value to color
  vtkNew<vtkColorTransferFunction> colorTransferFunction;
  colorTransferFunction->AddRGBPoint(80.0, 0.0, 0.0, 0.0);
  colorTransferFunction->AddRGBPoint(120.0, 0.0, 0.0, 1.0);
  colorTransferFunction->AddRGBPoint(160.0, 1.0, 0.0, 0.0);
  colorTransferFunction->AddRGBPoint(200.0, 0.0, 1.0, 0.0);
  colorTransferFunction->AddRGBPoint(255.0, 0.0, 1.0, 1.0);

  // The property describes how the data will look
  vtkNew<vtkVolumeProperty> volumeProperty;
  volumeProperty->SetColor(colorTransferFunction);
  volumeProperty->SetScalarOpacity(opacityTransferFunction);
  volumeProperty->ShadeOff();
  volumeProperty->SetInterpolationTypeToLinear();

  // The mapper / ray cast function know how to render the data
  vtkNew<vtkUnstructuredGridVolumeRayCastMapper> volumeMapper;
  volumeMapper->SetInputConnection(trifilter->GetOutputPort());

  vtkNew<vtkRenderWindow> renWin;
  renWin->SetSize(640, 512);
  vtkNew<vtkRenderer> ren1;

  // contour the second dataset
  vtkNew<vtkContourFilter> contour;
  contour->SetValue(0, 80);
  contour->SetInputConnection(reader2->GetOutputPort());

  // create a mapper for the polygonal data
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(contour->GetOutputPort());
  mapper->ScalarVisibilityOff();

  // create an actor for the polygonal data
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  ren1->AddViewProp(actor);

  ren1->SetBackground(0.1, 0.4, 0.2);

  renWin->AddRenderer(ren1);
  renWin->SetWindowName("IntermixedUnstructuredGrid");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkVolume> volume;
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);

  ren1->AddVolume(volume);

  ren1->ResetCamera();
  ren1->GetActiveCamera()->Zoom(1.5);

  // Render composite. In default mode. For coverage.
  renWin->Render();

  iren->Start();

  return EXIT_SUCCESS;
}
