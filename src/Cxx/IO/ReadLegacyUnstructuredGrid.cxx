#include <vtkBrush.h>
#include <vtkCategoryLegend.h>
#include <vtkCellData.h>
#include <vtkColor.h>
#include <vtkContextScene.h>
#include <vtkContextTransform.h>
#include <vtkContextView.h>
#include <vtkExtractEdges.h>
#include <vtkGenericCell.h>
#include <vtkGlyph3DMapper.h>
#include <vtkLookupTable.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkScalarsToColors.h>
#include <vtkShrinkFilter.h>
#include <vtkSphereSource.h>
#include <vtkTextMapper.h>
#include <vtkTubeFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkVariant.h>
#include <vtkVariantArray.h>

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <vtkCellIterator.h>
#include <vtkCellTypes.h>
#include <vtkLabeledDataMapper.h>

// Note that:
// vtkExtractEdges moved from vtkFiltersExtraction to vtkFiltersCore in
// VTK commit d9981b9aeb93b42d1371c6e295d76bfdc18430bd

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " filename.vtk" << std::endl;
    return EXIT_FAILURE;
  }
  vtkNew<vtkNamedColors> colors;

  // Create the reader for the data.
  std::string filename = argv[1];
  std::cout << "Loading " << filename.c_str() << std::endl;
  vtkNew<vtkUnstructuredGridReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();

  vtkNew<vtkExtractEdges> extractEdges;
  extractEdges->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkVariantArray> legendValues;
  auto it = reader->GetOutput()->NewCellIterator();
  for (it->InitTraversal(); !it->IsDoneWithTraversal(); it->GoToNextCell())
  {
    vtkNew<vtkGenericCell> cell;
    it->GetCell(cell);
    std::string cellName =
        vtkCellTypes::GetClassNameFromTypeId(cell->GetCellType());
#if 0
    std::cout << cellName
              << " NumberOfPoints: " << cell->GetNumberOfPoints()
              << " CellDimension: " << cell->GetCellDimension()
              << std::endl;
#endif
    legendValues->InsertNextValue(vtkVariant(cellName));
  }
  it->Delete();

  // Tube the edges
  vtkNew<vtkTubeFilter> tubes;
  tubes->SetInputConnection(extractEdges->GetOutputPort());
  tubes->SetRadius(.05);
  tubes->SetNumberOfSides(21);

  vtkNew<vtkPolyDataMapper> edgeMapper;
  edgeMapper->SetInputConnection(tubes->GetOutputPort());
  edgeMapper->SetScalarRange(0, 26);

  vtkNew<vtkActor> edgeActor;
  edgeActor->SetMapper(edgeMapper);
  edgeActor->GetProperty()->SetSpecular(0.6);
  edgeActor->GetProperty()->SetSpecularPower(30);
  ;

  // Glyph the points
  vtkNew<vtkSphereSource> sphere;
  sphere->SetPhiResolution(21);
  sphere->SetThetaResolution(21);
  sphere->SetRadius(0.08);

  vtkNew<vtkGlyph3DMapper> pointMapper;
  pointMapper->SetInputConnection(reader->GetOutputPort());
  pointMapper->SetSourceConnection(sphere->GetOutputPort());
  pointMapper->ScalingOff();
  pointMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> pointActor;
  pointActor->SetMapper(pointMapper);
  pointActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Banana").GetData());
  pointActor->GetProperty()->SetSpecular(0.6);
  pointActor->GetProperty()->SetSpecularColor(1.0, 1.0, 1.0);
  pointActor->GetProperty()->SetSpecularPower(100);
  ;

  // Label the points
  vtkNew<vtkLabeledDataMapper> labelMapper;
  labelMapper->SetInputConnection(reader->GetOutputPort());
  vtkNew<vtkActor2D> labelActor;
  labelActor->SetMapper(labelMapper);

  // The geometry
  vtkNew<vtkShrinkFilter> geometryShrink;
  geometryShrink->SetInputConnection(reader->GetOutputPort());
  geometryShrink->SetShrinkFactor(.8);

  // NOTE: We must copy the originalLut because the CategorialLegend
  // needs an indexed lookup table, but the geometryMapper uses a
  // non-index lookup table
  vtkNew<vtkLookupTable> categoricalLut;
  vtkSmartPointer<vtkLookupTable> originalLut =
      reader->GetOutput()->GetCellData()->GetScalars()->GetLookupTable();

  categoricalLut->DeepCopy(originalLut);
  categoricalLut->IndexedLookupOn();

  vtkNew<vtkDataSetMapper> geometryMapper;
  geometryMapper->SetInputConnection(geometryShrink->GetOutputPort());
  geometryMapper->SetScalarModeToUseCellData();
  geometryMapper->SetScalarRange(0, 11);

  vtkNew<vtkActor> geometryActor;
  geometryActor->SetMapper(geometryMapper);
  geometryActor->GetProperty()->SetLineWidth(3);
  geometryActor->GetProperty()->EdgeVisibilityOn();
  geometryActor->GetProperty()->SetEdgeColor(0, 0, 0);

  // Legend
  for (int v = 0; v < legendValues->GetNumberOfTuples(); ++v)
  {
    categoricalLut->SetAnnotation(legendValues->GetValue(v),
                                  legendValues->GetValue(v).ToString());
  }
  vtkNew<vtkCategoryLegend> legend;
  legend->SetScalarsToColors(categoricalLut);
  legend->SetValues(legendValues);
  legend->SetTitle("Cell Type");
  legend->GetBrush()->SetColor(colors->GetColor4ub("Silver").GetData());

  vtkNew<vtkContextTransform> placeLegend;
  placeLegend->AddItem(legend);
  placeLegend->Translate(640 - 20, 480 - 12 * 16);

  vtkNew<vtkContextView> contextView;
  contextView->GetScene()->AddItem(placeLegend);

  vtkSmartPointer<vtkRenderer> renderer = contextView->GetRenderer();

  vtkSmartPointer<vtkRenderWindow> renderWindow =
      contextView->GetRenderWindow();

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(geometryActor);
  renderer->AddActor(labelActor);
  renderer->AddActor(edgeActor);
  renderer->AddActor(pointActor);
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkCamera> aCamera;
  aCamera->Azimuth(-40.0);
  aCamera->Elevation(50.0);

  renderer->SetActiveCamera(aCamera);
  renderer->ResetCamera();

  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("ReadLegacyUnstructuredGrid");
  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
