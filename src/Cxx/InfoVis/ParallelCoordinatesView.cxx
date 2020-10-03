#include <vtkDelimitedTextReader.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParallelCoordinatesRepresentation.h>
#include <vtkParallelCoordinatesView.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

#include <vtksys/SystemTools.hxx>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
  std::string title;

  if (argc > 1)
  {
    std::string inputFilename;
    inputFilename = argv[1];
    vtkNew<vtkDelimitedTextReader> reader;
    reader->SetFileName(inputFilename.c_str());
    reader->SetHaveHeaders(1);
    reader->DetectNumericColumnsOn();
    reader->SetFieldDelimiterCharacters(",");
    reader->Update();
    table = reader->GetOutput();
    title = vtksys::SystemTools::GetFilenameWithoutExtension(
        vtksys::SystemTools::GetFilenameName(inputFilename));
  }
  else
  {
    title = "Generated Data";
    vtkNew<vtkIntArray> array1;
    array1->SetName("Array1");
    array1->SetNumberOfComponents(1);
    array1->InsertNextValue(0);
    array1->InsertNextValue(1);
    array1->InsertNextValue(2);
    array1->InsertNextValue(3);
    array1->InsertNextValue(4);

    vtkNew<vtkFloatArray> array2;
    array2->SetName("Array2");
    array2->SetNumberOfComponents(1);
    array2->InsertNextValue(-0);
    array2->InsertNextValue(-1);
    array2->InsertNextValue(-2);
    array2->InsertNextValue(-3);
    array2->InsertNextValue(-4);

    vtkNew<vtkDoubleArray> array3;
    array3->SetName("Array3");
    array3->SetNumberOfComponents(1);
    array3->InsertNextValue(0);
    array3->InsertNextValue(1);
    array3->InsertNextValue(4);
    array3->InsertNextValue(9);
    array3->InsertNextValue(16);

    table->AddColumn(array1);
    table->AddColumn(array2);
    table->AddColumn(array3);
  }

  vtkNew<vtkPolyData> polydata;

  for (vtkIdType i = 0; i < table->GetNumberOfColumns(); ++i)
  {
    if (dynamic_cast<vtkStringArray*>(table->GetColumn(i)))
    {
      std::cout << i << " is "
                << "StringArray named ";
    }
    else if (dynamic_cast<vtkDoubleArray*>(table->GetColumn(i)))
    {
      std::cout << i << " is "
                << "DoubleArray named ";
    }
    else if (dynamic_cast<vtkFloatArray*>(table->GetColumn(i)))
    {
      std::cout << i << " is "
                << "DoubleArray named ";
    }
    else if (dynamic_cast<vtkIntArray*>(table->GetColumn(i)))
    {
      std::cout << i << " is "
                << "IntArray named ";
    }
    else
    {
      std::cout << i << " is "
                << "Unknown type named ";
    }
    std::cout << "\"" << table->GetColumn(i)->GetName() << "\"" << std::endl;

    polydata->GetPointData()->AddArray(table->GetColumn(i));
  }

  vtkNew<vtkParallelCoordinatesRepresentation> rep;
  rep->SetInputData(polydata);

  // List all of the attribute arrays you want plotted in parallel coordinates
  // Set up the parallel coordinates Representation to be used in the View
  for (vtkIdType i = 0; i < table->GetNumberOfColumns(); ++i)
  {
    if (dynamic_cast<vtkStringArray*>(table->GetColumn(i)))
    {
      continue;
    }
    else
    {
      rep->SetInputArrayToProcess(i, 0, 0, 0, table->GetColumn(i)->GetName());
    }
  }
  rep->UseCurvesOn();
  rep->SetFontSize(.5);
  rep->SetPlotTitle(title.c_str());
  rep->SetLineOpacity(0.5);
  rep->SetLineColor(colors->GetColor3d("Gold").GetData());
  rep->SetAxisColor(colors->GetColor3d("OrangeRed").GetData());
  rep->SetAxisLabelColor(colors->GetColor3d("Yellow").GetData());

  // Set up the Parallel Coordinates View and hook in the Representation
  vtkNew<vtkParallelCoordinatesView> view;
  view->SetRepresentation(rep);
  view->SetInspectMode(1);
  view->SetDisplayHoverText(1);

  // Brush Mode determines the type of interaction you perform to select data
  view->SetBrushModeToLasso();
  view->SetBrushOperatorToReplace();

  // Set up render window
  view->GetRenderWindow()->SetSize(600, 300);
  view->GetRenderer()->SetBackground2(
      colors->GetColor3d("MidnightBlue").GetData());
  view->GetRenderer()->SetBackground(colors->GetColor3d("Indigo").GetData());

  view->GetRenderWindow()->SetWindowName("ParallelCoordinatesView");
  view->ResetCamera();
  view->Render();

  // Start interaction event loop
  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
