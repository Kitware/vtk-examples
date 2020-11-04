#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkFollower.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRibbonFilter.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTubeFilter.h>
#include <vtkVectorText.h>

#include <vtksys/SystemTools.hxx>

#include <string>

namespace {
void AddStock(std::vector<vtkSmartPointer<vtkRenderer>> renderers,
              char* filename, std::string name, double& zPosition,
              bool useTubes);
}

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // set up the stocks
  std::vector<vtkSmartPointer<vtkRenderer>> renderers;
  vtkNew<vtkRenderer> topRenderer;
  vtkNew<vtkRenderer> bottomRenderer;
  renderers.push_back(topRenderer);
  renderers.push_back(bottomRenderer);

  // List of one or more filenames corresponding to stocks.
  // e.g. GE.vtk GM.vtk IBM.vtk DEC.vtk [0|1]
  bool useTubes = true;
  if (atoi(argv[argc - 1]) == 1)
  {
    useTubes = false;
  }

  double zPosition = 0.0;
  for (int i = 1; i < argc - 1; ++i)
  {
    AddStock(renderers, argv[i],
             vtksys::SystemTools::GetFilenameWithoutExtension(argv[i]),
             zPosition, useTubes);
  }

  // Setup render window and interactor
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderers[0]);
  renderWindow->AddRenderer(renderers[1]);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderers[0]->SetViewport(0.0, .4, 1.0, 1.0);
  renderers[1]->SetViewport(0.0, 0.0, 1.0, .4);

  renderers[0]->GetActiveCamera()->SetViewAngle(5.0);
  renderers[0]->ResetCamera();
  renderers[0]->GetActiveCamera()->Zoom(1.4);
  renderers[0]->ResetCameraClippingRange();
  renderers[0]->SetBackground(colors->GetColor3d("SteelBlue").GetData());

  renderers[1]->GetActiveCamera()->SetViewUp(0, 0, -1);
  renderers[1]->GetActiveCamera()->SetPosition(0, 1, 0);
  renderers[1]->GetActiveCamera()->SetViewAngle(5.0);
  renderers[1]->ResetCamera();
  renderers[1]->GetActiveCamera()->Zoom(2.2);
  renderers[1]->ResetCameraClippingRange();
  renderers[1]->SetBackground(colors->GetColor3d("LightSteelBlue").GetData());

  renderWindow->SetSize(500, 800);
  renderWindow->SetWindowName("Stocks");

  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
// create the stocks
void AddStock(std::vector<vtkSmartPointer<vtkRenderer>> renderers,
              char* filename, std::string name, double& zPosition,
              bool useTubes)
{
  std::cout << "Adding " << name << std::endl;

  // read the data
  vtkNew<vtkPolyDataReader> PolyDataRead;
  PolyDataRead->SetFileName(filename);
  PolyDataRead->Update();

  // create labels
  vtkNew<vtkVectorText> TextSrc;
  TextSrc->SetText(name.c_str());

  vtkIdType numberOfPoints = PolyDataRead->GetOutput()->GetNumberOfPoints();

  double nameLocation[3];
  double x, y, z;

  vtkIdType nameIndex = (numberOfPoints - 1) * .8;

  PolyDataRead->GetOutput()->GetPoint(nameIndex, nameLocation);

  x = nameLocation[0] * .15;
  y = nameLocation[1] + 5.0;
  z = zPosition;

  // Create a tube and ribbpn filter. One or the other will be used

  vtkNew<vtkTubeFilter> TubeFilter;
  TubeFilter->SetInputConnection(PolyDataRead->GetOutputPort());
  TubeFilter->SetNumberOfSides(8);
  TubeFilter->SetRadius(0.5);
  TubeFilter->SetRadiusFactor(10000);

  vtkNew<vtkRibbonFilter> RibbonFilter;
  RibbonFilter->SetInputConnection(PolyDataRead->GetOutputPort());
  RibbonFilter->VaryWidthOn();
  RibbonFilter->SetWidthFactor(5);
  RibbonFilter->SetDefaultNormal(0, 1, 0);
  RibbonFilter->UseDefaultNormalOn();

  vtkNew<vtkLinearExtrusionFilter> Extrude;
  Extrude->SetInputConnection(RibbonFilter->GetOutputPort());
  Extrude->SetVector(0, 1, 0);
  Extrude->SetExtrusionType(1);
  Extrude->SetScaleFactor(0.7);

  vtkNew<vtkTransform> Transform;
  Transform->Translate(0, 0, zPosition);
  Transform->Scale(0.15, 1, 1);

  vtkNew<vtkTransformPolyDataFilter> TransformFilter;
  TransformFilter->SetTransform(Transform);

  // Select tubes or ribbons
  if (useTubes)
  {
    TransformFilter->SetInputConnection(TubeFilter->GetOutputPort());
  }
  else
  {
    TransformFilter->SetInputConnection(Extrude->GetOutputPort());
  }
  for (size_t r = 0; r < renderers.size(); ++r)
  {
    vtkNew<vtkPolyDataMapper> LabelMapper;
    LabelMapper->SetInputConnection(TextSrc->GetOutputPort());

    vtkNew<vtkFollower> LabelActor;
    LabelActor->SetMapper(LabelMapper);
    LabelActor->SetPosition(x, y, z);
    LabelActor->SetScale(2, 2, 2);
    LabelActor->SetOrigin(TextSrc->GetOutput()->GetCenter());

    // increment zPosition
    zPosition += 8.0;

    vtkNew<vtkPolyDataMapper> StockMapper;
    StockMapper->SetInputConnection(TransformFilter->GetOutputPort());
    StockMapper->SetScalarRange(0, 8000);
    vtkNew<vtkActor> StockActor;
    StockActor->SetMapper(StockMapper);

    renderers[r]->AddActor(StockActor);
    renderers[r]->AddActor(LabelActor);
    LabelActor->SetCamera(renderers[r]->GetActiveCamera());
  }
  return;
}
} // namespace
