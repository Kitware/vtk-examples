// This example demonstrates the use of multiline 2D text using
// vtkTextMappers.  It shows several justifications as well as
// single-line and multiple-line text inputs.

#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCoordinate.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

int main(int, char*[])
{
  int font_size = 24;

  // Create the text mappers and the associated Actor2Ds.

  // The font and text properties (except justification) are the same for
  // each single line mapper. Let's create a common text property object
  vtkNew<vtkTextProperty> singleLineTextProp;

  singleLineTextProp->SetFontSize(font_size);
  singleLineTextProp->SetFontFamilyToArial();
  singleLineTextProp->BoldOff();
  singleLineTextProp->ItalicOff();
  singleLineTextProp->ShadowOff();

  // The font and text properties (except justification) are the same for
  // each multi line mapper. Let's create a common text property object
  vtkNew<vtkTextProperty> multiLineTextProp;

  multiLineTextProp->ShallowCopy(singleLineTextProp);
  multiLineTextProp->BoldOn();
  multiLineTextProp->ItalicOn();
  multiLineTextProp->ShadowOn();
  multiLineTextProp->SetLineSpacing(0.8);

  vtkNew<vtkNamedColors> colors;

  // The text is on a single line and bottom-justified.
  vtkNew<vtkTextMapper> singleLineTextB;
  singleLineTextB->SetInput("Single line (bottom)");
  auto tprop = singleLineTextB->GetTextProperty();
  tprop->ShallowCopy(singleLineTextProp);
 
  tprop->SetVerticalJustificationToBottom();
  tprop->SetColor(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkActor2D> singleLineTextActorB;
  singleLineTextActorB->SetMapper(singleLineTextB);
  singleLineTextActorB->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  singleLineTextActorB->GetPositionCoordinate()->SetValue(0.05, 0.85);

  // The text is on a single line and center-justified (vertical
  // justification).
  vtkNew<vtkTextMapper> singleLineTextC;
  singleLineTextC->SetInput("Single line (centered)");
  tprop = singleLineTextC->GetTextProperty();
  tprop->ShallowCopy(singleLineTextProp);
  tprop->SetVerticalJustificationToCentered();
  tprop->SetColor(colors->GetColor3d("DarkGreen").GetData());
  vtkNew<vtkActor2D> singleLineTextActorC;

  singleLineTextActorC->SetMapper(singleLineTextC);
  singleLineTextActorC->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  singleLineTextActorC->GetPositionCoordinate()->SetValue(0.05, 0.75);

  // The text is on a single line and top-justified.
  vtkNew<vtkTextMapper> singleLineTextT;
  singleLineTextT->SetInput("Single line (top)");
  tprop = singleLineTextT->GetTextProperty();
  tprop->ShallowCopy(singleLineTextProp);
  tprop->SetVerticalJustificationToTop();
  tprop->SetColor(colors->GetColor3d("Peacock").GetData());

  vtkNew<vtkActor2D> singleLineTextActorT;
  singleLineTextActorT->SetMapper(singleLineTextT);
  singleLineTextActorT->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  singleLineTextActorT->GetPositionCoordinate()->SetValue(0.05, 0.65);

  // The text is on multiple lines and left- and top-justified.
  vtkNew<vtkTextMapper> textMapperL;
  textMapperL->SetInput("This is\nmulti-line\ntext output\n(left-top)");
  tprop = textMapperL->GetTextProperty();
  tprop->ShallowCopy(multiLineTextProp);
  tprop->SetJustificationToLeft();
  tprop->SetVerticalJustificationToTop();
  tprop->SetColor(colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkActor2D> textActorL;
  textActorL->SetMapper(textMapperL);
  textActorL->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  textActorL->GetPositionCoordinate()->SetValue(0.05, 0.5);

  // The text is on multiple lines and center-justified (both horizontal and
  // vertical).
  vtkNew<vtkTextMapper> textMapperC;
  textMapperC->SetInput("This is\nmulti-line\ntext output\n(centered)");
  tprop = textMapperC->GetTextProperty();
  tprop->ShallowCopy(multiLineTextProp);
  tprop->SetJustificationToCentered();
  tprop->SetVerticalJustificationToCentered();
  tprop->SetColor(colors->GetColor3d("DarkGreen").GetData());

  vtkNew<vtkActor2D> textActorC;
  textActorC->SetMapper(textMapperC);
  textActorC->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  textActorC->GetPositionCoordinate()->SetValue(0.5, 0.5);

  // The text is on multiple lines and right- and bottom-justified.
  vtkNew<vtkTextMapper> textMapperR;
  textMapperR->SetInput("This is\nmulti-line\ntext output\n(right-bottom)");
  tprop = textMapperR->GetTextProperty();
  tprop->ShallowCopy(multiLineTextProp);
  tprop->SetJustificationToRight();
  tprop->SetVerticalJustificationToBottom();
  tprop->SetColor(colors->GetColor3d("Peacock").GetData());

  vtkNew<vtkActor2D> textActorR;
  textActorR->SetMapper(textMapperR);
  textActorR->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  textActorR->GetPositionCoordinate()->SetValue(0.95, 0.5);

  // Draw the grid to demonstrate the placement of the text.

  // Set up the necessary points.
  vtkNew<vtkPoints> Pts;
  Pts->InsertNextPoint(0.05, 0.0, 0.0);
  Pts->InsertNextPoint(0.05, 1.0, 0.0);
  Pts->InsertNextPoint(0.5, 0.0, 0.0);
  Pts->InsertNextPoint(0.5, 1.0, 0.0);
  Pts->InsertNextPoint(0.95, 0.0, 0.0);
  Pts->InsertNextPoint(0.95, 1.0, 0.0);
  Pts->InsertNextPoint(0.0, 0.5, 0.0);
  Pts->InsertNextPoint(1.0, 0.5, 0.0);
  Pts->InsertNextPoint(0.00, 0.85, 0.0);
  Pts->InsertNextPoint(0.50, 0.85, 0.0);
  Pts->InsertNextPoint(0.00, 0.75, 0.0);
  Pts->InsertNextPoint(0.50, 0.75, 0.0);
  Pts->InsertNextPoint(0.00, 0.65, 0.0);
  Pts->InsertNextPoint(0.50, 0.65, 0.0);

  // Set up the lines that use these points.
  vtkNew<vtkCellArray> Lines;
  Lines->InsertNextCell(2);
  Lines->InsertCellPoint(0);
  Lines->InsertCellPoint(1);
  Lines->InsertNextCell(2);
  Lines->InsertCellPoint(2);
  Lines->InsertCellPoint(3);
  Lines->InsertNextCell(2);
  Lines->InsertCellPoint(4);
  Lines->InsertCellPoint(5);
  Lines->InsertNextCell(2);
  Lines->InsertCellPoint(6);
  Lines->InsertCellPoint(7);
  Lines->InsertNextCell(2);
  Lines->InsertCellPoint(8);
  Lines->InsertCellPoint(9);
  Lines->InsertNextCell(2);
  Lines->InsertCellPoint(10);
  Lines->InsertCellPoint(11);
  Lines->InsertNextCell(2);
  Lines->InsertCellPoint(12);
  Lines->InsertCellPoint(13);

  // Create a grid that uses these points and lines.
  vtkNew<vtkPolyData> Grid;
  Grid->SetPoints(Pts);
  Grid->SetLines(Lines);
  // Set up the coordinate system.
  vtkNew<vtkCoordinate> normCoords;
  normCoords->SetCoordinateSystemToNormalizedViewport();

  // Set up the mapper and actor (2D) for the grid.
  vtkNew<vtkPolyDataMapper2D> mapper;
  mapper->SetInputData(Grid);
  mapper->SetTransformCoordinate(normCoords);
  vtkNew<vtkActor2D> gridActor;
  gridActor->SetMapper(mapper);
  gridActor->GetProperty()->SetColor(colors->GetColor3d("DimGray").GetData());

  // Create the Renderer, RenderWindow, and RenderWindowInteractor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;

  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Add the actors to the renderer; set the background and size; zoom in
  // closer to the image; render
  renderer->AddActor2D(textActorL);
  renderer->AddActor2D(textActorC);
  renderer->AddActor2D(textActorR);
  renderer->AddActor2D(singleLineTextActorB);
  renderer->AddActor2D(singleLineTextActorC);
  renderer->AddActor2D(singleLineTextActorT);
  renderer->AddActor2D(gridActor);

  renderer->SetBackground(colors->GetColor3d("Silver").GetData());
  renderWindow->SetSize(640, 480);
  renderer->GetActiveCamera()->Zoom(1.5);

  interactor->Initialize();
  renderWindow->SetWindowName("MultiLineText");

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
