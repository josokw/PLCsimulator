#ifndef QPLCDIALOG_H
#define QPLCDIALOG_H

#include <QButtonGroup>
#include <QFrame>
#include <QGridLayout>

class VirtualPLC;
class QLabel;
class QLed;
class QPlcReadY;
class QPushButton;

class QPlcDialog: public QFrame
{
  Q_OBJECT

public:
  QPlcDialog(VirtualPLC* plc, QWidget* parent = 0, Qt::WindowFlags f = 0);

  void connectToPLC(VirtualPLC* plc);

public slots:
  void PLCtoY();
  void PLCtoRunStatus();

private slots:
  void XtoPLC(int index);

private:
  int Xstatus;
  int Ystatus;
  bool runStatus;
  VirtualPLC* virtualPLC;
  QVector<bool> Xtoggle;
  QGridLayout* grid;
  QButtonGroup* Xbuttons;
  QButtonGroup* Ybuttons;
  QPushButton* runStatusButton;
  QPlcReadY* PLCreadY;
};

#endif // QPLCDIALOG_H
