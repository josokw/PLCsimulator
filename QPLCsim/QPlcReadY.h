#ifndef QPLCREADY_H
#define QPLCREADY_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

class VirtualPLC;
class QPlcDialog;

class QPlcReadY: public QThread
{
   Q_OBJECT

public:
   QPlcReadY(VirtualPLC* plc, int* Ystatus, bool* runStatus, QPlcDialog* dialog, QObject* parent = 0);
   ~QPlcReadY();

   virtual void run();

signals:
   void valueChanged(int);

private:
   QMutex mutex;
   QWaitCondition condition;
   bool abort;

   VirtualPLC* virtualPLC;
   int* Ystatus;
   bool* runStatus;
   QPlcDialog* dialog;
};

#endif // QPLCREADY_H
