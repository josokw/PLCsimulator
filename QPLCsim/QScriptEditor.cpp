#include <QAbstractItemView>
#include <QScrollBar>
#include "QScriptEditor.h"
#include "QLineNumberBar.h"

QScriptEditor::QScriptEditor(QWidget* parent)
   :  QPlainTextEdit(parent)
   ,  lineNumberBar(new QLineNumberBar(this))
   ,  completer(0)
{
   connect(this, SIGNAL(blockCountChanged(int)), this,
           SLOT(updateLineNumberBarWidth(int)));
   connect(this, SIGNAL(updateRequest(const QRect&, int)), this,
           SLOT(updateLineNumberBar(const QRect&, int)));
   connect(this, SIGNAL(cursorPositionChanged()), this,
           SLOT(highlightCurrentLine()));
   updateLineNumberBarWidth(0);
   highlightCurrentLine();
}

QScriptEditor::~QScriptEditor()
{ }

int QScriptEditor::lineNumberBarWidth()
{
   int digits = 1;
   int max = qMax(1, blockCount());
   while (max >= 10)
   {
      max /= 10;
      ++digits;
   }
   int space = 3 + fontMetrics().width(QLatin1Char('9')) * (digits + 1);

   return space;
}

void QScriptEditor::setCompleter(QCompleter* c)
{
   if (completer)
   {
      QObject::disconnect(completer, 0 , this, 0);
   }
   completer = c;
   if (!completer)
   {
      return;
   }
   completer->setWidget(this);
   completer->setCompletionMode(QCompleter::PopupCompletion);
   completer->setCaseSensitivity(Qt::CaseInsensitive);
   QObject::connect(completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

void QScriptEditor::updateLineNumberBarWidth(int /* newBlockCount */)
{
   setViewportMargins(lineNumberBarWidth(), 0, 0, 0);
}

void QScriptEditor::updateLineNumberBar(const QRect& rect, int dy)
{
   if (dy) {
      lineNumberBar->scroll(0, dy);
   }
   else {
      lineNumberBar->update(0, rect.y(), lineNumberBar->width(), rect.height());
   }
   if (rect.contains(viewport()->rect())) {
      updateLineNumberBarWidth(0);
   }
}

void QScriptEditor::highlightCurrentLine()
{
   QList<QTextEdit::ExtraSelection> extraSelections;

   if (!isReadOnly()) {
      QTextEdit::ExtraSelection selection;
      QColor lineColor = QColor(Qt::yellow).lighter(160);

      selection.format.setBackground(lineColor);
      selection.format.setProperty(QTextFormat::FullWidthSelection, true);
      selection.cursor = textCursor();
      selection.cursor.clearSelection();
      extraSelections.append(selection);
   }
   setExtraSelections(extraSelections);
}

void QScriptEditor::lineNumberBarPaintEvent(QPaintEvent* event)
{
   QPainter painter(lineNumberBar);
   painter.fillRect(event->rect(), Qt::lightGray);

   QTextBlock block = firstVisibleBlock();
   int blockNumber = block.blockNumber();
   int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
   int bottom = top + (int) blockBoundingRect(block).height();

   while (block.isValid() && top <= event->rect().bottom())
   {
      if (block.isVisible() && bottom >= event->rect().top())
      {
         QString number = QString::number(blockNumber + 1);
         painter.setPen(Qt::black);
         painter.drawText(0, top, lineNumberBar->width(), fontMetrics().height(),
                          Qt::AlignRight, number);
      }
      block = block.next();
      top = bottom;
      bottom = top + (int) blockBoundingRect(block).height();
      ++blockNumber;
   }
}

void QScriptEditor::resizeEvent(QResizeEvent *e)
{
   QPlainTextEdit::resizeEvent(e);
   QRect cr = contentsRect();
   lineNumberBar->setGeometry(QRect(cr.left(), cr.top(), lineNumberBarWidth(), cr.height()));
}

void QScriptEditor::keyPressEvent(QKeyEvent *e)
{
   if (completer && completer->popup()->isVisible())
   {
      switch (e->key())
      {
         case Qt::Key_Enter:
         case Qt::Key_Return:
         case Qt::Key_Escape:
         case Qt::Key_Tab:
         case Qt::Key_Backtab:
            e->ignore();
            return;
         default:
            break;
      }
   }

   bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
   if (!completer || !isShortcut)
   {
      QPlainTextEdit::keyPressEvent(e);
   }

   const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
   if (!completer || (ctrlOrShift && e->text().isEmpty()))
   {
      return;
   }

   static QString eow("~!@#$%*()_+{}|:\"<>?,./:'[]\\-=");
   bool hasModifier = (e->modifiers() != Qt::NoModifier) && ctrlOrShift;
   QString completionPrefix = textUnderCursor();

   if (!isShortcut &&
       (hasModifier || e->text().isEmpty()
        || completionPrefix.length() < 2 || eow.contains(e->text().right(1))))
   {
      completer->popup()->hide();
      return;
   }

   if (completionPrefix != completer->completionPrefix())
   {
      completer->setCompletionPrefix(completionPrefix);
      completer->popup()->setCurrentIndex(completer->completionModel()->index(0,0));
   }
   QRect cr = cursorRect();
   cr.setWidth(completer->popup()->sizeHintForColumn(0)
               + completer->popup()->verticalScrollBar()->sizeHint().width());
   completer->complete(cr);
}

void QScriptEditor::focusInEvent(QFocusEvent *e)
{
   if (completer)
   {
      completer->setWidget(this);
   }
   QPlainTextEdit::focusInEvent(e);
}

void QScriptEditor::insertCompletion(const QString &completion)
{
   if (completer->widget() != this)
   {
      return;
   }
   QTextCursor tc = textCursor();
   int extra = completion.length() - completer->completionPrefix().length();
   tc.movePosition(QTextCursor::Left);
   tc.movePosition(QTextCursor::EndOfWord);
   tc.insertText(completion.right(extra));
   setTextCursor(tc);
}

QString QScriptEditor::textUnderCursor() const
{
   QTextCursor tc = textCursor();
   tc.select(QTextCursor::WordUnderCursor);
   return tc.selectedText();
}
