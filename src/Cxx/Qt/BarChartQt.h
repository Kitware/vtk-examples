#ifndef BarChartQt_H
#define BarChartQt_H

#include <QMainWindow>

/*
 * See "The Single Inheritance Approach" in this link:
 * [Using a Designer UI File in Your C++
 * Application](https://doc.qt.io/qt-5/designer-using-a-ui-file.html)
 */
namespace Ui {
class BarChartQt;
}

class BarChartQt : public QMainWindow
{
  Q_OBJECT
public:
  // Constructor/Destructor
  explicit BarChartQt(QWidget* parent = nullptr);
  virtual ~BarChartQt();

public slots:

  void slotExit();

private:
  // Designer form
  Ui::BarChartQt* ui;
};

#endif
