#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkColorTransferFunction.h>
#include <vtkContourFilter.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShepardMethod.h>
#include <vtkSmartPointer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVertexGlyphFilter.h>

// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create a set of vertices (polydata)
  vtkNew<vtkPoints> points;
  points->InsertNextPoint(100.0, 0.0, 0.0);
  points->InsertNextPoint(300.0, 0.0, 0.0);

  // Setup colors
  unsigned char white[3] = {255, 255, 255};
  unsigned char black[3] = {0, 0, 0};

  vtkNew<vtkUnsignedCharArray> vertexColors;
  vertexColors->SetNumberOfComponents(3);
  vertexColors->SetName("Colors");
  vertexColors->InsertNextTupleValue(black);
  vertexColors->InsertNextTupleValue(white);

  // Create a scalar array for the pointdata, each value represents the distance
  // of the vertices from the first vertex
  vtkNew<vtkFloatArray> values;
  values->SetNumberOfComponents(1);
  values->SetName("Values");
  values->InsertNextValue(0.0);
  values->InsertNextValue(1.0);

  // We must make two objects, because the ShepardMethod uses the ActiveScalars,
  // as does the renderer!
  vtkNew<vtkPolyData> polydataToProcess;
  polydataToProcess->SetPoints(points);
  polydataToProcess->GetPointData()->SetScalars(values);

  vtkNew<vtkPolyData> polydataToVisualize;
  polydataToVisualize->SetPoints(points);
  polydataToVisualize->GetPointData()->SetScalars(vertexColors);

  vtkNew<vtkVertexGlyphFilter> vertexGlyphFilter;
  vertexGlyphFilter->AddInputData(polydataToVisualize);
  vertexGlyphFilter->Update();

  // Create a mapper and actor
  vtkNew<vtkPolyDataMapper> vertsMapper;
  // vertsMapper->ScalarVisibilityOff();
  vertsMapper->SetInputConnection(vertexGlyphFilter->GetOutputPort());

  vtkNew<vtkActor> vertsActor;
  vertsActor->SetMapper(vertsMapper);
  vertsActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
  vertsActor->GetProperty()->SetPointSize(3);

  // Create a shepard filter to interpolate the vertices over a regularized
  // image grid
  vtkNew<vtkShepardMethod> shepard;
  shepard->SetInputData(polydataToProcess);
  shepard->SetSampleDimensions(2, 2, 2);
  shepard->SetModelBounds(100, 300, -10, 10, -10, 10);
  shepard->SetMaximumDistance(1);

  // Contour the shepard generated image at 3 isovalues
  // The accuracy of the results are highly dependent on how the shepard filter
  // is set up
  vtkNew<vtkContourFilter> contourFilter;
  contourFilter->SetNumberOfContours(3);
  contourFilter->SetValue(0, 0.25);
  contourFilter->SetValue(1, 0.50);
  contourFilter->SetValue(2, 0.75);
  contourFilter->SetInputConnection(shepard->GetOutputPort());
  contourFilter->Update();

  // Create a mapper and actor for the resulting isosurfaces
  vtkNew<vtkPolyDataMapper> contourMapper;
  contourMapper->SetInputConnection(contourFilter->GetOutputPort());
  contourMapper->ScalarVisibilityOn();
  contourMapper->SetColorModeToMapScalars();

  vtkNew<vtkActor> contourActor;
  contourActor->SetMapper(contourMapper);
  contourActor->GetProperty()->SetAmbient(1);
  contourActor->GetProperty()->SetSpecular(0);
  contourActor->GetProperty()->SetDiffuse(0);

  // Report the results of the interpolation
  double* range = contourFilter->GetOutput()->GetScalarRange();

  std::cout << "Shepard interpolation:" << std::endl;
  std::cout << "contour output scalar range: " << range[0] << ", " << range[1]
            << std::endl;

  vtkIdType nCells = contourFilter->GetOutput()->GetNumberOfCells();
  double bounds[6];
  for (vtkIdType i = 0; i < nCells; ++i)
  {
    if (i %
        2) // each isosurface value only has 2 cells to report on the odd ones
    {
      contourFilter->GetOutput()->GetCellBounds(i, bounds);
      std::cout << "cell " << i << ", x position: " << bounds[0] << std::endl;
    }
  }

  // Create a transfer function to color the isosurfaces
  vtkNew<vtkColorTransferFunction> lut;
  lut->SetColorSpaceToRGB();
  lut->AddRGBPoint(range[0], 0, 0, 0); // black
  lut->AddRGBPoint(range[1], 1, 1, 1); // white
  lut->SetScaleToLinear();

  contourMapper->SetLookupTable(lut);

  // Create a renderer, render window and interactor
  vtkNew<vtkRenderer> renderer;
  renderer->GradientBackgroundOn();
  renderer->SetBackground(colors->GetColor3d("Blue").GetData());
  renderer->SetBackground2(colors->GetColor3d("Magenta").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("BackgroundTexture");

  renderer->AddActor(contourActor);
  renderer->AddActor(vertsActor);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Position the camera so that the image produced is viewable
  vtkCamera* camera = renderer->GetActiveCamera();
  camera->SetPosition(450, 100, 100);
  camera->SetFocalPoint(200, 0, 0);
  camera->SetViewUp(0, 0, 1);

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
