#include "vtkTestGraphAlgorithmFilter.h"

#include <vtkDataObject.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkMutableGraphHelper.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>

vtkStandardNewMacro(vtkTestGraphAlgorithmFilter);

int vtkTestGraphAlgorithmFilter::RequestData(
    vtkInformation* vtkNotUsed(request), vtkInformationVector** inputVector,
    vtkInformationVector* outputVector)
{

  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkGraph* input =
      dynamic_cast<vtkGraph*>(inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkGraph* output =
      dynamic_cast<vtkGraph*>(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkNew<vtkMutableDirectedGraph> mdg;

  vtkNew<vtkMutableUndirectedGraph> mug;

  if (input->IsA("vtkMutableUndirectedGraph"))
  {
    vtkNew<vtkMutableUndirectedGraph> ug;
    ug->AddVertex();
    output->ShallowCopy(ug);
  }
  else if (input->IsA("vtkMutableDirectedGraph"))
  {
    vtkNew<vtkMutableDirectedGraph> dg;
    dg->AddVertex();
    output->ShallowCopy(dg);
  }

  std::cout << "Output is type: " << output->GetClassName() << std::endl;

  return 1;
}
