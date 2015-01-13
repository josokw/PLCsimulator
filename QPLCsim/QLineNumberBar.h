#ifndef QLINENUMBERBAR_H
#define QLINENUMBERBAR_H

#include <QSize>
#include <QTextEdit>
#include <QWidget>

#include "QScriptEditor.h"

class QLineNumberBar : public QWidget
{
public:
   QLineNumberBar(QScriptEditor* editor);

   QSize sizeHint() const
   {
      return QSize(editor->lineNumberBarWidth(), 0);
   }

protected:
   void paintEvent(QPaintEvent* event)
   {
      editor->lineNumberBarPaintEvent(event);
   }

private:
   QScriptEditor* editor;
};

#endif // QLINENUMBERBAR_H
