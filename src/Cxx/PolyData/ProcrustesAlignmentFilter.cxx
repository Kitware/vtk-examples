#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkLandmarkTransform.h>
#include <vtkMultiBlockDataGroupFilter.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSet.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProcrustesAlignmentFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // create a sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetPhiResolution(31);
  sphereSource->SetThetaResolution(31);

  // make two copies of the shape and distort them a little
  vtkNew<vtkTransform> transform1;
  transform1->Translate(0.2, 0.1, 0.3);
  transform1->Scale(1.3, 1.1, 0.8);

  vtkNew<vtkTransform> transform2;
  transform2->Translate(0.3, 0.7, 0.1);
  transform2->Scale(1.0, 0.1, 1.8);

  vtkNew<vtkTransformPolyDataFilter> transformer1;
  transformer1->SetInputConnection(sphereSource->GetOutputPort());
  transformer1->SetTransform(transform1);

  vtkNew<vtkTransformPolyDataFilter> transformer2;
  transformer2->SetInputConnection(sphereSource->GetOutputPort());
  transformer2->SetTransform(transform2);

  // map these three shapes into the first renderer
  vtkNew<vtkPolyDataMapper> map1a;
  map1a->SetInputConnection(sphereSource->GetOutputPort());

  vtkNew<vtkActor> Actor1a;
  Actor1a->SetMapper(map1a);
  Actor1a->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkPolyDataMapper> map1b;
  map1b->SetInputConnection(transformer1->GetOutputPort());

  vtkNew<vtkActor> Actor1b;
  Actor1b->SetMapper(map1b);
  Actor1b->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Banana").GetData());

  vtkNew<vtkPolyDataMapper> map1c;
  map1c->SetInputConnection(transformer2->GetOutputPort());
  vtkNew<vtkActor> Actor1c;
  Actor1c->SetMapper(map1c);
  Actor1c->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Peacock").GetData());

  // align the shapes using Procrustes (using SetModeToRigidBody)
  vtkNew<vtkProcrustesAlignmentFilter> procrustes1;
  vtkNew<vtkMultiBlockDataGroupFilter> group;
  group->AddInputConnection(sphereSource->GetOutputPort());
  group->AddInputConnection(transformer1->GetOutputPort());
  group->AddInputConnection(transformer2->GetOutputPort());
  procrustes1->SetInputConnection(group->GetOutputPort());
  procrustes1->GetLandmarkTransform()->SetModeToRigidBody();

  // map the aligned shapes into the second renderer
  vtkNew<vtkDataSetMapper> map2a;

  procrustes1->Update();
  map2a->SetInputData(
      dynamic_cast<vtkDataSet*>(procrustes1->GetOutput()->GetBlock(0)));

  vtkNew<vtkActor> Actor2a;
  Actor2a->SetMapper(map2a);
  Actor2a->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkDataSetMapper> map2b;
  procrustes1->Update();
  map2b->SetInputData(
      dynamic_cast<vtkDataSet*>(procrustes1->GetOutput()->GetBlock(1)));

  vtkNew<vtkActor> Actor2b;
  Actor2b->SetMapper(map2b);
  Actor2b->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Banana").GetData());

  vtkNew<vtkDataSetMapper> map2c;
  procrustes1->Update();
  map2c->SetInputData(
      dynamic_cast<vtkDataSet*>(procrustes1->GetOutput()->GetBlock(2)));

  vtkNew<vtkActor> Actor2c;
  Actor2c->SetMapper(map2c);
  Actor2c->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Peacock").GetData());

  // align the shapes using Procrustes (using SetModeToSimilarity (default))
  vtkNew<vtkProcrustesAlignmentFilter> procrustes2;
  procrustes2->SetInputConnection(group->GetOutputPort());

  // map the aligned shapes into the third renderer
  vtkNew<vtkDataSetMapper> map3a;
  procrustes2->Update();
  map3a->SetInputData(
      dynamic_cast<vtkDataSet*>(procrustes2->GetOutput()->GetBlock(0)));

  vtkNew<vtkActor> Actor3a;
  Actor3a->SetMapper(map3a);
  Actor3a->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkDataSetMapper> map3b;
  procrustes2->Update();
  map3b->SetInputData(
      dynamic_cast<vtkDataSet*>(procrustes2->GetOutput()->GetBlock(1)));

  vtkNew<vtkActor> Actor3b;
  Actor3b->SetMapper(map3b);
  Actor3b->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Banana").GetData());

  vtkNew<vtkDataSetMapper> map3c;
  procrustes2->Update();
  map3c->SetInputData(
      dynamic_cast<vtkDataSet*>(procrustes2->GetOutput()->GetBlock(2)));

  vtkNew<vtkActor> Actor3c;
  Actor3c->SetMapper(map3c);
  Actor3c->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Peacock").GetData());

  // Create the RenderWindow and its three Renderers
  vtkNew<vtkRenderer> ren1;
  vtkNew<vtkRenderer> ren2;
  vtkNew<vtkRenderer> ren3;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->AddRenderer(ren2);
  renWin->AddRenderer(ren3);
  renWin->SetSize(600, 300);
  renWin->SetWindowName("ProcrustesAlignmentFilter");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renWin);

  // Add the actors to the renderer
  ren1->AddActor(Actor1a);
  ren1->AddActor(Actor1b);
  ren1->AddActor(Actor1c);

  ren2->AddActor(Actor2a);
  ren2->AddActor(Actor2b);
  ren2->AddActor(Actor2c);

  ren3->AddActor(Actor3a);
  ren3->AddActor(Actor3b);
  ren3->AddActor(Actor3c);

  // set the properties of the renderers

  ren1->SetBackground(colors->GetColor3d("Bisque").GetData());
  ren1->SetViewport(0.0, 0.0, 0.33, 1.0);
  ren1->GetActiveCamera()->SetPosition(1, -1, 0);
  ren1->ResetCamera();

  ren2->SetBackground(colors->GetColor3d("Cornsilk").GetData());
  ren2->SetViewport(0.33, 0.0, 0.66, 1.0);
  ren2->ResetCamera();
  ren2->GetActiveCamera()->SetPosition(1, -1, 0);
  ren2->ResetCamera();

  ren3->SetBackground(colors->GetColor3d("Eggshell").GetData());
  ren3->SetViewport(0.66, 0.0, 1.0, 1.0);
  ren3->ResetCamera();
  ren3->GetActiveCamera()->SetPosition(1, -1, 0);
  ren3->ResetCamera();

  renWin->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
