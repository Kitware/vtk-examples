#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkNew.h>

int main(int, char*[])
{
  /////////// Floats ///////////
  {
    vtkNew<vtkFloatArray> distances;
    distances->SetNumberOfComponents(1);
    distances->SetName("Distances");

    distances->InsertNextValue(5);
    distances->InsertNextValue(15);
    distances->InsertNextValue(25);

    // Get min and max
    double range[2];
    distances->GetRange(range);
    std::cout << "range = " << range[0] << " " << range[1] << std::endl;
  }

  /////////////// Ints //////////////
  {
    vtkNew<vtkIntArray> ints;
    ints->SetNumberOfComponents(1);
    ints->SetName("Ints");

    ints->InsertNextValue(5);
    ints->InsertNextValue(15);
    ints->InsertNextValue(25);

    // Get min and max
    int valuesRange[2];
    ints->GetValueRange(valuesRange); // Note this is not GetRange()!
    std::cout << "valuesRange = " << valuesRange[0] << " " << valuesRange[1]
              << std::endl;
  }

  /////////// Range with negative values ///////////
  {
    vtkNew<vtkIntArray> ints;
    ints->SetNumberOfComponents(1);
    ints->SetName("Ints");

    ints->InsertNextValue(-50);
    ints->InsertNextValue(15);
    ints->InsertNextValue(25);

    // Get min and max
    int valuesRange[2];
    ints->GetValueRange(valuesRange); // Note this is not GetRange()!
    std::cout << "valuesRange = " << valuesRange[0] << " " << valuesRange[1]
              << std::endl;
  }

  /////////// Magnitude range ///////////
  {
    vtkNew<vtkIntArray> ints;
    ints->SetNumberOfComponents(1);
    ints->SetName("Ints");

    ints->InsertNextValue(-50);
    ints->InsertNextValue(15);
    ints->InsertNextValue(25);

    // Get min and max
    int valuesRange[2];
    ints->GetValueRange(valuesRange, -1);
    std::cout << "valuesRange = " << valuesRange[0] << " " << valuesRange[1]
              << std::endl;
  }
  return EXIT_SUCCESS;
}
