#ifndef EventQtSlotConnect_H
#define EventQtSlotConnect_H

#include <QMainWindow>

#include <vtkObject.h>
#include <vtkSmartPointer.h>

// Forward class declarations
class vtkEventQtSlotConnect;
class Ui_EventQtSlotConnect;

class EventQtSlotConnect : public QMainWindow
{
  Q_OBJECT
public:
  EventQtSlotConnect();

public slots:

  void slot_clicked(vtkObject*, unsigned long, void*, void*);

private:
  vtkSmartPointer<vtkEventQtSlotConnect> Connections;
  // Designer form
  Ui_EventQtSlotConnect* ui;
};

#endif
