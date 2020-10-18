#include <vtkIdList.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPointLocator.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

int main(int, char*[])
{
  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(400);
  pointSource->Update();

  // Create the tree
  vtkNew<vtkPointLocator> pointLocator;
  pointLocator->SetDataSet(pointSource->GetOutput());
  pointLocator->AutomaticOn();
  pointLocator->SetNumberOfPointsPerBucket(2);
  pointLocator->BuildLocator();

  double radius = 0.1;
  double center[3] = {0.0, 0.0, 0.0};
  vtkNew<vtkIdList> result;
  pointLocator->FindPointsWithinRadius(radius, center, result);

  for (vtkIdType i = 0; i < result->GetNumberOfIds(); i++)
  {
    unsigned int id = result->GetId(i);
    std::cout << "id of point " << i << " : " << id << std::endl;
  }

  return EXIT_SUCCESS;
}
