#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkFloatArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNew.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkWeightedTransformFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Use a sphere as a basis of the shape
  vtkNew<vtkSphereSource> sphere;
  sphere->SetPhiResolution(40);
  sphere->SetThetaResolution(40);
  sphere->Update();

  vtkPolyData* sphereData = sphere->GetOutput();

  // Create a data array to hold the weighting coefficients
  vtkNew<vtkFloatArray> tfarray;
  vtkIdType npoints = sphereData->GetNumberOfPoints();
  tfarray->SetNumberOfComponents(2);
  tfarray->SetNumberOfTuples(npoints);

  // Parameterize the sphere along the z axis, and fill the weights
  // with (1.0-a, a) to linearly interpolate across the shape
  for (int i = 0; i < npoints; i++)
  {
    double pt[3];
    sphereData->GetPoint(i, pt);
    //    double x = pt[0];
    //    double y = pt[1];
    double z = pt[2];

    double zn = z + 0.5;
    double zn1 = 1.0 - zn;
    if (zn > 1.0)
    {
      zn = 1.0;
    }
    if (zn1 < 0.0)
    {
      zn1 = 0.0;
    }

    tfarray->SetComponent(i, 0, zn1);
    tfarray->SetComponent(i, 1, zn);
  }

  // Create field data to hold the array, and bind it to the sphere
  //  vtkNew<vtkFieldData> fd;
  tfarray->SetName("weights");
  sphereData->GetPointData()->AddArray(tfarray);

  // Use an ordinary transform to stretch the shape
  vtkNew<vtkTransform> stretch;
  stretch->Scale(1, 1, 3.2);

  vtkNew<vtkTransformFilter> stretchFilter;
  stretchFilter->SetInputData(sphereData);
  stretchFilter->SetTransform(stretch);

  // Now, for the weighted transform stuff
  vtkNew<vtkWeightedTransformFilter> weightedTrans;

  // Create two transforms to interpolate between
  vtkNew<vtkTransform> identity;
  identity->Identity();

  vtkNew<vtkTransform> rotated;
  double rotatedAngle = 45;
  rotated->RotateX(rotatedAngle);

  weightedTrans->SetNumberOfTransforms(2);
  weightedTrans->SetTransform(identity, 0);
  weightedTrans->SetTransform(rotated, 1);
  // which data array should the filter use ?
  weightedTrans->SetWeightArray("weights");

  weightedTrans->SetInputConnection(stretchFilter->GetOutputPort());

  vtkNew<vtkPolyDataMapper> weightedTransMapper;
  weightedTransMapper->SetInputConnection(weightedTrans->GetOutputPort());
  vtkNew<vtkActor> weightedTransActor;
  weightedTransActor->SetMapper(weightedTransMapper);
  weightedTransActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Banana").GetData());
  weightedTransActor->GetProperty()->SetRepresentationToSurface();

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("WeightedTransformFilter");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->AddActor(weightedTransActor);
  renderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());
  renderWindow->SetSize(300, 300);

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(90);
  renderer->GetActiveCamera()->Dolly(1);

  renderWindow->Render();

  interactor->Start();

  return EXIT_SUCCESS;
}
