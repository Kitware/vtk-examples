#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTimerLog.h>
#include <vtksys/SystemTools.hxx>

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

#include <vtkFieldData.h>
#include <vtksys/RegularExpression.hxx>

namespace {
vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName);
void SaveSceneToFieldData(vtkDataSet* data, vtkActor* actor, vtkCamera* camera);
void RestoreSceneFromFieldData(vtkDataSet* data, vtkActor* actor,
                               vtkCamera* camera);
} // namespace
int main(int argc, char* argv[])
{
  auto polyData = ReadPolyData(argc > 1 ? argv[1] : "");

  // Visualize
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(polyData);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Crimson").GetData());
  actor->GetProperty()->SetSpecular(.6);
  actor->GetProperty()->SetSpecularPower(30);
  ;
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("SaveSceneToFieldData");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Silver").GetData());

  // Interact to change camera
  renderWindow->Render();
  renderWindowInteractor->Start();

  // After the interaction is done, save the scene
  SaveSceneToFieldData(polyData, actor, renderer->GetActiveCamera());
  renderWindow->Render();
  renderWindowInteractor->Start();

  // After interaction , restore the scene
  RestoreSceneFromFieldData(polyData, actor, renderer->GetActiveCamera());
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

#include <sstream>
namespace {
void SaveSceneToFieldData(vtkDataSet* data, vtkActor* /* actor */,
                          vtkCamera* camera)
{
  std::ostringstream buffer;
  double vector[3];
  double scalar;

  camera->GetFocalPoint(vector);
  buffer << "Camera:FocalPoint " << vector[0] << ", " << vector[1] << ", "
         << vector[2] << std::endl;
  camera->GetPosition(vector);
  buffer << "Camera:Position " << vector[0] << ", " << vector[1] << ", "
         << vector[2] << std::endl;
  camera->GetViewUp(vector);
  buffer << "Camera:ViewUp " << vector[0] << ", " << vector[1] << ", "
         << vector[2] << std::endl;
  scalar = camera->GetViewAngle();
  buffer << "Camera:ViewAngle " << scalar << std::endl;
  camera->GetClippingRange(vector);
  buffer << "Camera:ClippingRange " << vector[0] << ", " << vector[1]
         << std::endl;
  vtkNew<vtkStringArray> cameraArray;
  cameraArray->SetNumberOfValues(1);
  cameraArray->SetValue(0, buffer.str());
  cameraArray->SetName("Camera");
  data->GetFieldData()->AddArray(cameraArray);
}

void RestoreSceneFromFieldData(vtkDataSet* data, vtkActor* /* actor */,
                               vtkCamera* camera)
{

  std::istringstream buffer;

  // Get the saved camera information from the field data
  if (dynamic_cast<vtkStringArray*>(
          data->GetFieldData()->GetAbstractArray("Camera")))
  {
    buffer.str(dynamic_cast<vtkStringArray*>(
                   data->GetFieldData()->GetAbstractArray("Camera"))
                   ->GetValue(0));
  }
  else
  {
    return;
  }

  std::string line;

  vtksys::RegularExpression reCP("^Camera:Position");
  vtksys::RegularExpression reCFP("^Camera:FocalPoint");
  vtksys::RegularExpression reCVU("^Camera:ViewUp");
  vtksys::RegularExpression reCVA("^Camera:ViewAngle");
  vtksys::RegularExpression reCCR("^Camera:ClippingRange");
  vtksys::RegularExpression floatNumber(
      "[^0-9\\.\\-]*([0-9e\\.\\-]*[^,])[^0-9\\.\\-]*([0-9e\\.\\-]*[^,])[^0-9\\."
      "\\-]*([0-9e\\.\\-]*[^,])");
  vtksys::RegularExpression floatScalar("[^0-9\\.\\-]*([0-9\\.\\-e]*[^,])");

  while (std::getline(buffer, line))
  {
    if (reCFP.find(line))
    {
      std::string rest(line, reCFP.end());
      if (floatNumber.find(rest))
      {
        camera->SetFocalPoint(atof(floatNumber.match(1).c_str()),
                              atof(floatNumber.match(2).c_str()),
                              atof(floatNumber.match(3).c_str()));
      }
    }
    else if (reCP.find(line))
    {
      std::string rest(line, reCP.end());
      if (floatNumber.find(rest))
      {
        camera->SetPosition(atof(floatNumber.match(1).c_str()),
                            atof(floatNumber.match(2).c_str()),
                            atof(floatNumber.match(3).c_str()));
      }
    }
    else if (reCVU.find(line))
    {
      std::string rest(line, reCVU.end());
      if (floatNumber.find(rest))
      {
        camera->SetViewUp(atof(floatNumber.match(1).c_str()),
                          atof(floatNumber.match(2).c_str()),
                          atof(floatNumber.match(3).c_str()));
      }
    }
    else if (reCVA.find(line))
    {
      std::string rest(line, reCVA.end());
      if (floatScalar.find(rest))
      {
        camera->SetViewAngle(atof(floatScalar.match(1).c_str()));
      }
    }
    else if (reCCR.find(line))
    {
      std::string rest(line, reCCR.end());
      if (floatNumber.find(rest))
      {
        camera->SetClippingRange(atof(floatNumber.match(1).c_str()),
                                 atof(floatNumber.match(2).c_str()));
      }
    }
    else
    {
      std::cout << "Unrecognized line: " << line << std::endl;
    }
  }
}

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
    polyData = reader->GetOutput();
  }
  else if (extension == ".obj")
  {
    vtkNew<vtkOBJReader> reader;
    reader->SetFileName(fileName);
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
  else if (extension == ".g")
  {
    vtkNew<vtkBYUReader> reader;
    reader->SetGeometryFileName(fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    vtkNew<vtkSphereSource> source;
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}
} // namespace
