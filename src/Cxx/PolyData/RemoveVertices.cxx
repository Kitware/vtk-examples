#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " filename.vtp" << std::endl;
    return EXIT_FAILURE;
  }
  std::string filename = argv[1];

  // Read all the data from the file
  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();

  vtkNew<vtkPolyData> output;
  output->ShallowCopy(reader->GetOutput());
  // output->SetVerts(NULL);
  vtkNew<vtkCellArray> verts;
  output->SetVerts(verts);
  // output->SetPoints(reader->GetOutput()->GetPoints());
  // output->SetPolys(reader->GetOutput()->GetPolys());

  vtkNew<vtkXMLPolyDataWriter> writer;
  writer->SetFileName("test.vtp");
  writer->SetInputData(output);
  writer->Write();

  return EXIT_SUCCESS;
}
