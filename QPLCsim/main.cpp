#include <iostream>
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  std::clog.rdbuf(std::cout.rdbuf());
#ifdef NDEBUG
  std::clog.rdbuf(0);
#endif
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}
