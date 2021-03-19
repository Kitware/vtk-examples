/*
 * The human data file is taken from:
 * https://github.com/Slicer/Slicer/blob/master/Base/Logic/Resources/OrientationMarkers/Human.vtp
 * Thanks to the Slicer people for providing this.
 *
 */

#include <vtkAnnotatedCubeActor.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkVectorText.h>
#include <vtkXMLPolyDataReader.h>

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <vector>

namespace {

/**
 * @param scale Sets the scale and direction of the axes.
 * @param xyzLabels Labels for the axes.
 * @return The axes actor.
 */
vtkSmartPointer<vtkAxesActor>
MakeAxesActor(std::array<double, 3>& scale,
              std::array<std::string, 3>& xyzLabels);

/**
 * @param colors Used to set the colors of the cube faces.
 * @return The annotated cube actor.
 */
vtkSmartPointer<vtkAnnotatedCubeActor>
MakeAnnotatedCubeActor(vtkNamedColors* colors);

/**
 * @param scale Sets the scale and direction of the axes.
 * @param xyzLabels Labels for the axes.
 * @param colors Used to set the colors of the cube faces.
 * @return The combined axes and annotated cube prop.
 */
vtkSmartPointer<vtkPropAssembly>
MakeCubeActor(std::array<double, 3>& scale,
              std::array<std::string, 3>& xyzLabels, vtkNamedColors* colors);

/**
 * Make the traverse, coronal and saggital planes.
 *
 * @param colors Used to set the color of the planes.
 * @return The planes actors.
 */
std::vector<vtkSmartPointer<vtkActor>> MakePlanesActors(vtkNamedColors* colors);

/**
 * Generate text to place on the planes.
 * Careful placement is needed here.
 * @return The text actors.
 */
std::vector<vtkSmartPointer<vtkActor>> AddTextToPlanes();
} // namespace

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " fileName" << std::endl;
    std::cout << "where: fileName is Human.vtp." << std::endl;
    return EXIT_FAILURE;
  }

  std::string fileName(argv[1]);

  vtkNew<vtkNamedColors> colors;

  // Create the rendering window, renderer, and interactive renderer.
  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  renWin->SetSize(780, 780);
  renWin->AddRenderer(ren);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Make an annotated cube actor with axes and then add it into an orientation
  // marker widget.
  // Three of these need to be made.

  // Right Posterior Superior
  std::array<std::string, 3> xyzLabels{{"X", "Y", "Z"}};
  std::array<double, 3> scale{{1.5, -1.5, 1.5}};
  auto axes = MakeCubeActor(scale, xyzLabels, colors);
  vtkNew<vtkOrientationMarkerWidget> om;
  om->SetOrientationMarker(axes);
  // Position upper left in the viewport.
  om->SetViewport(0.0, 0.8, 0.2, 1.0);
  om->SetInteractor(iren);
  om->EnabledOn();
  om->InteractiveOn();

  // Right, Anterior, Superior.
  std::array<double, 3> scale1{{1.5, 1.5, 1.5}};
  auto axes1 = MakeCubeActor(scale1, xyzLabels, colors);
  vtkNew<vtkOrientationMarkerWidget> om1;
  om1->SetOrientationMarker(axes1);
  // Position lower left in the viewport.
  om1->SetViewport(0, 0, 0.2, 0.2);
  om1->SetInteractor(iren);
  om1->EnabledOn();
  om1->InteractiveOn();

  // Left, Posterior, Superior.
  std::array<double, 3> scale2{{-1.5, -1.5, 1.5}};
  auto axes2 = MakeCubeActor(scale2, xyzLabels, colors);
  vtkNew<vtkOrientationMarkerWidget> om2;
  om2->SetOrientationMarker(axes2);
  // Position lower right in the viewport.
  om2->SetViewport(0.8, 0, 1.0, 0.2);
  om2->SetInteractor(iren);
  om2->EnabledOn();
  om2->InteractiveOn();

  // Finally create an annotated cube actor adding it into an orientation marker
  // widget.
  auto axes3 = MakeAnnotatedCubeActor(colors);
  vtkNew<vtkOrientationMarkerWidget> om3;
  om3->SetOrientationMarker(axes3);
  // Position upper right in the viewport.
  om3->SetViewport(0.8, 0.8, 1.0, 1.0);
  om3->SetInteractor(iren);
  om3->EnabledOn();
  om3->InteractiveOn();

  // Read in the model.
  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(fileName.c_str());
  reader->Update();

  vtkNew<vtkPolyDataMapper> humanMapper;
  humanMapper->SetInputConnection(reader->GetOutputPort());
  humanMapper->SetScalarModeToUsePointFieldData();
  humanMapper->SelectColorArray("Color");
  humanMapper->SetColorModeToDirectScalars();

  vtkNew<vtkActor> humanActor;
  humanActor->SetMapper(humanMapper);
  std::vector<double> bounds(6, 0);
  humanActor->GetBounds(&bounds[0]);
  std::vector<double>::iterator maxElt =
      std::max_element(bounds.begin(), bounds.end());
  // Scale the actor
  humanActor->SetScale(1.0 / *maxElt);
  ren->AddActor(humanActor);

  // Make the planes.
  auto actors = MakePlanesActors(colors);
  for (auto actor : actors)
  {
    ren->AddViewProp(actor);
  }

  // Label them.
  auto textActors = AddTextToPlanes();
  for (auto actor : textActors)
  {
    ren->AddViewProp(actor);
  }

  // Interact
  ren->SetBackground2(colors->GetColor3d("OldLace").GetData());
  ren->SetBackground(colors->GetColor3d("MistyRose").GetData());
  ren->GradientBackgroundOn();
  ren->ResetCamera();
  ren->GetActiveCamera()->Zoom(1.6);
  ren->GetActiveCamera()->SetPosition(-2.3, 4.1, 4.2);
  ren->GetActiveCamera()->SetViewUp(0.0, 0.0, 1.0);
  ren->ResetCameraClippingRange();
  renWin->Render();
  //  Call SetWindowName after renWin.Render() is called.
  renWin->SetWindowName("AnatomicalOrientation");

  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}

namespace {

vtkSmartPointer<vtkAxesActor>
MakeAxesActor(std::array<double, 3>& scale,
              std::array<std::string, 3>& xyzLabels)
{
  vtkNew<vtkAxesActor> axes;
  axes->SetScale(scale[0], scale[1], scale[2]);
  axes->SetShaftTypeToCylinder();
  axes->SetXAxisLabelText(xyzLabels[0].c_str());
  axes->SetYAxisLabelText(xyzLabels[1].c_str());
  axes->SetZAxisLabelText(xyzLabels[2].c_str());
  axes->SetCylinderRadius(0.5 * axes->GetCylinderRadius());
  axes->SetConeRadius(1.025 * axes->GetConeRadius());
  axes->SetSphereRadius(1.5 * axes->GetSphereRadius());
  vtkTextProperty* tprop =
      axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty();
  tprop->ItalicOn();
  tprop->ShadowOn();
  tprop->SetFontFamilyToTimes();
  // Use the same text properties on the other two axes.
  axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy(tprop);
  axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy(tprop);
  return axes;
}

vtkSmartPointer<vtkAnnotatedCubeActor>
MakeAnnotatedCubeActor(vtkNamedColors* colors)
{
  // A cube with labeled faces.
  vtkNew<vtkAnnotatedCubeActor> cube;
  cube->SetXPlusFaceText("R");  // Right
  cube->SetXMinusFaceText("L"); // Left
  cube->SetYPlusFaceText("A");  // Anterior
  cube->SetYMinusFaceText("P"); // Posterior
  cube->SetZPlusFaceText("S");  // Superior/Cranial
  cube->SetZMinusFaceText("I"); // Inferior/Caudal
  cube->SetFaceTextScale(0.5);
  cube->GetCubeProperty()->SetColor(colors->GetColor3d("Gainsboro").GetData());

  cube->GetTextEdgesProperty()->SetColor(
      colors->GetColor3d("LightSlateGray").GetData());

  // Change the vector text colors.
  cube->GetXPlusFaceProperty()->SetColor(
      colors->GetColor3d("Tomato").GetData());
  cube->GetXMinusFaceProperty()->SetColor(
      colors->GetColor3d("Tomato").GetData());
  cube->GetYPlusFaceProperty()->SetColor(
      colors->GetColor3d("DeepSkyBlue").GetData());
  cube->GetYMinusFaceProperty()->SetColor(
      colors->GetColor3d("DeepSkyBlue").GetData());
  cube->GetZPlusFaceProperty()->SetColor(
      colors->GetColor3d("SeaGreen").GetData());
  cube->GetZMinusFaceProperty()->SetColor(
      colors->GetColor3d("SeaGreen").GetData());
  return cube;
}

vtkSmartPointer<vtkPropAssembly>
MakeCubeActor(std::array<double, 3>& scale,
              std::array<std::string, 3>& xyzLabels, vtkNamedColors* colors)
{
  // We are combining a vtk.vtkAxesActor and a vtk.vtkAnnotatedCubeActor
  // into a vtk.vtkPropAssembly
  vtkSmartPointer<vtkAnnotatedCubeActor> cube = MakeAnnotatedCubeActor(colors);
  vtkSmartPointer<vtkAxesActor> axes = MakeAxesActor(scale, xyzLabels);

  // Combine orientation markers into one with an assembly.
  vtkNew<vtkPropAssembly> assembly;
  assembly->AddPart(axes);
  assembly->AddPart(cube);
  return assembly;
}

vtkSmartPointer<vtkTransformPolyDataFilter>
MakePlane(std::array<int, 2>& resolution, std::array<double, 3>& origin,
          std::array<double, 3>& point1, std::array<double, 3>& point2,
          std::array<double, 4>& wxyz, std::array<double, 3>& translate)
{
  vtkNew<vtkPlaneSource> plane;
  plane->SetResolution(resolution[0], resolution[1]);
  plane->SetOrigin(origin.data());
  plane->SetPoint1(point1.data());
  plane->SetPoint2(point2.data());
  vtkNew<vtkTransform> trnf;
  trnf->RotateWXYZ(wxyz[0], wxyz[1], wxyz[2], wxyz[3]);
  trnf->Translate(translate.data());
  vtkNew<vtkTransformPolyDataFilter> tpdPlane;
  tpdPlane->SetTransform(trnf);
  tpdPlane->SetInputConnection(plane->GetOutputPort());
  return tpdPlane;
}

std::vector<vtkSmartPointer<vtkActor>> MakePlanesActors(vtkNamedColors* colors)
{
  std::vector<vtkSmartPointer<vtkTransformPolyDataFilter>> planes;
  std::vector<vtkSmartPointer<vtkPolyDataMapper>> mappers;
  std::vector<vtkSmartPointer<vtkActor>> actors;

  // Parameters for a plane lying in the x-y plane.
  std::array<int, 2> resolution{{10, 10}};
  std::array<double, 3> origin{{0.0, 0.0, 0.0}};
  std::array<double, 3> point1{{1, 0, 0}};
  std::array<double, 3> point2{{0, 1, 0}};

  std::array<double, 4> wxyz0{{0, 0, 0, 0}};
  std::array<double, 3> translate{{-0.5, -0.5, 0}};
  std::array<double, 4> wxyz1{{-90, 1, 0, 0}};
  std::array<double, 4> wxyz2{{-90, 0, 1, 0}};
  planes.push_back(MakePlane(resolution, origin, point1, point2, wxyz0,
                             translate)); // x-y plane
  planes.push_back(MakePlane(resolution, origin, point1, point2, wxyz1,
                             translate)); // x-z plane
  planes.push_back(MakePlane(resolution, origin, point1, point2, wxyz2,
                             translate)); // y-z plane
  for (size_t i = 0; i < planes.size(); ++i)
  {
    mappers.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
    mappers[i]->SetInputConnection(planes[i]->GetOutputPort());
    actors.push_back(vtkSmartPointer<vtkActor>::New());
    actors[i]->SetMapper(mappers[i]);
  }
  actors[0]->GetProperty()->SetColor(
      colors->GetColor3d("SeaGreen").GetData()); // Transverse plane
  actors[1]->GetProperty()->SetColor(
      colors->GetColor3d("DeepSkyBlue").GetData()); // Coronal plane
  actors[2]->GetProperty()->SetColor(
      colors->GetColor3d("Tomato").GetData()); // Saggital plane
  return actors;
}

std::vector<vtkSmartPointer<vtkActor>> AddTextToPlanes()
{
  std::vector<vtkSmartPointer<vtkActor>> textActors;
  std::array<double, 3> scale{{0.04, 0.04, 0.04}};

  vtkNew<vtkVectorText> text1;
  text1->SetText("Transverse\nPlane\n\nSuperior\nCranial");
  vtkNew<vtkTransform> trnf1;
  trnf1->RotateZ(-90);
  vtkNew<vtkTransformPolyDataFilter> tpdPlane1;
  tpdPlane1->SetTransform(trnf1);
  tpdPlane1->SetInputConnection(text1->GetOutputPort());
  vtkNew<vtkPolyDataMapper> textMapper1;
  textMapper1->SetInputConnection(tpdPlane1->GetOutputPort());
  vtkNew<vtkActor> textActor1;
  textActor1->SetMapper(textMapper1);
  textActor1->SetScale(scale.data());
  textActor1->AddPosition(0.4, 0.49, 0.01);
  textActors.push_back(textActor1);

  vtkNew<vtkVectorText> text2;
  text2->SetText("Transverse\nPlane\n\nInferior\n(Caudal)");
  vtkNew<vtkTransform> trnf2;
  trnf2->RotateZ(270);
  trnf2->RotateWXYZ(180, 0, 1, 0);
  vtkNew<vtkTransformPolyDataFilter> tpdPlane2;
  tpdPlane2->SetTransform(trnf2);
  tpdPlane2->SetInputConnection(text2->GetOutputPort());
  vtkNew<vtkPolyDataMapper> textMapper2;
  textMapper2->SetInputConnection(tpdPlane2->GetOutputPort());
  vtkNew<vtkActor> textActor2;
  textActor2->SetMapper(textMapper2);
  textActor2->SetScale(scale.data());
  textActor2->AddPosition(0.4, -0.49, -0.01);
  textActors.push_back(textActor2);

  vtkNew<vtkVectorText> text3;
  text3->SetText("Sagittal\nPlane\n\nLeft");
  vtkNew<vtkTransform> trnf3;
  trnf3->RotateX(90);
  trnf3->RotateWXYZ(-90, 0, 1, 0);
  vtkNew<vtkTransformPolyDataFilter> tpdPlane3;
  tpdPlane3->SetTransform(trnf3);
  tpdPlane3->SetInputConnection(text3->GetOutputPort());
  vtkNew<vtkPolyDataMapper> textMapper3;
  textMapper3->SetInputConnection(tpdPlane3->GetOutputPort());
  vtkNew<vtkActor> textActor3;
  textActor3->SetMapper(textMapper3);
  textActor3->SetScale(scale.data());
  textActor3->AddPosition(-0.01, 0.49, 0.4);
  textActors.push_back(textActor3);

  vtkNew<vtkVectorText> text4;
  text4->SetText("Sagittal\nPlane\n\nRight");
  vtkNew<vtkTransform> trnf4;
  trnf4->RotateX(90);
  trnf4->RotateWXYZ(-270, 0, 1, 0);
  vtkNew<vtkTransformPolyDataFilter> tpdPlane4;
  tpdPlane4->SetTransform(trnf4);
  tpdPlane4->SetInputConnection(text4->GetOutputPort());
  vtkNew<vtkPolyDataMapper> textMapper4;
  textMapper4->SetInputConnection(tpdPlane4->GetOutputPort());
  vtkNew<vtkActor> textActor4;
  textActor4->SetMapper(textMapper4);
  textActor4->SetScale(scale.data());
  textActor4->AddPosition(0.01, -0.49, 0.4);
  textActors.push_back(textActor4);

  vtkNew<vtkVectorText> text5;
  text5->SetText("Coronal\nPlane\n\nAnterior");
  vtkNew<vtkTransform> trnf5;
  trnf5->RotateY(-180);
  trnf5->RotateWXYZ(-90, 1, 0, 0);
  vtkNew<vtkTransformPolyDataFilter> tpdPlane5;
  tpdPlane5->SetTransform(trnf5);
  tpdPlane5->SetInputConnection(text5->GetOutputPort());
  vtkNew<vtkPolyDataMapper> textMapper5;
  textMapper5->SetInputConnection(tpdPlane5->GetOutputPort());
  vtkNew<vtkActor> textActor5;
  textActor5->SetMapper(textMapper5);
  textActor5->SetScale(scale.data());
  textActor5->AddPosition(0.49, 0.01, 0.20);
  textActors.push_back(textActor5);

  vtkNew<vtkVectorText> text6;
  text6->SetText("Coronal\nPlane\n\nPosterior");
  vtkNew<vtkTransform> trnf6;
  trnf6->RotateWXYZ(90, 1, 0, 0);
  vtkNew<vtkTransformPolyDataFilter> tpdPlane6;
  tpdPlane6->SetTransform(trnf6);
  tpdPlane6->SetInputConnection(text6->GetOutputPort());
  vtkNew<vtkPolyDataMapper> textMapper6;
  textMapper6->SetInputConnection(tpdPlane6->GetOutputPort());
  vtkNew<vtkActor> textActor6;
  textActor6->SetMapper(textMapper6);
  textActor6->SetScale(scale.data());
  textActor6->AddPosition(-0.49, -0.01, 0.3);
  textActors.push_back(textActor6);

  return textActors;
}
} // namespace
