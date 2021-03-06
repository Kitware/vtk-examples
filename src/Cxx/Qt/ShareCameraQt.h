#ifndef ShareCameraQt_H
#define ShareCameraQt_H

#include <QMainWindow>

/*
 * See "The Single Inheritance Approach" in this link:
 * [Using a Designer UI File in Your C++
 * Application](https://doc.qt.io/qt-5/designer-using-a-ui-file.html)
 */
namespace Ui {
class ShareCameraQt;
}

class ShareCameraQt : public QMainWindow
{
  Q_OBJECT
public:
  // Constructor/Destructor
  explicit ShareCameraQt(QWidget* parent = nullptr);
  virtual ~ShareCameraQt() = default;

private:
  // Designer form
  Ui::ShareCameraQt *ui;

public slots:
  void slotExit();

protected:
  void ModifiedHandler();
};

#endif
