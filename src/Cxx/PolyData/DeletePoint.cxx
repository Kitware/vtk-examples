#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

namespace {
void OutputPoints(vtkSmartPointer<vtkPoints> points);
void ReallyDeletePoint(vtkSmartPointer<vtkPoints> points, vtkIdType id);
} // namespace

int main(int, char*[])
{
  // Create a set of points
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(1.0, 0.0, 0.0);
  points->InsertNextPoint(0.0, 1.0, 0.0);
  points->InsertNextPoint(0.0, 0.0, 1.0);

  std::cout << "Number of points: " << points->GetNumberOfPoints() << std::endl;
  OutputPoints(points);

  ReallyDeletePoint(points, 1);

  std::cout << "Number of points: " << points->GetNumberOfPoints() << std::endl;
  OutputPoints(points);

  return EXIT_SUCCESS;
}

namespace {
void OutputPoints(vtkSmartPointer<vtkPoints> points)
{
  for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
  {
    double p[3];
    points->GetPoint(i, p);
    cout << p[0] << " " << p[1] << " " << p[2] << endl;
  }
}

void ReallyDeletePoint(vtkSmartPointer<vtkPoints> points, vtkIdType id)
{
  vtkNew<vtkPoints> newPoints;

  for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
  {
    if (i != id)
    {
      double p[3];
      points->GetPoint(i, p);
      newPoints->InsertNextPoint(p);
    }
  }

  points->ShallowCopy(newPoints);
}
} // namespace
