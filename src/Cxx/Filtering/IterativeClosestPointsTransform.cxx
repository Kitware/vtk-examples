#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkGlyph3DMapper.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkLandmarkTransform.h>
#include <vtkLine.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

namespace {
void CreatePolyData(vtkSmartPointer<vtkPolyData> polydata);
void PerturbPolyData(vtkSmartPointer<vtkPolyData> polydata);
void TranslatePolyData(vtkSmartPointer<vtkPolyData> polydata);
void AxesLines(vtkSmartPointer<vtkPolyData> linesPolyData);

/**
 * Convert points to glyphs.
 *
 * @param points - The points to glyph
 * @param scale - The scale, used to determine the size of the glyph
 * representing the point, expressed as a fraction of the largest side of the
 * bounding box surrounding the points. e.g. 0.05
 *
 * @return The actor.
 */
vtkSmartPointer<vtkActor> PointToGlyph(vtkPoints* points, double const& scale);

} // namespace

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyData> source;
  vtkNew<vtkPolyData> target;
  // An aid to orient the view of the created data.
  vtkNew<vtkPolyData> linesPolyData;
  auto createdData = false;

  if (argc == 3)
  {
    std::cout << "Reading data..." << std::endl;
    std::string strSource = argv[1];
    std::string strTarget = argv[2];
    vtkNew<vtkXMLPolyDataReader> sourceReader;
    sourceReader->SetFileName(strSource.c_str());
    sourceReader->Update();
    source->ShallowCopy(sourceReader->GetOutput());

    vtkNew<vtkXMLPolyDataReader> targetReader;
    targetReader->SetFileName(strTarget.c_str());
    targetReader->Update();
    target->ShallowCopy(targetReader->GetOutput());
  }
  else
  {
    std::cout << "Creating data..." << std::endl;
    CreatePolyData(source);
    target->ShallowCopy(source);
    TranslatePolyData(target);
    PerturbPolyData(target);
    AxesLines(linesPolyData);
    createdData = true;
  }

  // Setup ICP transform
  vtkNew<vtkIterativeClosestPointTransform> icp;
  icp->SetSource(source);
  icp->SetTarget(target);
  icp->GetLandmarkTransform()->SetModeToRigidBody();
  icp->SetMaximumNumberOfIterations(20);
  // icp->StartByMatchingCentroidsOn();
  icp->Modified();
  icp->Update();

  // Get the resulting transformation matrix (this matrix takes the source
  // points to the target points)
  vtkSmartPointer<vtkMatrix4x4> m = icp->GetMatrix();
  std::cout << "The resulting matrix is: " << *m << std::endl;

  // Transform the source points by the ICP solution
  vtkNew<vtkTransformPolyDataFilter> icpTransformFilter;
  icpTransformFilter->SetInputData(source);
  icpTransformFilter->SetTransform(icp);
  icpTransformFilter->Update();

  /*
  // If you need to take the target points to the source points, the matrix is:
  icp->Inverse();
  vtkSmartPointer<vtkMatrix4x4> minv = icp->GetMatrix();
  std::cout << "The resulting inverse matrix is: " << *minv << std::cout;
  */

  // Visualize
  // Map the points to spheres
  auto sourceActor = PointToGlyph(source->GetPoints(), 0.03);
  sourceActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

  auto targetActor = PointToGlyph(target->GetPoints(), 0.03);
  targetActor->GetProperty()->SetColor(colors->GetColor3d("Lime").GetData());

  auto solutionActor =
      PointToGlyph(icpTransformFilter->GetOutput()->GetPoints(), 0.03);
  solutionActor->GetProperty()->SetColor(colors->GetColor3d("Blue").GetData());

  vtkNew<vtkPolyDataMapper> axesMapper;
  vtkNew<vtkActor> axesActor;
  if (createdData)
  {
    axesMapper->SetInputData(linesPolyData);

    axesActor->SetMapper(axesMapper);
    axesActor->GetProperty()->SetLineWidth(1);
  }

  // Create a renderer, render window, and interactor
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->SetWindowName("IterativeClosestPointsTransform");

  // Add the actor to the scene
  renderer->AddActor(sourceActor);
  renderer->AddActor(targetActor);
  renderer->AddActor(solutionActor);
  if (createdData)
  {
    renderer->AddActor(axesActor);
  }

  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace // anonymous
{

void CreatePolyData(vtkSmartPointer<vtkPolyData> polydata)
{
  // This function creates a set of 4 points (the origin and a point unit
  // distance along each axis)

  vtkNew<vtkPoints> points;

  // Create points
  double origin[3] = {0.0, 0.0, 0.0};
  points->InsertNextPoint(origin);
  double p1[3] = {1.0, 0.0, 0.0};
  points->InsertNextPoint(p1);
  double p2[3] = {0.0, 1.0, 0.0};
  points->InsertNextPoint(p2);
  double p3[3] = {0.0, 0.0, 1.0};
  points->InsertNextPoint(p3);

  vtkNew<vtkPolyData> temp;
  temp->SetPoints(points);

  vtkNew<vtkVertexGlyphFilter> vertexFilter;
  vertexFilter->SetInputData(temp);
  vertexFilter->Update();

  polydata->ShallowCopy(vertexFilter->GetOutput());
}

void PerturbPolyData(vtkSmartPointer<vtkPolyData> polydata)
{
  vtkNew<vtkPoints> points;
  points->ShallowCopy(polydata->GetPoints());

  for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
  {
    double p[3];
    points->GetPoint(i, p);
    double perturb[3];
    if (i % 3 == 0)
    {
      perturb[0] = .1;
      perturb[1] = 0;
      perturb[2] = 0;
    }
    else if (i % 3 == 1)
    {
      perturb[0] = 0;
      perturb[1] = .1;
      perturb[2] = 0;
    }
    else
    {
      perturb[0] = 0;
      perturb[1] = 0;
      perturb[2] = .1;
    }

    for (unsigned int j = 0; j < 3; j++)
    {
      p[j] += perturb[j];
    }
    points->SetPoint(i, p);
  }

  polydata->SetPoints(points);
}

void TranslatePolyData(vtkSmartPointer<vtkPolyData> polydata)
{
  vtkNew<vtkTransform> transform;
  transform->Translate(0, .3, 0);

  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetInputData(polydata);
  transformFilter->SetTransform(transform);
  transformFilter->Update();

  polydata->ShallowCopy(transformFilter->GetOutput());
}

void AxesLines(vtkSmartPointer<vtkPolyData> linesPolyData)
{
  // Create four points
  double origin[3] = {0.0, 0.0, 0.0};
  double p0[3] = {1.5, 0.0, 0.0};
  double p1[3] = {0.0, 1.5, 0.0};
  double p2[3] = {0.0, 0.0, 1.5};

  // Create a vtkPoints container and store the points in it
  vtkNew<vtkPoints> pts;
  pts->InsertNextPoint(origin);
  pts->InsertNextPoint(p0);
  pts->InsertNextPoint(p1);
  pts->InsertNextPoint(p2);

  // Add the points to the polydata container
  linesPolyData->SetPoints(pts);

  // Create the lines (between Origin and p0, ,p1, p2)
  vtkNew<vtkLine> line0;
  line0->GetPointIds()->SetId(0, 0);
  line0->GetPointIds()->SetId(1, 1);
  vtkNew<vtkLine> line1;
  line1->GetPointIds()->SetId(0, 0);
  line1->GetPointIds()->SetId(1, 2);
  vtkNew<vtkLine> line2;
  line2->GetPointIds()->SetId(0, 0);
  line2->GetPointIds()->SetId(1, 3);

  // Create a vtkCellArray container and store the lines in it
  vtkNew<vtkCellArray> lines;
  lines->InsertNextCell(line0);
  lines->InsertNextCell(line1);
  lines->InsertNextCell(line2);

  // Add the lines to the polydata container
  linesPolyData->SetLines(lines);

  vtkNew<vtkNamedColors> namedColors;

  // Create a vtkUnsignedCharArray container and store the colors in it
  vtkNew<vtkUnsignedCharArray> colors;
  colors->SetNumberOfComponents(3);
  colors->InsertNextTupleValue(namedColors->GetColor3ub("DarkRed").GetData());
  colors->InsertNextTupleValue(namedColors->GetColor3ub("DarkGreen").GetData());
  colors->InsertNextTupleValue(
      namedColors->GetColor3ub("MidnightBlue").GetData());

  // Color the lines.
  linesPolyData->GetCellData()->SetScalars(colors);
}

vtkSmartPointer<vtkActor> PointToGlyph(vtkPoints* points, double const& scale)
{
  auto bounds = points->GetBounds();
  double maxLen = 0;
  for (int i = 1; i < 3; ++i)
  {
    maxLen = std::max(bounds[i + 1] - bounds[i], maxLen);
  }

  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(scale * maxLen);

  vtkNew<vtkPolyData> pd;
  pd->SetPoints(points);

  vtkNew<vtkGlyph3DMapper> mapper;
  mapper->SetInputData(pd);
  mapper->SetSourceConnection(sphereSource->GetOutputPort());
  mapper->ScalarVisibilityOff();
  mapper->ScalingOff();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  return actor;
}

} // end anonymous namespace
