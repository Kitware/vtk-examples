#ifndef RenderWindowUISingleInheritance_H
#define RenderWindowUISingleInheritance_H

#include <vtkSmartPointer.h>

#include <QMainWindow>

/*
 * See "The Single Inheritance Approach" in this link:
 * [Using a Designer UI File in Your C++
 * Application](https://doc.qt.io/qt-5/designer-using-a-ui-file.html)
 */
namespace Ui {
class RenderWindowUISingleInheritance;
}

class RenderWindowUISingleInheritance : public QMainWindow
{
  Q_OBJECT
public:
  // Constructor/Destructor
  explicit RenderWindowUISingleInheritance(QWidget* parent = nullptr);
  virtual ~RenderWindowUISingleInheritance();

private:
  // Designer form
  Ui::RenderWindowUISingleInheritance* ui;

public slots:
  void slotExit();
};

#endif
