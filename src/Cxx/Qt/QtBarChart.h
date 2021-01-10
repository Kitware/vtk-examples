#ifndef QtBarChart_H
#define QtBarChart_H

#include <QMainWindow>

// Forward Qt class declarations
class Ui_QtBarChart;

class QtBarChart : public QMainWindow
{
  Q_OBJECT
public:
  // Constructor/Destructor
  QtBarChart();
  ~QtBarChart() = default;

public slots:

  void slotExit();

private:
  // Designer form
  Ui_QtBarChart* ui;
};

#endif
