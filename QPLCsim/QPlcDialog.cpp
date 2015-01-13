#include "QPlcDialog.h"

#include <QAbstractButton>
#include <QColor>
#include <QLabel>
#include <QList>
#include <QPalette>
#include <QPushButton>

#include <iostream>

#include "VirtualPLC.h"
#include "QPlcReadY.h"

using namespace std;

QPlcDialog::QPlcDialog(VirtualPLC* plc, QWidget* parent, Qt::WindowFlags f)
  :  QFrame(parent, f)
  ,  Xstatus(0)
  ,  Ystatus(0)
  ,  runStatus(false)
  ,  virtualPLC(plc)
  ,  Xtoggle(virtualPLC->getMaxX())
  ,  grid(new QGridLayout(this))
  ,  Xbuttons(new QButtonGroup(this))
  ,  Ybuttons(new QButtonGroup(this))
  ,  PLCreadY(new QPlcReadY(virtualPLC, &Ystatus, &runStatus, this))
{
  setFrameStyle(3);
  grid->addWidget(new QLabel("PLC ", this), 0, 0);
  grid->addWidget(new QLabel("", this), 3, 0);
  grid->addWidget(new QLabel(" Inputs", this), 4, 0);
  grid->addWidget(new QLabel(" Outputs", this), 1, 0);

  for (int i = 0; i < virtualPLC->getMaxX(); ++i) {
      Xbuttons->addButton(new QPushButton("X"  + QString('0' + i), this), i);
      Xbuttons->button(i)->setStyleSheet("background: grey; color:blue; font-weight: bold;");
      grid->addWidget(Xbuttons->button(i), 4, i + 1);
      Xtoggle[i] = true;
    }

  for (int i = 0; i < virtualPLC->getMaxY(); ++i) {
      Ybuttons->addButton(new QPushButton("Y" + QString(('0' + i)),this), i);
      Ybuttons->button(i)->setStyleSheet("background: grey; color:yellow; font-weight: bold;");
      Ybuttons->button(i)->setEnabled(false);
      grid->addWidget(Ybuttons->button(i), 1, i + 1);
    }

  runStatusButton = new QPushButton("running", this);
  runStatusButton->setEnabled(false);
  runStatusButton->setStyleSheet("background: grey; color:black; font-weight: bold;");
  grid->addWidget(runStatusButton, 0, 10);

  connect(Xbuttons, SIGNAL(buttonClicked(int)), this, SLOT(XtoPLC(int)));
  connect(PLCreadY, SIGNAL(valueChanged(int)), this, SLOT(PLCtoY()), Qt::QueuedConnection);
  connect(PLCreadY, SIGNAL(valueChanged(int)), this, SLOT(PLCtoRunStatus()), Qt::QueuedConnection);

  PLCreadY->start();
}

void QPlcDialog::connectToPLC(VirtualPLC* plc)
{
  virtualPLC = plc;
}

void QPlcDialog::XtoPLC(int index)
{
  if (Xtoggle[index]) {
      Xbuttons->button(index)->setStyleSheet("background: red; color:blue; font-weight: bold;");
      virtualPLC->setX(index, true);
    }
  else {
      Xbuttons->button(index)->setStyleSheet("background: grey; color:blue; font-weight: bold;");
      virtualPLC->setX(index, false);
    }
  Xtoggle[index] = !Xtoggle[index];
}

void QPlcDialog::PLCtoY()
{
  for (int i = 0; i < (Ybuttons->buttons().size()); ++i)
    {
      if (Ystatus & (1 << i)) {
          Ybuttons->button(i)->setStyleSheet("background: red; color:yellow; font-weight: bold;");
          //Ybuttons->button(i)->update();
        }
      else {
          Ybuttons->button(i)->setStyleSheet("background: grey; color:yellow; font-weight: bold;");
          //Ybuttons->button(i)->update();
        }
    }
}

void QPlcDialog::PLCtoRunStatus()
{
  if (runStatus) {
      runStatusButton->setStyleSheet("background: green; color:black; font-weight: bold;");
    }
  else {
      runStatusButton->setStyleSheet("background: grey; color:black; font-weight: bold;");
    }
}
