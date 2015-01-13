#include "QPlcReadY.h"

#include <iostream>

#include "QPlcDialog.h"
#include "VirtualPLC.h"

using namespace std;

QPlcReadY::QPlcReadY(VirtualPLC* plc, int* Ystatus, bool* runStatus, QPlcDialog* dialog, QObject* parent)
:  QThread(parent)
,  mutex()
,  condition()
,  abort(false)
,  virtualPLC(plc)
,  Ystatus(Ystatus)
,  runStatus(runStatus)
,  dialog(dialog)
{
}

QPlcReadY::~QPlcReadY()
{
   mutex.lock();
   abort = true;
   condition.wakeOne();
   mutex.unlock();
   wait();
}


void QPlcReadY::run()
{
   int YstatusPrevious = virtualPLC->getYs();
   emit valueChanged(*Ystatus);  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   while (!abort)
   {
      mutex.lock();
      *Ystatus = virtualPLC->getYs();
      if (YstatusPrevious != *Ystatus) {
         emit valueChanged(*Ystatus);  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      }
      YstatusPrevious = *Ystatus;

      *runStatus = virtualPLC->isRunning();
      emit valueChanged(*runStatus);  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      mutex.unlock();
      msleep(25);
   }
   clog << "[QPlcReadY] stopped" << endl;
}
