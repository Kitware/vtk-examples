#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkFrenetSerretFrame.h>
#include <vtkGlyph3D.h>
#include <vtkMaskPoints.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSplineWidget.h>

namespace {
void MakeGlyphs(vtkPolyData* src, double size, vtkGlyph3D* glyph);

struct Pipeline
{
  vtkFrenetSerretFrame* frame;

  vtkGlyph3D* glyph3DNormals;
  vtkGlyph3D* glyph3DTangents;
  vtkGlyph3D* glyph3DBinormals;

  vtkPolyDataMapper* glyph3DNormalsMapper;
  vtkPolyDataMapper* glyph3DTangentsMapper;
  vtkPolyDataMapper* glyph3DBinormalsMapper;
};

void SplineCallback(vtkObject* caller, long unsigned int vtkNotUsed(eventId),
                    void* clientData, void* vtkNotUsed(callData))
{
  vtkSplineWidget* splineWidget = static_cast<vtkSplineWidget*>(caller);
  vtkNew<vtkPolyData> polyData;
  splineWidget->GetPolyData(polyData.GetPointer());

  Pipeline* pipeline = static_cast<Pipeline*>(clientData);

  pipeline->frame->SetInputData(polyData);

  vtkNew<vtkMaskPoints> maskPoints;
  maskPoints->SetInputConnection(pipeline->frame->GetOutputPort());
  maskPoints->SetOnRatio(10);
  maskPoints->Update();

  // for each vector, create a Glyph3D and DeepCopy the output
  double radius = 0.1;
  maskPoints->GetOutput()->GetPointData()->SetActiveVectors("FSNormals");
  MakeGlyphs(maskPoints->GetOutput(), radius, pipeline->glyph3DNormals);
  vtkNew<vtkPolyData> normalsPolyData;
  normalsPolyData->DeepCopy(pipeline->glyph3DNormals->GetOutput());

  maskPoints->GetOutput()->GetPointData()->SetActiveVectors("FSTangents");
  MakeGlyphs(maskPoints->GetOutput(), radius, pipeline->glyph3DTangents);
  vtkNew<vtkPolyData> tangentsPolyData;
  tangentsPolyData->DeepCopy(pipeline->glyph3DTangents->GetOutput());

  maskPoints->GetOutput()->GetPointData()->SetActiveVectors("FSBinormals");
  MakeGlyphs(maskPoints->GetOutput(), radius, pipeline->glyph3DBinormals);
  vtkNew<vtkPolyData> binormalsPolyData;
  binormalsPolyData->DeepCopy(pipeline->glyph3DBinormals->GetOutput());

  pipeline->glyph3DNormalsMapper->SetInputData(normalsPolyData);
  pipeline->glyph3DTangentsMapper->SetInputData(tangentsPolyData);
  pipeline->glyph3DBinormalsMapper->SetInputData(binormalsPolyData);
}
} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create the frame
  vtkNew<vtkFrenetSerretFrame> frame;
  frame->ConsistentNormalsOn();

  // Define the glyph3D's
  vtkNew<vtkGlyph3D> glyph3DNormals;
  vtkNew<vtkGlyph3D> glyph3DTangents;
  vtkNew<vtkGlyph3D> glyph3DBinormals;

  vtkNew<vtkPolyDataMapper> glyph3DNormalsMapper;
  vtkNew<vtkPolyDataMapper> glyph3DTangentsMapper;
  vtkNew<vtkPolyDataMapper> glyph3DBinormalsMapper;

  // Populate the pipeline object
  Pipeline pipeline;
  pipeline.glyph3DNormals = glyph3DNormals.GetPointer();
  pipeline.glyph3DTangents = glyph3DTangents.GetPointer();
  pipeline.glyph3DBinormals = glyph3DBinormals.GetPointer();
  pipeline.frame = frame.GetPointer();
  pipeline.glyph3DNormalsMapper = glyph3DNormalsMapper.GetPointer();
  pipeline.glyph3DTangentsMapper = glyph3DTangentsMapper.GetPointer();
  pipeline.glyph3DBinormalsMapper = glyph3DBinormalsMapper.GetPointer();

  // Define the actors
  vtkNew<vtkActor> glyph3DNormalsActor;
  glyph3DNormalsActor->SetMapper(glyph3DNormalsMapper);
  glyph3DNormalsActor->GetProperty()->SetColor(
      colors->GetColor3d("Banana").GetData());

  vtkNew<vtkActor> glyph3DTangentsActor;
  glyph3DTangentsActor->SetMapper(glyph3DTangentsMapper);
  glyph3DTangentsActor->GetProperty()->SetColor(
      colors->GetColor3d("Tomato").GetData());

  vtkNew<vtkActor> glyph3DBinormalsActor;
  glyph3DBinormalsActor->SetMapper(glyph3DBinormalsMapper);
  glyph3DBinormalsActor->GetProperty()->SetColor(
      colors->GetColor3d("ForestGreen").GetData());

  // A renderer and render window
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("FrenetSerretFrameDemo");

  // An interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // The spline widget
  vtkNew<vtkSplineWidget> splineWidget;
  splineWidget->SetInteractor(renderWindowInteractor);

  // The callback does the work
  vtkNew<vtkCallbackCommand> splineCallback;
  splineCallback->SetCallback(SplineCallback);
  splineCallback->SetClientData(&pipeline);
  splineWidget->AddObserver(vtkCommand::StartInteractionEvent, splineCallback);
  splineWidget->AddObserver(vtkCommand::InteractionEvent, splineCallback);
  splineWidget->AddObserver(vtkCommand::EndInteractionEvent, splineCallback);

  // Add the actors
  renderer->AddActor(glyph3DNormalsActor);
  renderer->AddActor(glyph3DTangentsActor);
  renderer->AddActor(glyph3DBinormalsActor);

  renderWindowInteractor->Initialize();
  renderWindow->Render();

  // Prime the pipeline
  splineWidget->SetHandlePosition(2, 0, .5, 0);
  splineWidget->On();
  SplineCallback(splineWidget, 0, &pipeline, 0);

  renderWindow->Render();
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

namespace {
void MakeGlyphs(vtkPolyData* src, double size, vtkGlyph3D* glyph)
{
  // Source for the glyph filter
  vtkNew<vtkArrowSource> arrow;
  arrow->SetTipResolution(16);
  arrow->SetTipLength(.3);
  arrow->SetTipRadius(.1);

  glyph->SetSourceConnection(arrow->GetOutputPort());
  glyph->SetInputData(src);
  glyph->SetVectorModeToUseVector();
  glyph->SetScaleModeToScaleByVector();
  glyph->SetScaleFactor(size);
  glyph->OrientOn();
  glyph->Update();
}
} // namespace
