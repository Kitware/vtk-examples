#include <vtkFreeTypeTools.h>
#include <vtkImageActor.h>
#include <vtkImageBlend.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGWriter.h>
#include <vtkPointData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStdString.h>
#include <vtkTextProperty.h>

#include <array>

int main(int argc, char* argv[])
{
  vtkNew<vtkNamedColors> colors;

  std::array<double, 3> drawColor1{0, 0, 0};
  std::array<double, 3> drawColor2{0, 0, 0};
  auto color1 = colors->GetColor3ub("Blue").GetData();
  auto color2 = colors->GetColor3ub("Red").GetData();
  for (auto i = 0; i < 3; ++i)
  {
    drawColor1[i] = color1[i];
    drawColor2[i] = color2[i];
  }
  // Create a blue image with a red circle of radius 50 centered at (60, 60)
  vtkNew<vtkImageCanvasSource2D> drawing;
  drawing->SetScalarTypeToUnsignedChar(); // PNGWriter requires unsigned char
                                          // (or unsigned short)
  drawing->SetNumberOfScalarComponents(3);
  drawing->SetExtent(0, 150, 0, 120, 0,
                     0); // xmin, xmax, ymin, ymax, zmin, zmax
  drawing->SetDrawColor(drawColor1.data());
  drawing->FillBox(0, 150, 0, 120);
  drawing->SetDrawColor(drawColor2.data());
  drawing->DrawCircle(60, 60, 50); // parameters: x, y, radius
  // Create an image of text
  vtkFreeTypeTools* freeType = vtkFreeTypeTools::GetInstance();
  vtkNew<vtkTextProperty> textProperty;
  textProperty->SetColor(colors->GetColor3d("Yellow").GetData());
  textProperty->SetJustificationToLeft();
  textProperty->SetFontSize(24);
  textProperty->SetOrientation(25);
  vtkStdString text("   Test String");
  vtkNew<vtkImageData> textImage;
  freeType->RenderString(textProperty, text, 70, textImage.GetPointer());

  // Combine the images
  vtkNew<vtkImageBlend> blend;
  blend->AddInputConnection(drawing->GetOutputPort());
  blend->AddInputData(textImage);
  blend->SetOpacity(0, 1.0); // background image: 50% opaque
  blend->SetOpacity(1, 1.0); // text: 100% opaque
  blend->Update();

  vtkNew<vtkImageActor> blendActor;
  blendActor->GetMapper()->SetInputConnection(blend->GetOutputPort());

  vtkNew<vtkRenderer> blendRenderer;
  blendRenderer->AddActor(blendActor);
  blendRenderer->ResetCamera();
  blendRenderer->SetBackground(colors->GetColor3d("DodgerBlue").GetData());

  // vtkNew<vtkPNGWriter> writer;
  // writer->SetFileName("output.png");
  // writer->SetInputConnection(blend->GetOutputPort());
  // writer->Write();
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(300, 300);
  renderWindow->AddRenderer(blendRenderer);
  renderWindow->SetWindowName("ImageText");

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleImage> style;

  renderWindowInteractor->SetInteractorStyle(style);

  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
