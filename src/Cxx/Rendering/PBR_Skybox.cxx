#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCameraPass.h>
#include <vtkCubeSource.h>
#include <vtkEquirectangularToCubeMapTexture.h>
#include <vtkFloatArray.h>
#include <vtkHDRReader.h>
#include <vtkImageFlip.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLightsPass.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOpaquePass.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOpenGLTexture.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkOverlayPass.h>
#include <vtkParametricBoy.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricMobius.h>
#include <vtkParametricRandomHills.h>
#include <vtkParametricTorus.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataTangents.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderPassCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSequencePass.h>
#include <vtkSkybox.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkTexture.h>
#include <vtkTexturedSphereSource.h>
#include <vtkToneMappingPass.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkVersion.h>
#include <vtk_cli11.h>
#include <vtk_jsoncpp.h>

// For writing out the image.
#include <vtkImageWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkWindowToImageFilter.h>

#include <array>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#if VTK_VERSION_NUMBER >= 90020210809ULL
#define VTK_HAS_COW 1
#endif

#if VTK_HAS_COW
#include <vtkCameraOrientationWidget.h>
#endif

namespace fs = std::filesystem;

namespace {

/**
 * Check the VTK version.
 *
 * @param major: Major version.
 * @param major: Minor version.
 * @param major: Build version.
 *
 * @return True if the requested VTK version is greater or equal to the actual
 * VTK version.
 */
bool VTKVersionOk(unsigned long long const& major,
                  unsigned long long const& minor,
                  unsigned long long const& build);

struct Parameters
{
  std::map<std::string, std::string> parameters;
  std::vector<std::string> cubemap;
  bool skybox{true};
  bool parsedOk{false};
};

/**
 * Read the parameters from a json file and check that the file paths exist.
 *
 * @param fnPath: The path to the json file.
 * @param surfaceName:  The surface name.
 * @param parameters:  The parameters.
 */
void GetParameters(const fs::path fnPath, const std::string& surfaceName,
                   Parameters& parameters);

std::string DisplayParameters(Parameters& parameters);

/**
 *  Read an equirectangular environment file and convert it to a texture.
 *
 * @param fileName: The equirectangular file path.
 * @param flipX:  Flip x-axis of the texture.
 *
 * @return The texture.
 */
vtkNew<vtkTexture> EquirectangularFileToTexture(std::string const& fileName,
                                                bool flipX = false);
/**
 * Read an equirectangular environment file and convert it to a cube map.
 *
 * @param fileName: The equirectangular file path.
 * @param flipX:  Flip x-axis of the texture.
 *
 * @return The cubemap texture.
 */
vtkNew<vtkTexture> EquirectangularFileToCubemap(std::string const& fileName,
                                                bool flipX = false);

/**
 * Read six images forming a cubemap.
 *
 * @param fileNames: The paths to the six cubemap files.
 *
 * @return The cubemap texture.
 */
vtkNew<vtkTexture> ReadCubeMap(std::vector<std::string> const& fileNames);

// Some sample surfaces to try.
vtkSmartPointer<vtkPolyData> GetBoy();
vtkSmartPointer<vtkPolyData> GetMobius();
vtkSmartPointer<vtkPolyData> GetRandomHills();
vtkSmartPointer<vtkPolyData> GetTorus();
vtkSmartPointer<vtkPolyData> GetSphere();
vtkSmartPointer<vtkPolyData> GetCube();

/**
 * Generate u, v texture coordinates on a parametric surface.
 *
 * @param uResolution: u resolution
 * @param vResolution: v resolution
 * @param pd: The polydata representing the surface.
 *
 * @return The polydata with the texture coordinates added.
 */
vtkNew<vtkPolyData> UVTcoords(const float& uResolution,
                              const float& vResolution, vtkNew<vtkPolyData> pd);

class SliderCallbackExposure : public vtkCommand
{
public:
  static SliderCallbackExposure* New()
  {
    return new SliderCallbackExposure;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    this->property->SetExposure(value);
  }
  SliderCallbackExposure() : property(nullptr)
  {
  }
  vtkToneMappingPass* property;
};

class SliderCallbackMetallic : public vtkCommand
{
public:
  static SliderCallbackMetallic* New()
  {
    return new SliderCallbackMetallic;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    this->property->SetMetallic(value);
  }
  SliderCallbackMetallic() : property(nullptr)
  {
  }
  vtkProperty* property;
};

class SliderCallbackRoughness : public vtkCommand
{
public:
  static SliderCallbackRoughness* New()
  {
    return new SliderCallbackRoughness;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation2D*>(
                       sliderWidget->GetRepresentation())
                       ->GetValue();
    this->property->SetRoughness(value);
  }
  SliderCallbackRoughness() : property(nullptr)
  {
  }
  vtkProperty* property;
};

struct SliderProperties
{
  // Set up the sliders
  double tubeWidth{0.008};
  double sliderLength{0.075};
  double sliderWidth{0.025};
  double endCapLength = 0.025;
  double endCapWidth = 0.025;
  double titleHeight{0.025};
  double labelHeight{0.020};

  double minimumValue{0.0};
  double maximumValue{1.0};
  double initialValue{0.0};

  std::array<double, 2> p1{0.02, 0.1};
  std::array<double, 2> p2{0.18, 0.1};

  std::string title{""};

  std::string titleColor{"Black"};
  std::string labelColor{"Black"};
  std::string valueColor{"DarkSlateGray"};
  std::string sliderColor{"BurlyWood"};
  std::string selectedColor{"Lime"};
  std::string barColor{"Black"};
  std::string barEndsColor{"Indigo"};
};

vtkNew<vtkSliderWidget> MakeSliderWidget(SliderProperties const& properties);

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
    if (this->fn_.empty())
    {
      std::cerr << "A file name is required." << std::endl;
      return;
    }

    // Note the use of reinterpret_cast to cast the caller to the expected type.
    auto rwi = reinterpret_cast<vtkRenderWindowInteractor*>(caller);

    // Get the keypress
    std::string key = rwi->GetKeySym();
    if (key == "k" && !this->fn_.empty())
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
      std::vector<std::string> jpeg{".jpeg", ".jpg"};
      auto writer = vtkSmartPointer<vtkImageWriter>::New();
      if (std::find(jpeg.begin(), jpeg.end(), ext_) != jpeg.end())
      {
        writer = vtkSmartPointer<vtkJPEGWriter>::New();
      }
      else
      {
        writer = vtkSmartPointer<vtkPNGWriter>::New();
      }
      writer->SetFileName(this->fn_.c_str());
      writer->SetInputConnection(w2If->GetOutputPort());
      writer->Write();
      std::cout << "Screenshot saved to: " << writer->GetFileName()
                << std::endl;
    }
  }

  /**
   * Set the parameters for writing the
   *  render window view to an image file.
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
    if (fileName.empty())
    {
      std::cerr << "A file name is required." << std::endl;
      return;
    }
    auto pth = fs::absolute(fileName);
    auto ext = pth.extension().generic_string();
    if (!pth.has_extension())
      ext = ".png";
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](char c) { return std::tolower(c); });
    std::vector<std::string> validSuffixes{".jpeg", ".jpg", ".png"};
    if (std::find(validSuffixes.begin(), validSuffixes.end(), ext) ==
        validSuffixes.end())
    {
      ext = ".png";
    }
    this->ext_ = ext;
    this->fn_ = pth.replace_extension(this->ext_).generic_string();
    this->imageQuality_ = imageQuality;
    this->rgba_ = rgba;
  }

private:
  PrintCallback(const PrintCallback&) = delete;
  void operator=(const PrintCallback&) = delete;

  std::string fn_;
  std::string ext_;
  int imageQuality_;
  bool rgba_;
};

} // namespace

int main(int argc, char* argv[])
{
  if (!VTKVersionOk(9, 0, 0))
  {
    std::cerr << "You need VTK version 9.0 or greater to run this program."
              << std::endl;
    return EXIT_FAILURE;
  }

  CLI::App app{"Demonstrates physically based rendering, image based lighting "
               "and a skybox."};

  // Define options
  std::string fileName;
  app.add_option("fileName", fileName,
                 "The path to the JSON file containing the parameters.");
  std::string surfaceName;
  app.add_option("-s, --surface", surfaceName,
                 "The name of the surface. Overrides the surface entry in the "
                 "json file.");
  auto useCubemap{false};
  app.add_flag("-c, --use_cubemap", useCubemap,
               "Build the cubemap from the six cubemap files. Overrides the "
               "equirectangular entry in the json file.");

  CLI11_PARSE(app, argc, argv);

  auto fnPath = fs::path(fileName);
  if (!fnPath.has_extension())
  {
    fnPath.replace_extension(".json");
  }
  if (!fs::is_regular_file(fnPath))
  {
    std::cerr << "Unable to find: " << fnPath << std::endl;
    return EXIT_FAILURE;
  }

  Parameters parameters;
  GetParameters(fnPath, surfaceName, parameters);
  if (!parameters.parsedOk)
  {
    return EXIT_FAILURE;
  }

  auto res = DisplayParameters(parameters);
  std::cout << res << std::endl;

  // Choose how to generate the skybox.
  vtkNew<vtkTexture> cubeMap;
  vtkNew<vtkTexture> envTexture;
  auto hasEnvTexture = false;
  auto isHDR = false;
  auto hasCubeMap = false;
  auto gammaCorrect = false;
  if (!parameters.skybox)
  {
    useCubemap = false;
  }
  if (useCubemap && !parameters.cubemap.empty())
  {
    std::cout << "Using the cubemap files to generate the environment texture."
              << std::endl;
    cubeMap = ReadCubeMap(parameters.cubemap);
    hasCubeMap = true;
  }
  else if (parameters.parameters.find("equirectangular") !=
               parameters.parameters.end() &&
           !parameters.parameters["equirectangular"].empty())
  {
    std::cout
        << "Using the equirectangular file to generate the environment texture."
        << std::endl;
    // Setting flip_X to true allows us to set the environment texture of the
    // object correctly.
    envTexture = EquirectangularFileToTexture(
        parameters.parameters["equirectangular"], true);
    envTexture->MipmapOn();
    envTexture->InterpolateOn();
    hasEnvTexture = true;
    std::string extension = fs::path(parameters.parameters["equirectangular"])
                                .extension()
                                .generic_string();
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](char c) { return std::tolower(c); });
    std::string extensions{".hdr .pic"};
    if (std::string(extensions).find(extension, 0) != std::string::npos)
    {
      isHDR = true;
      gammaCorrect = true;
    }
    if (parameters.skybox)
    {
      cubeMap = EquirectangularFileToCubemap(
          parameters.parameters["equirectangular"], false);
      hasCubeMap = true;
    }
  }
  else if (!parameters.cubemap.empty())
  {
    std::cout << "Using the cubemap files to generate the environment texture."
              << std::endl;
    cubeMap = ReadCubeMap(parameters.cubemap);
    hasCubeMap = true;
  }
  else
  {
    std::cerr << "An environment texture is required,\n"
              << "please add the necessary equiangular"
              << " or cubemap file paths to the json file." << std::endl;
    return EXIT_FAILURE;
  }

  // Get the surface.
  std::string desiredSurface = parameters.parameters["object"];
  std::transform(desiredSurface.begin(), desiredSurface.end(),
                 desiredSurface.begin(),
                 [](char c) { return std::tolower(c); });
  std::map<std::string, int> availableSurfaces = {
      {"boy", 0},   {"mobius", 1}, {"randomhills", 2},
      {"torus", 3}, {"sphere", 4}, {"cube", 5}};
  if (availableSurfaces.find(desiredSurface) == availableSurfaces.end())
  {
    desiredSurface = "boy";
  }
  vtkSmartPointer<vtkPolyData> source;
  switch (availableSurfaces[desiredSurface])
  {
  case 1:
    source = GetMobius();
    break;
  case 2:
    source = GetRandomHills();
    break;
  case 3:
    source = GetTorus();
    break;
  case 4:
    source = GetSphere();
    break;
  case 5:
    source = GetCube();
    break;
  case 0:
  default:
    source = GetBoy();
  };

  vtkNew<vtkNamedColors> colors;

  // Set the background color.
  std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
  colors->SetColor("BkgColor", bkg.data());

  // Check for missing parameters.
  if (parameters.parameters.find("bkgcolor") == parameters.parameters.end())
  {
    parameters.parameters["bkgcolor"] = "BkgColor";
  }
  if (parameters.parameters.find("objcolor") == parameters.parameters.end())
  {
    parameters.parameters["objcolor"] = "White";
  }

  // Build the pipeline.
  // ren_1 is for the slider rendering,
  // ren_2 is for the object rendering.
  vtkNew<vtkRenderer> ren_1;
  vtkNew<vtkOpenGLRenderer> ren_2;
  vtkNew<vtkRenderWindow> renderWindow;
  // The order here is important.
  // This ensures that the sliders will be in ren_1.
  renderWindow->AddRenderer(ren_2);
  renderWindow->AddRenderer(ren_1);
  ren_1->SetViewport(0.0, 0.0, 0.2, 1.0);
  ren_2->SetViewport(0.2, 0.0, 1, 1);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  interactor->SetInteractorStyle(style);

  // Set up tone mapping so we can vary the exposure.
  // Custom Passes.
  vtkNew<vtkCameraPass> cameraP;
  vtkNew<vtkSequencePass> seq;
  vtkNew<vtkOpaquePass> opaque;
  vtkNew<vtkLightsPass> lights;
  vtkNew<vtkOverlayPass> overlay;

  vtkNew<vtkRenderPassCollection> passes;
  passes->AddItem(lights);
  passes->AddItem(opaque);
  passes->AddItem(overlay);
  seq->SetPasses(passes);
  cameraP->SetDelegatePass(seq);

  vtkNew<vtkToneMappingPass> toneMappingP;
  toneMappingP->SetToneMappingType(vtkToneMappingPass::GenericFilmic);
  toneMappingP->SetGenericFilmicUncharted2Presets();
  toneMappingP->SetExposure(1.0);
  toneMappingP->SetDelegatePass(cameraP);

  vtkOpenGLRenderer::SafeDownCast(ren_2)->SetPass(toneMappingP);

  // Lets use a smooth metallic surface.
  // Range: [0 ... 1]
  auto diffuseCoefficient = 1.0;
  // Range: [0 ... 1]
  auto roughnessCoefficient = 0.0;
  // Range: [0 ... 1]
  auto metallicCoefficient = 1.0;

  // Turn off the default lighting and use image based lighting.
  ren_2->AutomaticLightCreationOff();
  ren_2->UseImageBasedLightingOn();
  if (hasEnvTexture)
  {
    if (isHDR)
    {
      ren_2->UseSphericalHarmonicsOn();
      ren_2->SetEnvironmentTexture(envTexture);
    }
    else
    {
      ren_2->UseSphericalHarmonicsOff();
      ren_2->SetEnvironmentTexture(envTexture);
    }
  }
  else
  {
    ren_2->UseSphericalHarmonicsOff();
    ren_2->SetEnvironmentTexture(cubeMap);
  }
  ren_1->SetBackground(colors->GetColor3d("Snow").GetData());
  ren_2->SetBackground(
      colors->GetColor3d(parameters.parameters["bkgcolor"]).GetData());

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(source);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  // Enable PBR on the model.
  actor->GetProperty()->SetInterpolationToPBR();
  // Configure the basic properties.
  actor->GetProperty()->SetColor(
      colors->GetColor4d(parameters.parameters["objcolor"]).GetData());
  actor->GetProperty()->SetDiffuse(diffuseCoefficient);
  actor->GetProperty()->SetRoughness(roughnessCoefficient);
  actor->GetProperty()->SetMetallic(metallicCoefficient);
  ren_2->AddActor(actor);

  if (hasCubeMap)
  {
    vtkNew<vtkSkybox> skyboxActor;
    skyboxActor->SetTexture(cubeMap);
    if (gammaCorrect)
    {
      skyboxActor->GammaCorrectOn();
    }
    else
    {
      skyboxActor->GammaCorrectOff();
    }
    ren_2->AddActor(skyboxActor);
  }

  // Create the slider callbacks to manipulate various parameters.

  auto stepSize = 1.0 / 3.0;
  auto posY = 0.1;
  auto posX0 = 0.02;
  auto posX1 = 0.18;

  auto slwP = SliderProperties();
  slwP.initialValue = 1.0;
  slwP.maximumValue = 5.0;
  slwP.title = "Exposure";
  //  Screen coordinates.
  slwP.p1[0] = posX0;
  slwP.p1[1] = posY;
  slwP.p2[0] = posX1;
  slwP.p2[1] = posY;
  posY += stepSize;

  auto swExposure = MakeSliderWidget(slwP);
  swExposure->SetInteractor(interactor);
  swExposure->SetAnimationModeToAnimate();
  swExposure->EnabledOn();
  swExposure->SetCurrentRenderer(ren_1);
  vtkNew<SliderCallbackExposure> swExpCB;
  swExpCB->property = dynamic_cast<vtkToneMappingPass*>(ren_2->GetPass());
  swExposure->AddObserver(vtkCommand::InteractionEvent, swExpCB);

  slwP.initialValue = metallicCoefficient;
  slwP.maximumValue = 1.0;
  slwP.title = "Metallicity";
  //  Screen coordinates.
  slwP.p1[0] = posX0;
  slwP.p1[1] = posY;
  slwP.p2[0] = posX1;
  slwP.p2[1] = posY;
  posY += stepSize;

  auto swMetallic = MakeSliderWidget(slwP);
  swMetallic->SetInteractor(interactor);
  swMetallic->SetAnimationModeToAnimate();
  swMetallic->EnabledOn();
  swMetallic->SetCurrentRenderer(ren_1);
  vtkNew<SliderCallbackMetallic> swMetallicCB;
  swMetallicCB->property = actor->GetProperty();
  swMetallic->AddObserver(vtkCommand::InteractionEvent, swMetallicCB);

  slwP.initialValue = roughnessCoefficient;
  slwP.title = "Roughness";
  //  Screen coordinates.
  slwP.p1[0] = posX0;
  slwP.p1[1] = posY;
  slwP.p2[0] = posX1;
  slwP.p2[1] = posY;
  posY += stepSize;

  auto swRoughness = MakeSliderWidget(slwP);
  swRoughness->SetInteractor(interactor);
  swRoughness->SetAnimationModeToAnimate();
  swRoughness->EnabledOn();
  swRoughness->SetCurrentRenderer(ren_1);
  vtkNew<SliderCallbackRoughness> swRoughnessCB;
  swRoughnessCB->property = actor->GetProperty();
  swRoughness->AddObserver(vtkCommand::InteractionEvent, swRoughnessCB);

  auto name = fs::path(argv[0]).stem().generic_string();
  renderWindow->SetSize(1000, 625);
  renderWindow->Render();
  renderWindow->SetWindowName(name.c_str());

#if VTK_HAS_COW
  vtkNew<vtkCameraOrientationWidget> camOrientManipulator;
  camOrientManipulator->SetParentRenderer(ren_2);
  // Enable the widget.
  camOrientManipulator->On();
#else
  vtkNew<vtkAxesActor> axes;

  vtkNew<vtkOrientationMarkerWidget> widget;
  double rgba[4]{0.0, 0.0, 0.0, 0.0};
  colors->GetColor("Carrot", rgba);
  widget->SetOutlineColor(rgba[0], rgba[1], rgba[2]);
  widget->SetOrientationMarker(axes);
  widget->SetInteractor(interactor);
  widget->SetViewport(0.0, 0.0, 0.2, 0.2);
  widget->EnabledOn();
  widget->InteractiveOn();
#endif

  vtkNew<vtkRenderWindowInteractor> iRen;
  vtkNew<PrintCallback> printCallback;
  printCallback->SetParameters(name, 1, false);
  // printCallback->SetParameters(name + ".jpg", 1, false);
  interactor->AddObserver(vtkCommand::KeyPressEvent, printCallback);

  interactor->Start();

  return EXIT_SUCCESS;
}

namespace {

bool VTKVersionOk(unsigned long long const& major,
                  unsigned long long const& minor,
                  unsigned long long const& build)
{
  unsigned long long neededVersion =
      10000000000ULL * major + 100000000ULL * minor + build;
#ifndef VTK_VERSION_NUMBER
  vtkNew<vtkVersion>();
  ver;
  unsigned long long vtk_version_number =
      10000000000ULL * ver->GetVTKMajorVersion() +
      100000000ULL * ver->GetVTKMinorVersion() + ver->GetVTKBuildVersion();
  if (vtk_version_number >= neededVersion)
  {
    return true;
  }
  return false;
#else
  if (VTK_VERSION_NUMBER >= neededVersion)
  {
    return true;
  }
  return false;
#endif
}

void GetParameters(const fs::path fnPath, const std::string& surfaceName,
                   Parameters& parameters)
{
  std::ifstream ifs(fnPath);
  Json::Value root;

  if (ifs)
  {
    std::string str;
    std::string errors;
    Json::CharReaderBuilder builder{};
    auto reader = std::unique_ptr<Json::CharReader>(builder.newCharReader());

    std::ostringstream ss;
    ss << ifs.rdbuf(); // Read in the file comtents
    str = ss.str();
    auto parsingSuccessful =
        reader->parse(str.c_str(), str.c_str() + str.size(), &root, &errors);
    ifs.close();
    if (!parsingSuccessful)
    {
      std::cout << errors << std::endl;
      parameters.parsedOk = false;
      return;
    }
  }
  else
  {
    std::cerr << "Unable to open: " << fnPath << std::endl;
  }

  std::set<std::string> allowedKeyNoPaths{"object", "objcolor", "bkgcolor",
                                          "skybox"};
  std::set<std::string> allowedKeysPaths{
      "cubemap", "equirectangular", "albedo",  "normal", "material",
      "coat",    "anisotropy",      "emissive"};
  // Extract the values.
  for (Json::Value::const_iterator outer = root.begin(); outer != root.end();
       outer++)
  {
    if (allowedKeyNoPaths.find(outer.name()) != allowedKeyNoPaths.end())
    {
      if (outer.name() == "skybox")
      {
        parameters.skybox = outer->asBool();
      }
      else
      {
        parameters.parameters[outer.name()] = outer->asString();
      }
      continue;
    }

    if (allowedKeysPaths.find(outer.name()) != allowedKeysPaths.end())
    {
      if (outer.name() == "cubemap")
      {
        for (Json::Value::const_iterator fn = root["cubemap"].begin();
             fn != root["cubemap"].end(); fn++)
        {
          parameters.cubemap.push_back(fn->asString());
        }
      }
      else
      {
        parameters.parameters[outer.name()] = outer->asString();
      }
    }
    parameters.parsedOk = true;
  }

  // Build and check the paths.
  if (!parameters.cubemap.empty())
  {
    if (parameters.cubemap.size() != 6)
    {
      std::cerr << "There must be six filenames for the cubemap.";
      parameters.parsedOk = false;
    }
    else
    {
      for (size_t i = 0; i < parameters.cubemap.size(); i++)
      {
        if (parameters.cubemap[i].empty())
        {
          std::cerr << "A missing path in the cubemap." << std::endl;
          parameters.parsedOk = false;
        }
        else
        {
          parameters.cubemap[i] =
              (fnPath.parent_path() / fs::path(parameters.cubemap[i]))
                  .make_preferred()
                  .string();
          if (!fs::is_regular_file(parameters.cubemap[i]))
          {
            std::cerr << "Not a file: " << parameters.cubemap[i] << std::endl;
            parameters.parsedOk = false;
          }
        }
      }
    }
  }
  // We don't need cubemap now.
  allowedKeysPaths.erase("cubemap");
  for (auto& p : parameters.parameters)
  {
    if (allowedKeysPaths.find(p.first) != allowedKeysPaths.end())
    {
      if (p.second.empty())
      {
        std::cerr << "No path for the key " << p.first << std::endl;
        parameters.parsedOk = false;
      }
      else
      {
        p.second = (fnPath.parent_path() / fs::path(p.second))
                       .make_preferred()
                       .string();
        if (!fs::is_regular_file(p.second))
        {
          std::cerr << "Not a file, " << p.first << ": " << p.second
                    << std::endl;
          parameters.parsedOk = false;
        }
      }
    }
  }
  // Set the surface, if required.
  if (parameters.parameters["object"].empty())
  {
    parameters.parameters["object"] = "Boy";
  }
  if (!surfaceName.empty())
  {
    parameters.parameters["object"] = surfaceName;
  }

  return;
}

std::string DisplayParameters(Parameters& parameters)
{
  std::stringstream res;
  std::vector<std::string> parameterKeys{
      "object",          "objcolor", "bkgcolor", "skybox",   "cubemap",
      "equirectangular", "albedo",   "normal",   "material", "coat",
      "anisotropy",      "emissive"};
  for (auto const& e : parameterKeys)
  {
    if (e == "cubemap")
    {
      for (std::vector<std::string>::const_iterator it =
               parameters.cubemap.begin();
           it != parameters.cubemap.end(); ++it)
      {
        if (it == parameters.cubemap.cbegin())
        {
          res << std::setw(15) << std::left << "cubemap"
              << ": " << *it << "\n";
        }
        else
        {
          res << std::setw(17) << " " << *it << "\n";
        }
      }
    }
    else if (e == "skybox")
    {
      res << std::setw(15) << std::left << e << ": " << std::boolalpha
          << parameters.skybox << "\n";
    }
    else
    {
      if (parameters.parameters.find(e) != parameters.parameters.end())
      {
        res << std::setw(15) << std::left << e << ": "
            << parameters.parameters[e] << "\n";
      }
    }
  }

  return res.str();
}

vtkNew<vtkTexture> EquirectangularFileToTexture(std::string const& fileName,
                                                bool flipX)
{
  vtkNew<vtkTexture> texture;

  std::string extension = fs::path(fileName).extension().generic_string();
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 [](char c) { return std::tolower(c); });

  if (std::string(".jpeg .jpg .png").find(extension, 0) != std::string::npos)
  {
    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> imgReader;
    imgReader.TakeReference(
        readerFactory->CreateImageReader2(fileName.c_str()));
    imgReader->SetFileName(fileName.c_str());

    if (flipX)
    {
      vtkNew<vtkImageFlip> flip;
      flip->SetInputConnection(imgReader->GetOutputPort());
      flip->SetFilteredAxis(0);
      texture->SetInputConnection(flip->GetOutputPort());
    }
    else
    {
      texture->SetInputConnection(imgReader->GetOutputPort());
    }
  }
  else
  {
    vtkNew<vtkHDRReader> reader;
    auto extensions = reader->GetFileExtensions();
    if (std::string(extensions).find(extension, 0) != std::string::npos)
    {
      if (reader->CanReadFile(fileName.c_str()))
      {
        reader->SetFileName(fileName.c_str());

        if (flipX)
        {
          vtkNew<vtkImageFlip> flip;
          flip->SetInputConnection(reader->GetOutputPort());
          flip->SetFilteredAxis(0);
          texture->SetInputConnection(flip->GetOutputPort());
        }
        else
        {
          texture->SetInputConnection(reader->GetOutputPort());
        }
        texture->SetColorModeToDirectScalars();
      }
      else
      {
        std::cerr << "Unable to read the file: " << fileName << std::endl;
        return texture;
      }
    }
  }

  texture->MipmapOn();
  texture->InterpolateOn();

  return texture;
}

vtkNew<vtkTexture> EquirectangularFileToCubemap(std::string const& fileName,
                                                bool flipX)
{
  auto env_texture = EquirectangularFileToTexture(fileName, flipX);
  vtkNew<vtkEquirectangularToCubeMapTexture> cubeMap;
  cubeMap->SetInputTexture(vtkOpenGLTexture::SafeDownCast(env_texture));
  cubeMap->MipmapOn();
  cubeMap->InterpolateOn();

  return cubeMap;
}

vtkNew<vtkTexture> ReadCubeMap(std::vector<std::string> const& fileNames)
{
  vtkNew<vtkTexture> texture;
  texture->CubeMapOn();
  texture->MipmapOn();
  texture->InterpolateOn();

  auto i = 0;
  for (auto const& fn : fileNames)
  {
    // Read the images
    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> imgReader;
    imgReader.TakeReference(readerFactory->CreateImageReader2(fn.c_str()));
    imgReader->SetFileName(fn.c_str());

    vtkNew<vtkImageFlip> flip;
    flip->SetInputConnection(imgReader->GetOutputPort());
    flip->SetFilteredAxis(1); // flip y axis
    texture->SetInputConnection(i, flip->GetOutputPort(0));
    ++i;
  }

  return texture;
}

vtkSmartPointer<vtkPolyData> GetBoy()
{
  auto uResolution = 51;
  auto vResolution = 51;
  vtkNew<vtkParametricBoy> surface;

  vtkNew<vtkParametricFunctionSource> source;
  source->SetUResolution(uResolution);
  source->SetVResolution(vResolution);
  source->GenerateTextureCoordinatesOn();
  source->SetParametricFunction(surface);
  source->Update();

  // Build the tangents
  vtkNew<vtkPolyDataTangents> tangents;
  tangents->SetInputConnection(source->GetOutputPort());
  tangents->Update();

  return tangents->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetMobius()
{
  auto uResolution = 51;
  auto vResolution = 51;
  vtkNew<vtkParametricMobius> surface;
  surface->SetMinimumV(-0.25);
  surface->SetMaximumV(0.25);

  vtkNew<vtkParametricFunctionSource> source;
  source->SetUResolution(uResolution);
  source->SetVResolution(vResolution);
  source->GenerateTextureCoordinatesOn();
  source->SetParametricFunction(surface);
  source->Update();

  // Build the tangents
  vtkNew<vtkPolyDataTangents> tangents;
  tangents->SetInputConnection(source->GetOutputPort());
  tangents->Update();

  vtkNew<vtkTransform> transform;
  transform->RotateX(-90.0);
  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetInputConnection(tangents->GetOutputPort());
  transformFilter->SetTransform(transform);
  transformFilter->Update();

  return transformFilter->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetRandomHills()
{
  auto uResolution = 51;
  auto vResolution = 51;
  vtkNew<vtkParametricRandomHills> surface;
  surface->SetRandomSeed(1);
  surface->SetNumberOfHills(30);
  // If you want a plane
  // surface->SetHillAmplitude(0);

  vtkNew<vtkParametricFunctionSource> source;
  source->SetUResolution(uResolution);
  source->SetVResolution(vResolution);
  source->GenerateTextureCoordinatesOn();
  source->SetParametricFunction(surface);
  source->Update();

  // Build the tangents
  vtkNew<vtkPolyDataTangents> tangents;
  tangents->SetInputConnection(source->GetOutputPort());
  tangents->Update();

  vtkNew<vtkTransform> transform;
  transform->Translate(0.0, 5.0, 15.0);
  transform->RotateX(-90.0);
  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetInputConnection(tangents->GetOutputPort());
  transformFilter->SetTransform(transform);
  transformFilter->Update();

  return transformFilter->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetTorus()
{
  auto uResolution = 51;
  auto vResolution = 51;
  vtkNew<vtkParametricTorus> surface;

  vtkNew<vtkParametricFunctionSource> source;
  source->SetUResolution(uResolution);
  source->SetVResolution(vResolution);
  source->GenerateTextureCoordinatesOn();
  source->SetParametricFunction(surface);
  source->Update();

  // Build the tangents
  vtkNew<vtkPolyDataTangents> tangents;
  tangents->SetInputConnection(source->GetOutputPort());
  tangents->Update();

  vtkNew<vtkTransform> transform;
  transform->RotateX(-90.0);
  vtkNew<vtkTransformPolyDataFilter> transformFilter;
  transformFilter->SetInputConnection(tangents->GetOutputPort());
  transformFilter->SetTransform(transform);
  transformFilter->Update();

  return transformFilter->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetSphere()
{
  auto thetaResolution = 32;
  auto phiResolution = 32;
  vtkNew<vtkTexturedSphereSource> surface;
  surface->SetThetaResolution(thetaResolution);
  surface->SetPhiResolution(phiResolution);

  // Now the tangents
  vtkNew<vtkPolyDataTangents> tangents;
  tangents->SetInputConnection(surface->GetOutputPort());
  tangents->Update();
  return tangents->GetOutput();
}

vtkSmartPointer<vtkPolyData> GetCube()
{
  vtkNew<vtkCubeSource> surface;

  // Triangulate
  vtkNew<vtkTriangleFilter> triangulation;
  triangulation->SetInputConnection(surface->GetOutputPort());
  // Subdivide the triangles
  vtkNew<vtkLinearSubdivisionFilter> subdivide;
  subdivide->SetInputConnection(triangulation->GetOutputPort());
  subdivide->SetNumberOfSubdivisions(3);
  // Now the tangents
  vtkNew<vtkPolyDataTangents> tangents;
  tangents->SetInputConnection(subdivide->GetOutputPort());
  tangents->Update();
  return tangents->GetOutput();
}

vtkNew<vtkPolyData> UVTcoords(const float& uResolution,
                              const float& vResolution, vtkNew<vtkPolyData> pd)
{
  float u0 = 1.0;
  float v0 = 0.0;
  float du = 1.0 / (uResolution - 1.0);
  float dv = 1.0 / (vResolution - 1.0);
  vtkIdType numPts = pd->GetNumberOfPoints();
  vtkNew<vtkFloatArray> tCoords;
  tCoords->SetNumberOfComponents(2);
  tCoords->SetNumberOfTuples(numPts);
  tCoords->SetName("Texture Coordinates");
  vtkIdType ptId = 0;
  float u = u0;
  for (auto i = 0; i < uResolution; ++i)
  {
    float v = v0;
    for (auto j = 0; j < vResolution; ++j)
    {
      float tc[2]{u, v};
      tCoords->SetTuple(ptId, tc);
      v += dv;
      ptId++;
    }
    u -= du;
  }
  pd->GetPointData()->SetTCoords(tCoords);
  return pd;
}

vtkNew<vtkSliderWidget> MakeSliderWidget(SliderProperties const& properties)
{
  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkSliderRepresentation2D> slider;

  slider->SetMinimumValue(properties.minimumValue);
  slider->SetMaximumValue(properties.maximumValue);
  slider->SetValue(properties.initialValue);
  slider->SetTitleText(properties.title.c_str());

  slider->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  slider->GetPoint1Coordinate()->SetValue(properties.p1[0], properties.p1[1]);
  slider->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  slider->GetPoint2Coordinate()->SetValue(properties.p2[0], properties.p2[1]);

  slider->SetTubeWidth(properties.tubeWidth);
  slider->SetSliderLength(properties.sliderLength);
  slider->SetSliderWidth(properties.sliderWidth);
  slider->SetEndCapLength(properties.endCapLength);
  slider->SetEndCapWidth(properties.endCapWidth);
  slider->SetTitleHeight(properties.titleHeight);
  slider->SetLabelHeight(properties.labelHeight);

  // Set the color properties
  // Change the color of the title.
  slider->GetTitleProperty()->SetColor(
      colors->GetColor3d(properties.titleColor).GetData());
  // Change the color of the label.
  slider->GetLabelProperty()->SetColor(
      colors->GetColor3d(properties.labelColor).GetData());
  // Change the color of the bar.
  slider->GetTubeProperty()->SetColor(
      colors->GetColor3d(properties.barColor).GetData());
  // Change the color of the ends of the bar.
  slider->GetCapProperty()->SetColor(
      colors->GetColor3d(properties.barEndsColor).GetData());
  // Change the color of the knob that slides.
  slider->GetSliderProperty()->SetColor(
      colors->GetColor3d(properties.sliderColor).GetData());
  // Change the color of the knob when the mouse is held on it.
  slider->GetSelectedProperty()->SetColor(
      colors->GetColor3d(properties.selectedColor).GetData());
  // Change the color of the text displaying the value.
  slider->GetLabelProperty()->SetColor(
      colors->GetColor3d(properties.valueColor).GetData());

  vtkNew<vtkSliderWidget> sliderWidget;
  sliderWidget->SetRepresentation(slider);

  return sliderWidget;
}

} // namespace
