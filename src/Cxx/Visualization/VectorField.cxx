#include <vtkArrowSource.h>
#include <vtkCellArray.h>
#include <vtkGlyph2D.h>
#include <vtkImageData.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNew.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>


int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Create an image
  vtkNew<vtkImageData> image;

  // Specify the size of the image data
  image->SetDimensions(50, 50, 1);
  image->AllocateScalars(VTK_FLOAT, 3);

  int* dims = image->GetDimensions();

  // Zero the image
  for (auto z = 0; z < dims[2]; ++z)
  {
    for (auto y = 0; y < dims[1]; ++y)
    {
      for (auto x = 0; x < dims[0]; ++x)
      {
        auto pixel = static_cast<float*>(image->GetScalarPointer(x, y, z));
        pixel[0] = 0.0;
        pixel[1] = 0.0;
        pixel[2] = 0.0;
      }
    }
  }

  //Set two vectors
  {
    auto pixel = static_cast<float*>(image->GetScalarPointer(20, 20, 0));
    pixel[0] = -10.0;
    pixel[1] = 5.0;
  }

  {
    auto pixel = static_cast<float*>(image->GetScalarPointer(30, 30, 0));
    pixel[0] = 10.0;
    pixel[1] = 10.0;
  }

  // A better way to do this is (should be tested for compatibility and
  // correctness).
  // std::cout << image->GetPointData()->GetScalars()->GetName() << std::endl;
  image->GetPointData()->SetActiveVectors(
      image->GetPointData()->GetScalars()->GetName());
  // image->GetPointData()->SetActiveVectors("ImageScalars");

  // Setup the arrows
  vtkNew<vtkArrowSource> arrowSource;
  arrowSource->Update();

  vtkNew<vtkGlyph2D> glyphFilter;
  glyphFilter->SetSourceConnection(arrowSource->GetOutputPort());
  glyphFilter->OrientOn();
  glyphFilter->SetVectorModeToUseVector();
  glyphFilter->SetInputData(image);
  glyphFilter->Update();

  // Create actors
  vtkNew<vtkImageSliceMapper> imageMapper;
  imageMapper->SetInputData(image);

  vtkNew<vtkImageSlice> imageSlice;
  imageSlice->SetMapper(imageMapper);

  vtkNew<vtkPolyDataMapper> vectorMapper;
  vectorMapper->SetInputConnection(glyphFilter->GetOutputPort());
  vtkNew<vtkActor> vectorActor;
  vectorActor->SetMapper(vectorMapper);

  // Setup renderer
  vtkNew<vtkRenderer> renderer;
  renderer->AddViewProp(imageSlice);
  renderer->AddViewProp(vectorActor);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("VectorField");

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  // vtkNew<vtkInteractorStyleImage> style;
  // renderWindowInteractor->SetInteractorStyle(style);

  // Render and start interaction
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
