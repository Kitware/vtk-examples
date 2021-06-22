#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkDataSetMapper.h>
#include <vtkExplicitStructuredGrid.h>
#include <vtkExplicitStructuredGridToUnstructuredGrid.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridToExplicitStructuredGrid.h>

#include <array>
#include <vector>

namespace {

vtkNew<vtkExplicitStructuredGrid>
CreateExplicitStructuredGrid(std::array<int, 3> const& dimensions,
                             std::array<int, 3> const& spacing);

vtkNew<vtkUnstructuredGrid>
ConvertToUnstructuredGrid(vtkExplicitStructuredGrid* grid);

vtkNew<vtkExplicitStructuredGrid>
ConvertToExplicitStructuredGrid(vtkUnstructuredGrid* grid);
} // namespace

int main(int, char*[])
{

  vtkNew<vtkNamedColors> colors;

  std::array<int, 3> dimensions{0, 0, 0};
  std::array<int, 3> spacing{1, 1, 1};
  dimensions = {5, 6, 7};
  spacing = {20, 10, 1};
  auto grid = CreateExplicitStructuredGrid(dimensions, spacing);
  auto ug = ConvertToUnstructuredGrid(grid);
  auto esg = ConvertToExplicitStructuredGrid(ug);

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputData(grid);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->EdgeVisibilityOn();
  actor->GetProperty()->LightingOff();
  actor->GetProperty()->SetColor(colors->GetColor3d("Seashell").GetData());

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  vtkNew<vtkRenderWindow> window;
  window->AddRenderer(renderer);
  window->SetWindowName("CreateESGrid");
  window->SetSize(1024, 768);
  window->Render();

  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(8.383354, -72.468670, 94.262605);
  camera->SetFocalPoint(42.295234, 21.111537, -0.863606);
  camera->SetViewUp(0.152863, 0.676710, 0.720206);
  camera->SetDistance(137.681759);
  camera->SetClippingRange(78.173985, 211.583658);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(window);
  vtkNew<vtkInteractorStyleRubberBandPick> style;
  interactor->SetInteractorStyle(style);
  window->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {
vtkNew<vtkExplicitStructuredGrid>
CreateExplicitStructuredGrid(std::array<int, 3> const& dimensions,
                             std::array<int, 3> const& spacing)
{
  auto ni = dimensions[0];
  auto nj = dimensions[1];
  auto nk = dimensions[2];
  auto si = spacing[0];
  auto sj = spacing[1];
  auto sk = spacing[2];
  vtkNew<vtkPoints> points;
  for (auto z = 0; z < nk * sk; z += sk)
  {
    for (auto y = 0; y < nj * sj; y += sj)
    {
      for (auto x = 0; x < ni * si; x += si)
      {
        points->InsertNextPoint(x, y, z);
      }
    }
  }
  // std::vector < std::vector<int> > cells;
  vtkNew<vtkCellArray> cells;
  for (vtkIdType k = 0; k < nk - 1; ++k)
  {
    for (vtkIdType j = 0; j < nj - 1; ++j)
    {
      for (vtkIdType i = 0; i < ni - 1; ++i)
      {
        // std::cout << "(" << i << ", " << j << ", " << k << ")" << std::endl;
        std::vector<std::array<vtkIdType, 8>> multiIndex;
        multiIndex.push_back({i, i + 1, i + 1, i, i, i + 1, i + 1, i});
        multiIndex.push_back({j, j, j + 1, j + 1, j, j, j + 1, j + 1});
        multiIndex.push_back({k, k, k, k, k + 1, k + 1, k + 1, k + 1});
        // Now let's emulate:
        // pts = np.ravel_multi_index(multi_index, dimensions, order='F')
        cells->InsertNextCell(8);
        std::vector<vtkIdType> cell;
        for (auto idx = 0; idx < 8; ++idx)
        {
          vtkIdType ii = multiIndex[0][idx];
          vtkIdType jj = multiIndex[1][idx];
          vtkIdType kk = multiIndex[2][idx];
          cells->InsertCellPoint(ii + jj * ni + kk * nj * ni);
        }
      }
    }
  }

  vtkNew<vtkExplicitStructuredGrid> grid;
  grid->SetDimensions(ni, nj, nk);
  grid->SetPoints(points);
  grid->SetCells(cells);

  return grid;
}

vtkNew<vtkUnstructuredGrid>
ConvertToUnstructuredGrid(vtkExplicitStructuredGrid* grid)
{
  vtkNew<vtkExplicitStructuredGridToUnstructuredGrid> converter;
  converter->SetInputData(grid);
  converter->Update();
  vtkNew<vtkUnstructuredGrid> ug;
  ug->DeepCopy(converter->GetOutput());
  return ug;
}

vtkNew<vtkExplicitStructuredGrid>
ConvertToExplicitStructuredGrid(vtkUnstructuredGrid* grid)
{
  vtkNew<vtkUnstructuredGridToExplicitStructuredGrid> converter;
  converter->SetInputData(grid);
  // An explicit structured grid.
  // The "BLOCK_I", "BLOCK_J" and"BLOCK_K" cell arrays are required.
  converter->SetInputArrayToProcess(0, 0, 0, 1, "BLOCK_I");
  converter->SetInputArrayToProcess(1, 0, 0, 1, "BLOCK_J");
  converter->SetInputArrayToProcess(2, 0, 0, 1, "BLOCK_K");
  converter->Update();
  vtkNew<vtkExplicitStructuredGrid> esg;
  esg->DeepCopy(converter->GetOutput());
  return esg;
}

} // namespace
