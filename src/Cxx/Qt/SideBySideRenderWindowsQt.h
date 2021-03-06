#ifndef SideBySideRenderWindowsQt_H
#define SideBySideRenderWindowsQt_H

#include <QMainWindow>

/*
 * See "The Single Inheritance Approach" in this link:
 * [Using a Designer UI File in Your C++
 * Application](https://doc.qt.io/qt-5/designer-using-a-ui-file.html)
 */
namespace Ui {
class SideBySideRenderWindowsQt;
}

class SideBySideRenderWindowsQt : public QMainWindow
{
  Q_OBJECT
public:
  // Constructor/Destructor
  explicit SideBySideRenderWindowsQt(QWidget* parent = nullptr);
  virtual ~SideBySideRenderWindowsQt() = default;

private:
  // Designer form
  Ui::SideBySideRenderWindowsQt* ui;

public slots:

  void slotExit();
};

#endif
