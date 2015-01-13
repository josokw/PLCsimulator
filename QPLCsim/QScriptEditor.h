#ifndef QSCRIPTEDITOR_H
#define QSCRIPTEDITOR_H

#include <QtGui>
#include <QCompleter>
#include <QObject>
#include <QPlainTextEdit>
#include <QRect>

class QScriptEditor : public QPlainTextEdit
{
   Q_OBJECT

public:
   QScriptEditor(QWidget* parent = 0);
   ~QScriptEditor();

   void lineNumberBarPaintEvent(QPaintEvent* event);
   int lineNumberBarWidth();
   void setCompleter(QCompleter* c);
   //    QCompleter* completer() { return _completer; }

protected:
   void resizeEvent(QResizeEvent* event);
   void keyPressEvent(QKeyEvent *e);
   void focusInEvent(QFocusEvent *e);

private slots:
   void updateLineNumberBarWidth(int newBlockCount);
   void highlightCurrentLine();
   void updateLineNumberBar(const QRect&, int);
   void insertCompletion(const QString& completion);

private:
   QWidget* lineNumberBar;
   QCompleter* completer;
   QString textUnderCursor() const;
};

#endif // QSCRIPTEDITOR_H
