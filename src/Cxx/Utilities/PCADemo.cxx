#include <vtkActor.h>
#include <vtkBoxMuellerRandomSequence.h>
#include <vtkDoubleArray.h>
#include <vtkGlyph3DMapper.h>
#include <vtkLine.h>
#include <vtkLineSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPCAStatistics.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTable.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkVertexGlyphFilter.h>

namespace {
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

int main(int, char*[])
{
  vtkNew<vtkBoxMuellerRandomSequence> randomSequence;

  vtkNew<vtkPoints> points;
  for (unsigned int i = 0; i < 200; i++)
  {
    auto x = randomSequence->GetScaledValue(0, 2);
    randomSequence->Next();
    auto y = randomSequence->GetScaledValue(0, 5);
    randomSequence->Next();
    points->InsertNextPoint(x, y, 0);
  }
  vtkNew<vtkPolyData> polydata;
  polydata->SetPoints(points);

  vtkNew<vtkTransform> transform;
  transform->RotateZ(30);

  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetTransform(transform);
  transformFilter->SetInputData(polydata);
  transformFilter->Update();

  // These would be all of your "x" values.
  vtkNew<vtkDoubleArray> xArray;
  xArray->SetNumberOfComponents(1);
  xArray->SetName("x");

  // These would be all of your "y" values.
  vtkNew<vtkDoubleArray> yArray;
  yArray->SetNumberOfComponents(1);
  yArray->SetName("y");

  for (vtkIdType i = 0; i < polydata->GetNumberOfPoints(); i++)
  {
    double p[3];
    transformFilter->GetOutput()->GetPoint(i, p);
    xArray->InsertNextValue(p[0]);
    yArray->InsertNextValue(p[1]);
  }

  vtkNew<vtkTable> datasetTable;
  datasetTable->AddColumn(xArray);
  datasetTable->AddColumn(yArray);

  vtkNew<vtkPCAStatistics> pcaStatistics;
  pcaStatistics->SetInputData(vtkStatisticsAlgorithm::INPUT_DATA, datasetTable);

  pcaStatistics->SetColumnStatus("x", 1);
  pcaStatistics->SetColumnStatus("y", 1);

  pcaStatistics->RequestSelectedColumns();
  pcaStatistics->SetDeriveOption(true);
  pcaStatistics->Update();

  ///////// Eigenvalues ////////////
  vtkNew<vtkDoubleArray> eigenvalues;
  pcaStatistics->GetEigenvalues(eigenvalues);
  for (vtkIdType i = 0; i < eigenvalues->GetNumberOfTuples(); i++)
  {
    std::cout << "Eigenvalue " << i << " = " << eigenvalues->GetValue(i)
              << std::endl;
  }

  ///////// Eigenvectors ////////////
  vtkNew<vtkDoubleArray> eigenvectors;

  pcaStatistics->GetEigenvectors(eigenvectors);

  vtkNew<vtkDoubleArray> evec1;
  pcaStatistics->GetEigenvector(0, evec1);

  vtkNew<vtkDoubleArray> evec2;
  pcaStatistics->GetEigenvector(1, evec2);

  double scale = 3.0;
  vtkNew<vtkLineSource> vector1Source;
  vector1Source->SetPoint1(0, 0, 0);
  vector1Source->SetPoint1(scale * evec1->GetValue(0),
                           scale * evec1->GetValue(1), 0);

  vtkNew<vtkPolyDataMapper> vec1Mapper;
  vec1Mapper->SetInputConnection(vector1Source->GetOutputPort());

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkActor> vector1Actor;
  vector1Actor->SetMapper(vec1Mapper);
  vector1Actor->GetProperty()->SetColor(
      colors->GetColor3d("LimeGreen").GetData());
  vector1Actor->GetProperty()->SetLineWidth(3);

  vtkNew<vtkLineSource> vector2Source;
  vector2Source->SetPoint1(0, 0, 0);
  vector2Source->SetPoint1(scale * evec2->GetValue(0),
                           scale * evec2->GetValue(1), 0);

  vtkNew<vtkPolyDataMapper> vec2Mapper;
  vec2Mapper->SetInputConnection(vector2Source->GetOutputPort());

  vtkNew<vtkActor> vector2Actor;
  vector2Actor->SetMapper(vec2Mapper);
  vector2Actor->GetProperty()->SetColor(
      colors->GetColor3d("Crimson").GetData());
  vector2Actor->GetProperty()->SetLineWidth(3);

  // Project all of the points onto the eigenvector with
  // the largest eigenvalues
  double p0[3];
  p0[0] = -100 * evec1->GetValue(0);
  p0[1] = -100 * evec1->GetValue(1);
  p0[2] = 0;
  double p1[3];
  p1[0] = 100 * evec1->GetValue(0);
  p1[1] = 100 * evec1->GetValue(1);
  p1[2] = 0;

  vtkNew<vtkPoints> projectedPoints;
  for (vtkIdType i = 0; i < polydata->GetNumberOfPoints(); i++)
  {
    double p[3];
    transformFilter->GetOutput()->GetPoint(i, p);
    double t;
    double closestPoint[3];
    vtkLine::DistanceToLine(p, p0, p1, t, closestPoint);
    double newP[3];
    double v[3];
    vtkMath::Subtract(p1, p0, v);
    vtkMath::Normalize(v);
    vtkMath::MultiplyScalar(v, t);
    vtkMath::Add(p0, v, newP);
    projectedPoints->InsertNextPoint(t, 0, 0);
  }

  vtkNew<vtkPolyData> projectedPolyData;
  projectedPolyData->SetPoints(projectedPoints);

  vtkNew<vtkVertexGlyphFilter> projectedGlyphFilter;
  projectedGlyphFilter->SetInputData(projectedPolyData);
  projectedGlyphFilter->Update();

  vtkNew<vtkPolyDataMapper> projectedMapper;
  projectedMapper->SetInputConnection(projectedGlyphFilter->GetOutputPort());
  vtkNew<vtkActor> projectedActor;
  projectedActor->SetMapper(projectedMapper);
  projectedActor->GetProperty()->SetPointSize(2);
  projectedActor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

  // There will be one render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(600, 300);
  renderWindow->SetWindowName("PCADemo");

  // And one interactor
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
  double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

  // Setup both renderers
  vtkNew<vtkRenderer> leftRenderer;
  renderWindow->AddRenderer(leftRenderer);
  leftRenderer->SetViewport(leftViewport);
  leftRenderer->SetBackground(colors->GetColor3d("Burlywood").GetData());

  vtkNew<vtkRenderer> rightRenderer;
  renderWindow->AddRenderer(rightRenderer);
  rightRenderer->SetViewport(rightViewport);
  rightRenderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkVertexGlyphFilter> glyphFilter;
  glyphFilter->SetInputConnection(transformFilter->GetOutputPort());
  glyphFilter->Update();

  vtkNew<vtkPolyDataMapper> originalMapper;
  originalMapper->SetInputConnection(glyphFilter->GetOutputPort());
  vtkNew<vtkActor> originalActor;
  originalActor->SetMapper(originalMapper);
  originalActor->GetProperty()->SetPointSize(1);

  // Map the points to spheres
  auto sphereActor =
      PointToGlyph(transformFilter->GetOutput()->GetPoints(), 0.007);
  sphereActor->GetProperty()->SetColor(colors->GetColor3d("Blue").GetData());

  // leftRenderer->AddActor(originalActor);
  leftRenderer->AddActor(sphereActor);
  leftRenderer->AddActor(vector1Actor);
  leftRenderer->AddActor(vector2Actor);

  rightRenderer->AddActor(projectedActor);

  leftRenderer->ResetCamera();
  rightRenderer->ResetCamera();

  renderWindow->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {

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

} // namespace
