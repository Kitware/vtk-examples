#include <vtkMergePoints.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

int main(int, char*[])
{
  // Create a set of points
  vtkNew<vtkPointSource> pointsSource;
  pointsSource->SetNumberOfPoints(100);
  pointsSource->Update();

  vtkPolyData* points = pointsSource->GetOutput();

  // Get a point point in the set
  double inSet[3];
  points->GetPoint(25, inSet);

  std::cout << "There are " << points->GetNumberOfPoints() << " input points."
            << std::endl;

  vtkIdType id;

  // Insert all of the points
  vtkNew<vtkMergePoints> mergePoints;
  mergePoints->SetDataSet(points);
  mergePoints->SetDivisions(10, 10, 10);
  mergePoints->InitPointInsertion(points->GetPoints(), points->GetBounds());

  for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
  {
    mergePoints->InsertUniquePoint(points->GetPoint(i), id);
  }

  // Insert a few of the original points
  std::cout << "Insert some of the original points" << std::endl;
  int inserted;
  for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i += 10)
  {
    points->GetPoint(i, inSet);
    inserted = mergePoints->InsertUniquePoint(inSet, id);
    std::cout << "\tPoint: " << inSet[0] << ", " << inSet[1] << ", " << inSet[2]
              << " ";

    std::cout << "Inserted? " << ((inserted == 0) ? "No, " : "Yes, ");
    std::cout << "Id:: " << id << std::endl;
  }

  vtkNew<vtkMinimalStandardRandomSequence> rng;
  rng->SetSeed(8775070);
  // rng->SetSeed(0);

  // These points are probably outside the original set of points
  std::cout << "Insert some new points" << std::endl;
  double outsideSet[3];
  auto radius = pointsSource->GetRadius();
  for (unsigned int i = 0; i < 10; i++)
  {
    for (auto j = 0; j < 3; ++j)
    {
      outsideSet[j] = rng->GetRangeValue(-radius, radius);
      rng->Next();
    }

    inserted = mergePoints->InsertUniquePoint(outsideSet, id);
    std::cout << "\tPoint: " << outsideSet[0] << ", " << outsideSet[1] << ", "
              << outsideSet[2] << " ";

    std::cout << "Inserted? " << ((inserted == 0) ? "No, " : "Yes, ");
    std::cout << "Id:: " << id << std::endl;
  }

  std::cout << "There are now " << points->GetNumberOfPoints() << " points."
            << std::endl;

  return EXIT_SUCCESS;
}
