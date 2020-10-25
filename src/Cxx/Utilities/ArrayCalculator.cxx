#include <vtkArrayCalculator.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>

int main(int, char*[])
{
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(1.0, 0.0, 0.0);
  points->InsertNextPoint(2.0, 0.0, 0.0);
  points->InsertNextPoint(3.0, 0.0, 0.0);

  vtkNew<vtkDoubleArray> array;
  array->SetName("orig");
  array->InsertNextValue(1.0);
  array->InsertNextValue(2.0);
  array->InsertNextValue(3.0);

  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);
  polydata->GetPointData()->AddArray(array);

  vtkNew<vtkArrayCalculator> calc1;
  calc1->SetInputData(polydata);
  calc1->AddScalarArrayName("orig");
  calc1->SetFunction("orig+1");
  calc1->SetResultArrayName("orig");
  calc1->Update();

  auto output1 = dynamic_cast<vtkDoubleArray*>(
      calc1->GetPolyDataOutput()->GetPointData()->GetArray("orig"));

  for (vtkIdType i = 0; i < output1->GetNumberOfTuples(); i++)
  {
    double val = output1->GetValue(i);
    cout << "output1 value " << i << ": " << val << endl;
  }

  vtkNew<vtkArrayCalculator> calc2;
  calc2->SetInputData(polydata);
  calc2->AddScalarArrayName("orig");
  calc2->SetFunction("if(orig=2,1,orig)");
  calc2->SetResultArrayName("new");
  calc2->Update();

  auto output2 = dynamic_cast<vtkDoubleArray*>(
      calc2->GetPolyDataOutput()->GetPointData()->GetArray("new"));

  for (vtkIdType i = 0; i < output2->GetNumberOfTuples(); i++)
  {
    double val = output2->GetValue(i);
    cout << "output2 value " << i << ": " << val << endl;
  }

  return EXIT_SUCCESS;
}
