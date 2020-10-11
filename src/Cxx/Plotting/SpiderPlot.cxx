#include <vtkDataObject.h>
#include <vtkFieldData.h>
#include <vtkFloatArray.h>
#include <vtkIdList.h>
#include <vtkLegendBoxActor.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProperty2D.h>
#include <vtkRegressionTestImage.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSpiderPlotActor.h>
#include <vtkTestUtilities.h>
#include <vtkTextProperty.h>

//----------------------------------------------------------------------------
int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  int numTuples = 12;

  vtkNew<vtkFloatArray> bitter;
  bitter->SetNumberOfTuples(numTuples);

  vtkNew<vtkFloatArray> crispy;
  crispy->SetNumberOfTuples(numTuples);

  vtkNew<vtkFloatArray> crunchy;
  crunchy->SetNumberOfTuples(numTuples);

  vtkNew<vtkFloatArray> salty;
  salty->SetNumberOfTuples(numTuples);

  vtkNew<vtkFloatArray> oily;
  oily->SetNumberOfTuples(numTuples);

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(8775070);
  for (int i = 0; i < numTuples; i++)
  {
    bitter->SetTuple1(i, randomSequence->GetRangeValue(1, 10));
    randomSequence->Next();
    crispy->SetTuple1(i, randomSequence->GetRangeValue(-1, 1));
    randomSequence->Next();
    crunchy->SetTuple1(i, randomSequence->GetRangeValue(1, 100));
    randomSequence->Next();
    salty->SetTuple1(i, randomSequence->GetRangeValue(0, 10));
    randomSequence->Next();
    oily->SetTuple1(i, randomSequence->GetRangeValue(5, 25));
    randomSequence->Next();
  }

  vtkNew<vtkDataObject> dobj;
  dobj->GetFieldData()->AddArray(bitter);
  dobj->GetFieldData()->AddArray(crispy);
  dobj->GetFieldData()->AddArray(crunchy);
  dobj->GetFieldData()->AddArray(salty);
  dobj->GetFieldData()->AddArray(oily);

  vtkNew<vtkSpiderPlotActor> actor;
  actor->SetInputData(dobj);
  actor->SetTitle("Spider Plot");
  actor->SetIndependentVariablesToColumns();
  actor->GetPositionCoordinate()->SetValue(0.05, 0.1, 0.0);
  actor->GetPosition2Coordinate()->SetValue(0.95, 0.85, 0.0);
  actor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
  actor->SetAxisLabel(0, "Bitter");
  actor->SetAxisRange(0, 1, 10);
  actor->SetAxisLabel(1, "Crispy");
  actor->SetAxisRange(1, -1, 1);
  actor->SetAxisLabel(2, "Crunchy");
  actor->SetAxisRange(2, 1, 100);
  actor->SetAxisLabel(3, "Salty");
  actor->SetAxisRange(3, 0, 10);
  actor->SetAxisLabel(4, "Oily");
  actor->SetAxisRange(4, 5, 25);
  actor->GetLegendActor()->SetNumberOfEntries(numTuples);
  for (int i = 0; i < numTuples; i++)
  {
    double r, g, b;
    r = randomSequence->GetRangeValue(0.4, 1.0);
    randomSequence->Next();
    g = randomSequence->GetRangeValue(0.4, 1.0);
    randomSequence->Next();
    b = randomSequence->GetRangeValue(0.4, 1.0);
    randomSequence->Next();
    actor->SetPlotColor(i, r, g, b);
  }
  actor->LegendVisibilityOn();

  // Set text colors
  actor->GetTitleTextProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());
  actor->GetLabelTextProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());

  // Create the RenderWindow, Renderer and both Actors
  vtkNew<vtkRenderer> ren1;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  ren1->AddActor(actor);
  ren1->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
  renWin->SetSize(600, 500);
  renWin->SetWindowName("SpiderPlot");

  // render the image
  iren->Initialize();
  renWin->Render();

  iren->Start();

  return EXIT_SUCCESS;
}
