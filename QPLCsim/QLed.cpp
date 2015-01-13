#include "QLed.h"

#include <QLabel>
#include <QPicture>

QLed::QLed(QWidget* parent)
:  QWidget(parent)
,  ledON(new QPicture)
,  ledOFF(new QPicture)
,  led(new QLabel(this))
,  isOn(false)
{
   //ledON->load("../images/
   //led->setPicture(QPicture(
}


void QLed::setOn()
{
   isOn = true;
}

void QLed::setOff()
{
   isOn = false;
}
