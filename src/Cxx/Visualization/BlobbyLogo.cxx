//
// use implicit modeller to create the VTK logo
//

#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkContourFilter.h>
#include <vtkImplicitModeller.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

int main(int argc, char* argv[])
{
  if (argc < 4)
  {
    std::cout << "Usage: " << argv[0] << " v.vtk t.vtk k.vtk" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkRenderer> aRenderer;
  vtkNew<vtkRenderWindow> aRenderWindow;
  aRenderWindow->AddRenderer(aRenderer);
  aRenderWindow->SetWindowName("BlobbyLogo");

  vtkNew<vtkRenderWindowInteractor> anInteractor;
  anInteractor->SetRenderWindow(aRenderWindow);
  aRenderWindow->SetSize(300, 300);

  // read the geometry file containing the letter v
  vtkNew<vtkPolyDataReader> letterV;
  letterV->SetFileName(argv[1]);

  // read the geometry file containing the letter t
  vtkNew<vtkPolyDataReader> letterT;
  letterT->SetFileName(argv[2]);

  // read the geometry file containing the letter k
  vtkNew<vtkPolyDataReader> letterK;
  letterK->SetFileName(argv[3]);

  // create a transform and transform filter for each letter
  vtkNew<vtkTransform> VTransform;
  vtkNew<vtkTransformPolyDataFilter> VTransformFilter;
  VTransformFilter->SetInputConnection(letterV->GetOutputPort());
  VTransformFilter->SetTransform(VTransform);

  vtkNew<vtkTransform> TTransform;
  vtkNew<vtkTransformPolyDataFilter> TTransformFilter;
  TTransformFilter->SetInputConnection(letterT->GetOutputPort());
  TTransformFilter->SetTransform(TTransform);

  vtkNew<vtkTransform> KTransform;
  vtkNew<vtkTransformPolyDataFilter> KTransformFilter;
  KTransformFilter->SetInputConnection(letterK->GetOutputPort());
  KTransformFilter->SetTransform(KTransform);

  // now append them all
  vtkNew<vtkAppendPolyData> appendAll;
  appendAll->AddInputConnection(VTransformFilter->GetOutputPort());
  appendAll->AddInputConnection(TTransformFilter->GetOutputPort());
  appendAll->AddInputConnection(KTransformFilter->GetOutputPort());

  // create normals
  vtkNew<vtkPolyDataNormals> logoNormals;
  logoNormals->SetInputConnection(appendAll->GetOutputPort());
  logoNormals->SetFeatureAngle(60);

  // map to rendering primitives
  vtkNew<vtkPolyDataMapper> logoMapper;
  logoMapper->SetInputConnection(logoNormals->GetOutputPort());

  // now an actor
  vtkNew<vtkActor> logo;
  logo->SetMapper(logoMapper);

  // now create an implicit model of the same letter
  vtkNew<vtkImplicitModeller> blobbyLogoImp;
  blobbyLogoImp->SetInputConnection(appendAll->GetOutputPort());
  blobbyLogoImp->SetMaximumDistance(.075);
  blobbyLogoImp->SetSampleDimensions(64, 64, 64);
  blobbyLogoImp->SetAdjustDistance(0.05);

  // extract an iso surface
  vtkNew<vtkContourFilter> blobbyLogoIso;
  blobbyLogoIso->SetInputConnection(blobbyLogoImp->GetOutputPort());
  blobbyLogoIso->SetValue(1, 1.5);

  // map to rendering primitives
  vtkNew<vtkPolyDataMapper> blobbyLogoMapper;
  blobbyLogoMapper->SetInputConnection(blobbyLogoIso->GetOutputPort());
  blobbyLogoMapper->ScalarVisibilityOff();

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkProperty> tomato;
  tomato->SetDiffuseColor(colors->GetColor3d("tomato").GetData());
  tomato->SetSpecular(.3);
  tomato->SetSpecularPower(20);

  vtkNew<vtkProperty> banana;
  banana->SetDiffuseColor(colors->GetColor3d("banana").GetData());
  banana->SetDiffuse(.7);
  banana->SetSpecular(.4);
  banana->SetSpecularPower(20);

  // now an actor
  vtkNew<vtkActor> blobbyLogo;
  blobbyLogo->SetMapper(blobbyLogoMapper);
  blobbyLogo->SetProperty(banana);

  // position the letters

  VTransform->Translate(-16.0, 0.0, 12.5);
  VTransform->RotateY(40);

  KTransform->Translate(14.0, 0.0, 0.0);
  KTransform->RotateY(-40);

  // move the polygonal letters to the front
  logo->SetProperty(tomato);
  logo->SetPosition(0, 0, 6);

  aRenderer->AddActor(logo);
  aRenderer->AddActor(blobbyLogo);

  aRenderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  aRenderWindow->Render();

  // interact with data
  anInteractor->Start();

  return EXIT_SUCCESS;
}
