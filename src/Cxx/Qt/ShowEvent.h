#ifndef ShowEventQt_H
#define ShowEventQt_H

#include <QMainWindow>

/*
 * See "The Single Inheritance Approach" in this link:
 * [Using a Designer UI File in Your C++
 * Application](https://doc.qt.io/qt-5/designer-using-a-ui-file.html)
 */
namespace Ui {
class ShowEvent;
}

class ShowEvent : public QWidget //, private Ui::ShowEvent
{
  Q_OBJECT

public:
  explicit ShowEvent(QWidget* parent = nullptr);
  virtual ~ShowEvent() = default;

private:
  Ui::ShowEvent* ui;

protected:
  void showEvent(QShowEvent* event);
};

#endif
