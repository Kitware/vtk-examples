#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkImageConstantPad.h>
#include <vtkLookupTable.h>
#include <vtkMetaImageReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkWindowLevelLookupTable.h>

#include <map>
#include <string>

namespace {
void CreateFrogLut(vtkLookupTable* colorLut);
void SliceOrder(
    std::map<std::string, vtkSmartPointer<vtkTransform>>& sliceOrder);
} // namespace
int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " frog.mhd frogtissue.mhd [slice]"
              << std::endl;
    return EXIT_FAILURE;
  }
  int sliceNumber = 39; // to match figure 12-6
  if (argc > 3)
  {
    sliceNumber = atoi(argv[3]);
  }
  std::map<std::string, vtkSmartPointer<vtkTransform>> sliceOrder;
  SliceOrder(sliceOrder);

  vtkNew<vtkNamedColors> colors;

  // Now create the RenderWindow, Renderer and Interactor
  //
  vtkNew<vtkRenderer> ren1;
  vtkNew<vtkRenderer> ren2;
  vtkNew<vtkRenderer> ren3;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);
  renWin->AddRenderer(ren2);
  renWin->AddRenderer(ren3);
  renWin->SetWindowName("FrogSlice");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkMetaImageReader> greyReader;
  greyReader->SetFileName(argv[1]);
  greyReader->Update();

  vtkNew<vtkImageConstantPad> greyPadder;
  greyPadder->SetInputConnection(greyReader->GetOutputPort());
  greyPadder->SetOutputWholeExtent(0, 511, 0, 511, sliceNumber, sliceNumber);
  greyPadder->SetConstant(0);

  vtkNew<vtkPlaneSource> greyPlane;

  vtkNew<vtkTransformPolyDataFilter> greyTransform;
  greyTransform->SetTransform(sliceOrder["hfsi"]);
  greyTransform->SetInputConnection(greyPlane->GetOutputPort());

  vtkNew<vtkPolyDataNormals> greyNormals;
  greyNormals->SetInputConnection(greyTransform->GetOutputPort());
  greyNormals->FlipNormalsOff();

  vtkNew<vtkWindowLevelLookupTable> wllut;
  wllut->SetWindow(255);
  wllut->SetLevel(128);
  wllut->SetTableRange(0, 255);
  wllut->Build();

  vtkNew<vtkPolyDataMapper> greyMapper;
  greyMapper->SetInputConnection(greyPlane->GetOutputPort());

  vtkNew<vtkTexture> greyTexture;
  greyTexture->SetInputConnection(greyPadder->GetOutputPort());
  greyTexture->SetLookupTable(wllut);
  greyTexture->SetColorModeToMapScalars();
  greyTexture->InterpolateOn();

  vtkNew<vtkActor> greyActor;
  greyActor->SetMapper(greyMapper);
  greyActor->SetTexture(greyTexture);

  vtkNew<vtkMetaImageReader> segmentReader;
  segmentReader->SetFileName(argv[2]);
  segmentReader->Update();

  vtkNew<vtkImageConstantPad> segmentPadder;
  segmentPadder->SetInputConnection(segmentReader->GetOutputPort());
  segmentPadder->SetOutputWholeExtent(0, 511, 0, 511, sliceNumber, sliceNumber);
  segmentPadder->SetConstant(0);

  vtkNew<vtkPlaneSource> segmentPlane;

  vtkNew<vtkTransformPolyDataFilter> segmentTransform;
  segmentTransform->SetTransform(sliceOrder["hfsi"]);
  segmentTransform->SetInputConnection(segmentPlane->GetOutputPort());

  vtkNew<vtkPolyDataNormals> segmentNormals;
  segmentNormals->SetInputConnection(segmentTransform->GetOutputPort());
  segmentNormals->FlipNormalsOn();

  vtkNew<vtkLookupTable> colorLut;
  colorLut->SetNumberOfColors(17);
  colorLut->SetTableRange(0, 16);
  colorLut->Build();
  CreateFrogLut(colorLut);

  vtkNew<vtkPolyDataMapper> segmentMapper;
  segmentMapper->SetInputConnection(segmentPlane->GetOutputPort());

  vtkNew<vtkTexture> segmentTexture;
  segmentTexture->SetInputConnection(segmentPadder->GetOutputPort());
  segmentTexture->SetLookupTable(colorLut);
  segmentTexture->SetColorModeToMapScalars();
  segmentTexture->InterpolateOff();

  vtkNew<vtkActor> segmentActor;
  segmentActor->SetMapper(segmentMapper);
  segmentActor->SetTexture(segmentTexture);

  vtkNew<vtkActor> segmentOverlayActor;
  segmentOverlayActor->SetMapper(segmentMapper);
  segmentOverlayActor->SetTexture(segmentTexture);

  segmentOverlayActor->GetProperty()->SetOpacity(.5);
  ren1->SetBackground(0, 0, 0);
  ren1->SetViewport(0, .5, .5, 1);
  renWin->SetSize(640, 480);
  ren1->AddActor(greyActor);

  ren2->SetBackground(0, 0, 0);
  ren2->SetViewport(.5, .5, 1, 1);
  ren2->AddActor(segmentActor);

  vtkNew<vtkCamera> cam1;
  cam1->SetViewUp(0, -1, 0);
  cam1->SetPosition(0, 0, -1);
  ren1->SetActiveCamera(cam1);
  ren1->ResetCamera();
  cam1->SetViewUp(0, -1, 0);
  cam1->SetPosition(0.0554068, -0.0596001, -0.491383);
  cam1->SetFocalPoint(0.0554068, -0.0596001, 0);
  ren1->ResetCameraClippingRange();

  ren3->AddActor(greyActor);
  ren3->AddActor(segmentOverlayActor);
  segmentOverlayActor->SetPosition(0, 0, -0.01);

  ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());
  ren2->SetBackground(colors->GetColor3d("SlateGray").GetData());
  ren3->SetBackground(colors->GetColor3d("SlateGray").GetData());

  ren3->SetViewport(0, 0, 1, .5);

  ren2->SetActiveCamera(ren1->GetActiveCamera());
  ren3->SetActiveCamera(ren1->GetActiveCamera());

  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}

namespace {
void CreateFrogLut(vtkLookupTable* colorLut)
{
  vtkNew<vtkNamedColors> colors;
  colorLut->SetTableValue(0, colors->GetColor4d("black").GetData());
  colorLut->SetTableValue(1,
                          colors->GetColor4d("salmon").GetData()); // blood
  colorLut->SetTableValue(2,
                          colors->GetColor4d("beige").GetData()); // brain
  colorLut->SetTableValue(3,
                          colors->GetColor4d("orange").GetData()); // duodenum
  colorLut->SetTableValue(
      4,
      colors->GetColor4d("misty_rose").GetData()); // eye_retina
  colorLut->SetTableValue(5,
                          colors->GetColor4d("white").GetData()); // eye_white
  colorLut->SetTableValue(6,
                          colors->GetColor4d("tomato").GetData()); // heart
  colorLut->SetTableValue(7,
                          colors->GetColor4d("raspberry").GetData()); // ileum
  colorLut->SetTableValue(8,
                          colors->GetColor4d("banana").GetData()); // kidney
  colorLut->SetTableValue(9,
                          colors->GetColor4d("peru").GetData()); // l_intestine
  colorLut->SetTableValue(10,
                          colors->GetColor4d("pink").GetData()); // liver
  colorLut->SetTableValue(11,
                          colors->GetColor4d("powder_blue").GetData()); // lung
  colorLut->SetTableValue(12,
                          colors->GetColor4d("carrot").GetData()); // nerve
  colorLut->SetTableValue(13,
                          colors->GetColor4d("wheat").GetData()); // skeleton
  colorLut->SetTableValue(14,
                          colors->GetColor4d("violet").GetData()); // spleen
  colorLut->SetTableValue(15,
                          colors->GetColor4d("plum").GetData()); // stomach
}

void SliceOrder(
    std::map<std::string, vtkSmartPointer<vtkTransform>>& sliceOrder)
{
  //
  // these transformations permute medical image data to maintain proper
  // orientation regardless of the acquisition order. After applying these
  // transforms with vtkTransformFilter, a view up of 0,-1,0 will result in
  // the body part facing the viewer. NOTE: some transformations have a -1
  // scale factor for one of the components.
  //       To ensure proper polygon orientation and normal direction, you must
  //       apply the vtkPolyDataNormals filter.
  //
  // Naming:
  // si - superior to inferior (top to bottom)
  // is - inferior to superior (bottom to top)
  // ap - anterior to posterior (front to back)
  // pa - posterior to anterior (back to front)
  // lr - left to right
  // rl - right to left
  //
  double siMatrix[16] = {1, 0, 0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1};
  vtkNew<vtkTransform> si;
  si->SetMatrix(siMatrix);
  sliceOrder["si"] = si;

  double isMatrix[16] = {1, 0, 0, 0, 0, 0, -1, 0, 0, -1, 0, 0, 0, 0, 0, 1};
  vtkNew<vtkTransform> is;
  is->SetMatrix(isMatrix);
  sliceOrder["is"] = is;

  vtkNew<vtkTransform> ap;
  ap->Scale(1, -1, 1);
  sliceOrder["ap"] = ap;

  vtkNew<vtkTransform> pa;
  pa->Scale(1, -1, -1);
  sliceOrder["pa"] = pa;

  double lrMatrix[16] = {0, 0, -1, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1};
  vtkNew<vtkTransform> lr;
  lr->SetMatrix(lrMatrix);
  sliceOrder["lr"] = lr;

  double rlMatrix[16] = {0, 0, 1, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1};
  vtkNew<vtkTransform> rl;
  rl->SetMatrix(rlMatrix);
  sliceOrder["rl"] = rl;

  //
  // The previous transforms assume radiological views of the slices (viewed
  // from the feet). other modalities such as physical sectioning may view
  // from the head. these transforms modify the original with a 180 rotation
  // about y
  //
  double hfMatrix[16] = {-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1};
  vtkNew<vtkTransform> hf;
  hf->SetMatrix(hfMatrix);
  sliceOrder["hf"] = hf;

  vtkNew<vtkTransform> hfsi;
  hfsi->Concatenate(hf->GetMatrix());
  hfsi->Concatenate(si->GetMatrix());
  sliceOrder["hfsi"] = hfsi;

  vtkNew<vtkTransform> hfis;
  hfis->Concatenate(hf->GetMatrix());
  hfis->Concatenate(is->GetMatrix());
  sliceOrder["hfis"] = hfis;

  vtkNew<vtkTransform> hfap;
  hfap->Concatenate(hf->GetMatrix());
  hfap->Concatenate(ap->GetMatrix());
  sliceOrder["hfap"] = hfap;

  vtkNew<vtkTransform> hfpa;
  hfpa->Concatenate(hf->GetMatrix());
  hfpa->Concatenate(pa->GetMatrix());
  sliceOrder["hfpa"] = hfpa;

  vtkNew<vtkTransform> hflr;
  hflr->Concatenate(hf->GetMatrix());
  hflr->Concatenate(lr->GetMatrix());
  sliceOrder[""] = hflr;

  vtkNew<vtkTransform> hfrl;
  hfrl->Concatenate(hf->GetMatrix());
  hfrl->Concatenate(rl->GetMatrix());
  sliceOrder["hfrl"] = hfrl;
}
} // namespace
