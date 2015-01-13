#ifndef QSYNTAXHL_H
#define QSYNTAXHL_H

#include <QSyntaxHighlighter>
#include <QRegExp>
#include <QHash>
#include <QTextCharFormat>
#include <QTextEdit>

class QSyntaxHL : public QSyntaxHighlighter
{
  Q_OBJECT

public:
  explicit QSyntaxHL(QTextDocument* parent = 0);
protected:
  virtual void highlightBlock(const QString &text) override;
private:
  struct HighlightingRule
  {
    QRegExp pattern;
    QTextCharFormat format;
  };
  QVector<HighlightingRule> highlightingRules;
  QTextCharFormat keywordFormat;
  QTextCharFormat intFormat;
  QTextCharFormat XYioFormat;
  QTextCharFormat ladderDiagramFormat;
};

#endif // SYNTAXHL_H
