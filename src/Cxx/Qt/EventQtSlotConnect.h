#ifndef EventQtSlotConnect_H
#define EventQtSlotConnect_H

#include <QMainWindow>

#include <vtkObject.h>
#include <vtkSmartPointer.h>

// Forward class declarations
class vtkEventQtSlotConnect;

/*
 * See "The Single Inheritance Approach" in this link:
 * [Using a Designer UI File in Your C++
 * Application](https://doc.qt.io/qt-5/designer-using-a-ui-file.html)
 */
namespace Ui {
class EventQtSlotConnect;
}

class EventQtSlotConnect : public QMainWindow
{
  Q_OBJECT
public:
  explicit EventQtSlotConnect(QWidget* parent = nullptr);
  virtual ~EventQtSlotConnect();

public slots:

  void slot_clicked(vtkObject*, unsigned long, void*, void*);

private:
  vtkSmartPointer<vtkEventQtSlotConnect> Connections;
  // Designer form
  Ui::EventQtSlotConnect* ui;
};

#endif
