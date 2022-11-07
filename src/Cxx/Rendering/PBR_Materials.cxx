#include <vtkActor.h>
#include <vtkHDRReader.h>
#include <vtkImageFlip.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOpenGLTexture.h>
#include <vtkPBRIrradianceTexture.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataTangents.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSkybox.h>
#include <vtkSphereSource.h>
#include <vtkTexture.h>

#include <vtk_cli11.h>
#include <vtk_jsoncpp.h>

#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

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
 * @param parameters:  The parameters.
 */
void GetParameters(const fs::path fnPath, Parameters& parameters);

std::string DisplayParameters(Parameters& parameters);

/**
 * Read six images forming a cubemap.
 *
 * @param fileNames: The paths to the six cubemap files.
 *
 * @return The cubemap texture.
 */
vtkNew<vtkTexture> ReadCubemap(std::vector<std::string> const& fileNames);

/**
 *  Read an equirectangular environment file and convert it to a texture.
 *
 * @param fileName: The equirectangular file path.
 *
 * @return The texture.
 */
vtkNew<vtkTexture> ReadEquirectangularFile(std::string const& fileName);

} // namespace

int main(int argc, char* argv[])
{
  CLI::App app{"Render spheres with different materials using a skybox as "
               "image based lighting."};

  // Define options
  std::string fileName;
  app.add_option("fileName", fileName,
                 "The path to the JSON file containing the parameters.");
  auto useEquirectangular{false};
  app.add_flag("-e, --use_equirectangular", useEquirectangular,
               "Use the equirectangular entry in the json file.");

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
  auto useCubemap = !useEquirectangular;

  Parameters parameters;
  GetParameters(fnPath, parameters);
  if (!parameters.parsedOk)
  {
    return EXIT_FAILURE;
  }

  auto res = DisplayParameters(parameters);
  std::cout << res << std::endl;

  vtkNew<vtkNamedColors> colors;
  colors->SetColor("Brass",
                   std::array<unsigned char, 4>{184, 115, 51, 255}.data());

  vtkNew<vtkOpenGLRenderer> ren;
  ren->SetBackground(colors->GetColor3d("Black").GetData());

  vtkNew<vtkRenderWindow> renWin;
  renWin->SetSize(600, 600);
  renWin->AddRenderer(ren);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  iren->SetInteractorStyle(style);

  vtkSmartPointer<vtkPBRIrradianceTexture> irradiance =
      ren->GetEnvMapIrradiance();
  irradiance->SetIrradianceStep(0.3);

  vtkNew<vtkSkybox> skybox;

  // Choose how to generate the skybox.
  vtkNew<vtkTexture> cubeMap;
  vtkNew<vtkTexture> envTexture;
  auto isHDR = false;
  auto hasSkybox = false;
  auto gammaCorrect = false;

  if (useCubemap && !parameters.cubemap.empty())
  {
    std::cout << "Using the cubemap files to generate the environment texture."
              << std::endl;
    envTexture = ReadCubemap(parameters.cubemap);
    if (parameters.skybox)
    {
      skybox->SetTexture(envTexture);
      hasSkybox = true;
    }
  }
  else if (parameters.parameters.find("equirectangular") !=
               parameters.parameters.end() &&
           !parameters.parameters["equirectangular"].empty())
  {
    std::cout
        << "Using the equirectangular file to generate the environment texture."
        << std::endl;
    envTexture =
        ReadEquirectangularFile(parameters.parameters["equirectangular"]);
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
      skybox->SetTexture(envTexture);
      skybox->SetFloorRight(0, 0, 1);
      skybox->SetProjection(vtkSkybox::Sphere);
      skybox->SetTexture(envTexture);
      hasSkybox = true;
    }
  }
  else
  {
    std::cerr << "An environment texture is required,\n"
              << "please add the necessary equirectangular"
              << " or cubemap file paths to the json file." << std::endl;
    return EXIT_FAILURE;
  }

  ren->UseImageBasedLightingOn();
  if (isHDR)
  {
    ren->UseSphericalHarmonicsOn();
    ren->SetEnvironmentTexture(envTexture, false);
  }
  else
  {
    ren->UseSphericalHarmonicsOff();
    ren->SetEnvironmentTexture(envTexture, true);
  }

  vtkNew<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphere->GetOutputPort());

  for (int i = 0; i < 6; i++)
  {
    vtkNew<vtkActor> actorSphere;
    actorSphere->SetPosition(i, 0.0, 0.0);
    actorSphere->SetMapper(mapper);
    actorSphere->GetProperty()->SetInterpolationToPBR();
    actorSphere->GetProperty()->SetColor(colors->GetColor3d("White").GetData());
    actorSphere->GetProperty()->SetMetallic(1.0);
    actorSphere->GetProperty()->SetRoughness(i / 5.0);

    ren->AddActor(actorSphere);
  }

  for (int i = 0; i < 6; i++)
  {
    vtkNew<vtkActor> actorSphere;
    actorSphere->SetPosition(i, 1.0, 0.0);
    actorSphere->SetMapper(mapper);
    actorSphere->GetProperty()->SetInterpolationToPBR();
    actorSphere->GetProperty()->SetColor(colors->GetColor3d("Brass").GetData());
    actorSphere->GetProperty()->SetMetallic(1.0);
    actorSphere->GetProperty()->SetRoughness(i / 5.0);
    ren->AddActor(actorSphere);
  }

  for (int i = 0; i < 6; i++)
  {
    vtkNew<vtkActor> actorSphere;
    actorSphere->SetPosition(i, 2.0, 0.0);
    actorSphere->SetMapper(mapper);
    actorSphere->GetProperty()->SetInterpolationToPBR();
    actorSphere->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());
    actorSphere->GetProperty()->SetRoughness(i / 5.0);
    ren->AddActor(actorSphere);
  }

  for (int i = 0; i < 6; i++)
  {
    vtkNew<vtkActor> actorSphere;
    actorSphere->SetPosition(i, 3.0, 0.0);
    actorSphere->SetMapper(mapper);
    actorSphere->GetProperty()->SetInterpolationToPBR();
    actorSphere->GetProperty()->SetColor(colors->GetColor3d("Cyan").GetData());
    actorSphere->GetProperty()->SetRoughness(i / 5.0);
    ren->AddActor(actorSphere);
  }

  for (int i = 0; i < 6; i++)
  {
    vtkNew<vtkActor> actorSphere;
    actorSphere->SetPosition(i, 4.0, 0.0);
    actorSphere->SetMapper(mapper);
    actorSphere->GetProperty()->SetInterpolationToPBR();
    actorSphere->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
    actorSphere->GetProperty()->SetRoughness(i / 5.0);
    ren->AddActor(actorSphere);
  }

  if (hasSkybox)
  {
    if (gammaCorrect)
    {
      skybox->GammaCorrectOn();
    }
    else
    {
      skybox->GammaCorrectOff();
    }
    ren->AddActor(skybox);
  }

  renWin->SetWindowName("PBR_Materials");

  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}

namespace {

void GetParameters(const fs::path fnPath, Parameters& parameters)
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
    parameters.parsedOk = true;
  }
  else
  {
    std::cerr << "Unable to open: " << fnPath << std::endl;
    parameters.parsedOk = false;
  }

  // Extract the values.
  std::set<std::string> keysNoPaths{"title", "object", "objcolor", "bkgcolor",
                                    "skybox"};
  std::set<std::string> keysWithPaths{"cubemap",    "equirectangular", "albedo",
                                      "normal",     "material",        "coat",
                                      "anisotropy", "emissive"};
  fs::path cubemapPath;
  for (Json::Value::const_iterator outer = root.begin(); outer != root.end();
       ++outer)
  {
    if (keysNoPaths.find(outer.name()) != keysNoPaths.end())
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
    if (keysWithPaths.find(outer.name()) != keysWithPaths.end())
    {
      if (outer.name() == "cubemap")
      {
        std::string path;
        for (Json::Value::const_iterator pth = root["cubemap"].begin();
             pth != root["cubemap"].end(); ++pth)
        {
          if (pth.name() == "root")
          {
            cubemapPath = fs::path(pth->asString());
            std::cout << path << std::endl;
          }
          if (pth.name() == "files")
          {
            for (Json::Value::const_iterator fls =
                     root["cubemap"]["files"].begin();
                 fls != root["cubemap"]["files"].end(); ++fls)
            {
              parameters.cubemap.push_back(fls->asString());
            }
          }
        }
      }
      else
      {
        parameters.parameters[outer.name()] = outer->asString();
      }
    }
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
        auto pth = fnPath.parent_path() / cubemapPath /
            fs::path(parameters.cubemap[i]);
        if (parameters.cubemap[i].empty())
        {
          std::cerr << "A missing path in the cubemap." << std::endl;
          parameters.parsedOk = false;
        }
        else
        {
          parameters.cubemap[i] = pth.make_preferred().string();
          if (!(fs::is_regular_file(pth) && fs::exists(pth)))
          {
            std::cerr << "Not a file or path does not exist: "
                      << parameters.cubemap[i] << std::endl;
            parameters.parsedOk = false;
          }
        }
      }
    }
  }
  // Check the remaining paths, we don't need cubemap now.
  keysWithPaths.erase("cubemap");
  for (auto& p : parameters.parameters)
  {
    if (keysWithPaths.find(p.first) != keysWithPaths.end())
    {
      if (p.second.empty())
      {
        std::cerr << "No path for the key " << p.first << std::endl;
        parameters.parsedOk = false;
      }
      else
      {
        auto pth = fnPath.parent_path() / fs::path(p.second);
        p.second = pth.make_preferred().string();
        if (!(fs::is_regular_file(pth) && fs::exists(pth)))
        {
          std::cerr << "Not a file or path does not exist: " << p.second
                    << std::endl;
          parameters.parsedOk = false;
        }
      }
    }
  }

  return;
}

std::string DisplayParameters(Parameters& parameters)
{
  std::stringstream res;
  std::vector<std::string> parameterKeys{
      "title",   "object",          "objcolor", "bkgcolor", "skybox",
      "cubemap", "equirectangular", "albedo",   "normal",   "material",
      "coat",    "anisotropy",      "emissive"};
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

vtkNew<vtkTexture> ReadCubemap(std::vector<std::string> const& fileNames)
{
  vtkNew<vtkTexture> cubeMap;
  cubeMap->CubeMapOn();

  auto i = 0;
  for (auto const& fn : fileNames)
  {
    // Read the images
    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> imgReader;
    imgReader.TakeReference(readerFactory->CreateImageReader2(fn.c_str()));
    imgReader->SetFileName(fn.c_str());

    // Each image must be flipped in Y due to canvas
    // versus vtk ordering.
    vtkNew<vtkImageFlip> flip;
    flip->SetInputConnection(imgReader->GetOutputPort());
    flip->SetFilteredAxis(1); // flip y axis
    cubeMap->SetInputConnection(i, flip->GetOutputPort(0));
    ++i;
  }

  cubeMap->MipmapOn();
  cubeMap->InterpolateOn();

  return cubeMap;
}

vtkNew<vtkTexture> ReadEquirectangularFile(std::string const& fileName)
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

    texture->SetInputConnection(imgReader->GetOutputPort());
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

        texture->SetInputConnection(reader->GetOutputPort());
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

} // namespace
