#include <vtkArrayData.h>
#include <vtkArrayWriter.h>
#include <vtkDenseArray.h>
#include <vtkNew.h>

int main(int, char*[])
{
  vtkNew<vtkDenseArray<double>> array;
  array->Resize(1, 3);
  array->SetValue(0, 0, 1.0);
  array->SetValue(0, 1, 2.0);
  array->SetValue(0, 2, 3.0);
  {
    // Method 1
    vtkNew<vtkArrayWriter> writer;
    vtkNew<vtkArrayData> arrayData;
    arrayData->AddArray(array);
    writer->SetInputData(arrayData);
    vtkStdString file1 = "Test1.txt";
    writer->Write(file1);
  }
  {
    vtkStdString file2 = "Test2.txt";
    // Method 2
    vtkNew<vtkArrayWriter> writer;
    writer->Write(array, file2);
  }

  return EXIT_SUCCESS;
}
