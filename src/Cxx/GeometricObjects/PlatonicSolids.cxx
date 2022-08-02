#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlatonicSolidSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

#include <functional>
#include <string>
#include <tuple>
#include <vector>

namespace {

/** Get the platonic solid names and initial orientations.
 *
 * @return The solids and their initial orientations.
 */
std::vector<std::tuple<std::string, double, double, double>>
GetNameOrientation();

/** Get a specialised lookup table for the platonic solids.
 *
 * Since each face of a vtkPlatonicSolidSource has a different
 * cell scalar, we create a lookup table with a different colour
 * for each face.
 * The colors have been carefully chosen so that adjacent cells
 * are colored distinctly.
 *
 * @return The lookup table.
 */
vtkNew<vtkLookupTable> GetPlatonicLUT();

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  std::vector<vtkSmartPointer<vtkPolyDataMapper>> mappers;
  std::vector<vtkSmartPointer<vtkActor>> actors;
  std::vector<vtkSmartPointer<vtkTextMapper>> textMappers;
  std::vector<vtkSmartPointer<vtkActor2D>> textActors;
  std::vector<vtkSmartPointer<vtkRenderer>> renderers;

  // Create a common text property.
  vtkNew<vtkTextProperty> textProperty;
  textProperty->SetFontSize(16);
  textProperty->SetJustificationToCentered();

  // Create the render window and interactor.
  vtkNew<vtkRenderWindow> renWin;
  renWin->SetWindowName("PlatonicSolids");
  vtkNew<vtkRenderWindowInteractor> iRen;
  iRen->SetRenderWindow(renWin);

  // Platonic solids and orientation for display.
  auto nameOrientation = GetNameOrientation();
  auto lut = GetPlatonicLUT();
  std::vector<vtkSmartPointer<vtkPlatonicSolidSource>> PlatonicSolids;

  for (auto i = 0; i < nameOrientation.size(); ++i)
  {
    PlatonicSolids.push_back(vtkSmartPointer<vtkPlatonicSolidSource>::New());
    PlatonicSolids[i]->SetSolidType(i);

    mappers.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
    mappers[i]->SetInputConnection(PlatonicSolids[i]->GetOutputPort());
    mappers[i]->SetLookupTable(lut);
    mappers[i]->SetScalarRange(0, 19);

    actors.push_back(vtkSmartPointer<vtkActor>::New());
    actors[i]->SetMapper(mappers[i]);

    textMappers.push_back(vtkSmartPointer<vtkTextMapper>::New());
    textMappers[i]->SetInput(std::get<0>(nameOrientation[i]).c_str());
    textMappers[i]->SetTextProperty(textProperty);

    textActors.push_back(vtkSmartPointer<vtkActor2D>::New());
    textActors[i]->SetMapper(textMappers[i]);
    textActors[i]->SetPosition(120, 16);

    renderers.push_back(vtkSmartPointer<vtkRenderer>::New());
    renderers[i]->AddActor(actors[i]);
    renderers[i]->AddViewProp(textActors[i]);

    renWin->AddRenderer(renderers[i]);
  }

  // Set up the viewports
  auto gridDimensionsX = 3;
  auto gridDimensionsY = 2;
  auto rendererSize = 300;
  renWin->SetSize(rendererSize * gridDimensionsX,
                  rendererSize * gridDimensionsY);
  for (auto row = 0; row < gridDimensionsY; ++row)
  {
    for (auto col = 0; col < gridDimensionsX; ++col)
    {
      auto index = row * gridDimensionsX + col;

      // (xmin, ymin, xmax, ymax)
      double viewport[4] = {
          static_cast<double>(col) / gridDimensionsX,
          static_cast<double>(gridDimensionsY - (row + 1)) / gridDimensionsY,
          static_cast<double>(col + 1) / gridDimensionsX,
          static_cast<double>(gridDimensionsY - row) / gridDimensionsY};
      if (index > (actors.size() - 1))
      {
        // Add a renderer even if there is no actor.
        // This makes the render window background all the same color.
        vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
        ren->SetBackground(colors->GetColor3d("SlateGray").GetData());
        ren->SetViewport(viewport);
        renWin->AddRenderer(ren);
        continue;
      }

      renderers[index]->SetViewport(viewport);
      renderers[index]->SetBackground(
          colors->GetColor3d("SlateGray").GetData());
      renderers[index]->ResetCamera();
      renderers[index]->GetActiveCamera()->Azimuth(
          std::get<1>(nameOrientation[index]));
      renderers[index]->GetActiveCamera()->Elevation(
          std::get<2>(nameOrientation[index]));
      renderers[index]->GetActiveCamera()->Zoom(
          std::get<3>(nameOrientation[index]));
    }
  }

  iRen->Initialize();
  renWin->Render();
  iRen->Start();

  return EXIT_SUCCESS;
}

namespace {

std::vector<std::tuple<std::string, double, double, double>>
GetNameOrientation()
{
  auto getTuple = [](std::string const& name, double const& azimuth,
                     double const& elevation, double const& zoom) {
    return std::make_tuple(name, azimuth, elevation, zoom);
  };

  std::vector<std::tuple<std::string, double, double, double>> res;
  res.push_back(getTuple("Tetrahedron", 45.0, 30.0, 1.0));
  res.push_back(getTuple("Cube", -60.0, 45.0, 0.8));
  res.push_back(getTuple("Octahedron", -15.0, 10.0, 1.0));
  res.push_back(getTuple("Icosahedron", 4.5, 18.0, 1.0));
  res.push_back(getTuple("Dodecahedron", 171.0, 22.0, 1.0));

  return res;
}

vtkNew<vtkLookupTable> GetPlatonicLUT()
{
  vtkNew<vtkLookupTable> lut;
  lut->SetNumberOfTableValues(20);
  lut->SetTableRange(0.0, 19.0);
  lut->Build();
  lut->SetTableValue(0, 0.1, 0.1, 0.1);
  lut->SetTableValue(1, 0, 0, 1);
  lut->SetTableValue(2, 0, 1, 0);
  lut->SetTableValue(3, 0, 1, 1);
  lut->SetTableValue(4, 1, 0, 0);
  lut->SetTableValue(5, 1, 0, 1);
  lut->SetTableValue(6, 1, 1, 0);
  lut->SetTableValue(7, 0.9, 0.7, 0.9);
  lut->SetTableValue(8, 0.5, 0.5, 0.5);
  lut->SetTableValue(9, 0.0, 0.0, 0.7);
  lut->SetTableValue(10, 0.5, 0.7, 0.5);
  lut->SetTableValue(11, 0, 0.7, 0.7);
  lut->SetTableValue(12, 0.7, 0, 0);
  lut->SetTableValue(13, 0.7, 0, 0.7);
  lut->SetTableValue(14, 0.7, 0.7, 0);
  lut->SetTableValue(15, 0, 0, 0.4);
  lut->SetTableValue(16, 0, 0.4, 0);
  lut->SetTableValue(17, 0, 0.4, 0.4);
  lut->SetTableValue(18, 0.4, 0, 0);
  lut->SetTableValue(19, 0.4, 0, 0.4);
  return lut;
}

} // namespace
