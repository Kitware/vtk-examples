#ifndef BorderWidgetQt_H
#define BorderWidgetQt_H

#include <QMainWindow>
#include <vtkSmartPointer.h>

class vtkBorderWidget;

/*
 * See "The Single Inheritance Approach" in this link:
 * [Using a Designer UI File in Your C++
 * Application](https://doc.qt.io/qt-5/designer-using-a-ui-file.html)
 */
namespace Ui {
class BorderWidgetQt;
}

class BorderWidgetQt : public QMainWindow
{
  Q_OBJECT
public:
  // Constructor/Destructor
  explicit BorderWidgetQt(QWidget* parent = nullptr);
  virtual ~BorderWidgetQt();

private:
  vtkSmartPointer<vtkBorderWidget> BorderWidget;
  // Designer form
  Ui::BorderWidgetQt* ui;
};

#endif
