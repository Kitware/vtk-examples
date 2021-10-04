//
// GenerateCubesFromLabels
//   Usage: GenerateCubesFromLabels InputVolume Startlabel Endlabel
//          where
//          InputVolume is a meta file containing a 3 volume of
//            discrete labels.
//          StartLabel is the first label to be processed
//          EndLabel is the last label to be processed
//          NOTE: There can be gaps in the labeling. If a label does
//          not exist in the volume, it will be skipped.
//
//
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkGeometryFilter.h>
#include <vtkImageAccumulate.h>
#include <vtkImageData.h>
#include <vtkImageWrapPad.h>
#include <vtkMaskFields.h>
#include <vtkMetaImageReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkThreshold.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkUnstructuredGrid.h>

#include <iostream>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  if (argc < 4)
  {
    std::cout
        << "Usage: " << argv[0]
        << " InputVolume StartLabel EndLabel e.g. Frog/frogtissue.mhd 1 29"
        << std::endl;
    return EXIT_FAILURE;
  }

  // Define all of the variables
  unsigned int startLabel = atoi(argv[2]);
  unsigned int endLabel = atoi(argv[3]);

  // Generate cubes from labels
  // 1) Read the meta file
  // 2) Convert point data to cell data
  // 3) Convert to geometry and display

  vtkNew<vtkMetaImageReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  // Pad the volume so that we can change the point data into cell
  // data.
  int* extent = reader->GetOutput()->GetExtent();
  vtkNew<vtkImageWrapPad> pad;
  pad->SetInputConnection(reader->GetOutputPort());
  pad->SetOutputWholeExtent(extent[0], extent[1] + 1, extent[2], extent[3] + 1,
                            extent[4], extent[5] + 1);
  pad->Update();

  // Copy the scalar point data of the volume into the scalar cell data
  pad->GetOutput()->GetCellData()->SetScalars(
      reader->GetOutput()->GetPointData()->GetScalars());

  vtkNew<vtkThreshold> selector;
  selector->SetInputArrayToProcess(0, 0, 0,
                                   vtkDataObject::FIELD_ASSOCIATION_CELLS,
                                   vtkDataSetAttributes::SCALARS);
  selector->SetInputConnection(pad->GetOutputPort());
  selector->SetLowerThreshold(startLabel);
  selector->SetUpperThreshold(endLabel);
  selector->Update();

  // Shift the geometry by 1/2
  vtkNew<vtkTransform> transform;
  transform->Translate(-0.5, -0.5, -0.5);

  vtkNew<vtkTransformFilter> transformModel;
  transformModel->SetTransform(transform);
  transformModel->SetInputConnection(selector->GetOutputPort());

  vtkNew<vtkGeometryFilter> geometry;
  geometry->SetInputConnection(transformModel->GetOutputPort());

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(geometry->GetOutputPort());
  mapper->SetScalarRange(startLabel, endLabel);
  mapper->SetScalarModeToUseCellData();
  mapper->SetColorModeToMapScalars();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("GenerateCubesFromLabels");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateBlue").GetData());
  renderWindow->Render();

  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(42.301174, 939.893457, -124.005030);
  camera->SetFocalPoint(224.697134, 221.301653, 146.823706);
  camera->SetViewUp(0.262286, -0.281321, -0.923073);
  camera->SetDistance(789.297581);
  camera->SetClippingRange(168.744328, 1509.660206);

  renderWindowInteractor->Start();
  return EXIT_SUCCESS;
}
