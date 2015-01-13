#ifndef QLED_H
#define QLED_H

#include <QWidget>

class QPicture;
class QLabel;

class QLed: public QWidget
{
public:
   QLed(QWidget* parent = 0);
   void setOn();
   void setOff();
private:
   QPicture* ledON;
   QPicture* ledOFF;
   QLabel* led;
   bool isOn;
};

#endif // QLED_H
