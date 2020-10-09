#include <vtkActor.h>
#include <vtkDelaunay2D.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataWriter.h>

#include <iomanip>
#include <iostream>

// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

int main(int, char*[])
{
  vtkNew<vtkNamedColors> namedColors;

  // Create a grid of points (height/terrian map)
  vtkNew<vtkPoints> points;

  unsigned int GridSize = 20;
  double xx, yy, zz;
  vtkNew<vtkMinimalStandardRandomSequence> rng;
  rng->SetSeed(8775586); // For testing
  for (unsigned int x = 0; x < GridSize; x++)
  {
    for (unsigned int y = 0; y < GridSize; y++)
    {
      rng->Next();
      xx = x + rng->GetRangeValue(-0.2, 0.2);
      rng->Next();
      yy = y + rng->GetRangeValue(-0.2, 0.2);
      rng->Next();
      zz = rng->GetRangeValue(-0.5, 0.5);
      points->InsertNextPoint(xx, yy, zz);
    }
  }

  // Add the grid points to a polydata object
  vtkNew<vtkPolyData> inputPolyData;
  inputPolyData->SetPoints(points);

  // Triangulate the grid points
  vtkNew<vtkDelaunay2D> delaunay;
  delaunay->SetInputData(inputPolyData);
  delaunay->Update();
  vtkPolyData* outputPolyData = delaunay->GetOutput();

  double bounds[6];
  outputPolyData->GetBounds(bounds);

  // Find min and max z
  double minz = bounds[4];
  double maxz = bounds[5];

  std::cout.precision(3);
  std::cout << "minz: " << std::right << std::setw(6) << minz << std::endl;
  std::cout << "maxz: " << std::right << std::setw(6) << maxz << std::endl;
  std::cout.precision(0);

  // Create the color map
  vtkNew<vtkLookupTable> colorLookupTable;
  colorLookupTable->SetTableRange(minz, maxz);
  colorLookupTable->Build();

  // Generate the colors for each point based on the color map
  vtkNew<vtkUnsignedCharArray> colors;
  colors->SetNumberOfComponents(3);
  colors->SetName("Colors");

  std::cout << "There are " << outputPolyData->GetNumberOfPoints() << " points."
            << std::endl;
  std::cout.precision(6);
  std::cout << std::fixed;
  for (int i = 0; i < outputPolyData->GetNumberOfPoints(); i++)
  {
    double p[3];
    outputPolyData->GetPoint(i, p);

    double dcolor[3];
    colorLookupTable->GetColor(p[2], dcolor);
    // std::cout << "dcolor: " << std::right << std::setw(8) << dcolor[0] << " "
    //          << std::right << std::setw(8)<< dcolor[1] << " "
    //          << std::right << std::setw(8) << dcolor[2]
    //          << std::endl;
    unsigned char color[3];
    for (unsigned int j = 0; j < 3; j++)
    {
      color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
    }
    // std::cout << "color:  " << std::right << std::setw(8) << (int)color[0]
    //          << " " << std::right << std::setw(8) << (int)color[1] << " "
    //          << std::right << std::setw(8)
    //          << (int)color[2] << std::endl;

    colors->InsertNextTupleValue(color);
  }
  std::cout.precision(0);

  outputPolyData->GetPointData()->SetScalars(colors);

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(outputPolyData);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->SetWindowName("ColoredElevationMap");

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(namedColors->GetColor3d("DarkSlateGray").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
