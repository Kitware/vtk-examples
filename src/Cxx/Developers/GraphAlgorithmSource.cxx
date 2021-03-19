#include <vtkGraph.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNew.h>

#include "vtkTestGraphAlgorithmSource.h"

int main(int, char*[])
{
  vtkNew<vtkTestGraphAlgorithmSource> source;
  source->Update();

  vtkGraph* outputGraph = source->GetOutput();

  std::cout << "Output number of vertices: "
            << outputGraph->GetNumberOfVertices() << std::endl;

  return EXIT_SUCCESS;
}
