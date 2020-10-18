#include <vtkCellArray.h>
#include <vtkCellLocator.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include <vtkLine.h>
#include <vtkMath.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProbeFilter.h>
#include <vtkTriangle.h>
#include <vtkXMLPolyDataWriter.h>

int main(int, char*[])
{
  vtkNew<vtkImageData> image;
  image->SetExtent(0, 9, 0, 9, 0, 0);
  image->AllocateScalars(VTK_DOUBLE, 1);

  // Create a random set of heights on a grid. This is often called a
  //"terrain map"
  vtkNew<vtkPoints> points;

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(8775070);
  unsigned int GridSize = 10;
  for (unsigned int x = 0; x < GridSize; x++)
  {
    for (unsigned int y = 0; y < GridSize; y++)
    {
      double val = randomSequence->GetRangeValue(-1, 1);
      randomSequence->Next();
      points->InsertNextPoint(x, y, val);
      image->SetScalarComponentFromDouble(x, y, 0, 0, val);
    }
  }

  // add the grid points to a polydata object
  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);

  // triangulate the grid points
  vtkNew<vtkDelaunay2D> delaunay;
  delaunay->SetInputData(polydata);
  delaunay->Update();

  vtkNew<vtkXMLPolyDataWriter> writer;
  writer->SetFileName("surface.vtp");
  writer->SetInputConnection(delaunay->GetOutputPort());
  writer->Write();

  // Add some points to interpolate
  vtkNew<vtkPoints> probePoints;
  probePoints->InsertNextPoint(5.2, 3.2, 0);
  probePoints->InsertNextPoint(5.0, 3.0, 0);
  probePoints->InsertNextPoint(0.0, 0.0, 0);

  vtkNew<vtkPolyData> probePolyData;
  probePolyData->SetPoints(probePoints);

  vtkNew<vtkProbeFilter> probe;
  probe->SetSourceData(image);
  probe->SetInputData(probePolyData);
  probe->Update();

  vtkDataArray* data = probe->GetOutput()->GetPointData()->GetScalars();
  vtkDoubleArray* doubleData = dynamic_cast<vtkDoubleArray*>(data);

  for (int i = 0; i < doubleData->GetNumberOfTuples(); i++)
  {
    double val = doubleData->GetValue(i);
    cout << "Interpolation using ProbeFilter ";
    cout << "doubleData->GetValue(" << i << "): " << val << endl;
  }

  // Now find the elevation with a CellLocator
  vtkNew<vtkCellLocator> cellLocator;
  cellLocator->SetDataSet(delaunay->GetOutput());
  cellLocator->BuildLocator();

  for (int i = 0; i < doubleData->GetNumberOfTuples(); i++)
  {
    int subId;
    double t, xyz[3], pcoords[3];
    double rayStart[3], rayEnd[3];
    probePoints->GetPoint(i, rayStart);
    rayStart[2] += 1000.0;
    probePoints->GetPoint(i, rayEnd);
    rayEnd[2] -= 1000.0;

    if (cellLocator->IntersectWithLine(rayStart, rayEnd, 0.0001, t, xyz,
                                       pcoords, subId))
    {
      cout << "Interpolation using CellLocator ";
      cout << "Elevation at " << rayStart[0] << ", " << rayStart[1] << " is "
           << xyz[2] << endl;
    }
  }
  return EXIT_SUCCESS;
}
