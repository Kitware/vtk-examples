#include <QApplication>
#include <QPixmap>
#include <vtkNew.h>

#include <vtkQImageToImageSource.h>

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QPixmap pixmap(10, 10);
  QColor color(10, 20, 30);
  pixmap.fill(color);

  vtkNew<vtkQImageToImageSource> qimageToImageSource;
  QImage qimage = pixmap.toImage();
  qimageToImageSource->SetQImage(&qimage);
  qimageToImageSource->Update();

  //  vtkImageData* image = qimageToImageSource->GetOutput();

  return EXIT_SUCCESS;
}
