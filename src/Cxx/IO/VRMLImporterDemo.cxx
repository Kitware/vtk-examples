#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkLookupTable.h>
#include <vtkMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVRMLImporter.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Required arguments: Filename e.g. grasshop.wrl" << std::endl;
    return EXIT_FAILURE;
  }

  std::string filename = argv[1];
  std::cout << "Reading " << filename << std::endl;

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("VRMLImporterDemo");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // VRML Import
  vtkNew<vtkVRMLImporter> importer;
  importer->SetFileName(filename.c_str());
  importer->SetRenderWindow(renderWindow);
  importer->Update();

  auto actors = renderer->GetActors();
  std::cout << "There are " << actors->GetNumberOfItems() << " actors"
            << std::endl;
  actors->InitTraversal();
  for (vtkIdType a = 0; a < actors->GetNumberOfItems(); ++a)
  {
    vtkActor* actor = actors->GetNextActor();

    // The importer shininess parameter is between 0 and 1. VTK specular power
    // is usually 10-100. Also, the default for the specular factor for VRML is
    // 1, while VTK's is 0
    double specularPower = actor->GetProperty()->GetSpecularPower();
    if (specularPower <= 1.0)
    {
      actor->GetProperty()->SetSpecularPower(specularPower * 128.0);
    }
    double specular = actor->GetProperty()->GetSpecular();
    if (specular == 0.0)
    {
      actor->GetProperty()->SetSpecular(1.0);
    }
#if 0
    // The VRML default ambient intensity is .2
    double ambientIntensity = actor->GetProperty()->GetAmbient();
    if (ambientIntensity == 0.0)
    {
      actor->GetProperty()->SetAmbient(.2);
    }
#endif
    vtkPolyDataMapper* mapper =
        dynamic_cast<vtkPolyDataMapper*>(actor->GetMapper());
    if (mapper)
    {
      vtkPolyData* dataSet = dynamic_cast<vtkPolyData*>(mapper->GetInput());
      if (!dataSet->GetPointData()->GetNormals())
      {
        vtkNew<vtkPolyDataNormals> normals;
        normals->SetInputData(dataSet);
        normals->SplittingOff();
        normals->Update();
        mapper->SetInputData(normals->GetOutput());
      }

      // If there is a lookup table, convert it to point data
      vtkLookupTable* lut =
          dynamic_cast<vtkLookupTable*>(mapper->GetLookupTable());
      if (lut && mapper->GetScalarVisibility())
      {
        vtkNew<vtkUnsignedCharArray> pc;
        pc->SetNumberOfComponents(4);
        pc->SetNumberOfTuples(lut->GetNumberOfColors());
        for (int t = 0; t < lut->GetNumberOfColors(); ++t)
        {
          double* lutc = lut->GetTableValue(t);
          unsigned char lutuc[4];
          lut->GetColorAsUnsignedChars(lutc, lutuc);
          pc->SetTypedTuple(t, lutuc);
        }
        mapper->SetLookupTable(NULL);
        mapper->GetInput()->GetPointData()->SetScalars(pc);
      }
    }
  }

  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.4);
  renderer->ResetCameraClippingRange();

  renderer->SetBackground(colors->GetColor3d("SpringGreen").GetData());
  renderWindow->SetSize(640, 480);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
