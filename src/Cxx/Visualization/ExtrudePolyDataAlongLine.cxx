#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkCleanPolyData.h>
#include <vtkDiskSource.h>
#include <vtkFeatureEdges.h>
#include <vtkFrenetSerretFrame.h>
#include <vtkLineSource.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRuledSurfaceFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStripper.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>

#include <vtksys/SystemTools.hxx>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
}

int main(int argc, char* argv[])
{
  // For testing
  vtkMath::RandomSeed(7859821);
  // Read a polydata
  vtkSmartPointer<vtkPolyData> polyData = ReadPolyData(argc > 1 ? argv[1] : "");
  ;

  int numberOfContours = polyData->GetNumberOfLines();
  std::cout << "Number of contours: " << numberOfContours << std::endl;

  // Generate  some random points
  int numberOfPoints = 10;
  vtkNew<vtkPointSource> pointSource;
  pointSource->SetNumberOfPoints(numberOfPoints);
  pointSource->Update();

  vtkPoints* points = pointSource->GetOutput()->GetPoints();

  vtkNew<vtkParametricSpline> spline;
  spline->SetPoints(points);

  vtkNew<vtkParametricFunctionSource> functionSource;
  functionSource->SetParametricFunction(spline);
  functionSource->SetUResolution(50 * numberOfPoints);
  functionSource->SetVResolution(50 * numberOfPoints);
  functionSource->SetWResolution(50 * numberOfPoints);

  // Create the frame
  vtkNew<vtkFrenetSerretFrame> frame;
  frame->SetInputConnection(functionSource->GetOutputPort());
  frame->ConsistentNormalsOn();
  frame->Update();

  frame->GetOutput()->GetPointData()->SetActiveVectors("FSNormals");
  frame->GetOutput()->GetPointData()->SetActiveVectors("FSTangents");
  frame->GetOutput()->GetPointData()->SetActiveVectors("FSBinormals");

  vtkPoints* linePoints = frame->GetOutput()->GetPoints();

  std::vector<vtkSmartPointer<vtkAppendPolyData>> skeletons;
  for (int i = 0; i < numberOfContours; ++i)
  {
    skeletons.push_back(vtkSmartPointer<vtkAppendPolyData>::New());
  }

  for (int i = 0; i < linePoints->GetNumberOfPoints(); ++i)
  {
    vtkNew<vtkTransform> transform;

    // Compute a basis
    double normalizedX[3];
    frame->GetOutput()->GetPointData()->SetActiveVectors("FSNormals");
    frame->GetOutput()->GetPointData()->GetVectors()->GetTuple(i, normalizedX);
    double normalizedY[3];
    frame->GetOutput()->GetPointData()->SetActiveVectors("FSBinormals");
    frame->GetOutput()->GetPointData()->GetVectors()->GetTuple(i, normalizedY);
    double normalizedZ[3];
    frame->GetOutput()->GetPointData()->SetActiveVectors("FSTangents");
    frame->GetOutput()->GetPointData()->GetVectors()->GetTuple(i, normalizedZ);

    // Create the direction cosine matrix
    vtkNew<vtkMatrix4x4> matrix;
    matrix->Identity();
    for (unsigned int j = 0; j < 3; ++j)
    {
      matrix->SetElement(j, 0, normalizedX[j]);
      matrix->SetElement(j, 1, normalizedY[j]);
      matrix->SetElement(j, 2, normalizedZ[j]);
    }

    transform->Translate(linePoints->GetPoint(i)[0], linePoints->GetPoint(i)[1],
                         linePoints->GetPoint(i)[2]);
    transform->Scale(.02, .02, .02);
    transform->Concatenate(matrix);

    vtkNew<vtkTransformPolyDataFilter> transformPD;
    vtkNew<vtkPolyData> polyDataCopy;
    polyDataCopy->DeepCopy(polyData);

    transformPD->SetTransform(transform);
    transformPD->SetInputData(polyDataCopy);
    transformPD->Update();

    vtkNew<vtkPolyDataConnectivityFilter> contours;
    contours->SetInputConnection(transformPD->GetOutputPort());
    contours->Update();
    for (int r = 0; r < contours->GetNumberOfExtractedRegions(); ++r)
    {
      contours->SetExtractionModeToSpecifiedRegions();
      contours->InitializeSpecifiedRegionList();
      contours->AddSpecifiedRegion(r);
      contours->Update();
      vtkNew<vtkPolyData> skeleton;
      skeleton->DeepCopy(contours->GetOutput());
      skeletons[r]->AddInputData(skeleton);
    }
  }
  vtkNew<vtkRenderer> renderer;

  for (int i = 0; i < numberOfContours; ++i)
  {
    vtkNew<vtkRuledSurfaceFilter> ruled;
    ruled->SetRuledModeToPointWalk();
    ruled->CloseSurfaceOff();
    ruled->SetOnRatio(1);
    ruled->SetDistanceFactor(10000000);
    ruled->SetInputConnection(skeletons[i]->GetOutputPort());

    vtkNew<vtkPolyDataNormals> normals;
    normals->SetInputConnection(ruled->GetOutputPort());
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(normals->GetOutputPort());
    vtkNew<vtkProperty> backProp;
    backProp->SetColor(1.0, 0.3882, 0.278);
    vtkNew<vtkActor> actor;
    actor->SetBackfaceProperty(backProp);
    actor->GetProperty()->SetColor(0.8900, 0.8100, 0.3400);
    actor->SetMapper(mapper);
    renderer->AddActor(actor);
  }

  // Setup render window, renderer, and interactor
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->SetBackground(.4, .5, .7);

  // Pick a good view
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(120);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.1);
  renderer->ResetCameraClippingRange();

  renderWindow->SetSize(512, 512);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName)
{
  vtkSmartPointer<vtkPolyData> polyData;
  std::string extension =
      vtksys::SystemTools::GetFilenameExtension(std::string(fileName));
  if (extension == ".ply")
  {
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(fileName);
    reader->Update();

    polyData = reader->GetOutput();
  }
  else if (extension == ".vtp")
  {
    vtkNew<vtkXMLPolyDataReader> reader;
    reader->SetFileName(fileName);
    reader->Update();

    // Center the data
    vtkNew<vtkTransform> transform;
    std::cout << reader->GetOutput()->GetCenter()[0] << ", "
              << reader->GetOutput()->GetCenter()[1] << ", "
              << reader->GetOutput()->GetCenter()[2] << std::endl;
    transform->Translate(-reader->GetOutput()->GetCenter()[0],
                         -reader->GetOutput()->GetCenter()[1],
                         -reader->GetOutput()->GetCenter()[2]);
    vtkNew<vtkTransformPolyDataFilter> transformPD;
    transformPD->SetTransform(transform);
    transformPD->SetInputData(reader->GetOutput());
    transformPD->Update();

    polyData = transformPD->GetOutput();
  }
  else if (extension == ".obj")
  {
    vtkNew<vtkOBJReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".g")
  {
    vtkNew<vtkBYUReader> reader;
    reader->SetGeometryFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".stl")
  {
    vtkNew<vtkSTLReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtk")
  {
    vtkNew<vtkPolyDataReader> reader;
    reader->SetFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    vtkNew<vtkDiskSource> diskSource;
    diskSource->SetCircumferentialResolution(3);

    vtkNew<vtkCleanPolyData> clean;
    clean->SetInputConnection(diskSource->GetOutputPort());

    vtkNew<vtkFeatureEdges> edges;
    edges->SetInputConnection(clean->GetOutputPort());
    edges->NonManifoldEdgesOff();
    edges->ManifoldEdgesOff();
    edges->BoundaryEdgesOn();
    edges->FeatureEdgesOff();

    vtkNew<vtkStripper> stripper;
    stripper->SetInputConnection(edges->GetOutputPort());
    stripper->Update();
    polyData = stripper->GetOutput();

#if 0
    vtkNew<vtkLineSource> lineSource;
    lineSource->SetResolution(10);
    lineSource->Update();

    polyData = lineSource->GetOutput();
#endif
  }
  return polyData;
}
} // namespace
