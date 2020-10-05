//
// This example demonstrates how to read a series of dicom images
// and how to scroll with the mousewheel or the up/down keys
// through all slices
//
// some standard vtk headers
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
// headers needed for this example
#include <vtkActor2D.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleImage.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

// needed to easily convert int to std::string
#include <sstream>

namespace {

// helper class to format slice status message
class StatusMessage
{
public:
  static std::string Format(int slice, int maxSlice)
  {
    std::stringstream tmp;
    tmp << "Slice Number  " << slice + 1 << "/" << maxSlice + 1;
    return tmp.str();
  }
};

// Define own interaction style
class myVtkInteractorStyleImage : public vtkInteractorStyleImage
{
public:
  static myVtkInteractorStyleImage* New();
  vtkTypeMacro(myVtkInteractorStyleImage, vtkInteractorStyleImage);

protected:
  vtkImageViewer2* _ImageViewer;
  vtkTextMapper* _StatusMapper;
  int _Slice;
  int _MinSlice;
  int _MaxSlice;

public:
  void SetImageViewer(vtkImageViewer2* imageViewer)
  {
    _ImageViewer = imageViewer;
    _MinSlice = imageViewer->GetSliceMin();
    _MaxSlice = imageViewer->GetSliceMax();
    _Slice = _MinSlice;
    cout << "Slicer: Min = " << _MinSlice << ", Max = " << _MaxSlice
         << std::endl;
  }

  void SetStatusMapper(vtkTextMapper* statusMapper)
  {
    _StatusMapper = statusMapper;
  }

protected:
  void MoveSliceForward()
  {
    if (_Slice < _MaxSlice)
    {
      _Slice += 1;
      cout << "MoveSliceForward::Slice = " << _Slice << std::endl;
      _ImageViewer->SetSlice(_Slice);
      std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
      _StatusMapper->SetInput(msg.c_str());
      _ImageViewer->Render();
    }
  }

  void MoveSliceBackward()
  {
    if (_Slice > _MinSlice)
    {
      _Slice -= 1;
      cout << "MoveSliceBackward::Slice = " << _Slice << std::endl;
      _ImageViewer->SetSlice(_Slice);
      std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
      _StatusMapper->SetInput(msg.c_str());
      _ImageViewer->Render();
    }
  }

  virtual void OnKeyDown()
  {
    std::string key = this->GetInteractor()->GetKeySym();
    if (key.compare("Up") == 0)
    {
      // cout << "Up arrow key was pressed." << endl;
      MoveSliceForward();
    }
    else if (key.compare("Down") == 0)
    {
      // cout << "Down arrow key was pressed." << endl;
      MoveSliceBackward();
    }
    // forward event
    vtkInteractorStyleImage::OnKeyDown();
  }

  virtual void OnMouseWheelForward()
  {
    // std::cout << "Scrolled mouse wheel forward." << std::endl;
    MoveSliceForward();
    // don't forward events, otherwise the image will be zoomed
    // in case another interactorstyle is used (e.g. trackballstyle, ...)
    // vtkInteractorStyleImage::OnMouseWheelForward();
  }

  virtual void OnMouseWheelBackward()
  {
    // std::cout << "Scrolled mouse wheel backward." << std::endl;
    if (_Slice > _MinSlice)
    {
      MoveSliceBackward();
    }
    // don't forward events, otherwise the image will be zoomed
    // in case another interactorstyle is used (e.g. trackballstyle, ...)
    // vtkInteractorStyleImage::OnMouseWheelBackward();
  }
};

vtkStandardNewMacro(myVtkInteractorStyleImage);

} // namespace

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  // Verify input arguments
  if (argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " FolderName" << std::endl;
    return EXIT_FAILURE;
  }

  std::string folder = argv[1];
  // std::string folder = "C:\\VTK\\vtkdata-5.8.0\\Data\\DicomTestImages";

  // Read all the DICOM files in the specified directory.
  vtkNew<vtkDICOMImageReader> reader;
  reader->SetDirectoryName(folder.c_str());
  reader->Update();

  // Visualize
  vtkNew<vtkImageViewer2> imageViewer;
  imageViewer->SetInputConnection(reader->GetOutputPort());

  // slice status message
  vtkNew<vtkTextProperty> sliceTextProp;
  sliceTextProp->SetFontFamilyToCourier();
  sliceTextProp->SetFontSize(20);
  sliceTextProp->SetVerticalJustificationToBottom();
  sliceTextProp->SetJustificationToLeft();

  vtkNew<vtkTextMapper> sliceTextMapper;
  std::string msg = StatusMessage::Format(imageViewer->GetSliceMin(),
                                          imageViewer->GetSliceMax());
  sliceTextMapper->SetInput(msg.c_str());
  sliceTextMapper->SetTextProperty(sliceTextProp);

  vtkNew<vtkActor2D> sliceTextActor;
  sliceTextActor->SetMapper(sliceTextMapper);
  sliceTextActor->SetPosition(15, 10);

  // usage hint message
  vtkNew<vtkTextProperty> usageTextProp;
  usageTextProp->SetFontFamilyToCourier();
  usageTextProp->SetFontSize(14);
  usageTextProp->SetVerticalJustificationToTop();
  usageTextProp->SetJustificationToLeft();

  vtkNew<vtkTextMapper> usageTextMapper;
  usageTextMapper->SetInput(
      "- Slice with mouse wheel\n  or Up/Down-Key\n- Zoom with pressed right\n "
      " mouse button while dragging");
  usageTextMapper->SetTextProperty(usageTextProp);

  vtkNew<vtkActor2D> usageTextActor;
  usageTextActor->SetMapper(usageTextMapper);
  usageTextActor->GetPositionCoordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  usageTextActor->GetPositionCoordinate()->SetValue(0.05, 0.95);

  // create an interactor with our own style (inherit from
  // vtkInteractorStyleImage) in order to catch mousewheel and key events
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  vtkNew<myVtkInteractorStyleImage> myInteractorStyle;

  // make imageviewer2 and sliceTextMapper visible to our interactorstyle
  // to enable slice status message updates when scrolling through the slices
  myInteractorStyle->SetImageViewer(imageViewer);
  myInteractorStyle->SetStatusMapper(sliceTextMapper);

  imageViewer->SetupInteractor(renderWindowInteractor);
  // make the interactor use our own interactorstyle
  // cause SetupInteractor() is defining it's own default interatorstyle
  // this must be called after SetupInteractor()
  renderWindowInteractor->SetInteractorStyle(myInteractorStyle);
  // add slice status message and usage hint message to the renderer
  imageViewer->GetRenderer()->AddActor2D(sliceTextActor);
  imageViewer->GetRenderer()->AddActor2D(usageTextActor);

  // initialize rendering and interaction
  imageViewer->Render();
  imageViewer->GetRenderer()->ResetCamera();
  imageViewer->GetRenderer()->SetBackground(
      colors->GetColor3d("SlateGray").GetData());
  imageViewer->GetRenderWindow()->SetSize(800, 800);
  imageViewer->GetRenderWindow()->SetWindowName("ReadDICOMSeries");
  imageViewer->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
