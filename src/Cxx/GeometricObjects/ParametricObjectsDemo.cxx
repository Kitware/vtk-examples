#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtk_cli11.h>
#include <vtk_fmt.h>
// clang-format off
#include VTK_FMT(fmt/format.h)
// clang-format on

#include <vtkParametricBoy.h>
#include <vtkParametricConicSpiral.h>
#include <vtkParametricCrossCap.h>
#include <vtkParametricDini.h>
#include <vtkParametricEllipsoid.h>
#include <vtkParametricEnneper.h>
#include <vtkParametricFigure8Klein.h>
#include <vtkParametricKlein.h>
#include <vtkParametricMobius.h>
#include <vtkParametricRandomHills.h>
#include <vtkParametricRoman.h>
#include <vtkParametricSpline.h>
#include <vtkParametricSuperEllipsoid.h>
#include <vtkParametricSuperToroid.h>
#include <vtkParametricTorus.h>
// Extra parametric surfaces.
#include <vtkParametricBohemianDome.h>
#include <vtkParametricBour.h>
#include <vtkParametricCatalanMinimal.h>
#include <vtkParametricHenneberg.h>
#include <vtkParametricKuen.h>
#include <vtkParametricPluckerConoid.h>
#include <vtkParametricPseudosphere.h>

// For glyphing
#include <vtkArrowSource.h>
#include <vtkDataSet.h>
#include <vtkGlyph3D.h>
#include <vtkMaskPoints.h>

// For writing out the image.
#include <vtkPNGWriter.h>
#include <vtkWindowToImageFilter.h>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace {

/**
 * Create a map of the parametric functions and set some parameters.
 *
 * @return The map of functions.
 */
std::map<std::string, vtkSmartPointer<vtkParametricFunction>>
GetParametricFunctions();

/**
 * Get the centre of the object from the bounding box.
 */
std::vector<double> GetCentre(const std::vector<double>& bounds);

/**
 * Calculate the maximum length of side of the bounding box.
 */
double GetMaximumLength(const std::vector<double>& bounds);

/**
 * Display the dimensions of the bounding box, maximum diagonal length
 *    and coordinates of the centre.
 *
 * @param name: The name of the object.
 * @param bounds: The bounding box of the object.
 *
 */
void DisplayBoundingBoxAndCenter(std::string const& name,
                                 std::vector<double> const& bounds);

class PrintCallback : public vtkCallbackCommand
{
public:
  PrintCallback() : fn_{""}, imageQuality_(1), rgba_(true)
  {
  }

  static PrintCallback* New()
  {
    return new PrintCallback;
  }

  /*
   * Create a vtkCallbackCommand and reimplement it.
   *
   */
  void Execute(vtkObject* caller, unsigned long /*evId*/, void*) override
  {
    // Note the use of reinterpret_cast to cast the caller to the expected type.
    auto rwi = reinterpret_cast<vtkRenderWindowInteractor*>(caller);

    // Get the keypress
    std::string key = rwi->GetKeySym();
    if (key == "k" && !fn_.empty())
    {
      auto w2If = vtkSmartPointer<vtkWindowToImageFilter>::New();
      w2If->SetInput(rwi->GetRenderWindow());
      w2If->SetScale(this->imageQuality_, this->imageQuality_);
      if (rgba_)
      {
        w2If->SetInputBufferTypeToRGBA();
      }
      else
      {
        w2If->SetInputBufferTypeToRGB();
      }
      // Read from the front buffer.
      w2If->ReadFrontBufferOn();
      w2If->Update();
      auto writer = vtkSmartPointer<vtkPNGWriter>::New();
      writer->SetFileName(fn_.c_str());
      writer->SetInputConnection(w2If->GetOutputPort());
      writer->Write();
      std::cout << "Screenshot saved to: " << writer->GetFileName()
                << std::endl;
    }
  }

  /**
   * Set the parameters for writing the
   * the render window view to an image file.
   *
   * @param fileName The png image file name.
   * @param imageQuality The image quality.
   * @param rgba The buffer type, (if true, there is no background in the
   * screenshot).
   *
   */
  void SetParameters(const std::string& fileName, int imageQuality = 1,
                     bool rgba = true)
  {
    if (!fileName.empty())
    {
      this->fn_ = fileName;
      std::string ext{".png"};
      auto found = this->fn_.find_last_of(".");
      if (found == std::string::npos)
      {
        this->fn_ += ext;
      }
      else
      {
        this->fn_ = fileName.substr(0, fileName.find_last_of(".")) += ext;
      }
    }
    this->imageQuality_ = imageQuality;
    this->rgba_ = rgba;
  }

private:
  PrintCallback(const PrintCallback&) = delete;
  void operator=(const PrintCallback&) = delete;

  std::string fn_;
  int imageQuality_;
  bool rgba_;
};

} // namespace

int main(int argc, char* argv[])
{
  CLI::App app{"Display the parametric surfaces."};

  // Define options
  std::string surfaceName;
  app.add_option("-s, --surface_name", surfaceName, "The name of the surface.");
  auto backFace{false};
  app.add_flag("-b, --back_face", backFace, "Color the back face.");
  auto normals{false};
  app.add_flag("-n, --normals", normals, "Display normals.");
  auto limits{false};
  app.add_flag("-l, --limits", limits,
               "Display the geometric bounds of the object.");

  CLI11_PARSE(app, argc, argv);

  // Get the parametric functions.
  auto pfn = GetParametricFunctions();

  // Check for a single surface.
  std::pair<std::string, bool> singleSurface{"", false};
  if (!surfaceName.empty())
  {
    std::string sn = surfaceName;
    std::transform(sn.begin(), sn.end(), sn.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    // Is the surface name in the map?
    for (auto const& t : pfn)
    {
      std::string k = t.first;
      std::transform(k.begin(), k.end(), k.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      if (sn == k)
      {
        singleSurface.first = t.first;
        singleSurface.second = true;
      }
    }
  }
  if (!surfaceName.empty() && !singleSurface.second)
  {
    std::cout << "Nonexistent surface: " << surfaceName << std::endl;
    return EXIT_FAILURE;
  }

  auto rendererSize = 200;
  auto gridColumnDimensions = 5;
  auto gridRowDimensions = 5;

  if (singleSurface.second)
  {
    rendererSize = 1000;
    gridColumnDimensions = 1;
    gridRowDimensions = 1;
  }

  vtkNew<vtkNamedColors> colors;

  // Create one text property for all.
  vtkNew<vtkTextProperty> textProperty;
  textProperty->SetJustificationToCentered();
  textProperty->SetFontSize(rendererSize / 12);
  textProperty->SetColor(colors->GetColor3d("LavenderBlush").GetData());

  // Create a parametric function source, renderer, mapper, and actor
  // for each object.
  std::vector<vtkSmartPointer<vtkParametricFunctionSource>> pfnSrcs;
  std::vector<vtkSmartPointer<vtkRenderer>> renderers;
  std::vector<vtkSmartPointer<vtkPolyDataMapper>> mappers;
  std::vector<vtkSmartPointer<vtkActor>> actors;
  std::vector<vtkSmartPointer<vtkTextMapper>> textmappers;
  std::vector<vtkSmartPointer<vtkActor2D>> textactors;

  // Glyph the normals.
  std::vector<vtkSmartPointer<vtkMaskPoints>> maskPts;
  std::vector<vtkSmartPointer<vtkArrowSource>> arrow;
  std::vector<vtkSmartPointer<vtkGlyph3D>> glyph;
  std::vector<vtkSmartPointer<vtkPolyDataMapper>> glyphMapper;
  std::vector<vtkSmartPointer<vtkActor>> glyphActor;

  auto backProperty = vtkSmartPointer<vtkProperty>::New();
  if (backFace)
  {
    backProperty->SetColor(colors->GetColor3d("Peru").GetData());
  }

  // Now decide on the surfaces to build.
  std::map<std::string, vtkSmartPointer<vtkParametricFunction>> surfaces;
  if (singleSurface.second)
  {
    surfaces[singleSurface.first] = pfn[singleSurface.first];
  }
  else
  {
    surfaces = pfn;
  }

  // The bounding boxes for each object.
  std::map<std::string, std::vector<double>> boundingBoxes;
  std::map<int, std::string> indexedNames;
  //  The index of each parametric object.
  auto objIdx = -1;
  for (auto const& obj : surfaces)
  {
    objIdx++;
    indexedNames[objIdx] = obj.first;
    pfnSrcs.push_back(vtkSmartPointer<vtkParametricFunctionSource>::New());
    pfnSrcs[objIdx]->SetParametricFunction(obj.second);
    pfnSrcs[objIdx]->SetUResolution(51);
    pfnSrcs[objIdx]->SetVResolution(51);
    pfnSrcs[objIdx]->SetWResolution(51);
    pfnSrcs[objIdx]->Update();

    mappers.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
    mappers[objIdx]->SetInputConnection(pfnSrcs[objIdx]->GetOutputPort());

    actors.push_back(vtkSmartPointer<vtkActor>::New());
    actors[objIdx]->SetMapper(mappers[objIdx]);
    actors[objIdx]->GetProperty()->SetColor(
        colors->GetColor3d("NavajoWhite").GetData());
    if (backFace)
    {
      actors[objIdx]->SetBackfaceProperty(backProperty);
    }

    textmappers.push_back(vtkSmartPointer<vtkTextMapper>::New());
    textmappers[objIdx]->SetInput(obj.first.c_str());
    textmappers[objIdx]->SetTextProperty(textProperty);

    textactors.push_back(vtkSmartPointer<vtkActor2D>::New());
    textactors[objIdx]->SetMapper(textmappers[objIdx]);
    textactors[objIdx]->SetPosition(rendererSize / 2.0, 8);

    renderers.push_back(vtkSmartPointer<vtkRenderer>::New());
    renderers[objIdx]->SetBackground(
        colors->GetColor3d("MidnightBlue").GetData());

    double bounds[6];
    pfnSrcs[objIdx]->GetOutput()->GetBounds(bounds);
    std::vector<double> v(std::begin(bounds), std::end(bounds));
    boundingBoxes[obj.first] = v;

    if (normals)
    {
      // Glyphing
      maskPts.push_back(vtkSmartPointer<vtkMaskPoints>::New());
      maskPts[objIdx]->RandomModeOn();
      maskPts[objIdx]->SetMaximumNumberOfPoints(150);
      maskPts[objIdx]->SetInputConnection(pfnSrcs[objIdx]->GetOutputPort());

      arrow.push_back(vtkSmartPointer<vtkArrowSource>::New());
      arrow[objIdx]->SetTipResolution(16);
      arrow[objIdx]->SetTipLength(0.3);
      arrow[objIdx]->SetTipRadius(0.1);

      auto glyphScale = GetMaximumLength(boundingBoxes[obj.first]);

      glyph.push_back(vtkSmartPointer<vtkGlyph3D>::New());
      glyph[objIdx]->SetSourceConnection(arrow[objIdx]->GetOutputPort());
      glyph[objIdx]->SetInputConnection(maskPts[objIdx]->GetOutputPort());
      glyph[objIdx]->SetVectorModeToUseNormal();
      glyph[objIdx]->SetScaleFactor(glyphScale / 10.0);
      glyph[objIdx]->OrientOn();
      glyph[objIdx]->Update();

      glyphMapper.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
      glyphMapper[objIdx]->SetInputConnection(glyph[objIdx]->GetOutputPort());

      glyphActor.push_back(vtkSmartPointer<vtkActor>::New());
      glyphActor[objIdx]->SetMapper(glyphMapper[objIdx]);
      glyphActor[objIdx]->GetProperty()->SetColor(
          colors->GetColor3d("GreenYellow").GetData());
    }
  }

  // Need a renderer even if there is no actor.
  for (auto i = objIdx; i < gridColumnDimensions * gridRowDimensions; i++)
  {
    renderers.push_back(vtkSmartPointer<vtkRenderer>::New());
    static_cast<vtkRenderer*>(renderers.back().GetPointer())
        ->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
    indexedNames[i] = "";
  }

  auto renWin = vtkSmartPointer<vtkRenderWindow>::New();
  renWin->SetSize(rendererSize * gridColumnDimensions,
                  rendererSize * gridRowDimensions);

  for (auto row = 0; row < gridRowDimensions; row++)
  {
    for (auto col = 0; col < gridColumnDimensions; col++)
    {
      auto index = row * gridColumnDimensions + col;
      // (xmin, ymin, xmax, ymax)
      double viewport[4] = {
          static_cast<double>(col) * rendererSize /
              (gridColumnDimensions * rendererSize),
          static_cast<double>(gridRowDimensions - (row + 1)) * rendererSize /
              (gridRowDimensions * rendererSize),
          static_cast<double>(col + 1) * rendererSize /
              (gridColumnDimensions * rendererSize),
          static_cast<double>(gridRowDimensions - row) * rendererSize /
              (gridRowDimensions * rendererSize)};
      renWin->AddRenderer(renderers[index]);
      renderers[index]->SetViewport(viewport);
      if (index > objIdx)
      {
        continue;
      }
      renderers[index]->AddActor(actors[index]);
      // Normals can only be computed for polygons and triangle strips.
      // The Spline is a line.
      if (normals && indexedNames[index] != "Spline")
      {
        renderers[index]->AddActor(glyphActor[index]);
      }
      renderers[index]->AddActor(textactors[index]);
      renderers[index]->ResetCamera();
      renderers[index]->GetActiveCamera()->Azimuth(30);
      renderers[index]->GetActiveCamera()->Elevation(-30);
      renderers[index]->GetActiveCamera()->Zoom(0.9);
      renderers[index]->ResetCameraClippingRange();
    }
  }

  if (limits)
  {
    for (auto const& obj : boundingBoxes)
    {
      DisplayBoundingBoxAndCenter(obj.first, obj.second);
    }
  }

  std::string fn = "ParametricObjectsDemo";
  if (!surfaceName.empty())
  {
    fn = singleSurface.first;
  }
  renWin->SetWindowName(fn.c_str());

  vtkNew<vtkRenderWindowInteractor> iRen;
  vtkNew<PrintCallback> printCallback;
  printCallback->SetParameters(fn, 1, false);

  iRen->SetRenderWindow(renWin);
  iRen->AddObserver(vtkCommand::KeyPressEvent, printCallback);

  iRen->Initialize();
  iRen->Start();

  return EXIT_SUCCESS;
}

namespace {

std::map<std::string, vtkSmartPointer<vtkParametricFunction>>
GetParametricFunctions()
{
  std::map<std::string, vtkSmartPointer<vtkParametricFunction>> pfn;
  pfn["Boy"] = vtkSmartPointer<vtkParametricBoy>::New();
  pfn["ConicSpiral"] = vtkSmartPointer<vtkParametricConicSpiral>::New();
  pfn["CrossCap"] = vtkSmartPointer<vtkParametricCrossCap>::New();
  pfn["Dini"] = vtkSmartPointer<vtkParametricDini>::New();
  pfn["Ellipsoid"] = vtkSmartPointer<vtkParametricEllipsoid>::New();
  pfn["Enneper"] = vtkSmartPointer<vtkParametricEnneper>::New();
  pfn["Figure8Klein"] = vtkSmartPointer<vtkParametricFigure8Klein>::New();
  pfn["Klein"] = vtkSmartPointer<vtkParametricKlein>::New();
  pfn["Mobius"] = vtkSmartPointer<vtkParametricMobius>::New();
  pfn["RandomHills"] = vtkSmartPointer<vtkParametricRandomHills>::New();
  pfn["Roman"] = vtkSmartPointer<vtkParametricRoman>::New();
  pfn["SuperEllipsoid"] = vtkSmartPointer<vtkParametricSuperEllipsoid>::New();
  pfn["SuperToroid"] = vtkSmartPointer<vtkParametricSuperToroid>::New();
  pfn["Torus"] = vtkSmartPointer<vtkParametricTorus>::New();
  pfn["Spline"] = vtkSmartPointer<vtkParametricSpline>::New();
  // Extra parametric surfaces.
  pfn["BohemianDome"] = vtkSmartPointer<vtkParametricBohemianDome>::New();
  pfn["Bour"] = vtkSmartPointer<vtkParametricBour>::New();
  pfn["CatalanMinimal"] = vtkSmartPointer<vtkParametricCatalanMinimal>::New();
  pfn["Henneberg"] = vtkSmartPointer<vtkParametricHenneberg>::New();
  pfn["Kuen"] = vtkSmartPointer<vtkParametricKuen>::New();
  pfn["PluckerConoid"] = vtkSmartPointer<vtkParametricPluckerConoid>::New();
  pfn["Pseudosphere"] = vtkSmartPointer<vtkParametricPseudosphere>::New();

  // Now set some parameters.
  static_cast<vtkParametricEllipsoid*>(pfn["Ellipsoid"].GetPointer())
      ->SetXRadius(0.5);
  static_cast<vtkParametricEllipsoid*>(pfn["Ellipsoid"].GetPointer())
      ->SetYRadius(2.0);
  static_cast<vtkParametricMobius*>(pfn["Mobius"].GetPointer())->SetRadius(2.0);
  static_cast<vtkParametricMobius*>(pfn["Mobius"].GetPointer())
      ->SetMinimumV(-0.5);
  static_cast<vtkParametricMobius*>(pfn["Mobius"].GetPointer())
      ->SetMaximumV(0.5);
  static_cast<vtkParametricRandomHills*>(pfn["RandomHills"].GetPointer())
      ->AllowRandomGenerationOn();
  static_cast<vtkParametricRandomHills*>(pfn["RandomHills"].GetPointer())
      ->SetRandomSeed(1);
  static_cast<vtkParametricRandomHills*>(pfn["RandomHills"].GetPointer())
      ->SetNumberOfHills(30);
  static_cast<vtkParametricSuperEllipsoid*>(pfn["SuperEllipsoid"].GetPointer())
      ->SetN1(0.5);
  static_cast<vtkParametricSuperEllipsoid*>(pfn["SuperEllipsoid"].GetPointer())
      ->SetN2(0.4);
  static_cast<vtkParametricSuperToroid*>(pfn["SuperToroid"].GetPointer())
      ->SetN1(0.5);
  static_cast<vtkParametricSuperToroid*>(pfn["SuperToroid"].GetPointer())
      ->SetN2(3.0);
  // The spline needs points
  vtkNew<vtkPoints> inputPoints;
  vtkNew<vtkMinimalStandardRandomSequence> rng;
  rng->SetSeed(8775070);
  for (auto p = 0; p < 10; p++)
  {
    std::array<double, 3> xyz{0, 0, 0};
    for (auto& idx : xyz)
    {
      idx = rng->GetRangeValue(-1.0, 1.0);
      rng->Next();
    }
    inputPoints->InsertNextPoint(xyz.data());
  }
  static_cast<vtkParametricSpline*>(pfn["Spline"].GetPointer())
      ->SetPoints(inputPoints);
  // Extra parametric surfaces.
  static_cast<vtkParametricBohemianDome*>(pfn["BohemianDome"].GetPointer())
      ->SetA(5.0);
  static_cast<vtkParametricBohemianDome*>(pfn["BohemianDome"].GetPointer())
      ->SetB(1.0);
  static_cast<vtkParametricBohemianDome*>(pfn["BohemianDome"].GetPointer())
      ->SetC(2.0);
  static_cast<vtkParametricKuen*>(pfn["Kuen"].GetPointer())->SetDeltaV0(0.001);
  return pfn;
}

std::vector<double> GetCentre(const std::vector<double>& bounds)
{
  std::vector<double> centre;
  if (bounds.size() != 6)
  {
    return centre;
  }
  for (size_t i = 1; i < bounds.size(); i += 2)
  {
    centre.push_back(bounds[i] - (bounds[i] - bounds[i - 1]) / 2.0);
  }
  return centre;
}

double GetMaximumLength(const std::vector<double>& bounds)
{
  auto maxLen = -1.0;
  if (bounds.size() != 6)
  {
    return maxLen;
  }
  for (size_t i = 0; i < bounds.size(); i += 2)
  {
    maxLen = std::max(maxLen, std::abs(bounds[i + 1] - bounds[i]));
  }
  return maxLen;
}

void DisplayBoundingBoxAndCenter(std::string const& name,
                                 std::vector<double> const& bounds)
{
  if (bounds.size() != 6)
  {
    return;
  }
  auto maxLength = GetMaximumLength(bounds);
  auto centre = GetCentre(bounds);

  auto s = fmt::format("{:<21s}\n", name);
  s += fmt::format("{:21s}: ", "  Bounds (min, max)");
  s += fmt::format("{:s}:({:6.2f}, {:6.2f}) ", "x", bounds[0], bounds[1]);
  s += fmt::format("{:s}:({:6.2f}, {:6.2f}) ", "y", bounds[2], bounds[3]);
  s += fmt::format("{:s}:({:6.2f}, {:6.2f})\n", "z", bounds[4], bounds[5]);
  s += fmt::format("{:21s}: {:6.2f}\n", "  Maximum side length", maxLength);
  s += fmt::format("{:21s}: ({:6.2f}, {:6.2f}, {:6.2f})\n",
                   "  Centre (x, y, z)", centre[0], centre[1], centre[2]);
  std::cout << s << std::endl;
}

} // namespace
