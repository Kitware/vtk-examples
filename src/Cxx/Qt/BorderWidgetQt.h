#ifndef BorderWidgetQt_H
#define BorderWidgetQt_H

#include <QMainWindow>
#include <vtkSmartPointer.h>

class vtkBorderWidget;

// Forward Qt class declarations
class Ui_BorderWidgetQt;

class BorderWidgetQt : public QMainWindow
{
  Q_OBJECT
public:
  // Constructor/Destructor
  BorderWidgetQt();
  ~BorderWidgetQt() = default;

private:
  vtkSmartPointer<vtkBorderWidget> BorderWidget;
  // Designer form
  Ui_BorderWidgetQt* ui;
};

#endif
