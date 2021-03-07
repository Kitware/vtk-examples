#include <iostream>

#include "ShowEvent.h"
#include "ui_ShowEvent.h"

ShowEvent::ShowEvent(QWidget* parent) : QWidget(parent), ui(new Ui::ShowEvent)
{
  this->ui->setupUi(this);

  // Do not do anything related to VTK display here!
  std::cout << "Constructor" << std::endl;
}

void ShowEvent::showEvent(QShowEvent*)
{
  // Instead, do the VTK display things here!
  std::cout << "VTK Stuff" << std::endl;
}
