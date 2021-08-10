#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkBMPReader.h>
#include <vtkCubeSource.h>
#include <vtkDataSet.h>
#include <vtkEquirectangularToCubeMapTexture.h>
#include <vtkFloatArray.h>
#include <vtkHDRReader.h>
#include <vtkImageFlip.h>
#include <vtkImageReader2Factory.h>
#include <vtkJPEGReader.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOpenGLTexture.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPNGReader.h>
#include <vtkPNMReader.h>
#include <vtkParametricBoy.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricMobius.h>
#include <vtkParametricRandomHills.h>
#include <vtkParametricTorus.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataTangents.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSkybox.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <vtkSmartPointer.h>
#include <vtkTIFFReader.h>
#include <vtkTextProperty.h>
#include <vtkTexture.h>
#include <vtkTexturedSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkVersion.h>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>

#if VTK_VERSION_NUMBER >= 90000000000ULL
#define VTK_VER_GE_90 1
#endif

#if VTK_VERSION_NUMBER >= 90020210809ULL
#define VTK_HAS_COW 1
#endif

#if VTK_HAS_COW
#include <vtkCameraOrientationWidget.h>
#endif

namespace {
/**
 * Show the command lime parameters.
 *
 * @param fn: The program name.
 *
 * @return A string describing the usage.
 */
std::string ShowUsage(std::string fn);

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

// Some sample surfaces to try.
vtkSmartPointer<vtkPolyData> GetBoy();
vtkSmartPointer<vtkPolyData> GetMobius();
vtkSmartPointer<vtkPolyData> GetRandomHills();
vtkSmartPointer<vtkPolyData> GetTorus();
vtkSmartPointer<vtkPolyData> GetSphere();
vtkSmartPointer<vtkPolyData> GetCube();

/**
 * Read six images forming a cubemap.
 *
 * @param folderRoot: The folder where the cube maps are stored.
 * @param fileNames: The names of the cubemap files.
 *
 * @return The cubemap texture.
 */
vtkSmartPointer<vtkTexture>
ReadCubeMap(std::string const& folderRoot,
            std::vector<std::string> const& fileNames);

/**
 * Read an equirectangular environment file and convert it to a cube map.
 *
 * @param fileName: The equirectangular file.
 *
 * @return The cubemap texture.
 */
vtkSmartPointer<vtkTexture> ReadEnvironmentMap(std::string const& fileName);

/**
 * Read an image and convert it to a texture.
 *
 * @param path: The image path.
 *
 * @return The texture.
 */
vtkSmartPointer<vtkTexture> ReadTexture(std::string path);

} // namespace

int main(int argc, char* argv[])
{
  if (!VTKVersionOk(8, 90, 0))
  {
    std::cerr << "You need VTK version 8.90 or greater to run this program."
              << std::endl;
    return EXIT_FAILURE;
  }

  std::string filePath{""};
  if (argc < 6)
  {
    std::cout << ShowUsage(argv[0]) << std::endl;
    return EXIT_FAILURE;
  }

  // Split  path into its components.
  auto splitPath = [](std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');

    std::regex regex("/");

    std::vector<std::string> elements(
        std::sregex_token_iterator(path.begin(), path.end(), regex, -1),
        std::sregex_token_iterator());
    return elements;
  };

  // Build a string from a vector of strings using a separator.
  auto join = [](std::vector<std::string> strings, std::string separator) {
    std::string res =
        std::accumulate(std::begin(strings), std::end(strings), std::string(),
                        [&](std::string& ss, std::string& s) {
                          return ss.empty() ? s : ss + separator + s;
                        });
    return res;
  };

  // A map of the skybox folder name and the skybox files in it.
  std::map<std::string, std::vector<std::string>> skyboxFiles{
      {"skybox0",
       {"right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg",
        "back.jpg"}},
      {"skybox1",
       {"skybox-px.jpg", "skybox-nx.jpg", "skybox-py.jpg", "skybox-ny.jpg",
        "skybox-pz.jpg", "skybox-nz.jpg"}},
      {"skybox2",
       {"posx.jpg", "negx.jpg", "posy.jpg", "negy.jpg", "posz.jpg",
        "negz.jpg"}}};

  std::vector<std::string> path = splitPath(std::string(argv[1]));
  // std::string root = join(path, "/");

  vtkSmartPointer<vtkTexture> skybox;
  // Load the skybox or cube map.
  if (path.back().find(".", 0) != std::string::npos)
  {
    skybox = ReadEnvironmentMap(argv[1]);
  }
  else
  {
    skybox = ReadCubeMap(argv[1], skyboxFiles[path.back()]);
  }

  // Get the textures.
  auto baseColor = ReadTexture(argv[2]);
  baseColor->SetColorModeToDirectScalars();
  baseColor->UseSRGBColorSpaceOn();
  auto normal = ReadTexture(argv[3]);
  normal->SetColorModeToDirectScalars();
  auto material = ReadTexture(argv[4]);
  material->SetColorModeToDirectScalars();
  auto anisotropy = ReadTexture(argv[5]);
  anisotropy->SetColorModeToDirectScalars();

  // Get the surface.
  std::string desiredSurface = "boy";
  if (argc > 6)
  {
    desiredSurface = argv[6];
  }
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
  std::array<unsigned char, 4> col{{26, 51, 102, 255}};
  colors->SetColor("BkgColor", col.data());
  // VTK blue
  std::array<unsigned char, 4> col1{{6, 79, 141, 255}};
  colors->SetColor("VTKBlue", col1.data());
  // Let's make a complementary colour to VTKBlue.
  std::transform(col1.begin(), std::prev(col1.end()), col1.begin(),
                 [](unsigned char c) { return 255 - c; });
  colors->SetColor("VTKBlueComp", col1.data());

  vtkNew<vtkOpenGLRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  // Turn off the default lighting and use image based lighting.
  renderer->AutomaticLightCreationOff();
  renderer->UseImageBasedLightingOn();
#if VTK_VER_GE_90
  renderer->SetEnvironmentTexture(skybox);
#else
  renderer->SetEnvironmentCubeMap(skybox);
#endif
  renderer->UseSphericalHarmonicsOff();
  renderer->SetBackground(colors->GetColor3d("BkgColor").GetData());

  // Lets use a rough metallic surface.
  auto diffuseCoefficient = 1.0;
  auto roughnessCoefficient = 1.0;
  auto metallicCoefficient = 1.0;
  // Other parameters
  auto occlusionStrength = 1.0;
  auto normalScale = 1.0;
  auto anisotropyCoefficient = 1.0;
  auto anisotropyRotation = 0.0;

  // Build the pipeline.
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(source);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  // Enable PBR on the model.
  actor->GetProperty()->SetInterpolationToPBR();
  // Configure the basic properties.
  actor->GetProperty()->SetColor(colors->GetColor3d("White").GetData());
  actor->GetProperty()->SetDiffuse(diffuseCoefficient);
  actor->GetProperty()->SetRoughness(roughnessCoefficient);
  actor->GetProperty()->SetMetallic(metallicCoefficient);
  // Configure textures (needs tcoords on the mesh).
  actor->GetProperty()->SetBaseColorTexture(baseColor);
  actor->GetProperty()->SetORMTexture(material);
  actor->GetProperty()->SetOcclusionStrength(occlusionStrength);
  // Needs tcoords, normals and tangents on the mesh.
  actor->GetProperty()->SetNormalTexture(normal);
  actor->GetProperty()->SetNormalScale(normalScale);
  actor->GetProperty()->SetAnisotropyTexture(anisotropy);
  actor->GetProperty()->SetAnisotropy(anisotropyCoefficient);
  actor->GetProperty()->SetAnisotropyRotation(anisotropyRotation);

  vtkNew<vtkSkybox> skyboxActor;
  skyboxActor->SetTexture(skybox);

  renderer->AddActor(actor);
  // Comment out if you don't want a skybox.
  renderer->AddActor(skyboxActor);

  renderWindow->SetSize(800, 500);
  renderWindow->Render();
  renderWindow->SetWindowName("PBR_Skybox_Anisotropy");

  vtkNew<vtkAxesActor> axes;

  vtkNew<vtkOrientationMarkerWidget> widget;
  double rgba[4]{0.0, 0.0, 0.0, 0.0};
  colors->GetColor("Carrot", rgba);
  widget->SetOutlineColor(rgba[0], rgba[1], rgba[2]);
  widget->SetOrientationMarker(axes);
  widget->SetInteractor(interactor);
  widget->SetViewport(0.0, 0.0, 0.2, 0.2);
  widget->SetEnabled(1);
  widget->InteractiveOn();

  interactor->SetRenderWindow(renderWindow);
#if VTK_HAS_COW
  vtkNew<vtkCameraOrientationWidget> camOrientManipulator;
  camOrientManipulator->SetParentRenderer(renderer);
  // Enable the widget.
  camOrientManipulator->On();
#endif

  interactor->Start();
  return EXIT_SUCCESS;
}

namespace {
std::string ShowUsage(std::string fn)
{
  // Remove the folder (if present) then remove the extension in this order
  // since the folder name may contain periods.
  auto last_slash_idx = fn.find_last_of("\\/");
  if (std::string::npos != last_slash_idx)
  {
    fn.erase(0, last_slash_idx + 1);
  }
  auto period_idx = fn.rfind('.');
  if (std::string::npos != period_idx)
  {
    fn.erase(period_idx);
  }
  std::ostringstream os;
  os << "\nusage: " << fn
     << " path material_fn albedo_fn normal_fn anisotropy_fn [surface]\n\n"
     << "Demonstrates physically based rendering, image based lighting, "
        ", anisotropic texturing and a skybox.\n\n"
     << "positional arguments:\n"
     << "  path         The path to the cubemap files e.g. skyboxes/skybox2/\n"
     << "               or to a .hdr, .png, or .jpg equirectangular file.\n"
     << "  base_fn        The path to the base colour (albedo) texture file "
        "e.g. "
        "Textures/Anisotropic/CarbonFiberAniso_albedo.png\n"
     << "  normal_fn      The path to the normal texture file e.g. "
        "Textures/Anisotropic/CarbonFiberAniso_normal.png\n"
     << "  material_fn    The path to the  material (orm) texture file e.g. "
        "Textures/Anisotropic/CarbonFiberAniso_orm.png\n"
     << "  anisotropy_fn  The path to the anisotropic texture file e.g. "
        "Textures/Anisotropic/CarbonFiberAniso_anisotropyAngle.png\n"
     << "  surface        The surface to use. Boy's surface is the default.\n\n"
     << "Physically based rendering sets color, metallicity and roughness of "
        "the object.\n"
     << "Image based lighting uses a cubemap texture to specify the "
        "environment.\n"
     << "Texturing is used to generate lighting effects.\n"
     << "A Skybox is used to create the illusion of distant three-dimensional "
        "surroundings.\n"
     << "\n"
     << std::endl;
  return os.str();
}

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

vtkSmartPointer<vtkTexture>
ReadCubeMap(std::string const& folderRoot,
            std::vector<std::string> const& fileNames)
{
  auto root = folderRoot;
  if (folderRoot.back() != '/')
  {
    root += '/';
  }

  vtkNew<vtkTexture> texture;
  texture->CubeMapOn();
  // Build the file names.
  std::vector<std::string> paths;
  for (auto f : fileNames)
  {
    paths.push_back(root + f);
  }
  auto i = 0;
  for (auto const& fn : paths)
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
  texture->MipmapOn();
  texture->InterpolateOn();

  return texture;
}

vtkSmartPointer<vtkTexture> ReadEnvironmentMap(std::string const& fileName)
{
  // Split  path into its components.
  auto splitPath = [](std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');

    std::regex regex("/");

    std::vector<std::string> elements(
        std::sregex_token_iterator(path.begin(), path.end(), regex, -1),
        std::sregex_token_iterator());
    return elements;
  };

  // Build a string from a vector of strings using a separator.
  auto join = [](std::vector<std::string> strings, std::string separator) {
    std::string res =
        std::accumulate(std::begin(strings), std::end(strings), std::string(),
                        [&](std::string& ss, std::string& s) {
                          return ss.empty() ? s : ss + separator + s;
                        });
    return res;
  };

  // Get the file name extension.
  auto getSuffix = [](const std::string& fn) -> std::string {
    auto position = fn.find_last_of('.');
    if (position == std::string::npos)
      return "";
    else
    {
      std::string ext(fn.substr(position + 1));
      if (std::regex_search(ext, std::regex("[^A-Za-z0-9]")))
        return "";
      else
        return "." + ext;
    }
  };

  std::vector<std::string> path = splitPath(std::string(fileName));
  auto suffix = getSuffix(path.back());

  vtkNew<vtkTexture> texture;

  if (std::string(".jpg .png").find(suffix, 0) != std::string::npos)
  {
    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> imgReader;
    imgReader.TakeReference(
        readerFactory->CreateImageReader2(fileName.c_str()));
    imgReader->SetFileName(fileName.c_str());

    texture->SetInputConnection(imgReader->GetOutputPort());
  }
  else
  {
    vtkNew<vtkHDRReader> reader;
    auto extensions = reader->GetFileExtensions();
    if (std::string(extensions).find(suffix, 0) != std::string::npos)
    {
      if (reader->CanReadFile(fileName.c_str()))
      {
        reader->SetFileName(fileName.c_str());

        texture->SetColorModeToDirectScalars();
        texture->SetInputConnection(reader->GetOutputPort());
      }
      else
      {
        std::cerr << "Unable to read the file: " << fileName << std::endl;
        return texture;
      }
    }
  }

  // Convert to a cube map.
  vtkNew<vtkEquirectangularToCubeMapTexture> tcm;
  tcm->SetInputTexture(vtkOpenGLTexture::SafeDownCast(texture));
  // Enable mipmapping to handle HDR image.
  tcm->MipmapOn();
  tcm->InterpolateOn();

  return tcm;
}

vtkSmartPointer<vtkTexture> ReadTexture(std::string path)
{
  // Read the image which will be the texture
  std::string extension;
  if (path.find_last_of(".") != std::string::npos)
  {
    extension = path.substr(path.find_last_of("."));
  }
  // Make the extension lowercase
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 ::tolower);
  std::vector<std::string> validExtensions{".jpg", ".png", ".bmp", ".tiff",
                                           ".pnm", ".pgm", ".ppm"};
  vtkNew<vtkTexture> texture;
  texture->InterpolateOn();

  if (std::find(validExtensions.begin(), validExtensions.end(), extension) ==
      validExtensions.end())
  {
    std::cout << "Unable to read the texture file:" << path << std::endl;
    return texture;
  }
  // Read the images
  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> imgReader;
  imgReader.TakeReference(readerFactory->CreateImageReader2(path.c_str()));
  imgReader->SetFileName(path.c_str());

  texture->SetInputConnection(imgReader->GetOutputPort());
  texture->Update();

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

} // namespace
