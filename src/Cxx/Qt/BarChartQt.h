#ifndef BarChartQt_H
#define BarChartQt_H

#include <QMainWindow>

// Forward Qt class declarations
class Ui_BarChartQt;

class BarChartQt : public QMainWindow
{
  Q_OBJECT
public:
  // Constructor/Destructor
  BarChartQt();
  ~BarChartQt() = default;

public slots:

  void slotExit();

private:
  // Designer form
  Ui_BarChartQt* ui;
};

#endif
