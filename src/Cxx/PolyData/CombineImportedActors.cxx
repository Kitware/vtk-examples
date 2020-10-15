// Importers
#include <vtk3DSImporter.h>
#include <vtkGLTFImporter.h>
#include <vtkOBJImporter.h>
#include <vtkVRMLImporter.h>

// Rendering
#include <vtkActorCollection.h>
#include <vtkCamera.h>
#include <vtkDataSet.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Pipeline
#include <vtkAppendPolyData.h>
#include <vtkCellData.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkUnsignedCharArray.h>

#include <algorithm> // For transform()
#include <cctype>    // For to_lower
#include <string>    // For find_last_of()

int main(int argc, char* argv[])
{
  vtkSmartPointer<vtkRenderWindow> importRenderWindow;
  vtkSmartPointer<vtkRenderer> importRenderer;

  if (argc < 2)
  {
    std::cerr << "Need a file e.g. iflamingo.3ds" << std::endl;
    return EXIT_FAILURE;
  }

  std::string fileName = argv[1];
  std::string extension = "";
  int filePrefixArgOffset = 0; // depends on importer

  // Make the extension lowercase
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 ::tolower);
  if (fileName.find_last_of(".") != std::string::npos)
  {
    extension = fileName.substr(fileName.find_last_of(".") + 1);
  }
  if (extension == "wrl")
  {
    vtkNew<vtkVRMLImporter> importer;
    importer->SetFileName(argv[1]);
    importer->SetRenderWindow(importRenderWindow);
    importRenderWindow = importer->GetRenderWindow();
    importer->Read();
    importRenderer = importer->GetRenderer();
    filePrefixArgOffset = 2;
  }
  else if (extension == "3ds")
  {
    vtkNew<vtk3DSImporter> importer;
    importer->SetFileName(argv[1]);
    importer->SetRenderWindow(importRenderWindow);
    importRenderWindow = importer->GetRenderWindow();
    importer->Read();
    importRenderer = importer->GetRenderer();
    filePrefixArgOffset = 2;
  }
  else if (extension == "gltf" || extension == "glb")
  {
    vtkNew<vtkGLTFImporter> importer;
    importer->SetFileName(argv[1]);
    importer->SetRenderWindow(importRenderWindow);
    importRenderWindow = importer->GetRenderWindow();
    importer->Read();
    importRenderer = importer->GetRenderer();
    filePrefixArgOffset = 2;
  }
  else if (extension == "obj")
  {
    vtkNew<vtkOBJImporter> importer;
    importer->SetFileName(argv[1]);
    importer->SetFileNameMTL(argv[2]);
    importer->SetTexturePath(argv[3]);
    importer->SetRenderWindow(importRenderWindow);
    importRenderWindow = importer->GetRenderWindow();
    importer->Read();
    importRenderer = importer->GetRenderer();
    filePrefixArgOffset = 4;
  }
  vtkNew<vtkAppendPolyData> append;
  vtkActorCollection* allActors = importRenderer->GetActors();
  vtkCollectionSimpleIterator actorsIt;
  vtkActor* anActor = nullptr;
  ;
  allActors->InitTraversal(actorsIt);
  while ((anActor = allActors->GetNextActor(actorsIt)))
  {
    vtkNew<vtkPolyData> appendPD;
    vtkPolyData* pd;
    anActor->GetMapper()->Update();
    if (anActor->GetUserMatrix() != nullptr)
    {
      vtkNew<vtkTransform> transform;
      transform->SetMatrix(anActor->GetUserMatrix());
      vtkNew<vtkTransformPolyDataFilter> transformPD;
      transformPD->SetTransform(transform);
      transformPD->SetInputData(
          dynamic_cast<vtkPolyData*>(anActor->GetMapper()->GetInput()));
      transformPD->Update();
      appendPD->DeepCopy(transformPD->GetOutput());
    }
    else
    {
      pd = dynamic_cast<vtkPolyData*>(anActor->GetMapper()->GetInput());
      appendPD->DeepCopy(pd);
    }
    vtkNew<vtkUnsignedCharArray> cellData;
    cellData->SetNumberOfComponents(3);
    cellData->SetNumberOfTuples(appendPD->GetNumberOfCells());
    for (int i = 0; i < appendPD->GetNumberOfCells(); i++)
    {
      double rgb[4];
      anActor->GetProperty()->GetDiffuseColor(rgb);
      for (int j = 0; j < 4; ++j)
      {
        rgb[j] *= 255.0;
      }
      cellData->InsertTuple(i, rgb);
    }
    appendPD->GetCellData()->SetScalars(cellData);

    append->AddInputData(appendPD);
  }
  append->Update();

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputData(append->GetOutput());
  mapper->SetScalarModeToUseCellData();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Banana").GetData());

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(640, 480);
  renderWindow->SetWindowName("CombineImportedActors");

  vtkNew<vtkRenderer> renderer;
  renderer->UseHiddenLineRemovalOn();
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  // renderer->SetActiveCamera(importRenderer->GetActiveCamera());
  renderWindow->AddRenderer(renderer);
  renderer->AddActor(actor);

  auto camera = renderer->GetActiveCamera();
  camera->SetPosition(-7, 34, 2);
  camera->SetViewUp(0, 0, 1);
  renderer->ResetCamera();

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
