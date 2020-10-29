#include <vtkActor2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

#include <sstream>
#include <string>
#include <vtksys/SystemTools.hxx>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[1] << " font.ttf e.g. CopyPaste.ttf"
              << std::endl;
    return EXIT_FAILURE;
  }

  std::string fontFile(argv[1]);
  std::stringstream str;
  str << "Font: "
      << vtksys::SystemTools::GetFilenameWithoutExtension(std::string(argv[1]))
      << "\n"
      << "Sample multiline\ntext rendered\nusing FreeTypeTools.";

  int width = 640;
  int height = 480;

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkTextMapper> mapper;

  vtkNew<vtkActor2D> actor;
  actor->SetPosition(width / 2, height / 2);
  actor->SetMapper(mapper);
  mapper->GetTextProperty()->BoldOff();
  ;
  mapper->GetTextProperty()->SetFontSize(50);
  mapper->GetTextProperty()->SetColor(
      colors->GetColor3d("MistyRose").GetData());
  mapper->GetTextProperty()->SetJustificationToCentered();
  mapper->GetTextProperty()->SetVerticalJustificationToCentered();
  mapper->GetTextProperty()->SetFontFamily(VTK_FONT_FILE);
  mapper->GetTextProperty()->SetFontFile(fontFile.c_str());
  mapper->SetInput(str.str().c_str());

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("DimGray").GetData());

  vtkNew<vtkRenderWindow> renderWindow;

  renderWindow->SetSize(width, height);
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("FontFile");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);

  renderWindow->Render();
  interactor->Initialize();
  interactor->Start();

  return EXIT_SUCCESS;
}
