#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkImageDataToPointSet.h>
#include <vtkImageShrink3D.h>
#include <vtkMetaImageReader.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiThreshold.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPointSet.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
  vtkNew<vtkMetaImageReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkNew<vtkImageShrink3D> shrink;
  shrink->SetShrinkFactors(4, 4, 4);
  shrink->SetInputConnection(reader->GetOutputPort());
  shrink->Update();

  // Create a PointSet from the ImageData
  vtkNew<vtkImageDataToPointSet> imageDataToPointSet;
  imageDataToPointSet->SetInputConnection(reader->GetOutputPort());
  imageDataToPointSet->SetInputConnection(shrink->GetOutputPort());
  imageDataToPointSet->Update();

  // Extract voxels on the border between the inside and outside.
  vtkNew<vtkMultiThreshold> threshold;
  // Inside points have one or more points above the isosurface
  int insideId = threshold->AddIntervalSet(
      501, 20000, vtkMultiThreshold::CLOSED, vtkMultiThreshold::CLOSED,
      vtkDataObject::FIELD_ASSOCIATION_POINTS, "ImageScalars", 0, 0);
  // Border points have points that straddle the boundary
  int borderId = threshold->AddIntervalSet(
      499.9999, 501.0000, vtkMultiThreshold::OPEN, vtkMultiThreshold::OPEN,
      vtkDataObject::FIELD_ASSOCIATION_POINTS, "ImageScalars", 0, 0);

  threshold->SetInputData(imageDataToPointSet->GetOutput());

  // Select the intervals to be output
  threshold->OutputSet(insideId);
  threshold->OutputSet(borderId);
  threshold->Update();

  // Visualize
  vtkNew<vtkNamedColors> colors;
  // vtkColor3d outsideColor    = colors->GetColor3d("Crimson");
  vtkColor3d insideColor = colors->GetColor3d("Banana");
  vtkColor3d borderColor = colors->GetColor3d("Mint");
  // vtkColor3d surfaceColor    = colors->GetColor3d("Peacock");
  vtkColor3d backgroundColor = colors->GetColor3d("Silver");

  vtkNew<vtkPlane> plane;
  plane->SetOrigin(dynamic_cast<vtkUnstructuredGrid*>(
                       vtkMultiBlockDataSet::SafeDownCast(
                           threshold->GetOutput()->GetBlock(insideId))
                           ->GetBlock(0))
                       ->GetCenter());
  plane->SetNormal(1.0, -1.0, -1.0);

  // Inside
  vtkNew<vtkDataSetMapper> insideMapper;
  insideMapper->SetInputData(dynamic_cast<vtkUnstructuredGrid*>(
      vtkMultiBlockDataSet::SafeDownCast(
          threshold->GetOutput()->GetBlock(insideId))
          ->GetBlock(0)));
  insideMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> insideActor;
  insideActor->SetMapper(insideMapper);
  insideActor->GetProperty()->SetDiffuseColor(insideColor.GetData());
  insideActor->GetProperty()->SetSpecular(.6);
  insideActor->GetProperty()->SetSpecularPower(30);
  insideActor->GetProperty()->EdgeVisibilityOn();

  // Border
  vtkNew<vtkDataSetMapper> borderMapper;
  borderMapper->SetInputData(dynamic_cast<vtkUnstructuredGrid*>(
      vtkMultiBlockDataSet::SafeDownCast(
          threshold->GetOutput()->GetBlock(borderId))
          ->GetBlock(0)));
  borderMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> borderActor;
  borderActor->SetMapper(borderMapper);
  borderActor->GetProperty()->SetDiffuseColor(borderColor.GetData());
  borderActor->GetProperty()->SetSpecular(.6);
  borderActor->GetProperty()->SetSpecularPower(30);
  borderActor->GetProperty()->EdgeVisibilityOn();

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->SetBackground(backgroundColor.GetData());
  renderer->UseHiddenLineRemovalOn();

  renderer->AddActor(insideActor);
  renderer->AddActor(borderActor);

  renderWindow->SetWindowName("CellsOnBoundary");
  renderWindow->Render();

  // Setup a good view
  renderer->GetActiveCamera()->SetViewUp(0, 0, -1);
  renderer->GetActiveCamera()->SetPosition(0, -1, 0);
  renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
  renderer->GetActiveCamera()->Azimuth(30.0);
  renderer->GetActiveCamera()->Elevation(30.0);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Dolly(1.5);

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
